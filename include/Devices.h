#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "Core.h"
#include "ResistiveInput.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

class Loads {
  TwoWire bus_{1};
  bool initialized_=false;
  uint8_t sent_=0;
  uint32_t poll_=0,retry_=0,rawAt_[8]{},onAt_[7]{},faultAt_[7]{};
  bool faultTiming_[7]{};
  uint8_t channel_=0;
  bool write(uint8_t reg,uint8_t value);
public:
  bool online=false,adcOnline=false,brake=false;
  uint8_t raw[8]{};
  uint32_t errors=0;
  void begin();
  void tick(bike::State&,const bike::Config&,uint32_t);
  void off();
  uint32_t age(int i,uint32_t now)const{return now-rawAt_[i];}
};

class Keys {
  QueueHandle_t events_=nullptr;
  portMUX_TYPE configLock_=portMUX_INITIALIZER_UNLOCKED;
  bike::Config sampledConfig_;
  static void worker(void*);
  static void enqueue(bike::Event,void*);
  void sample(uint32_t,const bike::Config&,bike::EventSink,void*);
  bike::ResistiveInput ladder_;
  bike::Button powerButton_,modeButton_;
  uint32_t last_=0;
public:
  uint32_t millivolts=0,rawMillivolts=0,noiseMillivolts=0;
  uint32_t pullupDownMv=0,pullupUpMv=0;
  bool pullupDigital=false;
  float resistance=INFINITY;
  bike::Key decoded=bike::Key::None;
  void begin();
  void tick(uint32_t,const bike::Config&,bike::EventSink,void*);
};

