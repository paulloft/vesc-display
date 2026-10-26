#include "Ui.h"
#include "Arduino_GFX_Library.h"
#include <Wire.h>
#include <fstream>
#include <cassert>
#include <iostream>
static uint32_t now=100;
extern "C" uint32_t millis(void){return now;}
static size_t renderedPixels=0;
static void (*originalFlush)(lv_display_t*,const lv_area_t*,uint8_t*);
static void measureFlush(lv_display_t* driver,const lv_area_t* area,uint8_t* colors) {
  renderedPixels+=(area->x2-area->x1+1)*(area->y2-area->y1+1);originalFlush(driver,area,colors);
}
static UiAction last=UiAction::Neutral;
static bike::State* liveState=nullptr;
static void action(UiAction a){last=a;if(a>=UiAction::MeterTrip&&a<=UiAction::MeterTime)liveState->meterPage=int(a)-int(UiAction::MeterTrip);}
static void save(const char* file,Ui& ui){
  lv_obj_invalidate(lv_screen_active());now+=100;ui.tick();lv_refr_now(nullptr);
  std::ofstream out(file,std::ios::binary);out<<"P6\n480 320\n255\n";
  for(uint16_t c:capturedFrame){uint8_t rgb[]={uint8_t((c>>11)*255/31),uint8_t(((c>>5)&63)*255/63),uint8_t((c&31)*255/31)};out.write(reinterpret_cast<char*>(rgb),3);}
}
static void touchPoint(Ui& ui,int x,int y,bool pressed) {
  Wire.x=319-y;Wire.y=x;Wire.pressed=pressed;now+=35;ui.tick();
}
static void swipeLeft(Ui& ui) {
  touchPoint(ui,320,180,true);touchPoint(ui,260,180,true);touchPoint(ui,160,180,true);touchPoint(ui,100,180,true);touchPoint(ui,100,180,false);
}
int main(){
  bike::Config config;bike::State state;state.mode=bike::Mode::Drive;state.safe=false;state.limit=50;state.light=2;state.tripMeters=24800;
  bike::Telemetry t;t.valid=true;t.receivedMs=now;t.speedMs=42/3.6f;t.soc=.82f;t.motor=48;t.fet=38;
  liveState=&state;t.voltage=72;t.inputCurrent=8.2;state.maxSpeedKmh=46;state.usedWh=132.4;
  Ui ui;assert(ui.begin(config,action));ui.notice("VESC 7.0 · Drive · лимит 50");
  lv_obj_update_layout(lv_screen_active());
  auto mainPage=lv_obj_get_child(lv_screen_active(),2);
  std::vector<int> iconCenters;
  for(uint32_t i=0;i<lv_obj_get_child_count(mainPage);i++) {
    auto object=lv_obj_get_child(mainPage,i);
    if(lv_obj_check_type(object,&lv_image_class)&&lv_obj_get_width(object)==36&&lv_obj_get_height(object)==28) {
      assert(lv_obj_get_y(object)+lv_obj_get_height(object)/2==14);
      lv_area_t bounds;lv_obj_get_coords(object,&bounds);assert(bounds.y1==0&&bounds.y2==27);
      iconCenters.push_back(lv_obj_get_x(object)+lv_obj_get_width(object)/2);
    }
  }
  std::sort(iconCenters.begin(),iconCenters.end());assert(iconCenters.size()==7);
  assert(iconCenters.front()==24&&iconCenters.back()==456);
  for(size_t i=1;i<iconCenters.size();i++)assert(iconCenters[i]-iconCenters[i-1]==72);
  ui.update(state,t,now,true,true,false,"Пульт: 1500 мВ\nDIAG: 0 0 0 0 0 0 0 0\nWi-Fi: Ebike-123456\nПароль: example",true);
  lv_obj_update_layout(lv_screen_active());
  auto status=lv_obj_get_child(lv_screen_active(),1);lv_area_t statusBounds;lv_obj_get_coords(status,&statusBounds);
  assert(statusBounds.y1==233&&statusBounds.y2<254);
  bool badgeChecked=false,scaleChecked=false;
  for(uint32_t i=0;i<lv_obj_get_child_count(mainPage);i++) {
    auto o=lv_obj_get_child(mainPage,i);
    if(lv_obj_get_x(o)==184&&lv_obj_get_y(o)==202) {
      lv_area_t badge,text;lv_obj_get_coords(o,&badge);lv_obj_get_coords(lv_obj_get_child(o,0),&text);
      assert(abs((badge.y1+badge.y2)-(text.y1+text.y2))<=1&&badge.y2<statusBounds.y1);badgeChecked=true;
    }
    if(lv_obj_check_type(o,&lv_image_class)&&lv_obj_get_width(o)==221) {
      assert(lv_obj_get_x(o)==125&&lv_obj_get_y(o)==35&&lv_obj_get_height(o)==221);scaleChecked=true;
    }
  }
  assert(badgeChecked&&scaleChecked);
  save("build/ui-main.ppm",ui);
  now+=1000;ui.tick();for(int i=0;i<3;i++){now+=100;ui.tick();}int unchangedFlushes=frameFlushes;
  t.receivedMs=now;
  ui.update(state,t,now,true,true,false,"",true);now+=35;ui.tick();
  if(frameFlushes!=unchangedFlushes)std::cerr<<"Idle redraw: "<<frameFlushes-unchangedFlushes<<"\n";
  assert(frameFlushes==unchangedFlushes); // Stable dashboard must not redraw.

  auto driver=lv_display_get_default();originalFlush=lv_display_get_flush_cb(driver);lv_display_set_flush_cb(driver,measureFlush);
  t.speedMs=43/3.6f;t.receivedMs=now;ui.update(state,t,now,true,true,false,"",true);now+=100;ui.tick();
  assert(renderedPixels>0&&renderedPixels<480*232);lv_display_set_flush_cb(driver,originalFlush);
  swipeLeft(ui);assert(lv_obj_is_hidden(mainPage));
  assert(!lv_obj_has_flag(lv_obj_get_child(lv_screen_active(),3),LV_OBJ_FLAG_HIDDEN));
  save("build/ui-settings.ppm",ui);
  // Edit the shared config through +/- buttons and switches.
  auto screen=lv_screen_active();auto settings=lv_obj_get_child(screen,3);
  auto list=lv_obj_get_child(settings,1);
  auto plus=lv_obj_get_child(list,3);lv_obj_send_event(plus,LV_EVENT_CLICKED,nullptr);
  assert(config.blinkOnMs==500);
  auto flip=lv_obj_get_child(list,timingSettingCount*4+1);
  lv_obj_add_state(flip,LV_STATE_CHECKED);lv_obj_send_event(flip,LV_EVENT_VALUE_CHANGED,nullptr);assert(config.screenFlipped);
  lv_obj_update_layout(screen);lv_area_t plusArea;lv_obj_get_coords(plus,&plusArea);
  int rotatedX=479-(plusArea.x1+plusArea.x2)/2,rotatedY=319-(plusArea.y1+plusArea.y2)/2;
  touchPoint(ui,rotatedX,rotatedY,true);touchPoint(ui,rotatedX,rotatedY,true);touchPoint(ui,rotatedX,rotatedY,false);
  assert(config.blinkOnMs==550);
  lv_obj_remove_state(flip,LV_STATE_CHECKED);lv_obj_send_event(flip,LV_EVENT_VALUE_CHANGED,nullptr);assert(!config.screenFlipped);
  auto control=lv_obj_get_child(list,timingSettingCount*4+3);
  lv_obj_add_state(control,LV_STATE_CHECKED);lv_obj_send_event(control,LV_EVENT_VALUE_CHANGED,nullptr);assert(last==UiAction::ToggleControl);
  assert(!config.motionEnabled&&!lv_obj_has_state(control,LV_STATE_CHECKED)); // Rejected by main handler.
  auto cruise=lv_obj_get_child(list,timingSettingCount*4+5);
  lv_obj_add_state(cruise,LV_STATE_CHECKED);lv_obj_send_event(cruise,LV_EVENT_VALUE_CHANGED,nullptr);assert(last==UiAction::ToggleCruise);
  swipeLeft(ui);assert(!lv_obj_has_flag(lv_obj_get_child(lv_screen_active(),4),LV_OBJ_FLAG_HIDDEN));ui.update(state,t,now,true,true,false,"USB / Wi-Fi diagnostics",true);save("build/ui-diagnostics.ppm",ui);
  swipeLeft(ui);assert(!lv_obj_has_flag(lv_obj_get_child(screen,5),LV_OBJ_FLAG_HIDDEN));
  for(int i=0;i<60;i++){now+=1000;t.receivedMs=now;t.speedMs=(20+12*sinf(i*.12f))/3.6f;ui.update(state,t,now,true,true,false,"",true);}
  save("build/ui-stats.ppm",ui);
  ui.show(0);lv_obj_update_layout(screen);
  // Point navigation changes the odometer, without changing the full page.
  // A horizontal drag inside the card must scroll it, not switch screens.
  touchPoint(ui,459,100,true);touchPoint(ui,429,100,true);touchPoint(ui,395,100,true);
  touchPoint(ui,359,100,true);touchPoint(ui,359,100,false);
  for(int i=0;i<20;i++){now+=50;ui.tick();}
  assert(state.meterPage==1&&!lv_obj_is_hidden(mainPage));
  lv_obj_t* tripCard=nullptr;lv_obj_t* batteryCard=nullptr;
  for(uint32_t i=0;i<lv_obj_get_child_count(mainPage);i++) {
    auto o=lv_obj_get_child(mainPage,i);
    if(lv_obj_get_x(o)==351&&lv_obj_get_y(o)==35)tripCard=o;
    if(lv_obj_get_x(o)==8&&lv_obj_get_y(o)==35)batteryCard=o;
  }
  assert(tripCard&&batteryCard);
  lv_obj_send_event(lv_obj_get_child(tripCard,2),LV_EVENT_CLICKED,nullptr);
  assert(state.meterPage==1&&last==UiAction::MeterTotal);
  now+=300;ui.tick();
  auto tripView=lv_obj_get_child(tripCard,0);
  lv_tileview_set_tile_by_index(tripView,2,0,LV_ANIM_OFF);lv_obj_send_event(tripView,LV_EVENT_VALUE_CHANGED,nullptr);
  assert(state.meterPage==2&&!lv_obj_is_hidden(mainPage));
  auto batteryView=lv_obj_get_child(batteryCard,0);
  lv_tileview_set_tile_by_index(batteryView,1,0,LV_ANIM_OFF);lv_obj_send_event(batteryView,LV_EVENT_VALUE_CHANGED,nullptr);
  assert(state.meterPage==2); // Independent voltage/power carousel.
  lv_obj_send_event(lv_obj_get_child(tripCard,1),LV_EVENT_CLICKED,nullptr);
  lv_tileview_set_tile_by_index(batteryView,0,0,LV_ANIM_OFF);lv_obj_send_event(batteryView,LV_EVENT_VALUE_CHANGED,nullptr);
  now+=300;ui.tick();
  state.light=3;state.turn=bike::Turn::Hazard;t.fault=1;
  now=((now/(config.blinkOnMs+config.blinkOffMs))+1)*(config.blinkOnMs+config.blinkOffMs)+100;t.receivedMs=now;
  assert(t.fresh(now));ui.update(state,t,now,true,true,true,"",true);save("build/ui-alerts.ppm",ui);
  std::cout<<"PASS: LVGL 9 rendering, partial/idle redraw, four pages, live buttons, switches and carousel selection\n";
}
