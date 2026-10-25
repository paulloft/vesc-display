#include "Storage.h"
#include "Board.h"
#include "Settings.h"

// Keep the calibration parser separate from the large JSON settings parser:
// the bundled Xtensa GCC 8 otherwise exhausts float reload constraints at -Os.
static bool __attribute__((noinline)) decodeKeyCalibration(JsonVariantConst v,bike::Config& next,String& error) {
  if(!v["key_ohms"].isNull()) {
    auto a=v["key_ohms"].as<JsonArrayConst>();if(a.size()!=5){error="Нужно 5 сопротивлений";return false;}
    for(int i=0;i<5;i++) {float r=a[i].as<float>();if(!isfinite(r)||r<500||r>50000){error="Некорректное сопротивление";return false;}next.keyOhms[i]=r;}
  }
  if(!v["key_mv"].isNull()) {
    auto a=v["key_mv"].as<JsonArrayConst>();if(a.size()!=5){error="Нужно 5 напряжений";return false;}
    int zeroes=0;
    for(int i=0;i<5;i++) {
      if(!a[i].is<uint16_t>()){error="Напряжение должно быть целым числом мВ";return false;}
      uint16_t mv=a[i];zeroes+=mv==0;
      if(mv&&(mv<10||mv>1600)){error="Напряжение: 10–1600 мВ";return false;}
      next.keyMv[i]=mv;
    }
    if(zeroes&&zeroes!=5){error="Калибровка требует все 5 кнопок";return false;}
  }
  if(next.keyMv[0])for(int i=0;i<5;i++)for(int j=i+1;j<5;j++) {
      float gap=fabsf(float(next.keyMv[i])-next.keyMv[j]);
      float windows=std::max(3.0f,next.keyMv[i]*0.025f)+std::max(3.0f,next.keyMv[j]*0.025f);
      if(gap<=windows||(next.keyOhms[i]<next.keyOhms[j])!=(next.keyMv[i]<next.keyMv[j])) {
        error="Пересекающиеся или неверно упорядоченные уровни кнопок";return false;
      }
  }
  return true;
}

static bool __attribute__((noinline)) decodeVescBaseline(JsonVariantConst v,bike::Config& c,String& error) {
  if(!v["speed_erpm_per_m_s"].isNull()) {
    if(!v["speed_erpm_per_m_s"].is<float>()){error="Некорректный коэффициент скорости";return false;}
    float factor=v["speed_erpm_per_m_s"];
    if(!isfinite(factor)||(factor!=0&&(factor<1||factor>1000000))){error="Некорректный коэффициент скорости";return false;}
    c.speedErpmPerMs=factor;
  }
  if(!v["vesc_baseline_id"].isNull()) {
    if(!v["vesc_baseline_id"].is<uint32_t>()){error="Некорректный ID VESC";return false;}
    c.vescBaselineId=v["vesc_baseline_id"];
  }
  for(const char* key:{"vesc_baseline_max_erpm","vesc_baseline_drive_scale"})if(!v[key].isNull()&&!v[key].is<float>()){error="Некорректный исходный лимит VESC";return false;}
  if(!v["vesc_baseline_max_erpm"].isNull())c.vescBaselineMaxErpm=v["vesc_baseline_max_erpm"];
  if(!v["vesc_baseline_drive_scale"].isNull())c.vescBaselineDriveScale=v["vesc_baseline_drive_scale"];
  bool empty=c.vescBaselineId==0&&c.vescBaselineMaxErpm==0&&c.vescBaselineDriveScale==0;
  bool valid=c.vescBaselineId&&isfinite(c.vescBaselineMaxErpm)&&c.vescBaselineMaxErpm>0&&
    c.vescBaselineMaxErpm<=10000000&&isfinite(c.vescBaselineDriveScale)&&c.vescBaselineDriveScale>0&&c.vescBaselineDriveScale<=1;
  if(!empty&&!valid){error="Некорректные исходные лимиты VESC";return false;}
  return true;
}
bool decodeConfig(JsonVariantConst v,bike::Config& c,String& error) {
  if(!v.is<JsonObjectConst>()){error="Ожидается объект настроек";return false;}
  bike::Config next=c;
  auto integer=[&](const char* key,uint32_t& target,uint32_t min,uint32_t max) {
    if(v[key].isNull())return true;
    if(!v[key].is<uint32_t>()){error=String(key)+": требуется целое число";return false;}
    uint32_t x=v[key];if(x<min||x>max){error=String(key)+": вне диапазона";return false;}target=x;return true;
  };
  for(const auto& setting:timingSettings)if(!integer(setting.key,next.*(setting.field),setting.min,setting.max))return false;
  if(!integer("debounce_ms",next.debounceMs,10,100)||!integer("fault_grace_ms",next.faultGraceMs,20,1000)||
     !integer("fault_confirm_ms",next.faultConfirmMs,20,500))return false;
  if(next.doubleMs>=next.holdMs){error="Двойной клик должен быть короче удержания";return false;}
  if(!integer("key_pullup_ohms",next.keyPullupOhms,10000,100000))return false;
  if(!v["fault_threshold"].isNull()) {
    if(!v["fault_threshold"].is<int>()||v["fault_threshold"].as<int>()<1||v["fault_threshold"].as<int>()>254){error="Некорректный порог DIAG";return false;}
    next.faultThreshold=v["fault_threshold"];
  }
  auto real=[&](const char* key,float& x) {
    if(v[key].isNull())return true;
    if(!v[key].is<float>()){error=String(key)+": требуется число";return false;}
    float n=v[key];if(!isfinite(n)||n<0.05||n>5){error=String(key)+": допустимо 0.05–5 с";return false;}x=n;return true;
  };
  if(!real("drive_ramp_s",next.driveRampSeconds)||!real("sport_ramp_s",next.sportRampSeconds))return false;
  for(const char* key:{"motion_enabled","cruise_enabled","screen_flipped"})if(!v[key].isNull()&&!v[key].is<bool>()){error="Требуется true/false";return false;}
  if(!v["motion_enabled"].isNull())next.motionEnabled=v["motion_enabled"];
  if(!v["cruise_enabled"].isNull())next.cruiseEnabled=v["cruise_enabled"];
  if(!v["screen_flipped"].isNull())next.screenFlipped=v["screen_flipped"];
  if(!decodeKeyCalibration(v,next,error))return false;
  if(!decodeVescBaseline(v,next,error))return false;
  c=next;return true;
}
void encodeConfig(JsonObject o,const bike::Config& c) {
  o["blink_on_ms"]=c.blinkOnMs;o["blink_off_ms"]=c.blinkOffMs;
  o["hold_ms"]=c.holdMs;o["double_ms"]=c.doubleMs;o["debounce_ms"]=c.debounceMs;
  o["fault_grace_ms"]=c.faultGraceMs;o["fault_confirm_ms"]=c.faultConfirmMs;
  o["fault_threshold"]=c.faultThreshold;o["drive_ramp_s"]=c.driveRampSeconds;o["sport_ramp_s"]=c.sportRampSeconds;
  o["motion_enabled"]=c.motionEnabled;o["cruise_enabled"]=c.cruiseEnabled;
  o["vesc_baseline_id"]=c.vescBaselineId;o["vesc_baseline_max_erpm"]=c.vescBaselineMaxErpm;o["vesc_baseline_drive_scale"]=c.vescBaselineDriveScale;
  o["speed_erpm_per_m_s"]=c.speedErpmPerMs;
  o["key_pullup_ohms"]=c.keyPullupOhms;o["screen_flipped"]=c.screenFlipped;
  auto mv=o["key_mv"].to<JsonArray>();for(auto x:c.keyMv)mv.add(x);
  auto a=o["key_ohms"].to<JsonArray>();for(float x:c.keyOhms)a.add(x);
}
bool Storage::readSlot(const char* path,String& payload,uint32_t& sequence) {
  File f=SD.open(path,FILE_READ);if(!f)return false;
  if(f.size()>8192){f.close();return false;}
  JsonDocument d;auto e=deserializeJson(d,f);f.close();
  if(e||d["version"].as<int>()!=1||!d["payload"].is<String>()||!d["seq"].is<uint32_t>())return false;
  payload=d["payload"].as<String>();sequence=d["seq"];
  return bike::crc32(reinterpret_cast<const uint8_t*>(payload.c_str()),payload.length())==d["crc"].as<uint32_t>();
}
void Storage::begin(bike::Config& config,bike::State& state) {
  spi_.begin(board::sdSck,board::sdMiso,board::sdMosi,board::sdCs);
  mounted=SD.begin(board::sdCs,spi_,10000000);
  if(!mounted){error="Нет SD: настройки в RAM";return;}
  String payload[2];uint32_t seq[2]{};
  bool valid[2]={readSlot("/state-a.json",payload[0],seq[0]),readSlot("/state-b.json",payload[1],seq[1])};
  int index=valid[1]&&(!valid[0]||int32_t(seq[1]-seq[0])>0)?1:0;
  if(valid[index]) {
    JsonDocument d;String why;
    if(!deserializeJson(d,payload[index])&&decodeConfig(d["config"],config,why)) {
      sequence_=seq[index];
      state.safe=d["safe"]|true;state.setLimit(d["limit"]|uint8_t(25));
      state.light=std::min(uint8_t(3),d["light"]|uint8_t(0));
      state.previousLight=std::min(uint8_t(2),d["previous_light"]|uint8_t(0));
      state.meterPage=(d["meter_page"]|uint8_t(0))%3;
      double odo=d["total_meters"]|0.0;
      if(isfinite(odo)&&odo>=0)state.totalMeters=odo;
    } else error="Настройки повреждены; значения по умолчанию";
  }
  // Mode, cruise, faults, turns, horn and trip never restore from a snapshot.
  healthy=true;
}
bool Storage::save(const bike::Config& cfg,const bike::State& s,bool force) {
  if(!mounted)return false;
  uint32_t now=millis();if(!force&&now-lastSave_<5000)return true;lastSave_=now;
  JsonDocument data;encodeConfig(data["config"].to<JsonObject>(),cfg);
  data["safe"]=s.safe;data["limit"]=s.limit;data["light"]=s.light;data["previous_light"]=s.previousLight;
  data["meter_page"]=s.meterPage;data["total_meters"]=s.totalMeters;
  data["last_trip_meters"]=s.tripMeters;data["last_trip_seconds"]=uint32_t(now/1000);
  data["last_trip_max_kmh"]=s.maxSpeedKmh;data["last_trip_used_wh"]=s.usedWh;data["last_trip_charged_wh"]=s.chargedWh;
  String payload;serializeJson(data,payload);
  const uint32_t next=sequence_+1;const char* path=(next&1)?"/state-a.json":"/state-b.json";
  JsonDocument record;record["version"]=1;record["seq"]=next;record["payload"]=payload;
  record["crc"]=bike::crc32(reinterpret_cast<const uint8_t*>(payload.c_str()),payload.length());
  File f=SD.open(path,"w");if(!f){healthy=false;error="Ошибка записи SD";return false;}
  size_t n=serializeJson(record,f);f.flush();f.close();
  String verify;uint32_t seq=0;
  healthy=n>0&&readSlot(path,verify,seq)&&seq==next&&verify==payload;
  if(healthy){sequence_=next;error="";}else error="Ошибка проверки SD";
  return healthy;
}
