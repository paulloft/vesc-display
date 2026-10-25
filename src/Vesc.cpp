#include "Vesc.h"
#include "Board.h"
#include "CruiseProgram.h"

void Vesc::begin(uint32_t baud,bike::Config* config) {
  config_=config;
  // UART1 routed to P1; USB CDC remains the logging/programming interface.
  serial_.setRxBufferSize(2048);
  baud_=baud;serial_.begin(baud,SERIAL_8N1,board::vescRx,board::vescTx);
}
void Vesc::send(const uint8_t* p,size_t n) {
  uint8_t output[1030];size_t len=bike::frame(p,n,output,sizeof(output));
  if(len)serial_.write(output,len);
}
void Vesc::receive(const uint8_t* p,size_t n,void* self){static_cast<Vesc*>(self)->packet(p,n);}
void Vesc::packet(const uint8_t* p,size_t n) {
  if(!n)return;
  goodPackets++;
  if(p[0]==0 && n>=3) {
    fwMajor=p[1];fwMinor=p[2];
    size_t at=3;while(at<n&&p[at])at++;
    if(at<n&&n-at-1>=12)controllerId_=bike::crc32(p+at+1,12);
  }
  if(p[0]==51) {
    uint32_t before=telemetry.uptime;bool valid=telemetry.valid;
    if(!bike::parseSetup(p,n,receiveAt_,telemetry))badPayloads++;
    else if(valid&&telemetry.uptime<before&&uint32_t(before-telemetry.uptime)<0x80000000u) {
      cruise(false);haveLimits_=false;initializedControl_=false;limitConfirmed=false;rampConfirmed=false;
      fwMajor=fwMinor=0;pendingLimit_=rampPending_=false;wantedMode_=bike::Mode::N;
      controllerRestarted_=true;baselineTrusted_=false;
    }
  }
  if(p[0]==32&&n==17) {
    throttle=int32_t(bike::get32(p+1))/1000000.0f;
    brake=int32_t(bike::get32(p+9))/1000000.0f;adcAt_=millis();
  }
  if(p[0]==91&&n==50){actualDriveScale=bike::getFloat(p+5);actualMaxErpm=bike::getFloat(p+13);}
  if(p[0]==91 && n==50 && haveLimits_ && pendingLimit_) {
    float scale=bike::getFloat(p+5),maxErpm=bike::getFloat(p+13);
    float target=targetErpm();
    float wanted=wantedMode_==bike::Mode::N?0:baseline_[1];
    if(fabsf(scale-wanted)<0.0001f&&fabsf(maxErpm-target)<0.1f) {
      pendingLimit_=false;limitConfirmed=true;appliedMode_=wantedMode_;
    }
  }
  if(p[0]==91 && n==50 && !haveLimits_) {
    for(int i=0;i<10;i++)baseline_[i]=bike::getFloat(p+1+4*i);
    const int poles=p[41];const float gear=bike::getFloat(p+42), diameter=bike::getFloat(p+46);
    // A live TEMP reply may already contain our neutral/25 km/h limit after
    // an ESP-only reset. It is not the original controller configuration.
    bool cached=config_&&controllerId_&&config_->vescBaselineId==controllerId_&&
      config_->vescBaselineMaxErpm>0&&config_->vescBaselineDriveScale>0;
    bool cold=telemetry.valid&&telemetry.uptime<5000;
    bool capture=config_&&controllerId_&&(controllerRestarted_||(!cached&&cold));
    bool validSnapshot=poles>=2&&poles%2==0&&isfinite(gear)&&gear>0&&isfinite(diameter)&&diameter>0;
    for(float value:baseline_)validSnapshot&=isfinite(value);
    if(capture&&validSnapshot&&baseline_[1]>0&&baseline_[1]<=1&&baseline_[3]>0&&baseline_[3]<=10000000) {
      if(config_->vescBaselineId!=controllerId_)config_->speedErpmPerMs=0;
      config_->vescBaselineId=controllerId_;
      config_->vescBaselineMaxErpm=baseline_[3];config_->vescBaselineDriveScale=baseline_[1];
      baselineChanged=true;cached=true;controllerRestarted_=false;
    }
    baselineTrusted_=!config_||cached;
    if(cached){baseline_[3]=config_->vescBaselineMaxErpm;baseline_[1]=config_->vescBaselineDriveScale;}
    // Refuse to invent a previously configured current scale or wheel geometry.
    bool ok=baseline_[0]>=0&&baseline_[0]<=1&&baseline_[1]>0&&baseline_[1]<=1 &&
      poles>=2&&poles%2==0&&isfinite(gear)&&gear>0&&isfinite(diameter)&&diameter>0;
    for(float v:baseline_)ok&=isfinite(v);
    if(ok) {erpmPerMs_=(poles/2.0f)*60*gear/(diameter*PI);haveLimits_=true;}
    else message="Проверьте исходные лимиты VESC";
  }
  if(p[0]==48 && pendingLimit_) {
    const uint8_t readback=91;send(&readback,1);
  }
  if(p[0]==135 && n>1) { // COMM_LISP_PRINT; 138 is the outgoing REPL command.
    // A unique return token proves the conf-set returned true.
    String s;for(size_t i=1;i<n;i++)s+=char(p[i]);
    lastReplReply=s;
    if(rampPending_&&s.indexOf("EBIKE_RAMP_"+String(rampToken_)+"_OK")>=0) {rampPending_=false;rampConfirmed=true;}
    if(rampPending_&&s.indexOf("EBIKE_RAMP_FAIL")>=0) {
      applicationFailed=true;pendingLimit_=rampPending_=false;
      message="VESC отклонил настройку ускорения";
    }
    if(s.indexOf("EBIKE_CC_ON")>=0 && ccWanted_) {ccStarting_=false;cruiseActive=true;ccReply_=millis();}
    if(s.indexOf("EBIKE_CC_ALIVE")>=0 && ccWanted_)ccReply_=millis();
    if(s.indexOf("EBIKE_CC_OFF")>=0) {cruiseActive=false;ccWanted_=false;ccStarting_=false;ccCancel_=false;}
  }
}
bool Vesc::request(bike::Mode m,uint8_t limit,const bike::Config& cfg) {
  if(!controlEnabled) {message="Управление VESC выключено в настройках";return false;}
  if(!telemetry.fresh(millis())) {message="Нет свежих данных VESC: проверьте UART";return false;}
  if(!fwMajor) {message="Ожидается версия прошивки VESC";return false;}
  if(fwMajor!=7||fwMinor!=0) {message="Для управления нужна прошивка VESC 7.0";return false;}
  if(!haveLimits_) {message="Не прочитаны лимиты VESC: проверьте диагностику";return false;}
  if(m!=bike::Mode::N&&!baselineTrusted_) {message="Перезапустите Flipsky для чтения исходных лимитов";return false;}
  if(m!=bike::Mode::N && (millis()-adcAt_>1200||throttle>0.02f)) {message="Отпустите газ для смены режима";return false;}
  cruise(false);
  wantedMode_=m;wantedLimit_=limit;
  ramp_=(m==bike::Mode::Sport?cfg.sportRampSeconds:cfg.driveRampSeconds);
  rampToken_++;
  applicationAt_=millis();applicationFailed=false;
  limitConfirmed=false;rampConfirmed=(m==bike::Mode::N);rampPending_=m!=bike::Mode::N;
  applyLimits();
  return true;
}
void Vesc::repl(const char* expression) {
  size_t n=strlen(expression)+1;if(n>1023)return;
  uint8_t p[1024]={138};memcpy(p+1,expression,n);send(p,n+1);replSent_=millis();
}
bool Vesc::cruise(bool on) {
  if(!on) {
    if(cruiseActive||ccStarting_||ccWanted_)ccCancel_=true;
    cruiseActive=false;ccWanted_=false;return true;
  }
  if(!cruiseEnabled||!applied()||appliedMode_==bike::Mode::N||!telemetry.fresh(millis())||
     telemetry.fault||millis()-adcAt_>1200||brake>0.02f||telemetry.speedMs<1.4f||ccCancel_) {
    message="Круиз недоступен";return false;
  }
  targetErpm_=telemetry.speedMs*speedFactor();ccWanted_=true;ccStarting_=false;return true;
}
void Vesc::applyLimits() {
  // TEMP, store=false. Preserve regen, duty, watt and input-current limits.
  uint8_t p[45]={48,0,0,1,0};
  for(int i=0;i<10;i++)bike::putFloat(p+5+i*4,baseline_[i]);
  bike::putFloat(p+9,wantedMode_==bike::Mode::N?0.0f:baseline_[1]);
  const float maxErpm=targetErpm();
  bike::putFloat(p+17,maxErpm);
  send(p,sizeof(p));limitSent_=millis();pendingLimit_=true;
}
void Vesc::tick(uint32_t now) {
  if(!baselineTrusted_&&config_&&controllerId_&&config_->vescBaselineId==controllerId_&&
     config_->vescBaselineMaxErpm>0&&config_->vescBaselineDriveScale>0) {
    baseline_[3]=config_->vescBaselineMaxErpm;baseline_[1]=config_->vescBaselineDriveScale;
    baselineTrusted_=true;
  }
  // All packets consumed in this cycle use the same clock as freshness checks.
  receiveAt_=now;
  // Bound work so a noisy UART cannot starve buttons and load diagnostics.
  for(int n=0;n<2048&&serial_.available();n++){rxBytes++;parser_.feed(serial_.read(),now,receive,this);}
  if(now-versionPoll_>=1500 && (!fwMajor||!haveLimits_)) {
    versionPoll_=now;const uint8_t version=0;send(&version,1);
    if(fwMajor==7&&fwMinor==0&&!haveLimits_) {const uint8_t limits=91;send(&limits,1);}
  }
  if(now-poll_>=100) {poll_=now;uint8_t q[5]={51};bike::put32(q+1,bike::setupMask);send(q,5);const uint8_t adc=32;send(&adc,1);}
  const bool fresh=telemetry.fresh(now);
  if(!fresh) {
    if(wasFresh_) {cruise(false);limitConfirmed=false;rampConfirmed=false;message="Связь с VESC потеряна";}
  } else if(!wasFresh_) {
    message="Связь восстановлена; режим N";
    // Connection recovery never restores Drive or Sport automatically.
    if(ready()) {wantedMode_=bike::Mode::N;rampConfirmed=true;rampPending_=false;applicationAt_=now;applicationFailed=false;applyLimits();}
  }
  wasFresh_=fresh;
  if(!fresh||!ready())return;
  if(!initializedControl_) {
    initializedControl_=true;wantedMode_=bike::Mode::N;rampConfirmed=true;rampPending_=false;applicationAt_=now;applicationFailed=false;applyLimits();
  }
  if(applying()&&uint32_t(now-applicationAt_)>=5000) {
    message=pendingLimit_?"VESC не подтвердил лимит за 5 секунд":"VESC не подтвердил ускорение за 5 секунд";
    applicationFailed=true;pendingLimit_=rampPending_=false;
  }
  if(telemetry.fault||brake>0.02f)cruise(false);
  if(cruiseActive&&now-ccReply_>1600)cruise(false);
  if(pendingLimit_&&now-limitSent_>=700)applyLimits();
  if(now-replSent_<750)return;
  if(ccCancel_) {repl("(progn (def eb-on nil) (app-disable-output 0) \"EBIKE_CC_OFF\")");return;}
  if(rampPending_) {
    // Native REPL call changes only the live ADC ramp, without conf-store.
    // No uploaded controller application and no flash writes.
    char expression[160];
    snprintf(expression,sizeof(expression),"(if (conf-set 'adc-ramp-time-pos %.3f) \"EBIKE_RAMP_%lu_OK\" \"EBIKE_RAMP_FAIL\")",double(ramp_),static_cast<unsigned long>(rampToken_));
    repl(expression);return;
  }
  if(ccWanted_&&!ccStarting_&&!cruiseActive) {
    char program[1000];snprintf(program,sizeof(program),cruiseStart,double(targetErpm_));repl(program);ccStarting_=true;ccReply_=now;
  } else if(ccWanted_&&cruiseActive) {
    repl("(if eb-on (progn (setq eb-lease (systime)) \"EBIKE_CC_ALIVE\") \"EBIKE_CC_OFF\")");
  } else if(ccStarting_&&now-ccReply_>1600) {ccStarting_=false;ccWanted_=false;ccCancel_=true;message="Круиз: нет подтверждения";}
}
