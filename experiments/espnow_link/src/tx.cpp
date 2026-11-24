#include "radio.h"
#include <algorithm>
#include <cstring>
using namespace link;
namespace {
Ledger ledger;
bool ready=false, started=false, done=false;
uint32_t run=0, start_ms=0, total=0, pending=0, unexpected=0, outside=0;
char mode=0, line[48]{};
struct Trace {uint32_t sent=0, ack=0, rtt=0; bool matched=false; char outcome='-';};
Trace trace[slots];
uint32_t values[slots], n=0;
bool pause_slot(uint32_t s) {
    return mode=='p' && ((s>=21&&s<=70)||(s>=91&&s<=140)||(s>=161&&s<=210));
}
bool invalid_slot(uint32_t s) { return mode=='n' && s>=21 && s<=70; }
void report() {
    const auto& c=ledger.counts();
    std::sort(values,values+n);
    uint32_t lo=n?values[0]:0, hi=n?values[n-1]:0;
    uint32_t p50=n?values[(n*50+99)/100-1]:0, p95=n?values[(n*95+99)/100-1]:0;
    Serial.printf("TX_SUMMARY run=%08lx mode=%c scheduled=%lu skipped=%lu attempted=%lu send_error=%lu mac_success=%lu mac_failure=%lu ack_unique=%lu rejected=%lu late=%lu outside=%lu unexpected=%lu queue_drop=%lu pending=%lu rtt_n=%lu min=%lu p50=%lu p95=%lu max=%lu\n",
        (unsigned long)run,mode,(unsigned long)c.scheduled,(unsigned long)c.skipped,(unsigned long)c.attempted,(unsigned long)c.send_error,(unsigned long)c.mac_success,(unsigned long)c.mac_failure,(unsigned long)c.ack_unique,(unsigned long)c.rejected,(unsigned long)c.late,(unsigned long)outside,(unsigned long)unexpected,(unsigned long)radio::drops(),(unsigned long)pending,(unsigned long)n,(unsigned long)lo,(unsigned long)p50,(unsigned long)p95,(unsigned long)hi);
}
void dump() {
    for(uint32_t i=0;i<total;++i) {
        const auto& t=trace[i];
        Serial.printf("TX_SLOT seq=%lu outcome=%c sent_ms=%lu matched=%u ack_ms=%lu rtt_ms=%lu\n",(unsigned long)(i+1),t.outcome,(unsigned long)t.sent,unsigned(t.matched),(unsigned long)t.ack,(unsigned long)t.rtt);
        if((i&15)==15) delay(1);
    }
    report();
}
}
void setup(){
    Serial.begin(115200); ready=radio::init();
    Serial.printf("ROLE=TX LINK_TEST_V1 ready=%u; start XXXXXXXX s|b|p|n; dump after done\n",unsigned(ready));
}
void loop(){
    if(!ready){
        if(radio::command(line))Serial.println("INIT_FAILED; reset after resolving config");
        delay(10);return;
    }
    if(radio::command(line)){
        if(!started && radio::parse_start(line,run,mode)){
            total=mode=='s'?10:mode=='b'?slots:mode=='p'?230:90;
            started=true;start_ms=millis()+1000;
            Serial.printf("TX_START run=%08lx mode=%c start_ms=%lu slots=%lu\n",(unsigned long)run,mode,(unsigned long)start_ms,(unsigned long)total);
        }else if(done && !strcmp(line,"dump")) dump();
        else if(!strcmp(line,"info")) Serial.printf("ROLE=TX LINK_TEST_V1 build=%s sdk=%s channel_ok=%u encrypted=1\n",LINK_BUILD_SHA,ESP.getSdkVersion(),unsigned(radio::channel_ok()));
        else if(!strcmp(line,"status")) report();
        else Serial.println("COMMAND_REJECT one run per boot");
    }
    radio::Event e;
    for(unsigned k=0;k<32 && radio::pop(e);++k){
        if(e.send){
            if(pending && !memcmp(e.peer,radio::peer(),6)){
                if(!done) ledger.mac(pending,e.success);
                pending=0;
            }else ++unexpected;
            continue;
        }
        if(!started||done||int32_t(e.ms-start_ms)<0||uint32_t(e.ms-start_ms)>=total*period_ms+drain_ms){++outside;continue;}
        Frame a{};
        if(decode(e.bytes,e.length,Kind::ack,e.peer,radio::peer(),run,a)==Result::ok && invalid_slot(a.seq)){++unexpected;continue;}
        uint32_t rtt;
        if(ledger.ack(e.bytes,e.length,e.peer,radio::peer(),run,e.ms,rtt)==Result::ok){
            auto& t=trace[a.seq-1]; t.ack=e.ms;t.rtt=rtt;t.matched=true;values[n++]=rtt;
        }
    }
    if(started&&!done){
        uint32_t now=millis();
        if(int32_t(now-start_ms)>=0){
            uint32_t elapsed=now-start_ms;
            uint32_t due=std::min(total,elapsed/period_ms+1);
            while(ledger.counts().scheduled<due){
                uint32_t s=ledger.counts().scheduled+1;
                auto& t=trace[s-1];
                if(s<due||elapsed>=total*period_ms||pending||pause_slot(s)){
                    ledger.record(s,Ledger::Slot::skipped,now);t.outcome='K';continue;
                }
                Wire w;encode({Kind::data,run,s},w);
                if(invalid_slot(s))w[2]=2;
                t.sent=millis();
                bool ok=radio::send(w);
                ledger.record(s,ok?Ledger::Slot::sent:Ledger::Slot::error,t.sent);
                t.outcome=ok?(invalid_slot(s)?'N':'S'):'E';
                if(ok)pending=s;
            }
            if(elapsed>=total*period_ms+drain_ms){done=true;Serial.printf("TX_DONE ms=%lu\n",(unsigned long)now);report();}
        }
    }
    delay(1);
}
