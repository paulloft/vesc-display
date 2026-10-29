/**
 * @file ebike_display_gen.h
 */

#ifndef LVGL_PRO_EBIKE_DISPLAY_GEN_H
#define LVGL_PRO_EBIKE_DISPLAY_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif

#if defined(LV_USE_XML) && LV_USE_XML
    #include "lv_xml/lv_xml.h"
#endif



/* Prototypes for target functions, needed by responsive const definitions */

void ebike_display_set_target(uint32_t target);
uint32_t ebike_display_get_target(void);
bool ebike_display_check_target(uint32_t target);

/*********************
 *      DEFINES
 *********************/

#define EBIKE_DISPLAY_TARGET_UNDEFINED  (0 << 1)
#define EBIKE_DISPLAY_TARGET_TARGET1    (1 << 1)
#define EBIKE_DISPLAY_TARGET_ALL        0x0FFFFFFF

/* By default compile for all targets, allowing to switch to any targets at runtime */
#ifndef EBIKE_DISPLAY_COMPILE_TARGET
#define EBIKE_DISPLAY_COMPILE_TARGET EBIKE_DISPLAY_TARGET_ALL
#endif

#define EBIKE_DISPLAY_CHECK_COMPILE_TARGET(target) (EBIKE_DISPLAY_COMPILE_TARGET & (target) ? 1 : 0)

#define COLOR_BG lv_color_hex(0x06121e)
#define COLOR_TEXT lv_color_hex(0xe9f4ff)
#define COLOR_MUTED lv_color_hex(0x90a9bc)
#define COLOR_CYAN lv_color_hex(0x27c4f5)
#define COLOR_RED lv_color_hex(0xff6554)


#ifndef LV_XML_EVAL_STRING_BUF_SIZE
    #define LV_XML_EVAL_STRING_BUF_SIZE 256
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

/*----------------
 * Global styles
 *----------------*/

extern lv_style_t card;
extern lv_style_t button;
extern lv_style_t selected;
extern lv_style_t limit_selected;
extern lv_style_t battery_fill_style;
extern lv_style_t arc_indicator;
extern lv_style_t arc_knob;
extern lv_style_t minor_ticks;
extern lv_style_t major_ticks;
extern lv_style_t dot_selected_0;
extern lv_style_t dot_selected_1;
extern lv_style_t dot_selected_2;
extern lv_style_t clear;
extern lv_style_t switch_track;
extern lv_style_t switch_indicator;
extern lv_style_t switch_on;
extern lv_style_t switch_knob;

/*----------------
 * Fonts
 *----------------*/

/* Targets: any */
extern lv_font_t * ui_small;
extern lv_font_t * ui_medium;
extern lv_font_t * ui_value;
extern lv_font_t * ui_speed;


/*----------------
 * Images
 *----------------*/

/* Targets: any */
extern const void * ui_img_background;
extern const void * ui_img_left;
extern const void * ui_img_right;
extern const void * ui_img_position;
extern const void * ui_img_low_beam;
extern const void * ui_img_high_beam;
extern const void * ui_img_brake;
extern const void * ui_img_warning;
extern const void * ui_img_hazard;
extern const void * ui_img_battery;
extern const void * ui_img_thermometer;
extern const void * ui_img_switch_track;
extern const void * ui_img_switch_knob;

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t ui_speed_value;
extern lv_subject_t ui_battery;
extern lv_subject_t ui_mode;
extern lv_subject_t ui_limit;
extern lv_subject_t ui_meter;
extern lv_subject_t ui_hazard;
extern lv_subject_t ui_blink_on;
extern lv_subject_t ui_blink_off;
extern lv_subject_t ui_hold;
extern lv_subject_t ui_double;
extern lv_subject_t ui_flip;
extern lv_subject_t ui_motion;
extern lv_subject_t ui_cruise;
extern lv_subject_t ui_safe;
extern lv_subject_t ui_mode_text;
extern lv_subject_t ui_notice;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

/**
 * Initialize the component library
 */

void ebike_display_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widgets, components and screens of this library*/
#include "components/dashboard_view_gen.h"
#include "components/diagnostics_view_gen.h"
#include "components/settings_view_gen.h"
#include "components/trip_view_gen.h"
#include "screens/app_gen.h"
#include "screens/dashboard_gen.h"
#include "screens/diagnostics_gen.h"
#include "screens/settings_gen.h"
#include "screens/trip_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_EBIKE_DISPLAY_GEN_H*/