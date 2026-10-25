#include "Core.h"
#include <string.h>
namespace bike {
uint16_t crc16(const uint8_t* p,size_t n) {
  uint16_t c=0;while(n--) {c^=uint16_t(*p++)<<8;for(int j=0;j<8;j++)c=(c&0x8000)?(c<<1)^0x1021:c<<1;}return c;
}
uint32_t crc32(const uint8_t* p,size_t n) {
  uint32_t c=~0u;while(n--) {c^=*p++;for(int j=0;j<8;j++)c=(c>>1)^(0xedb88320u&-(c&1));}return ~c;
}
void put32(uint8_t* p,uint32_t v){p[0]=v>>24;p[1]=v>>16;p[2]=v>>8;p[3]=v;}
uint32_t get32(const uint8_t* p){return uint32_t(p[0])<<24|uint32_t(p[1])<<16|uint32_t(p[2])<<8|p[3];}
float getFloat(const uint8_t* p){uint32_t v=get32(p);float f;memcpy(&f,&v,4);return f;}
void putFloat(uint8_t* p,float f){uint32_t v;memcpy(&v,&f,4);put32(p,v);}
size_t frame(const uint8_t* p,size_t n,uint8_t* out,size_t capacity) {
  const size_t header=n<256?2:3;if(!n||n>1024||capacity<n+header+3)return 0;
  out[0]=header==2?2:3;if(header==2)out[1]=n;else {out[1]=n>>8;out[2]=n;}
  memcpy(out+header,p,n);uint16_t c=crc16(p,n);out[header+n]=c>>8;out[header+n+1]=c;out[header+n+2]=3;
  return n+header+3;
}
void PacketParser::feed(uint8_t b,uint32_t now,Sink sink,void* ctx) {
  if(state_ && uint32_t(now-last_)>100) {state_=0;errors++;}
  last_=now;
  switch(state_) {
    case 0: if(b==2){state_=1;length_=0;}else if(b==3){state_=2;length_=0;}break;
    case 2:length_=size_t(b)<<8;state_=1;break;
    case 1:length_|=b;pos_=0;if(!length_||length_>sizeof(data_)){errors++;state_=0;}else state_=3;break;
    case 3:data_[pos_++]=b;if(pos_==length_)state_=4;break;
    case 4:crc_=uint16_t(b)<<8;state_=5;break;
    case 5:crc_|=b;state_=6;break;
    case 6:state_=0;if(b==3 && crc_==crc16(data_,length_))sink(data_,length_,ctx);else errors++;break;
  }
}
bool parseSetup(const uint8_t* p,size_t n,uint32_t now,Telemetry& out) {
  if(n<5||p[0]!=51||get32(p+1)!=setupMask)return false;
  size_t at=5;Telemetry t=out;
  for(int bit=0;bit<22;bit++)if(setupMask&(1u<<bit)) {
    size_t len=(bit==0||bit==1||bit==7||bit==8)?2:bit==16?1:4;
    if(at+len>n)return false;
    float v=len==2?float(int16_t(uint16_t(p[at])<<8|p[at+1])):len==4?float(int32_t(get32(p+at))):p[at];
    switch(bit) {
      case 0:t.fet=v/10;break;case 1:t.motor=v/10;break;
      case 2:t.current=v/100;break;case 3:t.inputCurrent=v/100;break;
      case 5:t.erpm=v;break;
      case 6:t.speedMs=v/1000;break;case 7:t.voltage=v/10;break;
      case 8:t.soc=v/1000;break;case 11:t.wh=v/10000;break;
      case 12:t.chargedWh=v/10000;break;case 14:t.distanceAbs=v/1000;break;
      case 16:t.fault=p[at];break;case 20:t.odometer=get32(p+at);break;
      case 21:t.uptime=get32(p+at);break;
    }
    at+=len;
  }
  if(at!=n || t.soc<0||t.soc>1.01f||fabsf(t.speedMs)>150||t.voltage<0||t.voltage>150)return false;
  t.receivedMs=now;t.valid=true;out=t;return true;
}
}
