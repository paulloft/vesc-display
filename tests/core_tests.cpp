#include "Core.h"
#include "FrameBuffer.h"
#include <assert.h>
#include <stdio.h>
#include <vector>
#include <string.h>
using namespace bike;
static std::vector<Event> events;
static void event(Event e,void*){events.push_back(e);}
static std::vector<uint8_t> received;
static int packetCount=0;
static void packet(const uint8_t* p,size_t n,void*){received.assign(p,p+n);packetCount++;}
static void add16(std::vector<uint8_t>& p,int16_t x){p.push_back(uint16_t(x)>>8);p.push_back(x);}
static void add32(std::vector<uint8_t>& p,int32_t x){uint8_t b[4];put32(b,x);p.insert(p.end(),b,b+4);}
static std::vector<uint8_t> sample(){
  std::vector<uint8_t> p{51};add32(p,setupMask);
  add16(p,381);add16(p,482);add32(p,1200);add32(p,-350);
  add32(p,10000);add32(p,11667);add16(p,801);add16(p,820);
  add32(p,125000);add32(p,23000);add32(p,12345000);p.push_back(0);
  add32(p,123456);add32(p,9000);return p;
}
int main(){
  // Guard both physical framebuffer ends, including the last landscape column.
  std::vector<uint16_t> rotatedFrame(320*480+2,0xbeef),patch(480*24);
  for(size_t i=0;i<patch.size();i++)patch[i]=uint16_t(i);
  copyLandscapeToPortrait(rotatedFrame.data()+1,patch.data(),0,0,480,24);
  copyLandscapeToPortrait(rotatedFrame.data()+1,patch.data(),0,296,480,24);
  assert(rotatedFrame.front()==0xbeef&&rotatedFrame.back()==0xbeef);
  for(int row=0;row<24;row++)for(int col=0;col<480;col++) {
    assert(rotatedFrame[1+col*320+319-row]==patch[row*480+col]);
    assert(rotatedFrame[1+col*320+23-row]==patch[row*480+col]);
  }
  std::fill(rotatedFrame.begin(),rotatedFrame.end(),0xbeef);
  copyLandscapeToPortrait(rotatedFrame.data()+1,patch.data(),0,0,480,24,true);
  copyLandscapeToPortrait(rotatedFrame.data()+1,patch.data(),0,296,480,24,true);
  assert(rotatedFrame.front()==0xbeef&&rotatedFrame.back()==0xbeef);
  for(int row=0;row<24;row++)for(int col=0;col<480;col++) {
    assert(rotatedFrame[1+(479-col)*320+row]==patch[row*480+col]);
    assert(rotatedFrame[1+(479-col)*320+296+row]==patch[row*480+col]);
  }
  Config c;State s;
  s.safe=false;s.setLimit(70);s.setSafe(true);assert(s.limit==50);
  s.setSafe(false);assert(s.limit==50);s.setLimit(0);s.setSafe(true);assert(s.limit==50);
  s.light=1;s.highBeam();assert(s.light==3&&s.outputs(0,c)==7);s.lightClick();assert(s.light==1);
  s.light=0;s.flash=true;assert(s.outputs(0,c)==4);s.flash=false;
  s.turnKey(Turn::Left);assert(s.outputs(0,c)==32);assert(s.outputs(c.blinkOnMs,c)==0);
  s.turnKey(Turn::Right);assert(s.outputs(0,c)==16);s.turnKey(Turn::Right);assert(s.turn==Turn::Off);
  s.turn=Turn::Hazard;assert(s.outputs(0,c)==48);s.faults=1<<3;assert(s.outputs(0,c)==16);
  s.light=3;s.horn=true;s.faults=1<<6;assert(!(s.outputs(0,c)&8));
  s.faults=1<<5;assert((s.outputs(0,c)&7)==7); // Brake fault cannot switch a light off.
  for(int i=0;i<5;i++)assert(decodeResistance(c.keyOhms[i],c)==Key(i+1));
  assert(decodeResistance(3322.15f,c)==Key::Invalid);
  assert(decodeResistance(761.9f,c)==Key::Invalid);
  assert(decodeResistance(0,c)==Key::Invalid);assert(decodeResistance(200000,c)==Key::None);
  Button b;b.tick(true,0,Key::M,c,event,nullptr);b.tick(true,35,Key::M,c,event,nullptr);
  b.tick(false,100,Key::M,c,event,nullptr);b.tick(false,135,Key::M,c,event,nullptr);
  b.tick(true,200,Key::M,c,event,nullptr);b.tick(true,235,Key::M,c,event,nullptr);
  b.tick(false,300,Key::M,c,event,nullptr);b.tick(false,335,Key::M,c,event,nullptr);b.tick(false,900,Key::M,c,event,nullptr);
  int doubles=0,clicks=0;for(auto e:events){doubles+=e.gesture==Gesture::Double;clicks+=e.gesture==Gesture::Click;}
  assert(doubles==1&&clicks==0);
  events.clear();Button held;held.tick(true,0,Key::Light,c,event,nullptr);held.tick(true,35,Key::Light,c,event,nullptr);
  held.tick(true,2035,Key::Light,c,event,nullptr);held.tick(false,2100,Key::Light,c,event,nullptr);held.tick(false,2135,Key::Light,c,event,nullptr);held.tick(false,3000,Key::Light,c,event,nullptr);
  int holds=0;for(auto e:events){holds+=e.gesture==Gesture::Hold;assert(e.gesture!=Gesture::Click);}assert(holds==1);
  events.clear();Button rollover;rollover.tick(true,0xfffffff0u,Key::Horn,c,event,nullptr);rollover.tick(true,30,Key::Horn,c,event,nullptr);assert(events.size()==1&&events[0].gesture==Gesture::Down);
  rollover.cancel(Key::Horn,event,nullptr);assert(events.back().gesture==Gesture::Cancel);
  auto p=sample();Telemetry t;assert(parseSetup(p.data(),p.size(),100,t));
  assert(fabsf(t.speedMs*3.6f-42.0012f)<0.01f&&fabsf(t.soc-.82f)<.001f);
  assert(t.odometer==123456&&t.inputCurrent==-3.5f&&t.motor==48.2f);
  assert(t.erpm==10000);
  assert(t.fresh(1000)&&!t.fresh(1300));
  Telemetry before=t;assert(!parseSetup(p.data(),p.size()-1,200,t));assert(t.receivedMs==before.receivedMs);
  p[4]^=1;assert(!parseSetup(p.data(),p.size(),200,t));p[4]^=1;
  uint8_t bytes[1030];auto n=frame(p.data(),p.size(),bytes,sizeof(bytes));assert(n>0);
  PacketParser parser;for(size_t i=0;i<n;i++)parser.feed(bytes[i],100+i,packet,nullptr);
  assert(packetCount==1&&received==p);
  bytes[8]^=1;for(size_t i=0;i<n;i++)parser.feed(bytes[i],300+i,packet,nullptr);assert(packetCount==1&&parser.errors==1);bytes[8]^=1;
  parser.feed(2,500,packet,nullptr);parser.feed(50,501,packet,nullptr);
  for(size_t i=0;i<n;i++)parser.feed(bytes[i],700+i,packet,nullptr);assert(packetCount==2); // Recover after truncated packet.
  std::vector<uint8_t> big(500,0xa5);n=frame(big.data(),big.size(),bytes,sizeof(bytes));
  for(size_t i=0;i<n;i++)parser.feed(bytes[i],1000+i,packet,nullptr);assert(received==big);
  assert(frame(big.data(),big.size(),bytes,10)==0);
  const uint8_t known[]={'1','2','3','4','5','6','7','8','9'};
  assert(crc16(known,9)==0x31c3&&crc32(known,9)==0xcbf43926);
  uint8_t f[4];putFloat(f,123.25f);assert(getFloat(f)==123.25f);
  printf("PASS: modes, light/turn/fault mapping, resistor ambiguity, double/hold/cancel, millis wrap, telemetry validation, packet recovery/CRC, float encoding\n");
}
