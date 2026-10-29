/**
 * @file diagnostics_view_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "diagnostics_view_gen.h"
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

lv_obj_t * diagnostics_view_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if EBIKE_DISPLAY_CHECK_COMPILE_TARGET(EBIKE_DISPLAY_TARGET_ALL)
    if (ebike_display_check_target(EBIKE_DISPLAY_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "diagnostics_view_#");
        lv_obj_set_width(lv_obj_0, 480);
        lv_obj_set_height(lv_obj_0, 320);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, true);
        lv_obj_set_style_text_font(lv_obj_0, ui_small, 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_0, COLOR_BG, 0);

        lv_obj_add_style(lv_obj_0, &clear, 0);
        lv_obj_t * diagnostics_text = lv_label_create(lv_obj_0);
        lv_obj_set_name(diagnostics_text, "diagnostics_text");
        lv_label_set_text(diagnostics_text, "VESC: нет подключения в редакторе\nSD: демонстрация\nПлата I/O: демонстрация\nБезопасный режим: вкл\nUART: 115200 бод\nПульт и нагрузки: данные поступят от ESP32");
        lv_obj_set_x(diagnostics_text, 12);
        lv_obj_set_y(diagnostics_text, 32);
        lv_obj_set_width(diagnostics_text, 456);
        lv_obj_set_style_text_font(diagnostics_text, ui_small, 0);
        lv_obj_set_style_text_color(diagnostics_text, COLOR_TEXT, 0);
        lv_obj_set_flag(diagnostics_text, LV_OBJ_FLAG_CLICKABLE, false);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

