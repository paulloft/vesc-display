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
static void action(UiAction a){last=a;if(a==UiAction::ToggleSafe)liveState->setSafe(!liveState->safe);if(a>=UiAction::MeterTrip&&a<=UiAction::MeterTime)liveState->meterPage=int(a)-int(UiAction::MeterTrip);}
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
static void swipeCard(Ui& ui,int x,int y,int width,bool left,int steps=4) {
  int from=x+(left?width-8:8),to=x+(left?8:width-8);
  touchPoint(ui,from,y,true);
  for(int step=1;step<=steps;step++)touchPoint(ui,from+(to-from)*step/steps,y,true);
  touchPoint(ui,to,y,false);
  for(int i=0;i<8;i++){now+=35;ui.tick();}
}
int main(int argc,char** argv){
  bike::Config config;bike::State state;state.mode=bike::Mode::Drive;state.safe=false;state.limit=50;state.light=2;state.tripMeters=24800;
  bike::Telemetry t;t.valid=true;t.receivedMs=now;t.speedMs=42/3.6f;t.soc=.82f;t.motor=48;t.fet=38;
  liveState=&state;t.voltage=72;t.inputCurrent=8.2;state.maxSpeedKmh=46;state.usedWh=132.4;
  Ui ui;assert(ui.begin(config,action));ui.notice("VESC 7.0 · Drive · лимит 50");
  if(argc==2) {
    // Restore the saved page before the first frame, as on an actual boot.
    const uint8_t restoredPage=std::stoi(argv[1]);state.meterPage=restoredPage;
    ui.update(state,t,now,true,true,false,"",true);
    for(int frame=0;frame<20;frame++) {
      now+=35;ui.tick();lv_obj_update_layout(lv_screen_active());
      auto view=lv_obj_find_by_name(lv_screen_active(),"trip_pages");
      assert(state.meterPage==restoredPage);
      assert(lv_tabview_get_tab_active(view)==restoredPage);
      for(int i=0;i<3;i++) {
        auto dot=lv_tabview_get_tab_button(view,i);
        bool selected=i==state.meterPage;
        if(lv_obj_has_state(dot,LV_STATE_CHECKED)!=selected)
          std::cerr<<"Restored page "<<int(state.meterPage)<<", frame "<<frame<<", dot "<<i<<" has wrong state\n";
        assert(lv_obj_has_state(dot,LV_STATE_CHECKED)==selected);
        assert(lv_obj_get_width(dot)==(selected?16:5));
      }
    }
    std::cout<<"Restored odometer page "<<int(state.meterPage)<<": dots OK\n";
    return 0;
  }
  lv_obj_update_layout(lv_screen_active());
  auto screen=lv_screen_active();
  auto named=[&](const char* name){auto o=lv_obj_find_by_name(screen,name);assert(o);return o;};
  auto mainPage=named("page_view_0");
  auto tabs=named("app_pages");
  assert(std::string(lv_label_get_text(named("speed_value")))=="--"); // Preview fixtures never reach hardware.
  std::vector<int> iconCenters;
  int iconRow=lv_obj_get_y(named("status_left"));
  for(uint32_t i=0;i<lv_obj_get_child_count(mainPage);i++) {
    auto object=lv_obj_get_child(mainPage,i);
    if(lv_obj_check_type(object,&lv_image_class)&&lv_obj_get_width(object)==36&&lv_obj_get_height(object)==28) {
      assert(lv_obj_get_y(object)+lv_obj_get_height(object)/2==iconRow+14);
      lv_area_t bounds;lv_obj_get_coords(object,&bounds);assert(bounds.y1==iconRow&&bounds.y2==iconRow+27);
      iconCenters.push_back(lv_obj_get_x(object)+lv_obj_get_width(object)/2);
    }
  }
  std::sort(iconCenters.begin(),iconCenters.end());assert(iconCenters.size()==7);
  assert(iconCenters.front()==24&&iconCenters.back()==456);
  for(size_t i=1;i<iconCenters.size();i++)assert(iconCenters[i]-iconCenters[i-1]==72);
  ui.update(state,t,now,true,true,false,"Пульт: 1500 мВ\nDIAG: 0 0 0 0 0 0 0 0\nWi-Fi: Ebike-123456\nПароль: example",true);
  lv_obj_update_layout(lv_screen_active());
  auto status=named("status_line");lv_area_t statusBounds;lv_obj_get_coords(status,&statusBounds);
  assert(statusBounds.y1>=225&&statusBounds.y2<254);
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
  // Compare visible glyph bounds, not label rectangles: the converter's
  // cropped font metrics used to make the percentage touch its caption.
  auto inkBounds=[&](lv_obj_t* label,uint32_t letter){
    auto font=lv_obj_get_style_text_font(label,LV_PART_MAIN);lv_font_glyph_dsc_t glyph{};
    assert(lv_font_get_glyph_dsc(font,&glyph,letter,0));lv_area_t box;lv_obj_get_coords(label,&box);
    int top=box.y1+font->line_height-font->base_line-glyph.box_h-glyph.ofs_y;
    return std::pair<int,int>{top,top+glyph.box_h-1};
  };
  auto titleInk=inkBounds(named("battery_title"),0x417); // Cyrillic З
  auto valueInk=inkBounds(named("battery_value_0"),'8');
  assert(valueInk.first-titleInk.second>=4);
  auto speedInk=inkBounds(named("speed_value"),'4');
  auto unitsInk=inkBounds(named("speed_units"),0x41a);
  assert(speedInk.first>=95&&unitsInk.first-speedInk.second>=4);
  lv_area_t hazardBefore,hazardAfter;auto hazard=named("hazard");auto hazardIcon=named("hazard_icon");
  lv_obj_get_coords(hazardIcon,&hazardBefore);lv_obj_add_state(hazard,LV_STATE_CHECKED);lv_obj_update_layout(screen);
  lv_obj_get_coords(hazardIcon,&hazardAfter);assert(!memcmp(&hazardBefore,&hazardAfter,sizeof(hazardBefore)));
  lv_obj_remove_state(hazard,LV_STATE_CHECKED);lv_obj_update_layout(screen);
  save("build/ui-main.ppm",ui);
  auto batteryPreview=named("battery_pages");
  lv_tabview_set_active(batteryPreview,1,LV_ANIM_OFF);save("build/ui-voltage.ppm",ui);
  lv_tabview_set_active(batteryPreview,2,LV_ANIM_OFF);save("build/ui-power.ppm",ui);
  lv_tabview_set_active(batteryPreview,0,LV_ANIM_OFF);save("build/ui-main.ppm",ui);
  // The first stationary touch after boot must not paint a halo or shadow.
  std::vector<uint16_t> beforeTouch(capturedFrame,capturedFrame+480*320);
  touchPoint(ui,240,120,true);lv_refr_now(nullptr);
  assert(std::equal(beforeTouch.begin(),beforeTouch.end(),capturedFrame));
  touchPoint(ui,240,120,false);lv_refr_now(nullptr);
  now+=1000;ui.tick();for(int i=0;i<3;i++){now+=100;ui.tick();}int unchangedFlushes=frameFlushes;
  t.receivedMs=now;
  ui.update(state,t,now,true,true,false,"",true);now+=35;ui.tick();
  if(frameFlushes!=unchangedFlushes)std::cerr<<"Idle redraw: "<<frameFlushes-unchangedFlushes<<"\n";
  assert(frameFlushes==unchangedFlushes); // Stable dashboard must not redraw.

  auto driver=lv_display_get_default();originalFlush=lv_display_get_flush_cb(driver);lv_display_set_flush_cb(driver,measureFlush);
  t.speedMs=43/3.6f;t.receivedMs=now;ui.update(state,t,now,true,true,false,"",true);now+=100;ui.tick();
  assert(renderedPixels>0&&renderedPixels<480*232);lv_display_set_flush_cb(driver,originalFlush);
  // Check the gauge after partial redraws in either direction against
  // a clean full refresh.
  for(float kmh:{18.0f,68.0f,0.0f,43.0f}) {
    t.speedMs=kmh/3.6f;t.receivedMs=now;
    ui.update(state,t,now,true,true,false,"",true);now+=35;ui.tick();lv_refr_now(nullptr);
    std::vector<uint16_t> partial(capturedFrame,capturedFrame+480*320);
    lv_obj_invalidate(screen);lv_refr_now(nullptr);
    assert(std::equal(partial.begin(),partial.end(),capturedFrame));
  }
  swipeLeft(ui);assert(lv_tabview_get_tab_active(tabs)==1);
  save("build/ui-settings.ppm",ui);
  // Edit the shared config through +/- buttons and switches.
  auto plus=named("timing_plus_0");lv_obj_send_event(plus,LV_EVENT_CLICKED,nullptr);
  assert(config.blinkOnMs==500);
  auto flip=named("flip_switch");
  lv_obj_add_state(flip,LV_STATE_CHECKED);lv_obj_send_event(flip,LV_EVENT_VALUE_CHANGED,nullptr);assert(config.screenFlipped);
  lv_obj_update_layout(screen);lv_area_t plusArea;lv_obj_get_coords(plus,&plusArea);
  int rotatedX=479-(plusArea.x1+plusArea.x2)/2,rotatedY=319-(plusArea.y1+plusArea.y2)/2;
  touchPoint(ui,rotatedX,rotatedY,true);touchPoint(ui,rotatedX,rotatedY,true);touchPoint(ui,rotatedX,rotatedY,false);
  assert(config.blinkOnMs==550);
  lv_obj_remove_state(flip,LV_STATE_CHECKED);lv_obj_send_event(flip,LV_EVENT_VALUE_CHANGED,nullptr);assert(!config.screenFlipped);
  auto control=named("control_switch");
  lv_obj_add_state(control,LV_STATE_CHECKED);lv_obj_send_event(control,LV_EVENT_VALUE_CHANGED,nullptr);assert(last==UiAction::ToggleControl);
  assert(!config.motionEnabled&&!lv_obj_has_state(control,LV_STATE_CHECKED)); // Rejected by main handler.
  auto cruise=named("cruise_switch");
  lv_obj_add_state(cruise,LV_STATE_CHECKED);lv_obj_send_event(cruise,LV_EVENT_VALUE_CHANGED,nullptr);assert(last==UiAction::ToggleCruise);
  config.cruiseEnabled=true;ui.show(1);lv_obj_scroll_to_view(cruise,LV_ANIM_OFF);
  for(auto sw:{flip,control,cruise}) {
    assert(lv_obj_get_style_radius(sw,LV_PART_MAIN)>=15);
    assert(lv_obj_get_style_radius(sw,LV_PART_KNOB)>=15);
    assert(lv_obj_get_style_bg_image_src(sw,LV_PART_KNOB)!=nullptr);
  }
  save("build/ui-switches.ppm",ui);
  auto safe=named("safe_switch");state.limit=70;
  lv_obj_add_state(safe,LV_STATE_CHECKED);lv_obj_send_event(safe,LV_EVENT_VALUE_CHANGED,nullptr);
  assert(last==UiAction::ToggleSafe&&state.safe&&state.limit==50);
  ui.update(state,t,now,true,true,false,"",true);lv_obj_update_layout(screen);
  assert(lv_obj_has_state(safe,LV_STATE_CHECKED));
  auto limits=named("limit_buttons");
  assert(lv_obj_is_hidden(named("limit_70"))&&lv_obj_is_hidden(named("limit_0")));
  assert(abs(lv_obj_get_width(named("limit_25"))-lv_obj_get_width(named("limit_50")))<=1);
  assert(lv_obj_get_width(named("limit_25"))+lv_obj_get_width(named("limit_50"))+6==lv_obj_get_content_width(limits));
  ui.show(0);save("build/ui-safe.ppm",ui);ui.show(1);
  lv_obj_remove_state(safe,LV_STATE_CHECKED);lv_obj_send_event(safe,LV_EVENT_VALUE_CHANGED,nullptr);
  assert(!state.safe);ui.update(state,t,now,true,true,false,"",true);lv_obj_update_layout(screen);
  assert(!lv_obj_has_state(safe,LV_STATE_CHECKED));
  int totalWidth=0;
  for(const char* name:{"limit_25","limit_50","limit_70","limit_0"}) {
    auto button=named(name);assert(!lv_obj_is_hidden(button));totalWidth+=lv_obj_get_width(button);
    assert(abs(lv_obj_get_width(button)-lv_obj_get_width(named("limit_25")))<=1);
  }
  assert(totalWidth+18==lv_obj_get_content_width(limits));
  swipeLeft(ui);assert(lv_tabview_get_tab_active(tabs)==2);ui.update(state,t,now,true,true,false,"USB / Wi-Fi diagnostics",true);save("build/ui-diagnostics.ppm",ui);
  swipeLeft(ui);assert(lv_tabview_get_tab_active(tabs)==3);
  for(int i=0;i<60;i++){now+=1000;t.receivedMs=now;t.speedMs=(20+12*sinf(i*.12f))/3.6f;ui.update(state,t,now,true,true,false,"",true);}
  save("build/ui-stats.ppm",ui);
  ui.show(0);lv_obj_update_layout(screen);
  // Point navigation changes the odometer, without changing the full page.
  // A horizontal drag inside the card must scroll it, not switch screens.
  touchPoint(ui,459,100,true);touchPoint(ui,429,100,true);touchPoint(ui,395,100,true);
  touchPoint(ui,359,100,true);touchPoint(ui,359,100,false);
  for(int i=0;i<20;i++){now+=50;ui.tick();}
  assert(state.meterPage==1&&lv_tabview_get_tab_active(tabs)==0);
  auto tripView=named("trip_pages");auto batteryView=named("battery_pages");
  lv_obj_send_event(named("trip_dot_1"),LV_EVENT_CLICKED,nullptr);
  assert(state.meterPage==1&&last==UiAction::MeterTotal);
  now+=300;ui.tick();
  lv_tabview_set_active(tripView,2,LV_ANIM_OFF);lv_obj_send_event(tripView,LV_EVENT_VALUE_CHANGED,nullptr);
  assert(state.meterPage==2&&lv_tabview_get_tab_active(tabs)==0);
  lv_tabview_set_active(batteryView,1,LV_ANIM_OFF);lv_obj_send_event(batteryView,LV_EVENT_VALUE_CHANGED,nullptr);
  assert(state.meterPage==2);
  lv_obj_send_event(named("trip_dot_0"),LV_EVENT_CLICKED,nullptr);
  lv_tabview_set_active(batteryView,0,LV_ANIM_OFF);lv_obj_send_event(batteryView,LV_EVENT_VALUE_CHANGED,nullptr);
  now+=300;ui.tick();
  // Real touch gestures wrap both carousels in either direction, including
  // repeated swipes. The dashboard must not turn into the settings page.
  lv_tabview_set_active(tripView,0,LV_ANIM_OFF);lv_obj_send_event(tripView,LV_EVENT_VALUE_CHANGED,nullptr);
  for(int i=0;i<7;i++) {
    swipeCard(ui,351,95,121,true);
    assert(state.meterPage==(i+1)%3&&lv_tabview_get_tab_active(tabs)==0);
  }
  for(int i=0;i<7;i++) {
    swipeCard(ui,351,95,121,false);
    assert(state.meterPage==(6-i)%3&&lv_tabview_get_tab_active(tabs)==0);
  }
  for(int i=0;i<7;i++) {
    swipeCard(ui,8,72,112,false);
    assert(lv_tabview_get_tab_active(batteryView)==(20-i)%3&&state.meterPage==0);
  }
  for(int i=0;i<7;i++) {
    swipeCard(ui,8,72,112,true);
    assert(lv_tabview_get_tab_active(batteryView)==(i+3)%3&&state.meterPage==0);
  }
  // Swipe headers, values and the dot row, including slow drags.
  // These used to be intercepted by the outer Tabview's scroll handling.
  for(auto view:{batteryView,tripView}) {
    lv_area_t card;lv_obj_get_coords(lv_obj_get_parent(view),&card);
    for(int y:{int(card.y1)+6,(int(card.y1)+int(card.y2))/2,int(card.y2)-7}) {
      for(int steps:{4,20})for(bool left:{true,false}) {
        int previous=lv_tabview_get_tab_active(view);
        swipeCard(ui,card.x1,y,lv_area_get_width(&card),left,steps);
        assert(lv_tabview_get_tab_active(view)==(previous+(left?1:2))%3);
        assert(lv_tabview_get_tab_active(tabs)==0);
      }
    }
  }
  for(auto view:{tripView,batteryView}) {
    auto content=lv_tabview_get_content(view);
    assert(lv_obj_get_style_bg_opa(content,LV_PART_MAIN)==LV_OPA_TRANSP);
    assert(!lv_obj_is_scroll_elastic(content));
    assert(lv_obj_get_style_clip_corner(lv_obj_get_parent(view),LV_PART_MAIN));
  }
  state.light=3;state.turn=bike::Turn::Hazard;t.fault=1;
  now=((now/(config.blinkOnMs+config.blinkOffMs))+1)*(config.blinkOnMs+config.blinkOffMs)+100;t.receivedMs=now;
  assert(t.fresh(now));ui.update(state,t,now,true,true,true,"",true);save("build/ui-alerts.ppm",ui);
  std::cout<<"PASS: LVGL 9 rendering, partial/idle redraw, four pages, live buttons, rounded switches, circular carousels in both directions\n";
}
