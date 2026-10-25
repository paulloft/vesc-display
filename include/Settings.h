#pragma once
#include "Core.h"
// Extend this registry to expose another timing on both LCD and phone.
struct TimingSetting {
  const char* key;
  const char* label;
  uint32_t bike::Config::* field;
  uint32_t min,max,step;
};
constexpr TimingSetting timingSettings[]={
 {"blink_on_ms","Поворотник: включён",&bike::Config::blinkOnMs,100,2000,50},
 {"blink_off_ms","Поворотник: выключен",&bike::Config::blinkOffMs,100,2000,50},
 {"hold_ms","Удержание кнопки",&bike::Config::holdMs,500,5000,100},
 {"double_ms","Двойное нажатие",&bike::Config::doubleMs,150,600,25}
};
constexpr size_t timingSettingCount=sizeof(timingSettings)/sizeof(timingSettings[0]);
