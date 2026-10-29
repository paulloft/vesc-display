/**
 * @file ebike_display.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "ebike_display.h"
#include "fonts/ui_value_metrics.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ebike_display_init(const char * asset_path)
{
    LV_LOG("Initializing custom C code using LVGL v%d.%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

    ebike_display_init_gen(asset_path);

    #ifndef LV_EDITOR_PREVIEW
    /* The exporter trims line metrics to the glyph subset. Match the TTF
     * metrics used by the editor without writing into its const font in flash. */
    static lv_font_t value_font;
    value_font = *ui_value;
    value_font.line_height = EBIKE_VALUE_LINE_HEIGHT;
    value_font.base_line = EBIKE_VALUE_BASE_LINE;
    ui_value = &value_font;
    static lv_font_t speed_font;
    speed_font = *ui_speed;
    speed_font.line_height = EBIKE_SPEED_LINE_HEIGHT;
    speed_font.base_line = EBIKE_SPEED_BASE_LINE;
    ui_speed = &speed_font;
    #endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/