#pragma once
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <algorithm>

namespace bike {
enum class Mode : uint8_t { N, Drive, Sport };
enum class Key : uint8_t { None, Light, Cruise, Left, Right, Horn, Power, M, Invalid };
enum class Gesture : uint8_t { Down, Up, Click, Double, Hold, Cancel };
enum class Turn : uint8_t { Off, Left, Right, Hazard };
struct Config {
  uint32_t debounceMs=35, doubleMs=300, holdMs=2000, blinkOnMs=450, blinkOffMs=450;
  uint32_t faultGraceMs=120, faultConfirmMs=40;
  uint8_t faultThreshold=78;
  float driveRampSeconds=1.0f, sportRampSeconds=0.15f;
  bool motionEnabled=false, cruiseEnabled=false, screenFlipped=false;
  // Original limits captured before this display changes the controller's RAM.
  uint32_t vescBaselineId=0;
  float vescBaselineMaxErpm=0,vescBaselineDriveScale=0;
  float speedErpmPerMs=0; // 0 uses VESC geometry; calibrated wheel-sensor ratio otherwise.
  float keyOhms[5]={3200,5000,19900,9900,1000};
  uint16_t keyMv[5]={217,338,1259,662,54}; // Measured Z930/display profile; SD can override.
  uint32_t keyPullupOhms=45000; // Typical internal IO9 pull-up; calibrate per display.
};
struct State {
  Mode mode=Mode::N;
  uint8_t limit=25, light=0, previousLight=0, meterPage=0;
  bool safe=true, flash=false, horn=false, cruise=false;
  Turn turn=Turn::Off;
  uint8_t faults=0;
  double totalMeters=0, tripMeters=0, usedWh=0, chargedWh=0;
  float maxSpeedKmh=0;
  void setSafe(bool v) { safe=v; if(safe && (limit>50 || limit==0)) limit=50; }
  void setLimit(uint8_t v) {
    if(v==0 || v==25 || v==50 || v==70) { limit=v; setSafe(safe); }
  }
  void turnKey(Turn v) { turn=(turn==v ? Turn::Off : v); }
  void lightClick() { if(light==3) light=previousLight; else light=(light+1)%3; }
  void highBeam() { if(light!=3) {previousLight=light; light=3;} }
  bool lightGesture(Gesture gesture) {
    if(gesture==Gesture::Click) lightClick();
    else if(gesture==Gesture::Double) {light=2;previousLight=2;}
    else if(gesture==Gesture::Hold) highBeam();
    else return false;
    return true;
  }
  uint8_t outputs(uint32_t now,const Config& c) const {
    uint8_t v=light==0?0:light==1?1:light==2?3:7;
    if(flash) v|=4;
    if(horn) v|=8;
    if(now%(c.blinkOnMs+c.blinkOffMs)<c.blinkOnMs) {
      if(turn==Turn::Left || turn==Turn::Hazard) v|=32;
      if(turn==Turn::Right || turn==Turn::Hazard) v|=16;
    }
    // ADS order differs from MCP bit order; brake has no output bit.
    static constexpr int8_t bits[7]={0,1,2,5,4,-1,3};
    for(int i=0;i<7;i++) if((faults&(1<<i)) && bits[i]>=0) v&=~(1<<bits[i]);
    return v & 0x3f;
  }
};
struct Telemetry {
  float fet=0,motor=0,current=0,inputCurrent=0,speedMs=0,voltage=0,soc=0;
  float erpm=0;
  float wh=0,chargedWh=0,distanceAbs=0;
  uint32_t odometer=0, uptime=0, receivedMs=0;
  uint8_t fault=0;
  bool valid=false;
  bool fresh(uint32_t now,uint32_t timeout=1200)const {return valid && uint32_t(now-receivedMs)<timeout;}
};
struct Event { Key key; Gesture gesture; };
using EventSink = void(*)(Event,void*);
class Button {
  bool raw_=false, stable_=false, held_=false, pending_=false;
  uint32_t changed_=0, pressed_=0, released_=0;
public:
  bool pressed()const{return stable_;}
  void cancel(Key key,EventSink sink,void* ctx) {
    if(stable_ || pending_) sink({key,Gesture::Cancel},ctx);
    raw_=stable_=held_=pending_=false;
  }
  void tick(bool pressed,uint32_t now,Key key,const Config& c,EventSink sink,void* ctx) {
    if(pressed!=raw_) {raw_=pressed; changed_=now;}
    if(raw_!=stable_ && uint32_t(now-changed_)>=c.debounceMs) {
      stable_=raw_;
      if(stable_) {pressed_=now;held_=false;sink({key,Gesture::Down},ctx);}
      else {
        sink({key,Gesture::Up},ctx);
        if(!held_) {
          if(pending_ && uint32_t(now-released_)<=c.doubleMs) {
            pending_=false;sink({key,Gesture::Double},ctx);
          } else {pending_=true;released_=now;}
        }
      }
    }
    if(stable_ && !held_ && uint32_t(now-pressed_)>=c.holdMs) {
      held_=true;pending_=false;sink({key,Gesture::Hold},ctx);
    }
    if(pending_ && !stable_ && uint32_t(now-released_)>c.doubleMs) {
      pending_=false;sink({key,Gesture::Click},ctx);
    }
  }
};

// Reject values compatible with a chord; never turn a chord into a single key.
inline Key decodeResistance(float ohms,const Config& c,float tolerance=0.025f) {
  if(!isfinite(ohms) || ohms<50) return Key::Invalid;
  if(ohms>100000) return Key::None;
  int candidate=-1;
  for(int i=0;i<5;i++) if(fabsf(ohms-c.keyOhms[i])/c.keyOhms[i]<tolerance) {
    if(candidate!=-1) return Key::Invalid;
    candidate=i;
  }
  if(candidate<0) return Key::Invalid;
  for(int mask=1;mask<32;mask++) {
    if(!(mask&(mask-1))) continue;
    float g=0;
    for(int j=0;j<5;j++) if(mask&(1<<j)) g+=1/c.keyOhms[j];
    float equivalent=1/g;
    if(fabsf(ohms-equivalent)/equivalent<tolerance) return Key::Invalid;
  }
  return static_cast<Key>(candidate+1);
}

// A calibrated table avoids assuming the weak MOS pull-up is a linear resistor.
// Combinations are unsupported: electrically overlapping chords cannot be
// distinguished from single buttons by one analog wire.
inline Key decodeKeyVoltage(uint16_t mv,const Config& c) {
  int candidate=-1;
  for(int i=0;i<5;i++) {
    float tolerance=std::max(3.0f,c.keyMv[i]*0.025f);
    if(fabsf(float(mv)-c.keyMv[i])<=tolerance) {
      if(candidate>=0)return Key::Invalid;
      candidate=i;
    }
  }
  return candidate<0?Key::Invalid:Key(candidate+1);
}
// Convert a calibrated voltage to an approximate resistance for diagnostics.
inline float calibratedResistance(uint16_t mv,const Config& c) {
  int order[5]={0,1,2,3,4};
  std::sort(order,order+5,[&](int a,int b){return c.keyMv[a]<c.keyMv[b];});
  float x0=0,y0=0,x1=c.keyMv[order[0]],y1=c.keyOhms[order[0]];
  for(int i=1;i<5&&mv>x1;i++) {
    x0=x1;y0=y1;x1=c.keyMv[order[i]];y1=c.keyOhms[order[i]];
  }
  return x1>x0?y0+(mv-x0)*(y1-y0)/(x1-x0):INFINITY;
}

uint16_t crc16(const uint8_t*,size_t);
uint32_t crc32(const uint8_t*,size_t);
size_t frame(const uint8_t*,size_t,uint8_t*,size_t);
class PacketParser {
  uint8_t data_[1024]{};
  size_t pos_=0,length_=0;
  uint8_t state_=0;
  uint16_t crc_=0;
  uint32_t last_=0;
public:
  uint32_t errors=0;
  using Sink=void(*)(const uint8_t*,size_t,void*);
  void feed(uint8_t,uint32_t,Sink,void*);
};
constexpr uint32_t setupMask=(1u<<0)|(1u<<1)|(1u<<2)|(1u<<3)|(1u<<5)|(1u<<6)|(1u<<7)|
 (1u<<8)|(1u<<11)|(1u<<12)|(1u<<14)|(1u<<16)|(1u<<20)|(1u<<21);
bool parseSetup(const uint8_t*,size_t,uint32_t,Telemetry&);
void put32(uint8_t*,uint32_t);
uint32_t get32(const uint8_t*);
float getFloat(const uint8_t*);
void putFloat(uint8_t*,float);
}

