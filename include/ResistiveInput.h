#pragma once
#include "Core.h"

namespace bike {
inline uint16_t trimmedAdcMean(uint16_t (&values)[9]) {
  std::sort(values,values+9);uint32_t sum=0;
  for(int i=2;i<7;i++)sum+=values[i];
  return uint16_t((sum+2)/5);
}
// Median across 25 ms rejects whole-burst spikes as well as individual samples.
// Invalid readings need to persist before cancelling an active button; a single
// noisy conversion must not drop a horn/light hold or a pending double click.
class ResistiveInput {
  struct Sample {uint16_t mv;bool released;};
  Sample history_[5]{};
  unsigned next_=0;
  bool seeded_=false,invalidTiming_=false;
  uint32_t invalidAt_=0;
  Button button_;
  Key current_=Key::None;
public:
  uint16_t rawMv=0,filteredMv=0,noiseMv=0;
  float resistance=INFINITY;
  Key decoded=Key::None;
  void sample(uint16_t mv,bool released,uint16_t noise,uint32_t now,
              const Config& config,EventSink sink,void* context) {
    rawMv=mv;noiseMv=noise;
    if(!seeded_){for(auto& value:history_)value={mv,released};seeded_=true;}
    history_[next_]={mv,released};next_=(next_+1)%5;
    Sample sorted[5];std::copy(history_,history_+5,sorted);
    std::sort(sorted,sorted+5,[](Sample a,Sample b){return a.mv<b.mv;});
    filteredMv=sorted[2].mv;
    resistance=sorted[2].released?INFINITY:
      (filteredMv<3300?float(config.keyPullupOhms)*filteredMv/(3300.0f-filteredMv):INFINITY);
    if(config.keyMv[0]&&!sorted[2].released)resistance=calibratedResistance(filteredMv,config);
    decoded=sorted[2].released?Key::None:config.keyMv[0]?decodeKeyVoltage(filteredMv,config):decodeResistance(resistance,config);
    // Schmitt window: only a button whose Down was already debounced may use
    // the wider window. Release/another valid key still takes effect normally.
    if(decoded==Key::Invalid&&config.keyMv[0]&&button_.pressed()&&
       current_>=Key::Light&&current_<=Key::Horn) {
      auto centre=config.keyMv[int(current_)-1];
      if(fabsf(float(filteredMv)-centre)<=std::max(5.0f,centre*0.05f))decoded=current_;
    }
    Key effective=decoded;
    if(decoded==Key::Invalid) {
      if(!invalidTiming_){invalidTiming_=true;invalidAt_=now;}
      if(uint32_t(now-invalidAt_)>=20) {
        button_.cancel(current_,sink,context);current_=Key::None;return;
      }
      effective=current_;
    } else invalidTiming_=false;
    if(effective!=current_&&effective!=Key::None) {
      if(current_!=Key::None)button_.cancel(current_,sink,context);
      current_=effective;
    }
    if(current_!=Key::None)button_.tick(effective==current_,now,current_,config,sink,context);
  }
};
}
