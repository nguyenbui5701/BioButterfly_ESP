#include "radio.h"
#include <cstring>
using namespace link;
namespace {
Receiver rx;
bool ready=false,started=false,done=false,pending=false;
uint32_t run=0,ack_attempt=0,ack_error=0,ack_skip=0,mac_success=0,mac_failure=0,unexpected=0,outside=0,trace_overflow=0;
char mode=0,line[48]{};
struct Trace{uint32_t seq,ms;};
Trace trace[slots]; uint32_t count=0;
struct Reject{uint32_t ms,last,seq;int reason;};
Reject rejects[128];uint32_t reject_count=0;
void tick(uint32_t now){
    if(rx.tick(now))Serial.printf("RX_LINK_LOST ms=%lu last_valid_ms=%lu delay_ms=%lu last_seq=%lu\n",(unsigned long)now,(unsigned long)rx.last_ms,(unsigned long)(now-rx.last_ms),(unsigned long)rx.last_seq);
}
void report(){
    Serial.printf("RX_SUMMARY run=%08lx mode=%c unique=%lu rejected=%lu losses=%lu max_gap_ms=%lu last_seq=%lu last_valid_ms=%lu ack_attempt=%lu ack_error=%lu ack_skip=%lu mac_success=%lu mac_failure=%lu queue_drop=%lu trace_overflow=%lu unexpected=%lu outside=%lu pending=%u\n",
        (unsigned long)run,mode,(unsigned long)rx.unique,(unsigned long)rx.rejected,(unsigned long)rx.losses,(unsigned long)rx.max_gap_ms,(unsigned long)rx.last_seq,(unsigned long)rx.last_ms,(unsigned long)ack_attempt,(unsigned long)ack_error,(unsigned long)ack_skip,(unsigned long)mac_success,(unsigned long)mac_failure,(unsigned long)radio::drops(),(unsigned long)trace_overflow,(unsigned long)unexpected,(unsigned long)outside,unsigned(pending));
}
}
void setup(){
    Serial.begin(115200);ready=radio::init();
    Serial.printf("ROLE=RX LINK_TEST_V1 ready=%u; start XXXXXXXX s|b|p|n; end; dump\n",unsigned(ready));
}
void loop(){
    if(!ready){delay(10);return;}
    radio::Event e;
    for(unsigned k=0;k<32&&radio::pop(e);++k){
        if(e.send){
            if(pending&&!memcmp(e.peer,radio::peer(),6)){pending=false;if(e.success)++mac_success;else ++mac_failure;}
            else ++unexpected;
            continue;
        }
        if(!started||done){++outside;continue;}
        tick(e.ms);
        State before=rx.state;Frame a{};
        auto result=rx.accept(e.bytes,e.length,e.peer,radio::peer(),run,e.ms,a);
        if(result!=Result::ok){
            uint32_t seq=0;
            if(e.length==16)for(unsigned j=0;j<4;++j)seq|=uint32_t(e.bytes[8+j])<<(8*j);
            if(reject_count<128)rejects[reject_count++]={e.ms,rx.last_ms,seq,int(result)};
            else ++trace_overflow;
            continue;
        }
        if(count<slots)trace[count++]={a.seq,e.ms};else ++trace_overflow;
        if(before!=State::link_ok)Serial.printf("RX_LINK_OK ms=%lu seq=%lu\n",(unsigned long)e.ms,(unsigned long)a.seq);
        if(pending){++ack_skip;continue;}
        Wire w;encode(a,w);++ack_attempt;
        if(radio::send(w))pending=true;else ++ack_error;
    }
    if(started&&!done)tick(millis());
    if(radio::command(line)){
        if(!started&&radio::parse_start(line,run,mode)){
            started=true;Serial.printf("RX_WAITING run=%08lx mode=%c ms=%lu\n",(unsigned long)run,mode,(unsigned long)millis());
        }else if(started&&!done&&!strcmp(line,"end")){
            done=true;Serial.printf("RX_END ms=%lu\n",(unsigned long)millis());report();
        }else if(done&&!strcmp(line,"dump")){
            for(uint32_t i=0;i<count;++i){
                Serial.printf("RX_DATA seq=%lu ms=%lu\n",(unsigned long)trace[i].seq,(unsigned long)trace[i].ms);
                if((i&15)==15)delay(1);
            }
            for(uint32_t i=0;i<reject_count;++i){
                const auto& q=rejects[i];
                Serial.printf("RX_REJECT seq=%lu ms=%lu last_valid_ms=%lu reason=%d\n",(unsigned long)q.seq,(unsigned long)q.ms,(unsigned long)q.last,q.reason);
            }
            report();
        }else if(!strcmp(line,"status"))report();
        else Serial.println("COMMAND_REJECT one run per boot");
    }
    delay(1);
}
