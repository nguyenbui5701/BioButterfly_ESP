#include "link_protocol.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace link;
#define CHECK(x) do { if (!(x)) { std::fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); std::exit(1); } } while (0)
int main() {
    const uint8_t peer[6] = {2,0,0,0,0,1}, other[6] = {2,0,0,0,0,2};
    const uint32_t run = 0x11223344;
    const uint8_t golden[] = {0x7e,0xb1,1,1,0x44,0x33,0x22,0x11,1,0,0,0,0xa4,0xa5,0xa5,0xa5};
    Wire w{}; Frame f{};
    encode({Kind::data,run,1},w);
    CHECK(!std::memcmp(w,golden,16));
    CHECK(decode(w,16,Kind::data,peer,peer,run,f)==Result::ok && f.seq==1 && f.run==run);
    CHECK(decode(w,15,Kind::data,peer,peer,run,f)==Result::length);
    CHECK(decode(w,17,Kind::data,peer,peer,run,f)==Result::length);
    CHECK(decode(nullptr,16,Kind::data,peer,peer,run,f)==Result::length);
    CHECK(decode(w,16,Kind::data,other,peer,run,f)==Result::peer);
    CHECK(decode(w,16,Kind::data,peer,peer,run+1,f)==Result::run);
    w[0]^=1; CHECK(decode(w,16,Kind::data,peer,peer,run,f)==Result::magic); w[0]^=1;
    w[2]=2; CHECK(decode(w,16,Kind::data,peer,peer,run,f)==Result::version); w[2]=1;
    w[3]=3; CHECK(decode(w,16,Kind::data,peer,peer,run,f)==Result::kind); w[3]=1;
    w[12]^=1; CHECK(decode(w,16,Kind::data,peer,peer,run,f)==Result::pattern); w[12]^=1;
    encode({Kind::data,run,0},w); CHECK(decode(w,16,Kind::data,peer,peer,run,f)==Result::sequence);
    encode({Kind::data,run,1},w);
    std::puts("PASS codec golden, roundtrip, malformed frames");
    Receiver rx; Frame a{};
    CHECK(!rx.tick(900) && rx.state==State::waiting);
    CHECK(rx.accept(w,16,peer,peer,run,1000,a)==Result::ok && a.kind==Kind::ack && a.seq==1);
    Wire ack{}; encode(a,ack);
    CHECK(ack[3]==2 && !std::memcmp(ack+4,golden+4,12));
    CHECK(decode(ack,16,Kind::ack,peer,peer,run,f)==Result::ok);
    CHECK(rx.accept(w,16,peer,peer,run,1200,a)==Result::duplicate && rx.last_ms==1000);
    CHECK(!rx.tick(1449) && rx.tick(1450) && rx.losses==1);
    CHECK(!rx.tick(2000));
    encode({Kind::data,run,3},w);
    CHECK(rx.accept(w,16,peer,peer,run,3000,a)==Result::ok && rx.state==State::link_ok && rx.max_gap_ms==2000);
    encode({Kind::data,run,2},w);
    CHECK(rx.accept(w,16,peer,peer,run,3100,a)==Result::stale && rx.last_ms==3000 && rx.unique==2);
    w[2]=9; CHECK(rx.accept(w,16,peer,peer,run,3200,a)==Result::version && rx.last_ms==3000);
    Receiver wrap; encode({Kind::data,run,1},w);
    CHECK(wrap.accept(w,16,peer,peer,run,0xffffff00u,a)==Result::ok);
    CHECK(!wrap.tick(193u) && wrap.tick(194u));
    std::puts("PASS RX sequence, freshness, timeout, recovery, timer wrap");
    Ledger tx; uint32_t rtt=999;
    CHECK(tx.record(1,Ledger::Slot::sent,100));
    CHECK(tx.record(2,Ledger::Slot::skipped,150));
    CHECK(tx.record(3,Ledger::Slot::error,200));
    CHECK(tx.record(4,Ledger::Slot::sent,250));
    CHECK(!tx.record(4,Ledger::Slot::sent,250));
    CHECK(tx.mac(1,true) && !tx.mac(1,true) && !tx.mac(3,true));
    encode({Kind::ack,run,1},w);
    CHECK(tx.ack(w,16,peer,peer,run,115,rtt)==Result::ok && rtt==15);
    CHECK(tx.ack(w,16,peer,peer,run,116,rtt)==Result::duplicate);
    encode({Kind::ack,run,2},w); CHECK(tx.ack(w,16,peer,peer,run,200,rtt)==Result::unsent);
    encode({Kind::ack,run,3},w); CHECK(tx.ack(w,16,peer,peer,run,220,rtt)==Result::unsent);
    encode({Kind::ack,run,4},w); CHECK(tx.ack(w,16,peer,peer,run,751,rtt)==Result::late);
    CHECK(tx.ack(w,16,peer,peer,run+1,260,rtt)==Result::run);
    for (uint32_t i=5;i<=slots;++i) CHECK(tx.record(i,Ledger::Slot::skipped,0));
    CHECK(!tx.record(slots+1,Ledger::Slot::sent,0));
    const auto& c=tx.counts();
    CHECK(c.scheduled==1200 && c.skipped==1197 && c.attempted==3 && c.send_error==1);
    CHECK(c.mac_success==1 && c.ack_unique==1 && c.rejected==4 && c.late==1);
    CHECK(c.scheduled==c.skipped+c.attempted);
    std::puts("PASS TX bounded accounting, ACK uniqueness, skipped/error/late");
    std::puts("HOST TESTS PASS");
}
