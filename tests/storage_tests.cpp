#include "Storage.h"
#include <cassert>
#include <iostream>
static uint32_t now=100;
extern "C" uint32_t millis(void){return now;}
int main(){
  bike::Config config;bike::State state;Storage storage;storage.begin(config,state);
  assert(storage.mounted&&storage.healthy);
  state.safe=false;state.limit=70;state.light=3;state.previousLight=1;
  state.tripMeters=1200;state.totalMeters=40000;state.mode=bike::Mode::Sport;
  state.horn=true;state.cruise=true;state.faults=0x7f;state.turn=bike::Turn::Hazard;
  uint16_t centres[5]={217,338,1259,662,54};std::copy(centres,centres+5,config.keyMv);
  config.keyPullupOhms=51000;config.screenFlipped=true;
  config.vescBaselineId=123;config.vescBaselineMaxErpm=100000;config.vescBaselineDriveScale=.8f;
  config.speedErpmPerMs=1700;
  config.blinkOnMs=500;assert(storage.save(config,state,true));
  config.blinkOnMs=600;assert(storage.save(config,state,true));
  bike::Config restored;bike::State boot;Storage reboot;reboot.begin(restored,boot);
  assert(restored.blinkOnMs==600&&boot.limit==70&&!boot.safe&&boot.light==3&&boot.previousLight==1&&boot.totalMeters==40000);
  assert(restored.keyPullupOhms==51000&&restored.screenFlipped);
  assert(restored.vescBaselineId==123&&restored.vescBaselineMaxErpm==100000&&restored.vescBaselineDriveScale==.8f);
  assert(restored.speedErpmPerMs==1700);
  for(int i=0;i<5;i++)assert(restored.keyMv[i]==centres[i]);
  assert(boot.tripMeters==0&&boot.mode==bike::Mode::N&&!boot.horn&&!boot.cruise&&boot.faults==0&&boot.turn==bike::Turn::Off);
  // Simulate a partial write to the newer slot: the previous valid snapshot wins.
  *SD.files["/state-b.json"]="{\"version\":1,";
  bike::Config recovered;bike::State recoveredState;Storage interrupted;interrupted.begin(recovered,recoveredState);
  assert(recovered.blinkOnMs==500&&recoveredState.totalMeters==40000);
  JsonDocument bad;bad["blink_on_ms"]=0;String error;
  assert(!decodeConfig(bad.as<JsonVariantConst>(),recovered,error)&&recovered.blinkOnMs==500);
  bad.clear();bad["screen_flipped"]="true";assert(!decodeConfig(bad.as<JsonVariantConst>(),recovered,error));
  bad.clear();bad["motion_enabled"]="true";assert(!decodeConfig(bad.as<JsonVariantConst>(),recovered,error));
  for(float factor:{-1.0f,.5f,1000001.0f}) {
    bad.clear();bad["speed_erpm_per_m_s"]=factor;
    assert(!decodeConfig(bad.as<JsonVariantConst>(),recovered,error)&&recovered.speedErpmPerMs==1700);
  }
  bad.clear();bad["speed_erpm_per_m_s"]=0;
  assert(decodeConfig(bad.as<JsonVariantConst>(),recovered,error)&&recovered.speedErpmPerMs==0);
  for(int value:{0,4700,100001}) {
    bad.clear();bad["key_pullup_ohms"]=value;
    assert(!decodeConfig(bad.as<JsonVariantConst>(),recovered,error)&&recovered.keyPullupOhms==51000);
  }
  bad.clear();bad["key_pullup_ohms"]=48000;
  assert(decodeConfig(bad.as<JsonVariantConst>(),recovered,error)&&recovered.keyPullupOhms==48000);
  bad.clear();bad["blink_on_ms"]=750;assert(decodeConfig(bad.as<JsonVariantConst>(),recovered,error)&&recovered.blinkOnMs==750);
  for(const char* invalid:{"[217,338,1259,662,0]","[217,218,1259,662,54]","[217,338,1259,662,1]","[217,338,1259,662,54.5]"}) {
    JsonDocument calibration;deserializeJson(calibration,invalid);bad.clear();bad["key_mv"]=calibration.as<JsonVariantConst>();
    assert(!decodeConfig(bad.as<JsonVariantConst>(),recovered,error)&&recovered.keyMv[0]==217);
  }
  bad.clear();bad["key_mv"].to<JsonArray>();for(int i=0;i<5;i++)bad["key_mv"].as<JsonArray>().add(0);
  assert(decodeConfig(bad.as<JsonVariantConst>(),recovered,error)&&recovered.keyMv[0]==0);
  SD.present=false;bike::Config defaults;bike::State initial;Storage absent;absent.begin(defaults,initial);
  assert(!absent.mounted&&!absent.save(defaults,initial,true)&&defaults.blinkOnMs==450&&initial.safe&&initial.limit==25);
  assert(defaults.keyPullupOhms==45000&&!defaults.screenFlipped);
  std::cout<<"PASS: SD snapshot recovery after partial write, restore only persistent state, config validation and no-card defaults\n";
}
