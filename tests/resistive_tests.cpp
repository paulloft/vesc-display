#include "ResistiveInput.h"
#include <cassert>
#include <vector>
#include <iostream>
using namespace bike;
static std::vector<Event> events;
static void collect(Event event,void*){events.push_back(event);}
static void operateLight(Event event,void* context){
  collect(event,nullptr);
  if(event.key==Key::Light)static_cast<State*>(context)->lightGesture(event.gesture);
}
static uint32_t sampleAt=0,holdAt=0;
static void timed(Event event,void* context){collect(event,context);if(event.gesture==Gesture::Hold)holdAt=sampleAt;}
static int count(Key key,Gesture gesture){int n=0;for(auto e:events)n+=e.key==key&&e.gesture==gesture;return n;}
static uint16_t voltage(float ohms){return uint16_t(lroundf(3300*ohms/(45000+ohms)));}
int main(){
  uint16_t burst[9]={1000,1000,0,4095,1000,1000,1000,0,4095};
  assert(trimmedAdcMean(burst)==1000);
  Config config;for(auto& mv:config.keyMv)mv=0; // Exercise legacy uncalibrated fallback too.
  for(int key=0;key<5;key++) {
    events.clear();ResistiveInput input;
    const auto wanted=Key(key+1);const auto mv=voltage(config.keyOhms[key]);
    for(uint32_t now=0;now<2700;now+=5) {
      bool pressed=now>=100&&now<2400;
      // Sporadic complete bad bursts and small conversion noise while held.
      int noise[]={-1,0,1,0,0};
      uint16_t sample=pressed?mv+noise[(now/5)%5]:1816;
      if(pressed&&(now/5)%17==0)sample=1500;
      input.sample(sample,!pressed,pressed?4:0,now,config,collect,nullptr);
    }
    assert(count(wanted,Gesture::Down)==1&&count(wanted,Gesture::Up)==1);
    assert(count(wanted,Gesture::Hold)==1&&count(wanted,Gesture::Cancel)==0);
  }
  events.clear();ResistiveInput doubleClick;
  for(uint32_t now=0;now<1000;now+=5) {
    bool pressed=(now>=100&&now<190)||(now>=270&&now<360);
    doubleClick.sample(pressed?voltage(3200):1816,!pressed,0,now,config,collect,nullptr);
  }
  assert(count(Key::Light,Gesture::Double)==1&&count(Key::Light,Gesture::Click)==0);
  events.clear();ResistiveInput chord;
  for(uint32_t now=0;now<500;now+=5) {
    bool pressed=now>=100; // A sustained ambiguous parallel combination.
    chord.sample(pressed?voltage(3322.15f):1816,!pressed,0,now,config,collect,nullptr);
  }
  assert(events.empty());
  events.clear();ResistiveInput cancel;
  for(uint32_t now=0;now<500;now+=5) {
    uint16_t mv=now<100?1816:now<250?voltage(1000):1500;
    cancel.sample(mv,now<100,0,now,config,collect,nullptr);
  }
  assert(count(Key::Horn,Gesture::Down)==1&&count(Key::Horn,Gesture::Cancel)==1);
  Config measured;
  uint16_t centres[5]={217,338,1259,662,54};std::copy(centres,centres+5,measured.keyMv);
  // Two quick physical presses must select low beam from every light mode.
  // Exercise the calibrated ADC/filter/debounce path and the production action.
  for(uint8_t initial=0;initial<=3;initial++) {
    events.clear();ResistiveInput input;State lights;lights.light=initial;
    for(uint32_t now=0;now<1000;now+=5) {
      bool pressed=(now>=100&&now<190)||(now>=270&&now<360);
      input.sample(pressed?217:1816,!pressed,0,now,measured,operateLight,&lights);
    }
    assert(count(Key::Light,Gesture::Double)==1&&count(Key::Light,Gesture::Click)==0);
    assert(lights.light==2&&lights.outputs(0,measured)==3);
    assert(lights.lightGesture(Gesture::Hold)&&lights.outputs(0,measured)==7);
    assert(lights.lightGesture(Gesture::Click)&&lights.outputs(0,measured)==3);
    assert(!lights.lightGesture(Gesture::Up));
  }
  for(int key=0;key<5;key++) {
    events.clear();ResistiveInput input;
    for(uint32_t now=0;now<2800;now+=5) {
      bool pressed=now>=100&&now<2400;
      int offsets[]={-1,0,1,0,0};
      uint16_t mv=pressed?centres[key]+offsets[(now/5)%5]:1816;
      if(pressed&&(now/5)%17==0)mv=1500;
      input.sample(mv,!pressed,40,now,measured,collect,nullptr);
    }
    assert(count(Key(key+1),Gesture::Down)==1&&count(Key(key+1),Gesture::Up)==1);
    assert(count(Key(key+1),Gesture::Hold)==1&&count(Key(key+1),Gesture::Cancel)==0);
    assert(decodeKeyVoltage(centres[key],measured)==Key(key+1));
    assert(fabsf(calibratedResistance(centres[key],measured)-measured.keyOhms[key])<0.1f);
  }
  assert(decodeKeyVoltage(1450,measured)==Key::Invalid);
  events.clear();ResistiveInput hysteresis;
  for(uint32_t now=0;now<2800;now+=5) {
    bool pressed=now>=100&&now<2400;
    // A real measured drift outside the entry window must not restart hold.
    uint16_t mv=!pressed?1816:now<300?217:209+(now/5)%3;
    sampleAt=now;hysteresis.sample(mv,!pressed,40,now,measured,timed,nullptr);
  }
  assert(count(Key::Light,Gesture::Down)==1&&count(Key::Light,Gesture::Hold)==1);
  assert(count(Key::Light,Gesture::Up)==1&&count(Key::Light,Gesture::Cancel)==0);
  assert(holdAt>=2100&&holdAt<=2200);
  // The wider hold window must not start a fresh press from an unknown level.
  events.clear();ResistiveInput strictEntry;
  for(uint32_t now=0;now<400;now+=5)strictEntry.sample(now<100?1816:209,now<100,0,now,measured,collect,nullptr);
  assert(events.empty());
  std::cout<<"PASS: noisy ladder holds/releases, isolated spikes, double click, sustained ambiguity cancellation\n";
}
