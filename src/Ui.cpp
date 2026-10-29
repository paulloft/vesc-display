#include "Ui.h"
#include "Board.h"
#include "ebike_display.h"
#include <src/misc/lv_event_private.h>
#include "FrameBuffer.h"
#include <Arduino_GFX_Library.h>
#include <Wire.h>
#include <esp_heap_caps.h>
#include <stdarg.h>
#include <string.h>

namespace {
Arduino_ESP32QSPI qspi(45,47,21,48,40,39);
Arduino_AXS15231B lcd(&qspi,GFX_NOT_DEFINED,0,false,320,480);
Arduino_Canvas canvas(320,480,&lcd);
Ui* activeUi=nullptr;
constexpr uint32_t cyan=0x27c4f5,muted=0x90a9bc,green=0x31f29a,red=0xff6554;
enum RasterRole {Left,Right,Position,LowBeam,HighBeam,Hazard,Warning,Battery,MotorTemperature,ControllerTemperature,Brake};
void setLabel(lv_obj_t* obj,const char* value) {
  if(strcmp(lv_label_get_text(obj),value))lv_label_set_text(obj,value);
}
void textf(lv_obj_t* obj,const char* format,...) {
  char value[128];va_list args;va_start(args,format);vsnprintf(value,sizeof(value),format,args);va_end(args);setLabel(obj,value);
}
void checked(lv_obj_t* obj,bool value) {
  if(lv_obj_has_state(obj,LV_STATE_CHECKED)==value)return;
  if(value)lv_obj_add_state(obj,LV_STATE_CHECKED);else lv_obj_remove_state(obj,LV_STATE_CHECKED);
}
lv_obj_t* find(lv_obj_t* root,const char* name) {return lv_obj_find_by_name(root,name);}
void removePreviewActions(lv_obj_t* object,lv_event_code_t code) {
  for(int i=int(lv_obj_get_event_count(object))-1;i>=0;i--) {
    auto event=lv_obj_get_event_dsc(object,i);
    if((event->filter & ~LV_EVENT_PREPROCESS)==uint32_t(code))lv_obj_remove_event(object,i);
  }
}
float gaugeAngle(lv_obj_t* arc,float speed) {
  float start=lv_arc_get_bg_angle_start(arc),span=lv_arc_get_bg_angle_end(arc)-start;
  if(span<0)span+=360;
  int low=lv_arc_get_min_value(arc),high=lv_arc_get_max_value(arc);
  float fraction=std::max(0.0f,std::min(1.0f,(speed-low)/std::max(1,high-low)));
  return (lv_arc_get_rotation(arc)+start+fraction*span)*PI/180;
}
void line(lv_layer_t* layer,int x,int y,int x2,int y2,uint32_t color,int width=1) {
  lv_draw_line_dsc_t d;lv_draw_line_dsc_init(&d);d.color=lv_color_hex(color);d.width=width;d.round_start=d.round_end=1;
  d.p1.x=x;d.p1.y=y;d.p2.x=x2;d.p2.y=y2;lv_draw_line(layer,&d);
}
}
void Ui::drawGaugeOverlay(lv_event_t* event) {
  auto& u=*activeUi;auto layer=lv_event_get_layer(event);lv_area_t a;lv_obj_get_coords(static_cast<lv_obj_t*>(lv_event_get_target(event)),&a);
  if(u.iconFresh_) {
    int w=lv_area_get_width(&a),h=lv_area_get_height(&a),radius=std::min(w,h)/2;
    int cx=a.x1+w/2,cy=a.y1+h/2;float angle=gaugeAngle(u.arc_,u.speedValue_);
    line(layer,cx+int(lroundf(cosf(angle)*radius*.80f)),cy+int(lroundf(sinf(angle)*radius*.80f)),
      cx+int(lroundf(cosf(angle)*radius*.97f)),cy+int(lroundf(sinf(angle)*radius*.97f)),0xffffff,3);
  }
}

void Ui::flush(lv_display_t* display,const lv_area_t* a,uint8_t* pixels) {
  copyLandscapeToPortrait(canvas.getFramebuffer(),reinterpret_cast<uint16_t*>(pixels),a->x1,a->y1,a->x2-a->x1+1,a->y2-a->y1+1,activeUi->config_->screenFlipped);
  activeUi->metrics_.pixels+=(a->x2-a->x1+1)*(a->y2-a->y1+1);
  if(lv_display_flush_is_last(display)) {
    uint32_t started=millis();canvas.flush();activeUi->metrics_.frames++;
    activeUi->metrics_.flushMs=std::max(activeUi->metrics_.flushMs,uint32_t(millis()-started));
  }
  lv_display_flush_ready(display);
}
void Ui::touch(lv_indev_t*,lv_indev_data_t* data) {
  static lv_point_t last{0,0};data->point=last;data->state=LV_INDEV_STATE_RELEASED;
  const uint8_t cmd[]={0xb5,0xab,0xa5,0x5a,0,0,0,8};
  Wire.beginTransmission(board::touchAddress);Wire.write(cmd,sizeof(cmd));
  if(Wire.endTransmission()!=0||Wire.requestFrom(board::touchAddress,uint8_t(8))!=8)return;
  uint8_t p[8];for(auto& x:p)x=Wire.read();if(p[1]!=1)return;
  int x=((p[2]&0xf)<<8)|p[3],y=((p[4]&0xf)<<8)|p[5];if(x>=320||y>=480)return;
  last.x=activeUi->config_->screenFlipped?479-y:y;last.y=activeUi->config_->screenFlipped?x:319-x;
  data->point=last;data->state=LV_INDEV_STATE_PRESSED;
}
void Ui::buttonEvent(lv_event_t* e) {if(activeUi->sink_)activeUi->sink_(static_cast<UiAction>(reinterpret_cast<intptr_t>(lv_event_get_user_data(e))));}
void Ui::navigation(lv_event_t* e){activeUi->show(reinterpret_cast<intptr_t>(lv_event_get_user_data(e)));}
void Ui::swipe(lv_event_t*) {
  auto input=lv_indev_active();if(!input)return;auto direction=lv_indev_get_gesture_dir(input);
  if(direction==LV_DIR_LEFT||direction==LV_DIR_RIGHT){lv_indev_wait_release(input);activeUi->show((activeUi->page_+(direction==LV_DIR_LEFT?1:3))%4);}
}
void Ui::switchChanged(lv_event_t* e) {
  auto& u=*activeUi;if(u.refreshing_)return;int id=reinterpret_cast<intptr_t>(lv_event_get_user_data(e));
  if(id==0) {u.config_->screenFlipped=lv_obj_has_state(u.flipSwitch_,LV_STATE_CHECKED);lv_obj_invalidate(lv_screen_active());}
  else if(u.sink_)u.sink_(id==1?UiAction::ToggleControl:id==2?UiAction::ToggleCruise:UiAction::ToggleSafe);
  u.refreshSettings(); // Restore switch if the main handler rejected a moving change.
}
void Ui::adjustment(lv_event_t* e) {
  auto& u=*activeUi;int code=reinterpret_cast<intptr_t>(lv_event_get_user_data(e));const auto& setting=timingSettings[abs(code)-1];
  auto& v=u.config_->*(setting.field);v=std::max(int(setting.min),std::min(int(setting.max),int(v)+(code>0?int(setting.step):-int(setting.step))));
  if(u.config_->doubleMs>=u.config_->holdMs)u.config_->doubleMs=u.config_->holdMs-100;
  u.refreshSettings();
}
void Ui::refreshSettings() {
  refreshing_=true;
  if(flipSwitch_)checked(flipSwitch_,config_->screenFlipped);
  if(controlSwitch_)checked(controlSwitch_,config_->motionEnabled);
  if(cruiseSwitch_)checked(cruiseSwitch_,config_->cruiseEnabled);
  for(size_t i=0;i<timingSettingCount;i++) {
    auto value=config_->*(timingSettings[i].field);textf(settingValues_[i],"%lu мс",static_cast<unsigned long>(value));
  }
  refreshing_=false;
}
void Ui::setCarousel(uint8_t group,uint8_t index,bool animate,bool emit) {
  index%=3;auto view=group?batteryTiles_:tripTiles_;
  if(group)batteryPage_=index;else meterPage_=index;
  lv_tabview_set_active(view,index,animate?LV_ANIM_ON:LV_ANIM_OFF);
  // LVGL can keep the previous dot geometry after changing CHECKED during
  // startup. Refresh the state-dependent width before the next layout pass.
  for(uint32_t i=0;i<lv_tabview_get_tab_count(view);i++)
    lv_obj_refresh_style(lv_tabview_get_tab_button(view,i),LV_PART_MAIN,LV_STYLE_WIDTH);
  if(emit&&!group&&sink_)sink_(static_cast<UiAction>(int(UiAction::MeterTrip)+index));
}
void Ui::carouselChanged(lv_event_t* e) {
  auto& u=*activeUi;auto view=static_cast<lv_obj_t*>(lv_event_get_target(e));
  if(view!=lv_event_get_current_target(e))return;
  int group=reinterpret_cast<intptr_t>(lv_event_get_user_data(e));int index=lv_tabview_get_tab_active(view);
  if(index!=(group?u.batteryPage_:u.meterPage_))u.setCarousel(group,index,false,true);
}
void Ui::carouselSwipe(lv_event_t* e) {
  auto input=lv_indev_active();if(!input)return;
  auto direction=lv_indev_get_gesture_dir(input);
  if(direction!=LV_DIR_LEFT&&direction!=LV_DIR_RIGHT)return;
  auto& u=*activeUi;int group=reinterpret_cast<intptr_t>(lv_event_get_user_data(e));
  auto view=group?u.batteryTiles_:u.tripTiles_;
  int count=lv_tabview_get_tab_count(view),index=lv_tabview_get_tab_active(view);
  if(count<2)return;
  int next=(index+(direction==LV_DIR_LEFT?1:count-1))%count;
  bool wraps=(index==count-1&&next==0)||(index==0&&next==count-1);
  lv_event_stop_bubbling(e);lv_indev_wait_release(input);
  // At the boundary jump directly, rather than animate backwards across all tabs.
  u.setCarousel(group,next,!wraps,true);
}
bool Ui::begin(bike::Config& config,UiSink sink) {
  activeUi=this;config_=&config;sink_=sink;
  if(!psramFound())return false;
  pinMode(board::backlight,OUTPUT);digitalWrite(board::backlight,LOW);
  if(!canvas.begin(32000000))return false;
  canvas.setRotation(1);canvas.fillScreen(0);canvas.flush();
  Wire.begin(board::touchSda,board::touchScl,400000);Wire.setTimeOut(10);
  lv_init();lv_tick_set_cb([]()->uint32_t{return millis();});
  auto buf=static_cast<uint8_t*>(heap_caps_malloc(480*48*2,MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT));if(!buf)return false;
  auto display=lv_display_create(480,320);lv_display_set_color_format(display,LV_COLOR_FORMAT_RGB565);
  lv_display_set_buffers(display,buf,nullptr,480*48*2,LV_DISPLAY_RENDER_MODE_PARTIAL);lv_display_set_flush_cb(display,flush);
  auto input=lv_indev_create();lv_indev_set_type(input,LV_INDEV_TYPE_POINTER);lv_indev_set_read_cb(input,touch);
  lv_indev_set_gesture_min_velocity(input,1);lv_indev_set_gesture_min_distance(input,16);
  ebike_display_init("");
  auto previous=lv_screen_active();auto root=app_create();lv_screen_load(root);lv_obj_delete(previous);
  mainTabs_=find(root,"app_pages");status_=find(root,"status_line");
  if(!mainTabs_||!status_)return false;
  lv_obj_update_layout(root);statusMainY_=lv_obj_get_y(status_);
  // ESP32 switches full pages immediately. Native tabviews remain swipeable
  // in the desktop editor, while the MCU avoids animating a whole 480x320 frame.
  lv_obj_set_scrollable(mainTabs_,false);
  lv_obj_set_scrollable(lv_tabview_get_tab_bar(mainTabs_),false);
  lv_obj_set_scrollable(lv_tabview_get_content(mainTabs_),false);
  lv_obj_add_event_cb(root,swipe,LV_EVENT_GESTURE,nullptr);
  for(int i=0;i<4;i++){char name[24];snprintf(name,sizeof(name),"app_page_%d",i);pages_[i]=find(root,name);if(!pages_[i])return false;}
  speed_=find(root,"speed_value");arc_=find(root,"speed_arc");modeBadge_=find(root,"mode_badge");
  diag_=find(root,"diagnostics_text");hazardButton_=find(root,"hazard");
  batteryTiles_=find(root,"battery_pages");tripTiles_=find(root,"trip_pages");batteryBar_=find(root,"battery_fill");
  motorTemp_=find(root,"motor_value");controllerTemp_=find(root,"controller_value");
  flipSwitch_=find(root,"flip_switch");controlSwitch_=find(root,"control_switch");cruiseSwitch_=find(root,"cruise_switch");safeSwitch_=find(root,"safe_switch");
  chart_=find(root,"speed_chart");
  if(!speed_||!arc_||!modeBadge_||!diag_||!hazardButton_||!batteryTiles_||!tripTiles_||!batteryBar_||!motorTemp_||!controllerTemp_||!flipSwitch_||!controlSwitch_||!cruiseSwitch_||!safeSwitch_||!chart_)return false;
  for(int i=0;i<3;i++) {
    char name[32];snprintf(name,sizeof(name),"battery_value_%d",i);batteryValues_[i]=find(root,name);
    snprintf(name,sizeof(name),"trip_value_%d",i);tripValues_[i]=find(root,name);
    if(!batteryValues_[i]||!tripValues_[i])return false;
  }
  const char* modes[]={"select_N","select_Drive","select_Sport"};
  const char* limits[]={"limit_25","limit_50","limit_70","limit_0"};
  auto wireAction=[&](lv_obj_t* o,int id){removePreviewActions(o,LV_EVENT_CLICKED);lv_obj_add_event_cb(o,buttonEvent,LV_EVENT_CLICKED,reinterpret_cast<void*>(id));};
  for(int i=0;i<3;i++){modeButtons_[i]=find(root,modes[i]);if(!modeButtons_[i])return false;wireAction(modeButtons_[i],i);}
  for(int i=0;i<4;i++){limitButtons_[i]=find(root,limits[i]);if(!limitButtons_[i])return false;wireAction(limitButtons_[i],3+i);}
  wireAction(hazardButton_,int(UiAction::Hazard));auto save=find(root,"save_settings");if(!save)return false;wireAction(save,int(UiAction::Save));
  const char* roles[]={"status_left","status_right","status_position","status_low_beam","status_high_beam","hazard_icon","status_warning","battery_frame","motor_icon","controller_icon","status_brake"};
  for(int i=0;i<11;i++){rasterIcons_[i]=find(root,roles[i]);if(!rasterIcons_[i])return false;}
  removePreviewActions(rasterIcons_[Warning],LV_EVENT_CLICKED);
  lv_obj_add_event_cb(rasterIcons_[Warning],navigation,LV_EVENT_CLICKED,reinterpret_cast<void*>(2));
  for(size_t i=0;i<timingSettingCount;i++) {
    char name[32];snprintf(name,sizeof(name),"timing_value_%u",unsigned(i));settingValues_[i]=find(root,name);
    for(int sign:{-1,1}) {
      snprintf(name,sizeof(name),sign<0?"timing_minus_%u":"timing_plus_%u",unsigned(i));auto o=find(root,name);if(!o||!settingValues_[i])return false;
      removePreviewActions(o,LV_EVENT_CLICKED);lv_obj_add_event_cb(o,adjustment,LV_EVENT_CLICKED,reinterpret_cast<void*>(sign*intptr_t(i+1)));
    }
  }
  lv_obj_t* switches[]={flipSwitch_,controlSwitch_,cruiseSwitch_,safeSwitch_};
  for(int i=0;i<4;i++){removePreviewActions(switches[i],LV_EVENT_VALUE_CHANGED);lv_obj_add_event_cb(switches[i],switchChanged,LV_EVENT_VALUE_CHANGED,reinterpret_cast<void*>(i));}
  for(int group=0;group<2;group++) {
    auto view=group?batteryTiles_:tripTiles_;auto content=lv_tabview_get_content(view);
    // The Tabview itself must not steal a swipe to scroll its own children.
    lv_obj_set_scrollable(view,false);lv_obj_set_scrollable(lv_tabview_get_tab_bar(view),false);
    // Tabview creates an internal content object which XML 9.5 cannot style.
    // Its default white background must never show through at the ends.
    lv_obj_set_style_bg_opa(content,LV_OPA_TRANSP,0);
    lv_obj_set_scroll_elastic(content,false);lv_obj_set_scrollable(content,false);
    lv_obj_set_scroll_chain_hor(content,false);lv_obj_set_style_anim_duration(content,180,0);
    auto wireSwipe=[&](lv_obj_t* object){
      lv_obj_set_gesture_bubble(object,false);
      lv_obj_add_event_cb(object,carouselSwipe,LV_EVENT_GESTURE,reinterpret_cast<void*>(group));
    };
    wireSwipe(view);wireSwipe(content);
    for(uint32_t i=0;i<lv_obj_get_child_count(content);i++) {
      auto tab=lv_obj_get_child(content,i);lv_obj_set_scrollable(tab,false);wireSwipe(tab);
    }
    lv_obj_add_event_cb(view,carouselChanged,LV_EVENT_VALUE_CHANGED,reinterpret_cast<void*>(group));
  }
  for(int i=0;i<4;i++){char name[24];snprintf(name,sizeof(name),"stats_value_%d",i);statsValues_[i]=find(root,name);if(!statsValues_[i])return false;}
  speedSeries_=lv_chart_get_series_next(chart_,nullptr);if(!speedSeries_)return false;
  lv_chart_set_all_values(chart_,speedSeries_,LV_CHART_POINT_NONE);
  // Cache the scale exported from XML, keeping its actual position and size.
  auto scale=find(root,"speed_ticks");if(!scale)return false;lv_obj_update_layout(root);
  int x=lv_obj_get_x(scale),y=lv_obj_get_y(scale),w=lv_obj_get_width(scale),h=lv_obj_get_height(scale);auto parent=lv_obj_get_parent(scale);
  decorations_=lv_obj_create(parent);lv_obj_remove_style_all(decorations_);lv_obj_set_pos(decorations_,x,y);lv_obj_set_size(decorations_,w,h);
  lv_obj_set_clickable(decorations_,false);lv_obj_set_scrollable(decorations_,false);lv_obj_add_event_cb(decorations_,drawGaugeOverlay,LV_EVENT_DRAW_MAIN,nullptr);
  gaugeRaster_=lv_snapshot_take(scale,LV_COLOR_FORMAT_A8);
  if(gaugeRaster_) {
    int mx=(int(gaugeRaster_->header.w)-w)/2,my=(int(gaugeRaster_->header.h)-h)/2;
    auto cropped=lv_draw_buf_create(w,h,LV_COLOR_FORMAT_A8,LV_STRIDE_AUTO);
    if(cropped){lv_area_t area{mx,my,mx+w-1,my+h-1};lv_draw_buf_copy(cropped,nullptr,gaugeRaster_,&area);lv_draw_buf_destroy(gaugeRaster_);gaugeRaster_=cropped;mx=my=0;}
    auto image=lv_image_create(parent);lv_obj_set_name(image,"cached_speed_ticks");lv_image_set_src(image,gaugeRaster_);lv_obj_set_pos(image,x-mx,y-my);
    lv_obj_set_style_image_recolor(image,lv_obj_get_style_text_color(scale,LV_PART_INDICATOR),0);lv_obj_set_style_image_recolor_opa(image,LV_OPA_COVER,0);
    lv_obj_set_clickable(image,false);lv_obj_move_to_index(image,lv_obj_get_index(scale));lv_obj_delete(scale);
  }
  show(0);
  // Preview fixtures never reach the physical display, even during startup.
  bike::State initial;bike::Telemetry absent;update(initial,absent,millis(),false,false,false,"",false,false);
  notice("Загрузка…");digitalWrite(board::backlight,HIGH);return true;
}

void Ui::show(uint8_t index) {
  page_=index%4;lv_obj_set_y(status_,page_==0?statusMainY_:3);
  lv_tabview_set_active(mainTabs_,page_,LV_ANIM_OFF);refreshSettings();
}
void Ui::notice(const String& text){if(status_)setLabel(status_,text.c_str());}
void Ui::benchmark(void(*report)(int,bool,uint32_t)) {
  uint8_t previous=page_;auto background=find(pages_[0],"background");
  for(int pass=0;pass<2;pass++) {
    lv_obj_set_hidden(background,pass!=0);
    for(int page=0;page<4;page++) {
      show(page);uint32_t started=millis();lv_obj_invalidate(lv_screen_active());lv_refr_now(nullptr);
      report(page,pass==0,millis()-started);
    }
  }
  lv_obj_set_hidden(background,false);show(previous);
}
void Ui::tick(){lv_timer_handler();}
void Ui::update(const bike::State& s,const bike::Telemetry& t,uint32_t now,bool io,bool sd,bool brake,const String& diagnostics,bool confirmed,bool applying) {
  if(!speed_)return;
  bool fresh=t.fresh(now);float value=fresh?fabsf(t.speedMs)*3.6f:0;
  if(fresh) {
    textf(speed_,"%d",int(lroundf(value)));textf(batteryValues_[0],"%d%%",int(lroundf(t.soc*100)));
    textf(batteryValues_[1],"%.1f",double(t.voltage));textf(batteryValues_[2],"%.0f",double(t.voltage*t.inputCurrent));
    textf(motorTemp_,"%.0f°C",double(t.motor));textf(controllerTemp_,"%.0f°C",double(t.fet));
  } else {setLabel(speed_,"--");for(auto label:batteryValues_)setLabel(label,"—");setLabel(motorTemp_,"—");setLabel(controllerTemp_,"—");}
  int percent=fresh?int(lroundf(t.soc*100)):0;if(lv_bar_get_value(batteryBar_)!=percent)lv_bar_set_value(batteryBar_,percent,LV_ANIM_OFF);
  int newArc=std::min(int(lv_arc_get_max_value(arc_)),int(value));if(arcValue_!=newArc){arcValue_=newArc;lv_arc_set_value(arc_,newArc);}
  if(iconFresh_!=fresh||int(speedValue_)!=int(value)) {
    lv_area_t a;lv_obj_get_coords(decorations_,&a);
    auto invalidate=[&](float v){float angle=gaugeAngle(arc_,v);int w=lv_area_get_width(&a),h=lv_area_get_height(&a),r=std::min(w,h)/2;
      int x=a.x1+w/2,y=a.y1+h/2;
      int x1=x+int(lroundf(cosf(angle)*r*.80f)),y1=y+int(lroundf(sinf(angle)*r*.80f));
      int x2=x+int(lroundf(cosf(angle)*r*.97f)),y2=y+int(lroundf(sinf(angle)*r*.97f));
      lv_area_t part{std::min(x1,x2)-4,std::min(y1,y2)-4,std::max(x1,x2)+4,std::max(y1,y2)+4};lv_obj_invalidate_area(decorations_,&part);};
    if(iconFresh_)invalidate(speedValue_);
    if(fresh)invalidate(value);
  }
  speedValue_=value;iconFresh_=fresh;
  textf(tripValues_[0],"%.1f",s.tripMeters/1000);textf(tripValues_[1],"%.1f",s.totalMeters/1000);
  textf(tripValues_[2],"%02lu:%02lu",static_cast<unsigned long>(now/3600000),static_cast<unsigned long>((now/60000)%60));
  if(meterPage_!=s.meterPage)setCarousel(0,s.meterPage,false,false);
  uint8_t o=s.outputs(now,*config_);constexpr uint32_t inactive=0x354b5a;
  auto tint=[&](RasterRole role,uint32_t color){auto c=lv_color_hex(color);if(!lv_color_eq(lv_obj_get_style_image_recolor(rasterIcons_[role],LV_PART_MAIN),c))lv_obj_set_style_image_recolor(rasterIcons_[role],c,0);};
  tint(Left,o&32?green:inactive);tint(Right,o&16?green:inactive);tint(Position,o&1?green:inactive);tint(LowBeam,o&2?0xe3f3ff:inactive);tint(HighBeam,o&4?cyan:inactive);
  tint(Hazard,s.turn==bike::Turn::Hazard?red:0xd1aa76);tint(Warning,s.faults||t.fault||!io||!fresh?0xffb25a:inactive);tint(Brake,brake?red:inactive);
  tint(Battery,fresh?0xbdd9e8:inactive);tint(MotorTemperature,fresh?0xff9c60:inactive);tint(ControllerTemperature,fresh?0xff9c60:inactive);
  auto batteryColor=lv_color_hex(fresh&&percent<20?red:green);if(!lv_color_eq(lv_obj_get_style_bg_color(batteryBar_,LV_PART_INDICATOR),batteryColor))lv_obj_set_style_bg_color(batteryBar_,batteryColor,LV_PART_INDICATOR);
  textf(modeBadge_,"%s%s",s.cruise?"КРУИЗ":s.mode==bike::Mode::N?"N":s.mode==bike::Mode::Drive?"Drive":"Sport",confirmed||!config_->motionEnabled?"":applying?"…":" !");
  checked(hazardButton_,s.turn==bike::Turn::Hazard);for(int i=0;i<3;i++)checked(modeButtons_[i],i==int(s.mode));
  checked(safeSwitch_,s.safe);
  const int limits[]={25,50,70,0};for(int i=0;i<4;i++) {
    bool hidden=s.safe&&i>=2;if(hidden!=lv_obj_is_hidden(limitButtons_[i])){if(hidden)lv_obj_set_hidden(limitButtons_[i],true);else lv_obj_set_hidden(limitButtons_[i],false);}
    checked(limitButtons_[i],s.limit==limits[i]);
  }
  if(now-chartAt_>=1000) {chartAt_=now;lv_chart_set_next_value(chart_,speedSeries_,fresh?int(lroundf(value)):LV_CHART_POINT_NONE);}
  if(page_==3) {
    textf(statsValues_[0],"%.0f",double(s.maxSpeedKmh));textf(statsValues_[1],"%.1f",s.tripMeters/1000);textf(statsValues_[2],"%.1f",s.usedWh);
    textf(statsValues_[3],"%02lu:%02lu",static_cast<unsigned long>(now/3600000),static_cast<unsigned long>((now/60000)%60));
  }
  if(page_==2) {String text=String("VESC: ")+(fresh?"связь есть":"нет связи")+"\nSD: "+(sd?"готова":"нет сохранения")+"\nПлата I/O: "+(io?"готова":"нет связи")+"\nБезопасный режим: "+(s.safe?"вкл":"выкл")+"\n"+diagnostics;setLabel(diag_,text.c_str());}
}
