#include "Ui.h"
#include "Arduino_GFX_Library.h"
#include <Wire.h>
#include <fstream>
#include <cassert>
#include <iostream>
static uint32_t now=100;
extern "C" uint32_t millis(void){return now;}
static size_t renderedPixels=0;
static void (*originalFlush)(lv_disp_drv_t*,const lv_area_t*,lv_color_t*);
static void measureFlush(lv_disp_drv_t* driver,const lv_area_t* area,lv_color_t* colors) {
  renderedPixels+=(area->x2-area->x1+1)*(area->y2-area->y1+1);originalFlush(driver,area,colors);
}
static UiAction last=UiAction::Neutral;
static void action(UiAction a){last=a;}
static void save(const char* file,Ui& ui){
  lv_obj_invalidate(lv_scr_act());now+=100;ui.tick();lv_refr_now(nullptr);
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
  Ui ui;assert(ui.begin(config,action));ui.notice("VESC 7.0 · Drive · лимит 50");
  lv_obj_update_layout(lv_scr_act());
  auto mainPage=lv_obj_get_child(lv_scr_act(),2);
  std::vector<int> iconCenters;
  for(uint32_t i=0;i<lv_obj_get_child_cnt(mainPage);i++) {
    auto object=lv_obj_get_child(mainPage,i);
    if(lv_obj_check_type(object,&lv_img_class)&&lv_obj_get_width(object)==36&&lv_obj_get_height(object)==28) {
      assert(lv_obj_get_y(object)+lv_obj_get_height(object)/2==14);
      iconCenters.push_back(lv_obj_get_x(object)+lv_obj_get_width(object)/2);
    }
  }
  std::sort(iconCenters.begin(),iconCenters.end());assert(iconCenters.size()==7);
  assert(iconCenters.front()==24&&iconCenters.back()==456);
  for(size_t i=1;i<iconCenters.size();i++)assert(iconCenters[i]-iconCenters[i-1]==72);
  ui.update(state,t,now,true,true,false,"Пульт: 1500 мВ\nDIAG: 0 0 0 0 0 0 0 0\nWi-Fi: Ebike-123456\nПароль: example",true);
  save("build/ui-main.ppm",ui);
  now+=1000;ui.tick();for(int i=0;i<3;i++){now+=100;ui.tick();}int unchangedFlushes=frameFlushes;
  t.receivedMs=now;
  ui.update(state,t,now,true,true,false,"",true);now+=35;ui.tick();
  if(frameFlushes!=unchangedFlushes)std::cerr<<"Idle redraw: "<<frameFlushes-unchangedFlushes<<"\n";
  assert(frameFlushes==unchangedFlushes); // Stable dashboard must not redraw.

  auto driver=lv_disp_get_default()->driver;originalFlush=driver->flush_cb;driver->flush_cb=measureFlush;
  t.speedMs=43/3.6f;t.receivedMs=now;ui.update(state,t,now,true,true,false,"",true);now+=100;ui.tick();
  assert(renderedPixels>0&&renderedPixels<480*232);driver->flush_cb=originalFlush;
  swipeLeft(ui);assert(lv_obj_has_flag(mainPage,LV_OBJ_FLAG_HIDDEN));
  assert(!lv_obj_has_flag(lv_obj_get_child(lv_scr_act(),3),LV_OBJ_FLAG_HIDDEN));
  save("build/ui-settings.ppm",ui);
  // Settings uses the same live config as main, not a disconnected form.
  auto screen=lv_scr_act();auto settings=lv_obj_get_child(screen,3);
  auto list=lv_obj_get_child(settings,1);
  auto plus=lv_obj_get_child(list,3);lv_event_send(plus,LV_EVENT_CLICKED,nullptr);
  assert(config.blinkOnMs==500);
  auto flip=lv_obj_get_child(list,timingSettingCount*4+1);
  lv_event_send(flip,LV_EVENT_CLICKED,nullptr);assert(config.screenFlipped);
  // Tap the physically rotated + button: touch must rotate with the image.
  touchPoint(ui,32,244,true);touchPoint(ui,32,244,true);touchPoint(ui,32,244,false);
  assert(config.blinkOnMs==550);
  lv_event_send(flip,LV_EVENT_CLICKED,nullptr);assert(!config.screenFlipped);
  auto control=lv_obj_get_child(list,timingSettingCount*4+3);
  lv_event_send(control,LV_EVENT_CLICKED,nullptr);assert(last==UiAction::ToggleControl);
  assert(!config.motionEnabled); // Main handler checks stationarity before applying.
  swipeLeft(ui);assert(!lv_obj_has_flag(lv_obj_get_child(lv_scr_act(),4),LV_OBJ_FLAG_HIDDEN));ui.update(state,t,now,true,true,false,"USB / Wi-Fi diagnostics",true);save("build/ui-diagnostics.ppm",ui);
  ui.show(0);state.light=3;state.turn=bike::Turn::Hazard;t.fault=1;
  now=((now/(config.blinkOnMs+config.blinkOffMs))+1)*(config.blinkOnMs+config.blinkOffMs)+100;t.receivedMs=now;
  assert(t.fresh(now));ui.update(state,t,now,true,true,true,"",true);save("build/ui-alerts.ppm",ui);
  std::cout<<"PASS: real LVGL rendering, page navigation and live timing edit\n";
}
