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
extern lv_style_t battery_fill;
extern lv_style_t arc_indicator;
extern lv_style_t arc_knob;
extern lv_style_t minor_ticks;
extern lv_style_t major_ticks;
extern lv_style_t dot_selected_0;
extern lv_style_t dot_selected_1;
extern lv_style_t dot_selected_2;

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
extern const void * dashboard_background;
extern const void * icon_left;
extern const void * icon_right;
extern const void * icon_position;
extern const void * icon_low_beam;
extern const void * icon_high_beam;
extern const void * icon_brake;
extern const void * icon_warning;
extern const void * icon_hazard;
extern const void * icon_battery;
extern const void * icon_thermometer;

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t demo_speed;
extern lv_subject_t demo_battery;
extern lv_subject_t demo_mode;
extern lv_subject_t demo_limit;
extern lv_subject_t demo_meter;
extern lv_subject_t demo_hazard;
extern lv_subject_t demo_blink_on;
extern lv_subject_t demo_blink_off;
extern lv_subject_t demo_hold;
extern lv_subject_t demo_double;

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
#include "screens/dashboard_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_EBIKE_DISPLAY_GEN_H*/