#include "Ui.h"
#include "Board.h"
#include "IconAssets.h"
#include "FrameBuffer.h"
#include <Arduino_GFX_Library.h>
#include <Wire.h>
#include <esp_heap_caps.h>
#include <stdarg.h>
#include <string.h>

LV_FONT_DECLARE(font_ru_14);
LV_FONT_DECLARE(font_ru_20);
LV_FONT_DECLARE(font_speed_72);
LV_FONT_DECLARE(font_value_32);
LV_IMAGE_DECLARE(dashboard_background);
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
lv_obj_t* label(lv_obj_t* parent,const char* text,int x,int y,int w,const lv_font_t* font=&font_ru_14) {
  auto o=lv_label_create(parent);setLabel(o,text);lv_obj_set_pos(o,x,y);lv_obj_set_width(o,w);
  lv_obj_set_style_text_font(o,font,0);lv_obj_set_style_text_color(o,lv_color_hex(0xe9f4ff),0);
  lv_obj_set_clickable(o,false);return o;
}
lv_obj_t* centered(lv_obj_t* p,const char* text,int y,int w,const lv_font_t* font=&font_ru_14) {
  auto o=label(p,text,0,y,w,font);lv_obj_set_style_text_align(o,LV_TEXT_ALIGN_CENTER,0);return o;
}
lv_obj_t* panel(lv_obj_t* parent,int x,int y,int w,int h) {
  auto o=lv_obj_create(parent);lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,h);
  lv_obj_set_style_bg_color(o,lv_color_hex(0x06121e),0);lv_obj_set_style_bg_opa(o,235,0);
  lv_obj_set_style_border_color(o,lv_color_hex(0x315870),0);lv_obj_set_style_border_width(o,1,0);
  lv_obj_set_style_radius(o,12,0);lv_obj_set_style_pad_all(o,0,0);
  lv_obj_set_scrollable(o,false);return o;
}
lv_obj_t* button(lv_obj_t* parent,const char* text,int x,int y,int w,int h,lv_event_cb_t cb,intptr_t id) {
  auto o=lv_button_create(parent);lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,h);lv_obj_set_style_pad_all(o,0,0);
  lv_obj_set_style_bg_color(o,lv_color_hex(0x112b3d),0);lv_obj_set_style_bg_grad_color(o,lv_color_hex(0x050e18),0);
  lv_obj_set_style_bg_grad_dir(o,LV_GRAD_DIR_VER,0);lv_obj_set_style_bg_opa(o,LV_OPA_COVER,0);
  lv_obj_set_style_border_color(o,lv_color_hex(0x37647e),0);lv_obj_set_style_border_width(o,1,0);
  lv_obj_set_style_radius(o,10,0);lv_obj_set_style_shadow_width(o,0,0);
  lv_obj_set_style_bg_color(o,lv_color_hex(0x084b69),LV_STATE_CHECKED);
  lv_obj_set_style_border_color(o,lv_color_hex(cyan),LV_STATE_CHECKED);lv_obj_set_style_border_width(o,2,LV_STATE_CHECKED);
  lv_obj_set_style_bg_color(o,lv_color_hex(0x126480),LV_STATE_PRESSED);
  auto l=label(o,text,0,0,w);lv_obj_set_width(l,LV_SIZE_CONTENT);lv_obj_center(l);
  lv_obj_add_event_cb(o,cb,LV_EVENT_CLICKED,reinterpret_cast<void*>(id));return o;
}
lv_obj_t* rasterIcon(lv_obj_t* p,const lv_image_dsc_t* image,int cx,int cy) {
  auto o=lv_image_create(p);lv_image_set_src(o,image);
  lv_obj_set_pos(o,cx-image->header.w/2,cy-image->header.h/2);
  lv_obj_set_style_image_recolor_opa(o,LV_OPA_COVER,0);lv_obj_set_style_image_recolor(o,lv_color_white(),0);
  lv_obj_set_clickable(o,false);return o;
}
void line(lv_layer_t* layer,int x,int y,int x2,int y2,uint32_t color,int width=1) {
  lv_draw_line_dsc_t d;lv_draw_line_dsc_init(&d);d.color=lv_color_hex(color);d.width=width;d.round_start=d.round_end=1;
  d.p1.x=x;d.p1.y=y;d.p2.x=x2;d.p2.y=y2;lv_draw_line(layer,&d);
}
lv_obj_t* toggle(lv_obj_t* parent,int y,lv_event_cb_t cb,intptr_t id) {
  auto o=lv_switch_create(parent);lv_obj_set_pos(o,390,y);lv_obj_set_size(o,66,30);
  lv_obj_set_style_bg_color(o,lv_color_hex(0x243c4d),LV_PART_MAIN);
  lv_obj_set_style_bg_color(o,lv_color_hex(cyan),static_cast<lv_style_selector_t>(LV_PART_INDICATOR)|LV_STATE_CHECKED);
  lv_obj_add_event_cb(o,cb,LV_EVENT_VALUE_CHANGED,reinterpret_cast<void*>(id));return o;
}
}
void Ui::drawGaugeOverlay(lv_event_t* event) {
  auto& u=*activeUi;auto layer=lv_event_get_layer(event);lv_area_t a;lv_obj_get_coords(static_cast<lv_obj_t*>(lv_event_get_target(event)),&a);
  const int cx=a.x1+110,cy=a.y1+110;
  if(u.iconFresh_) {
    float angle=(150+std::min(100.0f,u.speedValue_)*2.4f)*PI/180;
    line(layer,cx+int(cosf(angle)*89),cy+int(sinf(angle)*89),cx+int(cosf(angle)*107),cy+int(sinf(angle)*107),0xffffff,3);
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
  else if(u.sink_)u.sink_(id==1?UiAction::ToggleControl:UiAction::ToggleCruise);
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
lv_obj_t* Ui::carousel(lv_obj_t* parent,int x,int y,int w,int h,uint8_t group,lv_obj_t** tiles) {
  auto card=panel(parent,x,y,w,h);auto view=lv_tileview_create(card);lv_obj_set_pos(view,1,1);lv_obj_set_size(view,w-2,h-17);
  lv_obj_set_style_bg_opa(view,LV_OPA_TRANSP,0);lv_obj_set_style_pad_all(view,0,0);lv_obj_set_style_border_width(view,0,0);
  lv_obj_set_scrollbar_mode(view,LV_SCROLLBAR_MODE_OFF);lv_obj_set_style_anim_duration(view,180,0);
  lv_obj_set_gesture_bubble(view,false);lv_obj_set_scroll_chain_hor(view,false);
  for(int i=0;i<3;i++) {
    tiles[i]=lv_tileview_add_tile(view,i,0,LV_DIR_HOR);lv_obj_set_style_pad_all(tiles[i],0,0);
    lv_obj_set_gesture_bubble(tiles[i],false);lv_obj_set_scrollable(tiles[i],false);
    auto hit=lv_obj_create(card);lv_obj_remove_style_all(hit);lv_obj_set_size(hit,24,16);lv_obj_set_pos(hit,w/2-36+i*24,h-17);
    lv_obj_set_clickable(hit,true);lv_obj_set_scrollable(hit,false);lv_obj_set_gesture_bubble(hit,false);
    lv_obj_add_event_cb(hit,dotClicked,LV_EVENT_CLICKED,reinterpret_cast<void*>(group*3+i));
    auto dot=lv_obj_create(hit);lv_obj_remove_style_all(dot);lv_obj_set_size(dot,5,5);lv_obj_center(dot);
    lv_obj_set_style_radius(dot,LV_RADIUS_CIRCLE,0);lv_obj_set_style_bg_opa(dot,LV_OPA_COVER,0);lv_obj_set_style_bg_color(dot,lv_color_hex(0x375469),0);
    (group?batteryDots_:tripDots_)[i]=dot;
  }
  lv_obj_add_event_cb(view,carouselChanged,LV_EVENT_VALUE_CHANGED,reinterpret_cast<void*>(group));
  return view;
}
void Ui::setCarousel(uint8_t group,uint8_t index,bool animate,bool emit) {
  index%=3;auto view=group?batteryTiles_:tripTiles_;auto dots=group?batteryDots_:tripDots_;
  if(group)batteryPage_=index;else meterPage_=index;
  lv_tileview_set_tile_by_index(view,index,0,animate?LV_ANIM_ON:LV_ANIM_OFF);
  for(int i=0;i<3;i++) {
    int width=i==index?16:5;if(lv_obj_get_width(dots[i])!=width){lv_obj_set_width(dots[i],width);lv_obj_center(dots[i]);}
    auto color=lv_color_hex(i==index?cyan:0x375469);
    if(!lv_color_eq(lv_obj_get_style_bg_color(dots[i],LV_PART_MAIN),color))lv_obj_set_style_bg_color(dots[i],color,0);
  }
  if(emit&&!group&&sink_)sink_(static_cast<UiAction>(int(UiAction::MeterTrip)+index));
}
void Ui::carouselChanged(lv_event_t* e) {
  auto& u=*activeUi;auto view=static_cast<lv_obj_t*>(lv_event_get_target(e));int group=reinterpret_cast<intptr_t>(lv_event_get_user_data(e));
  auto tile=lv_tileview_get_tile_active(view);if(!tile)return;int index=lv_obj_get_index(tile);
  if(index!=(group?u.batteryPage_:u.meterPage_))u.setCarousel(group,index,false,true);
}
void Ui::dotClicked(lv_event_t* e) {int code=reinterpret_cast<intptr_t>(lv_event_get_user_data(e));activeUi->setCarousel(code/3,code%3,true,true);}
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
  auto root=lv_screen_active();lv_obj_set_style_bg_color(root,lv_color_hex(0x040b12),0);lv_obj_set_style_text_font(root,&font_ru_14,0);
  lv_obj_set_scrollable(root,false);lv_obj_add_event_cb(root,swipe,LV_EVENT_GESTURE,nullptr);
  auto background=lv_image_create(root);lv_image_set_src(background,&dashboard_background);lv_obj_set_pos(background,0,0);
  status_=label(root,"Загрузка…",8,3,464);lv_label_set_long_mode(status_,LV_LABEL_LONG_MODE_DOTS);lv_obj_set_height(status_,18);
  lv_obj_set_style_text_color(status_,lv_color_hex(muted),0);
  for(auto& page:pages_){page=panel(root,0,25,480,295);lv_obj_set_style_bg_opa(page,LV_OPA_TRANSP,0);lv_obj_set_style_border_width(page,0,0);}
  auto main=pages_[0];lv_obj_set_y(main,0);lv_obj_set_height(main,320);lv_obj_t* tiles[3];
  batteryTiles_=carousel(main,8,35,112,70,1,tiles);
  label(tiles[0],"ЗАРЯД",40,4,69);batteryValues_[0]=label(tiles[0],"—",40,20,69,&font_value_32);
  batteryBar_=lv_bar_create(tiles[0]);lv_obj_set_pos(batteryBar_,15,17);lv_obj_set_size(batteryBar_,14,33);
  lv_obj_set_style_bg_color(batteryBar_,lv_color_hex(0x152a38),0);lv_obj_set_style_bg_color(batteryBar_,lv_color_hex(green),LV_PART_INDICATOR);
  rasterIcons_[Battery]=rasterIcon(tiles[0],&icon_battery,22,29);
  centered(tiles[1],"НАПРЯЖЕНИЕ",3,110);batteryValues_[1]=centered(tiles[1],"—",21,110,&font_value_32);
  centered(tiles[2],"МОЩНОСТЬ, Вт",3,110);batteryValues_[2]=centered(tiles[2],"—",21,110,&font_value_32);
  setCarousel(1,0,false,false);
  auto b=panel(main,8,113,112,100);label(b,"МОТОР",37,4,75);motorTemp_=label(b,"—",37,20,75,&font_ru_20);
  label(b,"КОНТР.",37,53,75);controllerTemp_=label(b,"—",37,69,75,&font_ru_20);
  auto divider=panel(b,9,49,94,1);lv_obj_set_style_bg_color(divider,lv_color_hex(0x284559),0);lv_obj_set_style_border_width(divider,0,0);
  rasterIcons_[MotorTemperature]=rasterIcon(main,&icon_thermometer,28,137);rasterIcons_[ControllerTemperature]=rasterIcon(main,&icon_thermometer,28,186);
  arc_=lv_arc_create(main);lv_obj_set_pos(arc_,125,35);lv_obj_set_size(arc_,221,221);
  lv_arc_set_rotation(arc_,150);lv_arc_set_bg_angles(arc_,0,240);lv_arc_set_range(arc_,0,100);lv_arc_set_value(arc_,0);
  lv_obj_set_style_arc_color(arc_,lv_color_hex(0x233f51),LV_PART_MAIN);lv_obj_set_style_arc_width(arc_,8,LV_PART_MAIN);
  lv_obj_set_style_arc_color(arc_,lv_color_hex(red),LV_PART_INDICATOR);lv_obj_set_style_arc_width(arc_,8,LV_PART_INDICATOR);
  lv_obj_set_style_bg_opa(arc_,LV_OPA_TRANSP,LV_PART_KNOB);lv_obj_set_style_pad_all(arc_,4,0);lv_obj_set_clickable(arc_,false);
  // Use the native radial scale so every tick shares the arc's center/angles.
  auto scale=lv_scale_create(main);lv_obj_remove_style_all(scale);lv_obj_set_pos(scale,125,35);lv_obj_set_size(scale,221,221);
  lv_obj_set_style_pad_all(scale,14,0);lv_obj_set_clickable(scale,false);lv_obj_set_scrollable(scale,false);
  lv_scale_set_mode(scale,LV_SCALE_MODE_ROUND_INNER);lv_scale_set_range(scale,0,100);
  lv_scale_set_angle_range(scale,240);lv_scale_set_rotation(scale,150);lv_scale_set_total_tick_count(scale,51);lv_scale_set_major_tick_every(scale,10);
  lv_obj_set_style_arc_opa(scale,LV_OPA_TRANSP,LV_PART_MAIN);lv_obj_set_style_line_width(scale,0,LV_PART_MAIN);
  lv_obj_set_style_length(scale,6,LV_PART_ITEMS);lv_obj_set_style_line_width(scale,1,LV_PART_ITEMS);
  lv_obj_set_style_line_color(scale,lv_color_hex(0x91b1c6),LV_PART_ITEMS);lv_obj_set_style_line_opa(scale,130,LV_PART_ITEMS);
  lv_obj_set_style_length(scale,13,LV_PART_INDICATOR);lv_obj_set_style_line_width(scale,2,LV_PART_INDICATOR);
  lv_obj_set_style_line_color(scale,lv_color_hex(0xc5dfef),LV_PART_INDICATOR);lv_obj_set_style_text_color(scale,lv_color_hex(0xbcd8ea),LV_PART_INDICATOR);
  lv_obj_set_style_text_font(scale,&font_ru_14,LV_PART_INDICATOR);
  decorations_=lv_obj_create(main);lv_obj_remove_style_all(decorations_);lv_obj_set_pos(decorations_,125,35);lv_obj_set_size(decorations_,221,221);
  lv_obj_set_clickable(decorations_,false);lv_obj_set_scrollable(decorations_,false);lv_obj_add_event_cb(decorations_,drawGaugeOverlay,LV_EVENT_DRAW_MAIN,nullptr);
  speed_=centered(main,"--",91,186,&font_speed_72);lv_obj_set_x(speed_,143);
  auto unit=centered(main,"КМ/Ч",165,120);lv_obj_set_x(unit,176);
  auto badge=panel(main,184,202,104,29);lv_obj_set_style_border_color(badge,lv_color_hex(cyan),0);lv_obj_set_style_border_width(badge,2,0);
  modeBadge_=label(badge,"N",0,0,LV_SIZE_CONTENT,&font_ru_20);lv_obj_center(modeBadge_);
  tripTiles_=carousel(main,351,35,121,102,0,tiles);
  const char* titles[]={"ПОЕЗДКА","ОБЩИЙ","ВРЕМЯ"};const char* units[]={"КМ","КМ","ЧАС : МИН"};
  for(int i=0;i<3;i++){centered(tiles[i],titles[i],5,119);tripValues_[i]=centered(tiles[i],i==2?"00:00":"0.0",28,119,&font_value_32);centered(tiles[i],units[i],65,119);}
  setCarousel(0,0,false,false);
  hazardButton_=button(main,"",351,145,121,68,buttonEvent,int(UiAction::Hazard));rasterIcons_[Hazard]=rasterIcon(hazardButton_,&icon_hazard,60,34);
  auto warning=button(main,"",364,0,40,27,navigation,2);lv_obj_set_style_bg_opa(warning,LV_OPA_TRANSP,0);lv_obj_set_style_border_width(warning,0,0);
  auto footer=panel(main,4,250,472,70);lv_obj_set_style_radius(footer,10,0);
  const char* modes[]={"N","Drive","Sport"};const int mx[]={8,66,144},mw[]={52,72,72};
  for(int i=0;i<3;i++){modeButtons_[i]=button(main,modes[i],mx[i],254,mw[i],62,buttonEvent,i);lv_obj_set_style_text_font(lv_obj_get_child(modeButtons_[i],0),&font_ru_20,0);}
  auto separator=panel(main,225,257,1,56);lv_obj_set_style_bg_color(separator,lv_color_hex(0x447b98),0);lv_obj_set_style_border_width(separator,0,0);
  const char* limits[]={"25","50","70","MAX"};for(int i=0;i<4;i++) {
    limitButtons_[i]=button(main,limits[i],235+i*60,254,55,62,buttonEvent,3+i);lv_obj_set_style_text_font(lv_obj_get_child(limitButtons_[i],0),&font_ru_20,0);
    lv_obj_set_style_border_color(limitButtons_[i],lv_color_hex(red),LV_STATE_CHECKED);lv_obj_set_style_bg_color(limitButtons_[i],lv_color_hex(0x491b20),LV_STATE_CHECKED);
  }
  rasterIcons_[Left]=rasterIcon(main,&icon_left,24,14);rasterIcons_[Position]=rasterIcon(main,&icon_position,96,14);
  rasterIcons_[LowBeam]=rasterIcon(main,&icon_low_beam,168,14);rasterIcons_[HighBeam]=rasterIcon(main,&icon_high_beam,240,14);
  rasterIcons_[Brake]=rasterIcon(main,&icon_brake,312,14);rasterIcons_[Warning]=rasterIcon(main,&icon_warning,384,14);rasterIcons_[Right]=rasterIcon(main,&icon_right,456,14);
  auto settings=pages_[1];lv_obj_set_style_bg_opa(settings,LV_OPA_COVER,0);label(settings,"НАСТРОЙКИ",12,3,300,&font_ru_20);
  auto list=panel(settings,4,32,472,211);lv_obj_set_scrollable(list,true);lv_obj_set_scroll_dir(list,LV_DIR_VER);
  lv_obj_set_style_border_width(list,0,0);lv_obj_set_style_bg_opa(list,LV_OPA_TRANSP,0);
  for(size_t i=0;i<timingSettingCount;i++) {
    int y=i*54;label(list,timingSettings[i].label,12,y+16,210);settingValues_[i]=label(list,"",217,y+16,105);
    lv_obj_set_style_text_color(settingValues_[i],lv_color_hex(cyan),0);lv_obj_set_style_text_align(settingValues_[i],LV_TEXT_ALIGN_RIGHT,0);
    button(list,"-",339,y+8,52,32,adjustment,-int(i+1));button(list,"+",400,y+8,52,32,adjustment,int(i+1));
  }
  int y=timingSettingCount*54;
  label(list,"Перевернуть экран",12,y+5,335);flipSwitch_=toggle(list,y,switchChanged,0);
  label(list,"Управление VESC",12,y+55,335);controlSwitch_=toggle(list,y+50,switchChanged,1);
  label(list,"Круиз-контроль",12,y+105,335);cruiseSwitch_=toggle(list,y+100,switchChanged,2);
  label(settings,"Изменения действуют сразу",12,263,300);button(settings,"Сохранить",335,253,133,38,buttonEvent,int(UiAction::Save));refreshSettings();
  auto diagnostics=pages_[2];lv_obj_set_style_bg_opa(diagnostics,LV_OPA_COVER,0);lv_obj_set_scrollable(diagnostics,true);lv_obj_set_scroll_dir(diagnostics,LV_DIR_VER);
  diag_=label(diagnostics,"",12,7,456);
  auto stats=pages_[3];lv_obj_set_style_bg_opa(stats,LV_OPA_COVER,0);label(stats,"ТЕКУЩАЯ ПОЕЗДКА",12,3,456,&font_ru_20);
  const char* captions[]={"МАКС. КМ/Ч","ПРОБЕГ, КМ","РАСХОД, Втч","ЧАС : МИН"};
  for(int i=0;i<4;i++){auto card=panel(stats,12+i*116,34,108,62);centered(card,captions[i],5,106);statsValues_[i]=centered(card,"—",26,106,&font_ru_20);}
  auto chartLabel=label(stats,"СКОРОСТЬ · ПОСЛЕДНЯЯ МИНУТА",14,111,450);lv_obj_set_style_text_color(chartLabel,lv_color_hex(muted),0);
  chart_=lv_chart_create(stats);lv_obj_set_pos(chart_,12,137);lv_obj_set_size(chart_,456,141);
  lv_obj_set_style_bg_color(chart_,lv_color_hex(0x071622),0);lv_obj_set_style_border_color(chart_,lv_color_hex(0x294e63),0);lv_obj_set_style_radius(chart_,10,0);
  lv_obj_set_style_size(chart_,0,0,LV_PART_INDICATOR);lv_obj_set_style_line_width(chart_,2,LV_PART_ITEMS);
  lv_obj_set_style_line_color(chart_,lv_color_hex(0x294456),LV_PART_MAIN);
  lv_obj_set_style_line_opa(chart_,LV_OPA_70,LV_PART_MAIN);
  lv_obj_set_style_pad_all(chart_,10,0);
  lv_chart_set_type(chart_,LV_CHART_TYPE_LINE);lv_chart_set_point_count(chart_,60);lv_chart_set_axis_range(chart_,LV_CHART_AXIS_PRIMARY_Y,0,100);
  lv_chart_set_div_line_count(chart_,5,6);speedSeries_=lv_chart_add_series(chart_,lv_color_hex(cyan),LV_CHART_AXIS_PRIMARY_Y);
  lv_chart_set_all_values(chart_,speedSeries_,LV_CHART_POINT_NONE);
  // Cache the native scale once; keep the live widget if allocation fails.
  lv_obj_update_layout(root);
  // The generic scale reserves a 100 px outer label margin. Crop it from
  // the cached round-inner image so its origin stays at the dial's origin.
  gaugeRaster_=lv_snapshot_take(scale,LV_COLOR_FORMAT_A8);
  if(gaugeRaster_) {
    int margin=(int(gaugeRaster_->header.w)-221)/2;
    auto cropped=lv_draw_buf_create(221,221,LV_COLOR_FORMAT_A8,LV_STRIDE_AUTO);
    if(cropped) {
      lv_area_t area{margin,margin,margin+220,margin+220};
      lv_draw_buf_copy(cropped,nullptr,gaugeRaster_,&area);lv_draw_buf_destroy(gaugeRaster_);gaugeRaster_=cropped;
      margin=0;
    }
    auto image=lv_image_create(main);lv_image_set_src(image,gaugeRaster_);lv_obj_set_pos(image,125-margin,35-margin);
    lv_obj_set_style_image_recolor(image,lv_color_hex(0xbcd8ea),0);lv_obj_set_style_image_recolor_opa(image,LV_OPA_COVER,0);
    lv_obj_set_clickable(image,false);lv_obj_move_to_index(image,lv_obj_get_index(scale));lv_obj_delete(scale);
  }
  show(0);digitalWrite(board::backlight,HIGH);return true;
}
void Ui::show(uint8_t index) {
  page_=index%4;lv_obj_set_y(status_,page_==0?233:3);for(int i=0;i<4;i++){if(i==page_)lv_obj_set_hidden(pages_[i],false);else lv_obj_set_hidden(pages_[i],true);}refreshSettings();
}
void Ui::notice(const String& text){if(status_)setLabel(status_,text.c_str());}
void Ui::benchmark(void(*report)(int,bool,uint32_t)) {
  uint8_t previous=page_;auto background=lv_obj_get_child(lv_screen_active(),0);
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
  int newArc=std::min(100,int(value));if(arcValue_!=newArc){arcValue_=newArc;lv_arc_set_value(arc_,newArc);}
  if(iconFresh_!=fresh||int(speedValue_)!=int(value)) {
    lv_area_t a;lv_obj_get_coords(decorations_,&a);
    auto invalidate=[&](float v){float angle=(150+std::min(100.0f,v)*2.4f)*PI/180;int x=a.x1+110,y=a.y1+110;
      int x1=x+int(cosf(angle)*89),y1=y+int(sinf(angle)*89),x2=x+int(cosf(angle)*107),y2=y+int(sinf(angle)*107);
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
