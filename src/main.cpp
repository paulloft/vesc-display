#include <Arduino.h>
#include <ArduinoJson.h>
#include "Board.h"
#include "Core.h"
#include "Devices.h"
#include "Storage.h"
#include "Ui.h"
#include "Vesc.h"
#include "Network.h"
#include <WiFi.h>

namespace {
bike::Config config;
bike::State state;
Keys keys;
Loads loads;
Storage storage;
Ui ui;
Vesc vesc;
Network network;
bool uiReady=false,previousFresh=false;
bool previousApplied=false;
uint32_t lastUi=0,lastSample=0;
float previousDistance=0,previousWh=0,previousChargeWh=0;
uint32_t previousUptime=0;
uint32_t lightPressedAt=0,lightHeldAfterMs=0,lightPresses=0,lightCancels=0;
String notice;
bool stationary();
bool applyConfig(const String&,String&);

void notify(const String& text){notice=text;Serial.println(text);if(uiReady)ui.notice(text);}
void setMode(bike::Mode mode) {
  if(vesc.request(mode,state.limit,config)) {state.mode=mode;state.cruise=false;notify("Применение режима…");}
  else notify(vesc.message);
}
void setLimit(uint8_t value) {
  if(state.safe&&(value>50||value==0))return;
  if(vesc.request(state.mode,value,config)) {state.setLimit(value);storage.save(config,state,true);notify("Применение лимита…");}
  else notify(vesc.message);
}
void action(UiAction a) {
  if(network.updating)return;
  if(a==UiAction::ToggleControl) {
    if(!stationary()){notify("Остановитесь и выберите N; нужна связь с VESC");return;}
    String result;
    applyConfig(config.motionEnabled?"{\"motion_enabled\":false}":"{\"motion_enabled\":true}",result);
    notify(result);return;
  }
  if(a==UiAction::Neutral)setMode(bike::Mode::N);
  if(a==UiAction::Drive)setMode(bike::Mode::Drive);
  if(a==UiAction::Sport)setMode(bike::Mode::Sport);
  if(a>=UiAction::Limit25&&a<=UiAction::LimitMax) {const uint8_t values[]={25,50,70,0};setLimit(values[int(a)-3]);}
  if(a==UiAction::Hazard)state.turnKey(bike::Turn::Hazard);
  if(a==UiAction::Meter){state.meterPage=(state.meterPage+1)%3;storage.save(config,state,true);}
  if(a==UiAction::Save) {
    bool saved=storage.save(config,state,true);
    notify(saved?"Настройки сохранены на SD":"Настройки применены в RAM; SD недоступна");
  }
}
void keyEvent(bike::Event e,void*) {
  if(network.updating)return;
  using bike::Key;using bike::Gesture;
  if(e.key==Key::Horn) {
    if(e.gesture==Gesture::Down)state.horn=true;
    if(e.gesture==Gesture::Up||e.gesture==Gesture::Cancel)state.horn=false;
    return;
  }
  if(e.key==Key::Power) {
    if(e.gesture==Gesture::Down)state.flash=true;
    if(e.gesture==Gesture::Up||e.gesture==Gesture::Cancel)state.flash=false;
    return;
  }
  if(e.key==Key::Left&&e.gesture==Gesture::Down)state.turnKey(bike::Turn::Left);
  if(e.key==Key::Right&&e.gesture==Gesture::Down)state.turnKey(bike::Turn::Right);
  if(e.key==Key::Light) {
    if(e.gesture==Gesture::Down){lightPressedAt=millis();lightPresses++;}
    if(e.gesture==Gesture::Cancel)lightCancels++;
    if(e.gesture==Gesture::Hold){lightHeldAfterMs=millis()-lightPressedAt;Serial.printf("Light HOLD after %lu ms\n",static_cast<unsigned long>(lightHeldAfterMs));}
    if(state.lightGesture(e.gesture))storage.save(config,state,true);
  }
  if(e.key==Key::M&&e.gesture==Gesture::Click)setMode(static_cast<bike::Mode>((int(state.mode)+1)%3));
  if(e.key==Key::M&&e.gesture==Gesture::Double) {
    state.setSafe(!state.safe);
    if(vesc.ready()&&!vesc.request(state.mode,state.limit,config))setMode(bike::Mode::N);
    storage.save(config,state,true);notify(state.safe?"Безопасный режим включён":"Безопасный режим выключен");
  }
  if(e.key==Key::Cruise&&e.gesture==Gesture::Down) {
    if(!vesc.cruise(!vesc.cruiseActive))notify(vesc.message);
  }
}
String statusJson() {
  JsonDocument d;const auto& t=vesc.telemetry;bool fresh=t.fresh(millis());
  d["wifi_ap_ok"]=network.online;d["wifi_clients"]=WiFi.softAPgetStationNum();
  d["heap_free"]=ESP.getFreeHeap();
  d["vesc_connected"]=fresh;d["firmware"]=String(vesc.fwMajor)+"."+vesc.fwMinor;
  d["uart_baud"]=vesc.baud();
  d["uart_rx_bytes"]=vesc.rxBytes;d["vesc_packets"]=vesc.goodPackets;
  d["vesc_crc_errors"]=vesc.crcErrors();d["vesc_bad_payloads"]=vesc.badPayloads;
  if(t.valid)d["vesc_telemetry_age_ms"]=uint32_t(millis()-t.receivedMs);
  if(fresh) {d["speed_kmh"]=fabsf(t.speedMs)*3.6f;d["battery_percent"]=t.soc*100;d["motor_c"]=t.motor;d["controller_c"]=t.fet;}
  d["mode_requested"]=int(state.mode);d["mode_confirmed"]=vesc.applied();d["limit_kmh"]=state.limit;
  d["vesc_applying"]=vesc.applying();d["vesc_apply_failed"]=vesc.applicationFailed;
  d["vesc_limit_confirmed"]=vesc.limitConfirmed;d["vesc_ramp_confirmed"]=vesc.rampConfirmed;
  d["vesc_repl_reply"]=vesc.lastReplReply;d["vesc_message"]=vesc.message;
  auto baseline=d["vesc_baseline"].to<JsonArray>();for(size_t i=0;i<10;i++)baseline.add(vesc.baselineValue(i));
  d["vesc_erpm_per_m_s"]=vesc.speedFactor();d["vesc_target_erpm"]=vesc.targetErpm();
  d["vesc_geometry_erpm_per_m_s"]=vesc.geometrySpeedFactor();
  d["vesc_erpm"]=t.erpm;d["vesc_throttle"]=vesc.throttle;d["vesc_brake"]=vesc.brake;
  d["vesc_actual_max_erpm"]=vesc.actualMaxErpm;d["vesc_actual_drive_scale"]=vesc.actualDriveScale;
  d["vesc_baseline_trusted"]=vesc.baselineTrusted();
  d["vesc_controller_id"]=vesc.controllerId();
  d["safe"]=state.safe;d["cruise"]=vesc.cruiseActive;d["sd"]=storage.mounted&&storage.healthy;
  d["io"]=loads.online;d["adc_online"]=loads.adcOnline;d["key_mv"]=keys.millivolts;
  d["key_raw_mv"]=keys.rawMillivolts;d["key_noise_mv"]=keys.noiseMillivolts;
  d["light_hold_after_ms"]=lightHeldAfterMs;d["light_down_count"]=lightPresses;d["light_cancel_count"]=lightCancels;
  d["key_decoded"]=int(keys.decoded);d["key_io9_high"]=keys.pullupDigital;
  d["key_pullup_test_down_mv"]=keys.pullupDownMv;d["key_pullup_test_up_mv"]=keys.pullupUpMv;
  if(isfinite(keys.resistance))d["key_ohms"]=keys.resistance;
  d["brake"]=loads.brake||vesc.brake>0.02f;d["load_fault_mask"]=state.faults;
  auto raw=d["diag_raw"].to<JsonArray>();for(auto v:loads.raw)raw.add(v);
  d["trip_meters"]=state.tripMeters;d["trip_seconds"]=millis()/1000;d["total_meters"]=state.totalMeters;
  d["trip_max_kmh"]=state.maxSpeedKmh;d["trip_used_wh"]=state.usedWh;d["trip_charge_wh"]=state.chargedWh;
  auto fields=d["settings_fields"].to<JsonArray>();
  for(const auto& setting:timingSettings) {
    auto field=fields.add<JsonObject>();field["key"]=setting.key;field["label"]=setting.label;
    field["min"]=setting.min;field["max"]=setting.max;field["step"]=setting.step;
    field["unit"]="мс";
  }
  auto pullup=fields.add<JsonObject>();pullup["key"]="key_pullup_ohms";
  pullup["label"]="Подтяжка пульта";pullup["unit"]="Ом";
  pullup["min"]=10000;pullup["max"]=100000;pullup["step"]=100;
  auto flip=fields.add<JsonObject>();flip["key"]="screen_flipped";flip["label"]="Перевернуть экран";flip["type"]="boolean";
  auto control=fields.add<JsonObject>();control["key"]="motion_enabled";control["label"]="Управление VESC (режимы и лимиты)";control["type"]="boolean";
  encodeConfig(d["config"].to<JsonObject>(),config);String out;serializeJson(d,out);return out;
}
bool stationary() {
  return state.mode==bike::Mode::N&&!vesc.cruiseActive&&vesc.telemetry.fresh(millis())&&fabsf(vesc.telemetry.speedMs)<0.1f;
}
bool applyConfig(const String& json,String& result) {
  JsonDocument d;if(deserializeJson(d,json)){result="Некорректный JSON";return false;}
  bike::Config next=config;if(!decodeConfig(d.as<JsonVariantConst>(),next,result))return false;
  bool wasEnabled=config.motionEnabled;
  config=next;vesc.controlEnabled=config.motionEnabled;vesc.cruiseEnabled=config.cruiseEnabled;
  if(wasEnabled&&!config.motionEnabled) {
    // Send neutral while control is still enabled, then disable commands.
    vesc.controlEnabled=true;setMode(bike::Mode::N);vesc.controlEnabled=false;
  } else if(!wasEnabled&&config.motionEnabled)setMode(bike::Mode::N);
  if(!config.cruiseEnabled)vesc.cruise(false);
  bool saved=storage.save(config,state,true);result=saved?"Сохранено на SD":"Применено в RAM; SD недоступна";
  if(uiReady)ui.show(1);return true;
}
// Nonblocking USB settings line, using the same validation/storage as Wi-Fi.
// Bare 'd' still returns status. A JSON object followed by newline applies settings.
void consoleTick() {
  static char line[1536];static size_t used=0;static bool overflow=false;
  int budget=128;
  while(Serial.available()&&budget-->0) {
    char ch=Serial.read();
    if(!used&&!overflow) {
      if(ch=='d'){Serial.println(statusJson());continue;}
      if(ch!='{')continue;
    }
    if(ch=='\r')continue;
    if(ch=='\n') {
      line[used]=0;String result;
      JsonDocument candidate;
      bool valid=!overflow&&!deserializeJson(candidate,line);
      bool allowed=stationary()||(state.mode==bike::Mode::N&&!config.motionEnabled&&!config.cruiseEnabled&&
        !candidate["motion_enabled"].as<bool>()&&!candidate["cruise_enabled"].as<bool>());
      if(!valid)Serial.println("USB config: invalid JSON/length");
      else if(!allowed||network.updating)Serial.println("USB config: requires stopped N");
      else {bool ok=applyConfig(String(line),result);Serial.println(String(ok?"USB config OK: ":"USB config ERROR: ")+result);}
      used=0;overflow=false;continue;
    }
    if(used<sizeof(line)-1)line[used++]=ch;else overflow=true;
  }
}
bool otaReady(){return stationary()&&vesc.applied();}
void prepareUpdate(){vesc.cruise(false);state.horn=false;state.flash=false;loads.off();storage.save(config,state,true);notify("Обновление ESP32…");}
void updateTrip(uint32_t now) {
  const auto& t=vesc.telemetry;bool fresh=t.fresh(now);
  if(!fresh) {
    if(previousFresh) {state.mode=bike::Mode::N;state.cruise=false;notify("Нет связи с VESC");}
    previousFresh=false;return;
  }
  previousFresh=true;
  if(lastSample==t.receivedMs)return;
  bool reboot=lastSample&&t.uptime<previousUptime&&uint32_t(previousUptime-t.uptime)<0x80000000u;
  if(lastSample&&!reboot) {
    float meters=t.distanceAbs-previousDistance;
    if(meters>=0&&meters<1000)state.tripMeters+=meters;
    float wh=t.wh-previousWh,charged=t.chargedWh-previousChargeWh;
    if(wh>=0&&wh<1000)state.usedWh+=wh;
    if(charged>=0&&charged<1000)state.chargedWh+=charged;
  }
  if(reboot) {state.mode=bike::Mode::N;vesc.cruise(false);notify("Контроллер перезапущен");}
  state.totalMeters=std::max(state.totalMeters,double(t.odometer));
  state.maxSpeedKmh=std::max(state.maxSpeedKmh,fabsf(t.speedMs)*3.6f);
  previousDistance=t.distanceAbs;previousWh=t.wh;previousChargeWh=t.chargedWh;previousUptime=t.uptime;lastSample=t.receivedMs;
}
}
void setup() {
  Serial.begin(115200);
  loads.begin();storage.begin(config,state);
  vesc.controlEnabled=config.motionEnabled;vesc.cruiseEnabled=config.cruiseEnabled;vesc.startupLimit(state.limit);vesc.begin(board::uartBaud,&config);
  network.begin(statusJson,applyConfig,stationary,otaReady,prepareUpdate,storage.mounted);
  uiReady=ui.begin(config,action);
  // Configure the RTC ADC last: later pinMode/RTC deinit calls can disturb it
  // with the bundled ESP-IDF GPIO driver.
  keys.begin();
  Serial.printf("Wi-Fi: %s\nPassword: %s\nHTTP: http://192.168.4.1 user=admin\n",network.ssid.c_str(),network.password().c_str());
  notify(uiReady?(config.motionEnabled?"Ожидание подтверждения N":"Телеметрия; команды мотору выключены"):"Ошибка LCD/PSRAM; диагностика по USB/Wi-Fi");
}
void loop() {
  static uint32_t previousLoop=0,maxLoop=0,reportAt=0,maxUi=0;
  uint32_t now=millis();
  if(previousLoop)maxLoop=std::max(maxLoop,now-previousLoop);previousLoop=now;
  if(now-reportAt>=5000) {
    Serial.printf("PERF loop_max=%lu ms ui_max=%lu ms heap=%u min_heap=%u AP=%s clients=%u\n",
      (unsigned long)maxLoop,(unsigned long)maxUi,unsigned(ESP.getFreeHeap()),unsigned(ESP.getMinFreeHeap()),
      network.online?"OK":"FAIL",unsigned(WiFi.softAPgetStationNum()));
    maxLoop=maxUi=0;reportAt=now;
  }
  consoleTick();
  vesc.tick(now);
  if(vesc.baselineChanged){vesc.baselineChanged=false;storage.save(config,state,true);}
  if(!network.updating) {
    keys.tick(now,config,keyEvent,nullptr);loads.tick(state,config,now);
    if(loads.brake)vesc.cruise(false);
    state.cruise=vesc.cruiseActive;updateTrip(now);storage.save(config,state);
    bool applied=vesc.applied()&&vesc.telemetry.fresh(now);
    if(applied!=previousApplied) {
      previousApplied=applied;
      notify(applied?"Режим и лимит подтверждены VESC":(config.motionEnabled?"Режим VESC не подтверждён":"Телеметрия; команды мотору выключены"));
    }
    if(vesc.applicationFailed&&notice!=vesc.message)notify(vesc.message);
  }
  if(uiReady) {
    if(now-lastUi>=100) {
      lastUi=now;
      String d="VESC "+String(vesc.fwMajor)+"."+vesc.fwMinor+"; "+vesc.baud()+" бод (фикс.)\nRX байт "+vesc.rxBytes+"; пакетов "+vesc.goodPackets+"; CRC "+vesc.crcErrors()+"; формат "+vesc.badPayloads+
       "\nГаз "+String(vesc.throttle,3)+"; тормоз "+String(vesc.brake,3)+
       "\nУправление: "+(vesc.applied()?String("подтверждено"):String("не подтверждено"))+
       "\nПульт: "+keys.millivolts+" мВ / "+String(keys.resistance,0)+" Ом"+
       "\nАЦП raw: "+keys.rawMillivolts+" мВ; разброс: "+keys.noiseMillivolts+" мВ"+
       "\nПодтяжка DOWN/UP: "+keys.pullupDownMv+" / "+keys.pullupUpMv+" мВ\nDIAG: ";
      for(int i=0;i<8;i++)d+=String(loads.raw[i])+" ";
      d+="\nОшибки: "+String(state.faults,HEX)+"; I2C: "+loads.errors+
        "\nWi-Fi: "+network.ssid+"\nПароль / admin: "+network.password()+"\nhttp://192.168.4.1\n"+storage.error;
      ui.update(state,vesc.telemetry,now,loads.online&&loads.adcOnline,storage.mounted&&storage.healthy,loads.brake||vesc.brake>0.02f,d,vesc.applied(),vesc.applying());
    }
    uint32_t renderStart=millis();ui.tick();maxUi=std::max(maxUi,uint32_t(millis()-renderStart));
  }
  network.tick();delay(1);
}
