#pragma once
#include <Arduino.h>
#include <lvgl.h>
#include "Core.h"
#include "Settings.h"
enum class UiAction : uint8_t { Neutral,Drive,Sport,Limit25,Limit50,Limit70,LimitMax,Hazard,Meter,Save,ToggleControl };
using UiSink=void(*)(UiAction);
class Ui {
  lv_obj_t* pages_[3]{};
  lv_obj_t *speed_=nullptr,*arc_=nullptr,*battery_=nullptr,*temps_=nullptr,*meter_=nullptr;
  lv_obj_t *diag_=nullptr,*status_=nullptr,*modeButtons_[3]{},*limitButtons_[4]{};
  lv_obj_t *meterTitle_=nullptr,*batteryBar_=nullptr;
  lv_obj_t *motorTemp_=nullptr,*controllerTemp_=nullptr,*modeBadge_=nullptr;
  lv_obj_t *meterUnits_=nullptr;
  lv_obj_t *decorations_=nullptr,*hazardButton_=nullptr;
  lv_obj_t* rasterIcons_[11]{};
  bool iconFresh_=false;
  int batteryPercent_=0;
  float speedValue_=0;
  lv_meter_indicator_t* speedIndicator_=nullptr;
  lv_obj_t* flipValue_=nullptr;
  lv_obj_t* controlValue_=nullptr;
  lv_obj_t* settingValues_[timingSettingCount]{};
  bike::Config* config_=nullptr;
  UiSink sink_=nullptr;
  uint8_t page_=0;
  static void flush(lv_disp_drv_t*,const lv_area_t*,lv_color_t*);
  static void touch(lv_indev_drv_t*,lv_indev_data_t*);
  static void buttonEvent(lv_event_t*);
  static void navigation(lv_event_t*);
  static void swipe(lv_event_t*);
  static void flipScreen(lv_event_t*);
  static void adjustment(lv_event_t*);
  static void drawGaugeOverlay(lv_event_t*);
  void refreshSettings();
public:
  bool begin(bike::Config&,UiSink);
  void tick();
  void show(uint8_t);
  void notice(const String&);
  void update(const bike::State&,const bike::Telemetry&,uint32_t,bool,bool,bool,const String&,bool confirmed=false,bool applying=false);
};

