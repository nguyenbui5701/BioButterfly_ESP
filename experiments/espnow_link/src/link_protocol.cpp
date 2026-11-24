#include "link_protocol.h"
#include <cstring>

namespace link {
static void put32(uint8_t* p, uint32_t v) {
    for (unsigned i = 0; i < 4; ++i) p[i] = static_cast<uint8_t>(v >> (8 * i));
}
static uint32_t get32(const uint8_t* p) {
    uint32_t v = 0;
    for (unsigned i = 0; i < 4; ++i) v |= uint32_t(p[i]) << (8 * i);
    return v;
}
void encode(const Frame& f, Wire& out) {
    out[0] = 0x7e; out[1] = 0xb1; out[2] = 1; out[3] = uint8_t(f.kind);
    put32(out + 4, f.run); put32(out + 8, f.seq);
    put32(out + 12, f.seq ^ 0xa5a5a5a5u);
}
Result decode(const uint8_t* b, std::size_t n, Kind expected,
              const uint8_t* source, const uint8_t* peer, uint32_t run, Frame& out) {
    if (!b || n != wire_size) return Result::length;
    if (!source || !peer || std::memcmp(source, peer, 6)) return Result::peer;
    if (b[0] != 0x7e || b[1] != 0xb1) return Result::magic;
    if (b[2] != 1) return Result::version;
    if ((expected != Kind::data && expected != Kind::ack) || b[3] != uint8_t(expected)) return Result::kind;
    if (!run || get32(b + 4) != run) return Result::run;
    const uint32_t seq = get32(b + 8);
    if (!seq) return Result::sequence;
    if (get32(b + 12) != (seq ^ 0xa5a5a5a5u)) return Result::pattern;
    out = Frame{expected, run, seq};
    return Result::ok;
}
bool Receiver::tick(uint32_t now) {
    if (state == State::link_ok && uint32_t(now - last_ms) >= stale_ms) {
        state = State::link_lost; ++losses; return true;
    }
    return false;
}
Result Receiver::accept(const uint8_t* b, std::size_t n, const uint8_t* source,
                        const uint8_t* peer, uint32_t run, uint32_t now, Frame& ack) {
    Frame f{};
    Result r = decode(b, n, Kind::data, source, peer, run, f);
    if (r == Result::ok && f.seq <= last_seq) r = f.seq == last_seq ? Result::duplicate : Result::stale;
    if (r != Result::ok) { ++rejected; return r; }
    if (unique) {
        const uint32_t gap = now - last_ms;
        if (gap > max_gap_ms) max_gap_ms = gap;
    }
    last_seq = f.seq; last_ms = now; ++unique; state = State::link_ok;
    ack = Frame{Kind::ack, f.run, f.seq};
    return Result::ok;
}
bool Ledger::record(uint32_t seq, Slot outcome, uint32_t now) {
    if (!seq || seq > slots || seq != counts_.scheduled + 1 ||
        (outcome != Slot::skipped && outcome != Slot::error && outcome != Slot::sent)) return false;
    Entry& e = entries_[seq - 1];
    e.status = outcome; e.sent_ms = now; ++counts_.scheduled;
    if (outcome == Slot::skipped) ++counts_.skipped;
    else { ++counts_.attempted; if (outcome == Slot::error) ++counts_.send_error; }
    return true;
}
bool Ledger::mac(uint32_t seq, bool success) {
    if (!seq || seq > slots) return false;
    Entry& e = entries_[seq - 1];
    if (e.mac_seen || (e.status != Slot::sent && e.status != Slot::acked)) return false;
    e.mac_seen = true;
    if (success) ++counts_.mac_success; else ++counts_.mac_failure;
    return true;
}
Result Ledger::ack(const uint8_t* b, std::size_t n, const uint8_t* source,
                   const uint8_t* peer, uint32_t run, uint32_t now, uint32_t& rtt) {
    Frame f{};
    Result r = decode(b, n, Kind::ack, source, peer, run, f);
    if (r != Result::ok) { ++counts_.rejected; return r; }
    if (f.seq > slots) { ++counts_.rejected; return Result::unsent; }
    Entry& e = entries_[f.seq - 1];
    if (e.status == Slot::acked) { ++counts_.rejected; return Result::duplicate; }
    if (e.status != Slot::sent) { ++counts_.rejected; return Result::unsent; }
    const uint32_t age = now - e.sent_ms;
    if (age > drain_ms) { ++counts_.late; return Result::late; }
    rtt = age; e.status = Slot::acked; ++counts_.ack_unique;
    return Result::ok;
}
} // namespace link
