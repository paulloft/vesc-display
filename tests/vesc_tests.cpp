#include "Vesc.h"
#include <cassert>
#include <iostream>
#include <vector>
static uint32_t now=0;
static uint32_t clockSkew=0;
extern "C" uint32_t millis(void){return now+clockSkew;}
static void inject(Vesc& v,const std::vector<uint8_t>& p){
  uint8_t f[1030];auto n=bike::frame(p.data(),p.size(),f,sizeof(f));assert(n);
  for(size_t i=0;i<n;i++)HardwareSerial::instance->incoming.push_back(f[i]);v.tick(now);
}
static std::vector<std::vector<uint8_t>> sent;
static void collect(const uint8_t* p,size_t n,void*){sent.emplace_back(p,p+n);}
static void drain(){bike::PacketParser parser;sent.clear();for(uint8_t b:HardwareSerial::instance->outgoing)parser.feed(b,now,collect,nullptr);HardwareSerial::instance->outgoing.clear();}
static std::vector<uint8_t> last(uint8_t id){for(auto i=sent.rbegin();i!=sent.rend();i++)if((*i)[0]==id)return *i;return {};}
static std::vector<uint8_t> limits(float scale,float erpm){
  std::vector<uint8_t> p(50);p[0]=91;
  float values[]={.8f,scale,-20000,erpm,.005f,.95f,-3000,8000,-20,60};
  for(int i=0;i<10;i++)bike::putFloat(p.data()+1+4*i,values[i]);p[41]=46;
  bike::putFloat(p.data()+42,1);bike::putFloat(p.data()+46,.6f);return p;
}
static std::vector<uint8_t> telemetry(){
  std::vector<uint8_t> p{51};uint8_t b[4];bike::put32(b,bike::setupMask);p.insert(p.end(),b,b+4);
  for(int bit=0;bit<22;bit++)if(bike::setupMask&(1u<<bit)){
    int len=(bit==0||bit==1||bit==7||bit==8)?2:bit==16?1:4;
    int value=bit==6?3000:bit==8?800:bit==21?9000:0;
    bike::put32(b,value);p.insert(p.end(),b+4-len,b+4);
  }
  return p;
}
static void adc(Vesc& v,float throttle){std::vector<uint8_t> p(17);p[0]=32;bike::put32(p.data()+1,int(throttle*1000000));inject(v,p);}
static void text(Vesc& v,const char* text){std::vector<uint8_t> p{135};while(*text)p.push_back(*text++);inject(v,p);}
int main(){
  Vesc v;bike::Config c;v.begin(115200);now=1600;v.tick(now);drain();assert(last(48).empty());
  inject(v,{0,7,0});inject(v,limits(1,20000));inject(v,telemetry());adc(v,0);drain();
  assert(!v.request(bike::Mode::Drive,25,c));drain();assert(last(48).empty()); // Read-only build sends no motor writes.
  assert(v.message=="Управление VESC выключено в настройках");
  v.controlEnabled=true;now+=1;v.tick(now);drain();auto neutral=last(48);assert(neutral.size()==45);
  assert(bike::getFloat(neutral.data()+5)==.8f&&bike::getFloat(neutral.data()+9)==0);
  assert(bike::getFloat(neutral.data()+13)==-20000&&bike::getFloat(neutral.data()+29)==-3000);
  float target=bike::getFloat(neutral.data()+17);
  assert(target>0&&target<20000&&!v.applied());
  inject(v,{48});assert(!v.applied());inject(v,limits(1,target));assert(!v.applied()); // ACK alone is insufficient.
  inject(v,limits(0,target));assert(v.applied());
  adc(v,.5f);assert(!v.request(bike::Mode::Drive,25,c));adc(v,0);
  assert(v.request(bike::Mode::Drive,25,c));drain();auto drive=last(48);assert(bike::getFloat(drive.data()+9)==1);
  inject(v,{48});inject(v,limits(1,target));text(v,"> \"EBIKE_RAMP_0_OK\"");assert(!v.applied());
  inject(v,{138,'E','B','I','K','E','_','R','A','M','P','_','1','_','O','K'});assert(!v.applied());
  now+=800;inject(v,telemetry());adc(v,0);v.tick(now);drain();assert(!last(138).empty());
  text(v,"> \"EBIKE_RAMP_1_OK\"");assert(v.applied());
  assert(!v.cruise(true));v.cruiseEnabled=true;assert(v.cruise(true));
  now+=800;inject(v,telemetry());adc(v,0);v.tick(now);drain();auto program=last(138);assert(program.size()>100&&program.size()<1024);
  text(v,"> \"EBIKE_CC_ON\"");assert(v.cruiseActive);
  now+=2000;v.tick(now);assert(!v.cruiseActive); // No heartbeat after telemetry loss.
  Vesc unsupported;unsupported.begin(115200);unsupported.controlEnabled=true;
  inject(unsupported,{0,6,6});inject(unsupported,limits(1,20000));inject(unsupported,telemetry());
  assert(!unsupported.ready()&&!unsupported.request(bike::Mode::Drive,25,c));drain();assert(last(48).empty());
  assert(unsupported.message=="Для управления нужна прошивка VESC 7.0");
  Vesc clocked;clocked.begin(115200);clockSkew=2;
  inject(clocked,telemetry());
  // Receiving can cross a millisecond boundary after loop() captured `now`.
  assert(clocked.telemetry.receivedMs==now&&clocked.telemetry.fresh(now));
  assert(clocked.rxBytes>0&&clocked.goodPackets==1&&clocked.badPayloads==0);
  clockSkew=0;
  now+=1200;clocked.tick(now);assert(!clocked.telemetry.fresh(now));
  clocked.controlEnabled=true;assert(!clocked.request(bike::Mode::Drive,25,c));
  assert(clocked.message=="Нет свежих данных VESC: проверьте UART");
  inject(clocked,telemetry());assert(!clocked.request(bike::Mode::Drive,25,c));
  assert(clocked.message=="Ожидается версия прошивки VESC");
  inject(clocked,{0,7,0});assert(!clocked.request(bike::Mode::Drive,25,c));
  assert(clocked.message=="Не прочитаны лимиты VESC: проверьте диагностику");
  Vesc disconnected;disconnected.begin(115200);
  for(int i=0;i<20;i++){now+=2300;disconnected.tick(now);assert(disconnected.baud()==115200);}
  Vesc timeout;timeout.begin(115200);timeout.controlEnabled=true;
  inject(timeout,{0,7,0});inject(timeout,limits(1,20000));inject(timeout,telemetry());adc(timeout,0);
  assert(timeout.request(bike::Mode::Drive,25,c)&&timeout.applying());
  now+=5000;inject(timeout,telemetry());
  assert(timeout.applicationFailed&&!timeout.applying()&&!timeout.applied());
  assert(timeout.message=="VESC не подтвердил лимит за 5 секунд");
  text(timeout,"EBIKE_RAMP_1_OK");assert(!timeout.rampConfirmed);
  adc(timeout,0);assert(timeout.request(bike::Mode::Drive,25,c));drain();
  auto retry=last(48);float retryTarget=bike::getFloat(retry.data()+17);
  inject(timeout,limits(1,retryTarget));now+=5000;inject(timeout,telemetry());
  assert(timeout.applicationFailed&&timeout.message=="VESC не подтвердил ускорение за 5 секунд");
  const uint8_t uid[12]={1,2,3,4,5,6,7,8,9,10,11,12};
  std::vector<uint8_t> version{0,7,0,'T',0};version.insert(version.end(),uid,uid+12);
  bike::Config cache;cache.vescBaselineId=bike::crc32(uid,12);
  cache.vescBaselineMaxErpm=100000;cache.vescBaselineDriveScale=1;
  Vesc warm;warm.startupLimit(0);warm.begin(115200,&cache);warm.controlEnabled=true;
  inject(warm,version);inject(warm,telemetry());
  // ESP reboot while VESC retains our old neutral and low speed RAM limit.
  inject(warm,limits(0,4510));adc(warm,0);
  assert(warm.baselineTrusted()&&warm.baselineValue(3)==100000&&warm.baselineValue(1)==1);
  for(uint8_t limit:{25,50,70,0}) {
    assert(warm.request(bike::Mode::Drive,limit,c));drain();auto command=last(48);
    float expected=limit?limit/3.6f*warm.speedFactor():100000;
    assert(fabsf(bike::getFloat(command.data()+17)-expected)<.01f);
    assert(bike::getFloat(command.data()+9)==1);
  }
  cache.speedErpmPerMs=1700; // Wheel speed override differs from nominal geometry.
  for(uint8_t limit:{25,50,70,0}) {
    assert(warm.request(bike::Mode::Drive,limit,c));drain();auto command=last(48);
    float expected=limit?limit/3.6f*1700:100000;
    assert(fabsf(bike::getFloat(command.data()+17)-expected)<.01f);
  }
  cache.vescBaselineId++;assert(warm.speedFactor()==warm.geometrySpeedFactor());
  bike::Config unknown;Vesc provisional;provisional.begin(115200,&unknown);provisional.controlEnabled=true;
  inject(provisional,version);inject(provisional,telemetry());inject(provisional,limits(1,4510));adc(provisional,0);
  assert(!provisional.baselineTrusted()&&!provisional.request(bike::Mode::Drive,0,c));
  // An observed controller restart reloads its saved limits, before ESP writes N.
  auto resetTelemetry=telemetry();bike::put32(resetTelemetry.data()+resetTelemetry.size()-4,1000);
  inject(provisional,resetTelemetry);inject(provisional,version);inject(provisional,limits(1,100000));adc(provisional,0);
  assert(provisional.baselineTrusted()&&provisional.baselineChanged);
  assert(unknown.vescBaselineId==bike::crc32(uid,12)&&unknown.vescBaselineMaxErpm==100000);
  assert(provisional.request(bike::Mode::Sport,0,c));drain();
  assert(bike::getFloat(last(48).data()+17)==100000);
  std::cout<<"PASS: read-only defaults, VESC version gate, preserved regen/duty/watts, neutral readback, throttle interlock, ramp token, cruise opt-in and link loss\n";
}
