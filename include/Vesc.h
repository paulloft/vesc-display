#pragma once
#include <Arduino.h>
#include "Core.h"

class Vesc {
  HardwareSerial serial_{1};
  bike::PacketParser parser_;
  uint32_t poll_=0, versionPoll_=0, limitSent_=0, replSent_=0;
  bool wasFresh_=false, haveLimits_=false, pendingLimit_=false;
  bool initializedControl_=false;
  uint32_t rampToken_=0;
  float baseline_[10]{};
  float erpmPerMs_=0;
  uint8_t wantedLimit_=25;
  bike::Mode wantedMode_=bike::Mode::N;
  bike::Mode appliedMode_=bike::Mode::N;
  float ramp_=1;
  bool rampPending_=false;
  bool ccWanted_=false,ccStarting_=false,ccCancel_=false;
  uint32_t ccReply_=0,adcAt_=0;
  float targetErpm_=0;
  uint32_t baud_=115200;
  uint32_t receiveAt_=0;
  uint32_t applicationAt_=0;
  bike::Config* config_=nullptr;
  uint32_t controllerId_=0;
  bool controllerRestarted_=false,baselineTrusted_=false;
  static void receive(const uint8_t*,size_t,void*);
  void packet(const uint8_t*,size_t);
  void send(const uint8_t*,size_t);
  void applyLimits();
  void repl(const char*);
public:
  bike::Telemetry telemetry;
  uint8_t fwMajor=0,fwMinor=0;
  uint32_t goodPackets=0, badPayloads=0;
  uint32_t rxBytes=0;
  bool applicationFailed=false;
  bool baselineChanged=false;
  String lastReplReply;
  bool controlEnabled=false, limitConfirmed=false, rampConfirmed=false;
  bool cruiseEnabled=false, cruiseActive=false;
  float throttle=0,brake=0;
  String message="Нет связи";
  void begin(uint32_t baud,bike::Config* config=nullptr);
  void startupLimit(uint8_t limit){wantedLimit_=limit;}
  void tick(uint32_t now);
  bool request(bike::Mode,uint8_t,const bike::Config&);
  bool ready()const{return controlEnabled&&haveLimits_&&fwMajor==7&&fwMinor==0;}
  bool applied()const{return ready()&&limitConfirmed&&rampConfirmed;}
  bool applying()const{return ready()&&!applicationFailed&&(pendingLimit_||rampPending_);}
  bool cruise(bool);
  uint32_t crcErrors()const{return parser_.errors;}
  uint32_t baud()const{return baud_;}
  float baselineValue(size_t index)const{return index<10?baseline_[index]:0;}
  float speedFactor()const{return config_&&controllerId_&&config_->vescBaselineId==controllerId_&&config_->speedErpmPerMs>0?config_->speedErpmPerMs:erpmPerMs_;}
  float geometrySpeedFactor()const{return erpmPerMs_;}
  float targetErpm()const{return wantedLimit_?fminf(baseline_[3],wantedLimit_/3.6f*speedFactor()):baseline_[3];}
  float actualMaxErpm=0,actualDriveScale=0;
  bool baselineTrusted()const{return baselineTrusted_;}
  uint32_t controllerId()const{return controllerId_;}
};

