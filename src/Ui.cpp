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
LV_IMG_DECLARE(dashboard_background);
namespace {
Arduino_ESP32QSPI qspi(45,47,21,48,40,39);
Arduino_AXS15231B lcd(&qspi,GFX_NOT_DEFINED,0,false,320,480);
Arduino_Canvas canvas(320,480,&lcd);
lv_disp_draw_buf_t drawBuffer;
lv_disp_drv_t displayDriver;
lv_indev_drv_t inputDriver;
Ui* activeUi=nullptr;
constexpr uint32_t cyan=0x27c4f5,muted=0x90a9bc,green=0x31f29a,red=0xff594f;
// LVGL invalidates even when formatting the same value again.
void setLabel(lv_obj_t* obj,const char* value) {
  if(strcmp(lv_label_get_text(obj),value))lv_label_set_text(obj,value);
}
void textf(lv_obj_t* obj,const char* format,...) {
  char value[96];va_list args;va_start(args,format);vsnprintf(value,sizeof(value),format,args);va_end(args);setLabel(obj,value);
}
lv_obj_t* label(lv_obj_t* parent,const char* text,int x,int y,int w,const lv_font_t* font=&font_ru_14) {
  auto o=lv_label_create(parent);setLabel(o,text);lv_obj_set_pos(o,x,y);lv_obj_set_width(o,w);
  lv_obj_set_style_text_font(o,font,0);lv_obj_set_style_text_color(o,lv_color_hex(0xe9f4ff),0);return o;
}
lv_obj_t* panel(lv_obj_t* parent,int x,int y,int w,int h) {
  auto o=lv_obj_create(parent);lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,h);
  lv_obj_set_style_bg_color(o,lv_color_hex(0x040d16),0);lv_obj_set_style_bg_opa(o,LV_OPA_COVER,0);
  lv_obj_set_style_border_color(o,lv_color_hex(0x447b98),0);
  lv_obj_set_style_shadow_color(o,lv_color_hex(0x1a5a78),0);lv_obj_set_style_shadow_width(o,0,0);lv_obj_set_style_shadow_opa(o,55,0);
  lv_obj_set_style_border_width(o,1,0);lv_obj_set_style_radius(o,12,0);lv_obj_set_style_pad_all(o,0,0);
  lv_obj_clear_flag(o,LV_OBJ_FLAG_SCROLLABLE);return o;
}
lv_obj_t* button(lv_obj_t* parent,const char* text,int x,int y,int w,int h,lv_event_cb_t cb,intptr_t id) {
  auto o=lv_btn_create(parent);lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,h);
  lv_obj_set_style_pad_all(o,0,0);
  lv_obj_set_style_bg_color(o,lv_color_hex(0x102332),0);lv_obj_set_style_bg_grad_color(o,lv_color_hex(0x030a12),0);
  lv_obj_set_style_bg_grad_dir(o,LV_GRAD_DIR_VER,0);lv_obj_set_style_bg_opa(o,LV_OPA_COVER,0);
  lv_obj_set_style_border_color(o,lv_color_hex(0x37647e),0);
  lv_obj_set_style_border_width(o,1,0);lv_obj_set_style_radius(o,9,0);
  lv_obj_set_style_bg_color(o,lv_color_hex(0x075a7c),LV_STATE_CHECKED);
  lv_obj_set_style_border_color(o,lv_color_hex(cyan),LV_STATE_CHECKED);
  lv_obj_set_style_border_width(o,2,LV_STATE_CHECKED);
  lv_obj_set_style_shadow_color(o,lv_color_hex(cyan),LV_STATE_CHECKED);
  lv_obj_set_style_shadow_width(o,0,LV_STATE_CHECKED);lv_obj_set_style_shadow_opa(o,110,LV_STATE_CHECKED);
  auto l=label(o,text,0,0,w);lv_obj_set_width(l,LV_SIZE_CONTENT);lv_obj_center(l);
  lv_obj_add_event_cb(o,cb,LV_EVENT_CLICKED,reinterpret_cast<void*>(id));return o;
}
void line(lv_draw_ctx_t* ctx,int x,int y,int x2,int y2,uint32_t color,int width=2,lv_opa_t opacity=255) {
  lv_draw_line_dsc_t d;lv_draw_line_dsc_init(&d);d.color=lv_color_hex(color);d.width=width;d.opa=opacity;d.round_start=d.round_end=1;
  lv_point_t a{lv_coord_t(x),lv_coord_t(y)},b{lv_coord_t(x2),lv_coord_t(y2)};lv_draw_line(ctx,&d,&a,&b);
}
lv_obj_t* rasterIcon(lv_obj_t* parent,const lv_img_dsc_t* image,int cx,int cy) {
  auto object=lv_img_create(parent);lv_img_set_src(object,image);
  lv_obj_set_pos(object,cx-image->header.w/2,cy-image->header.h/2);
  lv_obj_set_style_img_recolor_opa(object,LV_OPA_COVER,0);
  lv_obj_clear_flag(object,LV_OBJ_FLAG_CLICKABLE);
  return object;
}
enum RasterRole {Left,Right,Position,LowBeam,HighBeam,Hazard,Warning,Battery,MotorTemperature,ControllerTemperature,Brake};
}
void Ui::drawGaugeOverlay(lv_event_t* event) {
  auto& u=*activeUi;auto ctx=lv_event_get_draw_ctx(event);lv_area_t area;lv_obj_get_coords(lv_event_get_target(event),&area);
  const int ox=area.x1,oy=area.y1;
  // The generated battery frame is raster; only live charge bars are data-driven.
  for(int i=0;i<4;i++) {
    bool on=u.iconFresh_&&u.batteryPercent_>i*25;
    uint32_t color=on?(u.batteryPercent_<20?red:green):0x162633;
    lv_draw_rect_dsc_t d;lv_draw_rect_dsc_init(&d);d.radius=1;d.bg_color=lv_color_hex(color);d.bg_opa=LV_OPA_COVER;d.border_width=0;
    lv_area_t bar{lv_coord_t(ox+25),lv_coord_t(oy+77-i*8),lv_coord_t(ox+38),lv_coord_t(oy+82-i*8)};
    lv_draw_rect(ctx,&d,&bar);
  }
  if(u.iconFresh_) {
    float angle=(150+float(std::min(100,int(u.speedValue_)))*2.4f)*PI/180;
    line(ctx,ox+235+int(cosf(angle)*86),oy+131+int(sinf(angle)*86),ox+235+int(cosf(angle)*105),oy+131+int(sinf(angle)*105),0xffffff,3);
  }
}
void Ui::flush(lv_disp_drv_t* d,const lv_area_t* a,lv_color_t* colors) {
  copyLandscapeToPortrait(canvas.getFramebuffer(),reinterpret_cast<uint16_t*>(colors),a->x1,a->y1,a->x2-a->x1+1,a->y2-a->y1+1,activeUi->config_->screenFlipped);
  // AXS15231B requires complete frame transfers. Canvas holds the physical frame.
  if(lv_disp_flush_is_last(d))canvas.flush();
  lv_disp_flush_ready(d);
}
void Ui::touch(lv_indev_drv_t*,lv_indev_data_t* data) {
  static lv_point_t last{0,0};data->point=last;data->state=LV_INDEV_STATE_REL;
  const uint8_t cmd[]={0xb5,0xab,0xa5,0x5a,0,0,0,8};
  Wire.beginTransmission(board::touchAddress);Wire.write(cmd,sizeof(cmd));
  if(Wire.endTransmission()!=0||Wire.requestFrom(board::touchAddress,uint8_t(8))!=8)return;
  uint8_t p[8];for(auto& x:p)x=Wire.read();
  if(p[1]!=1)return;
  int x=((p[2]&0xf)<<8)|p[3],y=((p[4]&0xf)<<8)|p[5];
  if(x<0||x>=320||y<0||y>=480)return;
  // Match software canvas rotation 1; raw native portrait -> landscape.
  last.x=activeUi->config_->screenFlipped?479-y:y;last.y=activeUi->config_->screenFlipped?x:319-x;data->point=last;data->state=LV_INDEV_STATE_PR;
}
void Ui::buttonEvent(lv_event_t* e) {if(activeUi&&activeUi->sink_)activeUi->sink_(static_cast<UiAction>(reinterpret_cast<intptr_t>(lv_event_get_user_data(e))));}
void Ui::swipe(lv_event_t*) {
  auto input=lv_indev_get_act();if(!input)return;
  auto direction=lv_indev_get_gesture_dir(input);
  if(direction==LV_DIR_LEFT||direction==LV_DIR_RIGHT) {
    lv_indev_wait_release(input);
    activeUi->show((activeUi->page_+(direction==LV_DIR_LEFT?1:2))%3);
  }
}
void Ui::navigation(lv_event_t* e){activeUi->show(reinterpret_cast<intptr_t>(lv_event_get_user_data(e)));}
void Ui::adjustment(lv_event_t* e) {
  auto& u=*activeUi;int code=reinterpret_cast<intptr_t>(lv_event_get_user_data(e));
  int i=abs(code)-1;const auto& setting=timingSettings[i];
  auto& value=u.config_->*(setting.field);
  value=std::max(int(setting.min),std::min(int(setting.max),int(value)+(code>0?int(setting.step):-int(setting.step))));
  if(u.config_->doubleMs>=u.config_->holdMs)u.config_->doubleMs=u.config_->holdMs-100;
  u.refreshSettings();
}
void Ui::flipScreen(lv_event_t*) {
  activeUi->config_->screenFlipped=!activeUi->config_->screenFlipped;
  activeUi->refreshSettings();lv_obj_invalidate(lv_scr_act());
}
void Ui::refreshSettings() {
  if(controlValue_)setLabel(controlValue_,config_->motionEnabled?"Вкл":"Выкл");
  if(flipValue_)setLabel(flipValue_,config_->screenFlipped?"Вкл":"Выкл");
  for(size_t i=0;i<timingSettingCount;i++)textf(settingValues_[i],"%lu мс",static_cast<unsigned long>(config_->*(timingSettings[i].field)));
}
bool Ui::begin(bike::Config& config,UiSink sink) {
  activeUi=this;config_=&config;sink_=sink;
  if(!psramFound())return false;
  pinMode(board::backlight,OUTPUT);digitalWrite(board::backlight,LOW);
  if(!canvas.begin(32000000))return false;
  canvas.setRotation(1);canvas.fillScreen(0);canvas.flush();
  Wire.begin(board::touchSda,board::touchScl,400000);Wire.setTimeOut(10);
  lv_init();
  auto buf=static_cast<lv_color_t*>(heap_caps_malloc(480*64*sizeof(lv_color_t),MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT));
  if(!buf)return false;
  lv_disp_draw_buf_init(&drawBuffer,buf,nullptr,480*64);
  lv_disp_drv_init(&displayDriver);displayDriver.hor_res=480;displayDriver.ver_res=320;
  displayDriver.flush_cb=flush;displayDriver.draw_buf=&drawBuffer;lv_disp_drv_register(&displayDriver);
  lv_indev_drv_init(&inputDriver);inputDriver.type=LV_INDEV_TYPE_POINTER;inputDriver.read_cb=touch;lv_indev_drv_register(&inputDriver);
  auto root=lv_scr_act();lv_obj_set_style_bg_color(root,lv_color_hex(0x040b12),0);
  lv_obj_set_style_text_font(root,&font_ru_14,0);lv_obj_clear_flag(root,LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(root,swipe,LV_EVENT_GESTURE,nullptr);
  auto background=lv_img_create(root);lv_img_set_src(background,&dashboard_background);lv_obj_set_pos(background,0,0);
  status_=label(root,"Загрузка…",8,4,464);
  lv_label_set_long_mode(status_,LV_LABEL_LONG_DOT);lv_obj_set_height(status_,18);lv_obj_set_style_text_color(status_,lv_color_hex(muted),0);
  for(auto& p:pages_) {p=panel(root,0,25,480,295);lv_obj_set_style_bg_opa(p,LV_OPA_TRANSP,0);lv_obj_set_style_border_width(p,0,0);}
  auto main=pages_[0];
  auto b=panel(main,8,29,112,70);label(b,"ЗАРЯД",41,5,70);battery_=label(b,"—",41,21,70,&font_value_32);
  batteryBar_=lv_bar_create(b);lv_obj_set_pos(batteryBar_,41,60);lv_obj_set_size(batteryBar_,61,4);
  lv_obj_set_style_bg_color(batteryBar_,lv_color_hex(green),LV_PART_INDICATOR);
  b=panel(main,8,107,112,100);label(b,"МОТОР",37,4,75);motorTemp_=label(b,"—",37,20,75,&font_ru_20);
  label(b,"КОНТР.",37,53,75);controllerTemp_=label(b,"—",37,69,75,&font_ru_20);
  auto divider=lv_obj_create(b);lv_obj_set_pos(divider,9,49);lv_obj_set_size(divider,94,1);lv_obj_set_style_bg_color(divider,lv_color_hex(0x284559),0);lv_obj_set_style_border_width(divider,0,0);
  arc_=lv_meter_create(main);lv_obj_set_pos(arc_,125,21);lv_obj_set_size(arc_,221,221);
  lv_obj_set_style_bg_opa(arc_,LV_OPA_TRANSP,0);lv_obj_set_style_border_width(arc_,0,0);
  lv_obj_set_style_bg_opa(arc_,LV_OPA_TRANSP,LV_PART_INDICATOR);lv_obj_set_style_border_width(arc_,0,LV_PART_INDICATOR);
  lv_obj_set_style_pad_all(arc_,5,0);lv_obj_set_style_text_font(arc_,&font_ru_14,0);
  lv_obj_set_style_text_color(arc_,lv_color_hex(muted),LV_PART_TICKS);
  auto scale=lv_meter_add_scale(arc_);lv_meter_set_scale_ticks(arc_,scale,51,1,7,lv_color_hex(0x527084));
  lv_meter_set_scale_major_ticks(arc_,scale,10,2,13,lv_color_hex(0xc5dfef),12);
  lv_meter_set_scale_range(arc_,scale,0,100,240,150);
  auto track=lv_meter_add_arc(arc_,scale,9,lv_color_hex(0x203949),0);lv_meter_set_indicator_end_value(arc_,track,100);
  speedIndicator_=lv_meter_add_arc(arc_,scale,9,lv_color_hex(0xff6554),0);
  speed_=label(main,"--",143,76,186,&font_speed_72);lv_obj_set_style_text_align(speed_,LV_TEXT_ALIGN_CENTER,0);
  auto unit=label(main,"КМ/Ч",176,151,120,&font_ru_14);lv_obj_set_style_text_align(unit,LV_TEXT_ALIGN_CENTER,0);
  auto badge=panel(main,185,192,102,29);lv_obj_set_style_border_color(badge,lv_color_hex(cyan),0);
  lv_obj_set_style_border_width(badge,2,0);lv_obj_set_style_shadow_opa(badge,130,0);
  modeBadge_=label(badge,"N ?",0,3,102,&font_ru_20);lv_obj_set_style_text_align(modeBadge_,LV_TEXT_ALIGN_CENTER,0);
  b=button(main,"",351,29,121,102,buttonEvent,int(UiAction::Meter));
  lv_obj_set_style_border_color(b,lv_color_hex(0x447b98),0);
  meterTitle_=label(b,"ПОЕЗДКА",8,12,105);
  meter_=label(b,"0.0",8,36,105,&font_value_32);
  meterUnits_=label(b,"КМ",8,80,105);
  hazardButton_=button(main,"",351,139,121,68,buttonEvent,int(UiAction::Hazard));
  rasterIcons_[Hazard]=rasterIcon(hazardButton_,&icon_hazard,60,34);

  auto warningButton=button(main,"",364,0,40,27,navigation,2);
  lv_obj_set_style_bg_opa(warningButton,LV_OPA_TRANSP,0);lv_obj_set_style_border_width(warningButton,0,0);
  lv_obj_set_style_shadow_width(warningButton,0,0);lv_obj_set_style_outline_width(warningButton,0,0);
  auto footer=panel(main,4,225,472,70);lv_obj_set_style_radius(footer,10,0);
  const char* modes[]={"N","Drive","Sport"};
  const int modeX[]={8,66,144},modeW[]={52,72,72};
  for(int i=0;i<3;i++)modeButtons_[i]=button(main,modes[i],modeX[i],229,modeW[i],62,buttonEvent,i);
  auto separator=lv_obj_create(main);lv_obj_remove_style_all(separator);
  lv_obj_set_pos(separator,225,232);lv_obj_set_size(separator,1,56);
  lv_obj_set_style_bg_color(separator,lv_color_hex(0x447b98),0);lv_obj_set_style_bg_opa(separator,LV_OPA_COVER,0);
  for(auto btn:modeButtons_)lv_obj_set_style_text_font(lv_obj_get_child(btn,0),&font_ru_20,0);
  const char* limits[]={"25","50","70","MAX"};for(int i=0;i<4;i++) {
    limitButtons_[i]=button(main,limits[i],235+i*60,229,55,62,buttonEvent,3+i);
    lv_obj_set_style_text_font(lv_obj_get_child(limitButtons_[i],0),&font_ru_20,0);
    lv_obj_set_style_border_color(limitButtons_[i],lv_color_hex(red),LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(limitButtons_[i],lv_color_hex(0x491b20),LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(limitButtons_[i],lv_color_hex(red),LV_STATE_CHECKED);
  }
  // Seven aligned indicators; turn signals occupy the two outermost cells.
  rasterIcons_[Left]=rasterIcon(main,&icon_left,24,14);
  rasterIcons_[Position]=rasterIcon(main,&icon_position,96,14);
  rasterIcons_[LowBeam]=rasterIcon(main,&icon_low_beam,168,14);
  rasterIcons_[HighBeam]=rasterIcon(main,&icon_high_beam,240,14);
  rasterIcons_[Brake]=rasterIcon(main,&icon_brake,312,14);
  rasterIcons_[Warning]=rasterIcon(main,&icon_warning,384,14);
  rasterIcons_[Right]=rasterIcon(main,&icon_right,456,14);
  rasterIcons_[Battery]=rasterIcon(main,&icon_battery,32,64);
  rasterIcons_[MotorTemperature]=rasterIcon(main,&icon_thermometer,28,131);
  rasterIcons_[ControllerTemperature]=rasterIcon(main,&icon_thermometer,28,180);
  decorations_=lv_obj_create(main);lv_obj_remove_style_all(decorations_);lv_obj_set_pos(decorations_,0,0);lv_obj_set_size(decorations_,480,232);
  lv_obj_clear_flag(decorations_,LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(decorations_,drawGaugeOverlay,LV_EVENT_DRAW_MAIN,nullptr);

  auto settings=pages_[1];lv_obj_set_style_bg_opa(settings,235,0);label(settings,"НАСТРОЙКИ",10,3,300,&font_ru_20);
  auto list=panel(settings,0,32,480,211);lv_obj_add_flag(list,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_scroll_dir(list,LV_DIR_VER);
  lv_obj_set_style_border_width(list,0,0);lv_obj_set_style_bg_opa(list,LV_OPA_TRANSP,0);
  for(size_t i=0;i<timingSettingCount;i++) {
    int y=i*43;label(list,timingSettings[i].label,12,y+9,245);
    button(list,"-",267,y,42,36,adjustment,-int(i+1));
    settingValues_[i]=label(list,"",316,y+9,100);
    button(list,"+",426,y,42,36,adjustment,i+1);
  }
  label(list,"Перевернуть экран",12,int(timingSettingCount)*43+9,245);
  auto flip=button(list,"Выкл",316,int(timingSettingCount)*43,152,36,flipScreen,0);
  flipValue_=lv_obj_get_child(flip,0);
  int controlY=int(timingSettingCount+1)*43;
  label(list,"Управление VESC",12,controlY+9,245);
  auto control=button(list,"Выкл",316,controlY,152,36,buttonEvent,int(UiAction::ToggleControl));
  controlValue_=lv_obj_get_child(control,0);
  label(settings,"Изменения действуют сразу",12,261,290);
  button(settings,"Сохранить",335,253,133,38,buttonEvent,int(UiAction::Save));refreshSettings();
  auto diagnostics=pages_[2];lv_obj_add_flag(diagnostics,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_scroll_dir(diagnostics,LV_DIR_VER);lv_obj_set_style_bg_opa(diagnostics,235,0);
  diag_=label(diagnostics,"",10,5,460);
  show(0);digitalWrite(board::backlight,HIGH);return true;
}
void Ui::show(uint8_t index){page_=index%3;for(int i=0;i<3;i++){if(i==page_)lv_obj_clear_flag(pages_[i],LV_OBJ_FLAG_HIDDEN);else lv_obj_add_flag(pages_[i],LV_OBJ_FLAG_HIDDEN);}refreshSettings();}
void Ui::notice(const String& s){if(status_)setLabel(status_,s.c_str());}
void Ui::tick(){lv_timer_handler();}
void Ui::update(const bike::State& s,const bike::Telemetry& t,uint32_t now,bool io,bool sd,bool brake,const String& diagnostics,bool confirmed,bool applying) {
  if(!speed_)return;
  bool fresh=t.fresh(now);
  if(fresh) {
    textf(speed_,"%d",int(lroundf(fabsf(t.speedMs)*3.6f)));
    int value=std::min(100,int(fabsf(t.speedMs)*3.6f));
    if(speedIndicator_->end_value!=value)lv_meter_set_indicator_end_value(arc_,speedIndicator_,value);
    textf(battery_,"%d%%",int(lroundf(t.soc*100)));
    lv_bar_set_value(batteryBar_,int(t.soc*100),LV_ANIM_OFF);
    textf(motorTemp_,"%.0f°C",double(t.motor));textf(controllerTemp_,"%.0f°C",double(t.fet));
  } else {setLabel(speed_,"--");if(speedIndicator_->end_value)lv_meter_set_indicator_end_value(arc_,speedIndicator_,0);lv_bar_set_value(batteryBar_,0,LV_ANIM_OFF);setLabel(battery_,"—");setLabel(motorTemp_,"—");setLabel(controllerTemp_,"—");}
  if(s.meterPage==0){setLabel(meterTitle_,"ПОЕЗДКА");setLabel(meterUnits_,"КМ");textf(meter_,"%.1f",s.tripMeters/1000);}
  if(s.meterPage==1){setLabel(meterTitle_,"ОБЩИЙ");setLabel(meterUnits_,"КМ");textf(meter_,"%.1f",s.totalMeters/1000);}
  if(s.meterPage==2){setLabel(meterTitle_,"ВРЕМЯ");setLabel(meterUnits_,"ЧАС : МИН");textf(meter_,"%02lu:%02lu",static_cast<unsigned long>(now/3600000),static_cast<unsigned long>((now/60000)%60));}
  uint8_t o=s.outputs(now,*config_);
  // Invalidate only the old/new marker strokes and battery bars, not the
  // transparent decoration object's entire 480x232 area on every speed change.
  lv_area_t overlay;lv_obj_get_coords(decorations_,&overlay);
  auto invalidateMarker=[&](float speed) {
    float angle=(150+float(std::min(100,int(speed)))*2.4f)*PI/180;
    int x1=235+int(cosf(angle)*86),y1=131+int(sinf(angle)*86);
    int x2=235+int(cosf(angle)*105),y2=131+int(sinf(angle)*105);
    lv_area_t area{lv_coord_t(overlay.x1+std::min(x1,x2)-4),lv_coord_t(overlay.y1+std::min(y1,y2)-4),
                   lv_coord_t(overlay.x1+std::max(x1,x2)+4),lv_coord_t(overlay.y1+std::max(y1,y2)+4)};
    lv_obj_invalidate_area(decorations_,&area);
  };
  float newSpeed=fabsf(t.speedMs)*3.6f;
  if(iconFresh_!=fresh||int(speedValue_)!=int(newSpeed)) {
    if(iconFresh_)invalidateMarker(speedValue_);
    if(fresh)invalidateMarker(newSpeed);
  }
  if(iconFresh_!=fresh||batteryPercent_!=int(t.soc*100)) {
    lv_area_t area{lv_coord_t(overlay.x1+24),lv_coord_t(overlay.y1+52),lv_coord_t(overlay.x1+39),lv_coord_t(overlay.y1+83)};
    lv_obj_invalidate_area(decorations_,&area);
  }
  iconFresh_=fresh;
  auto tint=[&](RasterRole role,uint32_t color){if(lv_obj_get_style_img_recolor(rasterIcons_[role],0).full!=lv_color_hex(color).full)lv_obj_set_style_img_recolor(rasterIcons_[role],lv_color_hex(color),0);};
  constexpr uint32_t inactive=0x354b5a;
  tint(Left,o&32?green:inactive);tint(Right,o&16?green:inactive);
  tint(Position,o&1?green:inactive);tint(LowBeam,o&2?0xe3f3ff:inactive);tint(HighBeam,o&4?cyan:inactive);
  tint(Hazard,s.turn==bike::Turn::Hazard?red:0xd1aa76);
  tint(Warning,s.faults||t.fault||!io||!fresh?0xffb25a:inactive);
  tint(Brake,brake?red:inactive);
  tint(Battery,fresh?0xbdd9e8:inactive);
  tint(MotorTemperature,fresh?0xff9c60:inactive);tint(ControllerTemperature,fresh?0xff9c60:inactive);
  batteryPercent_=int(t.soc*100);speedValue_=newSpeed;
  textf(modeBadge_,"%s%s",s.cruise?"КРУИЗ":s.mode==bike::Mode::N?"N":s.mode==bike::Mode::Drive?"Drive":"Sport",confirmed||!config_->motionEnabled?"":applying?"…":" !");
  if(s.turn==bike::Turn::Hazard)lv_obj_add_state(hazardButton_,LV_STATE_CHECKED);else lv_obj_clear_state(hazardButton_,LV_STATE_CHECKED);
  for(int i=0;i<3;i++)if(i==int(s.mode))lv_obj_add_state(modeButtons_[i],LV_STATE_CHECKED);else lv_obj_clear_state(modeButtons_[i],LV_STATE_CHECKED);
  const int lim[]={25,50,70,0};
  for(int i=0;i<4;i++) {
    bool hidden=s.safe&&i>=2;
    if(hidden!=lv_obj_has_flag(limitButtons_[i],LV_OBJ_FLAG_HIDDEN)) {
      if(hidden)lv_obj_add_flag(limitButtons_[i],LV_OBJ_FLAG_HIDDEN);else lv_obj_clear_flag(limitButtons_[i],LV_OBJ_FLAG_HIDDEN);
    }
    if(s.limit==lim[i])lv_obj_add_state(limitButtons_[i],LV_STATE_CHECKED);else lv_obj_clear_state(limitButtons_[i],LV_STATE_CHECKED);
  }
  if(page_!=2)return;
  String diagnosticText=String("VESC: ")+(fresh?"связь есть":"нет связи")+"\nSD: "+(sd?"готова":"нет сохранения")+"\nПлата I/O: "+(io?"готова":"нет связи")+"\nБезопасный режим: "+(s.safe?"вкл":"выкл")+"\n"+diagnostics;
  setLabel(diag_,diagnosticText.c_str());
}
