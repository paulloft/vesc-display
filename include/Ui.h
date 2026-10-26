#pragma once
#include <Arduino.h>
#include <lvgl.h>
#include "Core.h"
#include "Settings.h"
enum class UiAction : uint8_t {
  Neutral,Drive,Sport,Limit25,Limit50,Limit70,LimitMax,Hazard,Meter,Save,ToggleControl,
  MeterTrip,MeterTotal,MeterTime,ToggleCruise
};
using UiSink=void(*)(UiAction);
class Ui {
  lv_obj_t* pages_[4]{};
  lv_obj_t *speed_=nullptr,*arc_=nullptr,*status_=nullptr,*modeBadge_=nullptr;
  lv_obj_t *diag_=nullptr,*hazardButton_=nullptr,*modeButtons_[3]{},*limitButtons_[4]{};
  lv_obj_t *batteryTiles_=nullptr,*tripTiles_=nullptr,*batteryBar_=nullptr;
  lv_obj_t *batteryValues_[3]{},*tripValues_[3]{},*batteryDots_[3]{},*tripDots_[3]{};
  lv_obj_t *motorTemp_=nullptr,*controllerTemp_=nullptr,*decorations_=nullptr;
  lv_obj_t *flipSwitch_=nullptr,*controlSwitch_=nullptr,*cruiseSwitch_=nullptr;
  lv_obj_t *settingValues_[timingSettingCount]{};
  lv_obj_t *statsValues_[4]{},*chart_=nullptr;
  lv_chart_series_t* speedSeries_=nullptr;
  lv_draw_buf_t* gaugeRaster_=nullptr;
  lv_obj_t* rasterIcons_[11]{};
  bike::Config* config_=nullptr;
  UiSink sink_=nullptr;
  uint8_t page_=0,meterPage_=255,batteryPage_=0;
  bool refreshing_=false,iconFresh_=false;
  float speedValue_=0;
  int arcValue_=-1;
  uint32_t chartAt_=0;
  struct Metrics {uint32_t frames=0,pixels=0,flushMs=0;};
  Metrics metrics_;
  static void flush(lv_display_t*,const lv_area_t*,uint8_t*);
  static void touch(lv_indev_t*,lv_indev_data_t*);
  static void buttonEvent(lv_event_t*);
  static void navigation(lv_event_t*);
  static void swipe(lv_event_t*);
  static void switchChanged(lv_event_t*);
  static void adjustment(lv_event_t*);
  static void carouselChanged(lv_event_t*);
  static void dotClicked(lv_event_t*);
  static void drawGaugeOverlay(lv_event_t*);
  void refreshSettings();
  void setCarousel(uint8_t group,uint8_t index,bool animate,bool emit);
  lv_obj_t* carousel(lv_obj_t*,int,int,int,int,uint8_t,lv_obj_t**);
public:
  Metrics takeMetrics(){auto result=metrics_;metrics_={};return result;}
  bool begin(bike::Config&,UiSink);
  void benchmark(void(*report)(int,bool,uint32_t));
  void tick();
  void show(uint8_t);
  void notice(const String&);
  void update(const bike::State&,const bike::Telemetry&,uint32_t,bool,bool,bool,const String&,bool confirmed=false,bool applying=false);
};
