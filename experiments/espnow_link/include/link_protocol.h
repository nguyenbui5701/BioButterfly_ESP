#pragma once
#include <cstddef>
#include <cstdint>

namespace link {
constexpr std::size_t wire_size = 16;
constexpr uint32_t stale_ms = 450, period_ms = 50, slots = 1200, drain_ms = 500;
enum class Kind : uint8_t { data = 1, ack = 2 };
enum class Result { ok, length, magic, version, kind, peer, run, sequence, pattern, stale, unsent, duplicate, late };
enum class State { waiting, link_ok, link_lost };
struct Frame { Kind kind; uint32_t run, seq; };
using Wire = uint8_t[wire_size];
void encode(const Frame& f, Wire& out);
Result decode(const uint8_t* bytes, std::size_t n, Kind expected,
              const uint8_t* source, const uint8_t* peer, uint32_t run, Frame& out);

struct Receiver {
    State state = State::waiting;
    uint32_t last_seq = 0, last_ms = 0, unique = 0, max_gap_ms = 0;
    uint32_t rejected = 0, losses = 0;
    bool tick(uint32_t now);
    Result accept(const uint8_t* bytes, std::size_t n, const uint8_t* source,
                  const uint8_t* peer, uint32_t run, uint32_t now, Frame& ack);
};

// Fixed-size baseline ledger. One record per scheduled slot; sequence is the slot
// number, so skipped slots are visible as gaps. Never reuse a ledger across runs.
class Ledger {
public:
    enum class Slot : uint8_t { unused, skipped, error, sent, acked };
    struct Counts {
        uint32_t scheduled = 0, skipped = 0, attempted = 0, send_error = 0;
        uint32_t mac_success = 0, mac_failure = 0, ack_unique = 0, rejected = 0, late = 0;
    };
    bool record(uint32_t seq, Slot outcome, uint32_t now);
    bool mac(uint32_t seq, bool success);
    Result ack(const uint8_t* bytes, std::size_t n, const uint8_t* source,
               const uint8_t* peer, uint32_t run, uint32_t now, uint32_t& rtt);
    const Counts& counts() const { return counts_; }
private:
    struct Entry { uint32_t sent_ms = 0; Slot status = Slot::unused; bool mac_seen = false; };
    Entry entries_[slots]{};
    Counts counts_{};
};
} // namespace link
