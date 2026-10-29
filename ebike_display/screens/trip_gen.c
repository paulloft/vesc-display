/**
 * @file trip_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "trip_gen.h"
#include "../ebike_display.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * trip_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if EBIKE_DISPLAY_CHECK_COMPILE_TARGET(EBIKE_DISPLAY_TARGET_ALL)
    if (ebike_display_check_target(EBIKE_DISPLAY_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "trip_#");
        lv_obj_set_width(lv_obj_0, 480);
        lv_obj_set_height(lv_obj_0, 320);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_bg_color(lv_obj_0, COLOR_BG, 0);

        lv_obj_t * trip_view = trip_view_create(lv_obj_0);
        lv_obj_set_name(trip_view, "trip_view");

        lv_obj_t * standalone_status = lv_label_create(lv_obj_0);
        lv_obj_set_name(standalone_status, "standalone_status");
        lv_label_set_text(standalone_status, "ПРЕДПРОСМОТР · тестовые показания");
        lv_obj_set_x(standalone_status, 8);
        lv_obj_set_y(standalone_status, 3);
        lv_obj_set_width(standalone_status, 464);
        lv_obj_set_style_text_font(standalone_status, ui_small, 0);
        lv_obj_set_style_text_color(standalone_status, COLOR_MUTED, 0);
        lv_obj_set_flag(standalone_status, LV_OBJ_FLAG_CLICKABLE, false);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

