/**
 * @file app_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "app_gen.h"
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

lv_obj_t * app_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if EBIKE_DISPLAY_CHECK_COMPILE_TARGET(EBIKE_DISPLAY_TARGET_ALL)
    if (ebike_display_check_target(EBIKE_DISPLAY_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "app_#");
        lv_obj_set_width(lv_obj_0, 480);
        lv_obj_set_height(lv_obj_0, 320);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_bg_color(lv_obj_0, COLOR_BG, 0);
        lv_obj_set_style_text_font(lv_obj_0, ui_small, 0);

        lv_obj_t * app_pages = lv_tabview_create(lv_obj_0);
        lv_obj_set_name(app_pages, "app_pages");
        lv_obj_set_x(app_pages, 0);
        lv_obj_set_y(app_pages, 0);
        lv_obj_set_width(app_pages, 480);
        lv_obj_set_height(app_pages, 320);
        lv_obj_add_style(app_pages, &clear, 0);
        lv_obj_t * lv_tabview_tab_bar_0 = lv_tabview_get_tab_bar(app_pages);
        lv_obj_set_height(lv_tabview_tab_bar_0, 0);
        lv_obj_set_flag(lv_tabview_tab_bar_0, LV_OBJ_FLAG_HIDDEN, true);
        lv_obj_set_style_pad_all(lv_tabview_tab_bar_0, 0, 0);
        lv_obj_set_style_border_width(lv_tabview_tab_bar_0, 0, 0);
        lv_obj_t * app_page_0 = lv_tabview_add_tab(app_pages, "");
        lv_obj_set_name(app_page_0, "app_page_0");
        lv_obj_set_flag(app_page_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(app_page_0, &clear, 0);
        lv_obj_t * page_view_0 = dashboard_view_create(app_page_0);
        lv_obj_set_name(page_view_0, "page_view_0");

        lv_obj_t * app_page_1 = lv_tabview_add_tab(app_pages, "");
        lv_obj_set_name(app_page_1, "app_page_1");
        lv_obj_set_flag(app_page_1, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(app_page_1, &clear, 0);
        lv_obj_t * page_view_1 = settings_view_create(app_page_1);
        lv_obj_set_name(page_view_1, "page_view_1");

        lv_obj_t * app_page_2 = lv_tabview_add_tab(app_pages, "");
        lv_obj_set_name(app_page_2, "app_page_2");
        lv_obj_set_flag(app_page_2, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(app_page_2, &clear, 0);
        lv_obj_t * page_view_2 = diagnostics_view_create(app_page_2);
        lv_obj_set_name(page_view_2, "page_view_2");

        lv_obj_t * app_page_3 = lv_tabview_add_tab(app_pages, "");
        lv_obj_set_name(app_page_3, "app_page_3");
        lv_obj_set_flag(app_page_3, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(app_page_3, &clear, 0);
        lv_obj_t * page_view_3 = trip_view_create(app_page_3);
        lv_obj_set_name(page_view_3, "page_view_3");

        lv_obj_t * status_line = lv_label_create(lv_obj_0);
        lv_obj_set_name(status_line, "status_line");
        lv_label_set_text(status_line, "ПРЕДПРОСМОТР · тестовые показания");
        lv_obj_set_x(status_line, 8);
        lv_obj_set_y(status_line, 231);
        lv_obj_set_width(status_line, 464);
        lv_obj_set_style_text_font(status_line, ui_small, 0);
        lv_obj_set_style_text_color(status_line, COLOR_MUTED, 0);
        lv_obj_set_flag(status_line, LV_OBJ_FLAG_CLICKABLE, false);
        lv_label_bind_text(status_line, &ui_notice, NULL);
        lv_obj_set_height(status_line, 18);
        lv_label_set_long_mode(status_line, LV_LABEL_LONG_MODE_DOTS);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

