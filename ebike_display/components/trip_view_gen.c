/**
 * @file trip_view_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "trip_view_gen.h"
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

lv_obj_t * trip_view_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if EBIKE_DISPLAY_CHECK_COMPILE_TARGET(EBIKE_DISPLAY_TARGET_ALL)
    if (ebike_display_check_target(EBIKE_DISPLAY_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "trip_view_#");
        lv_obj_set_width(lv_obj_0, 480);
        lv_obj_set_height(lv_obj_0, 320);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_text_font(lv_obj_0, ui_small, 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_0, COLOR_BG, 0);

        lv_obj_add_style(lv_obj_0, &clear, 0);
        lv_obj_t * trip_title = lv_label_create(lv_obj_0);
        lv_obj_set_name(trip_title, "trip_title");
        lv_label_set_text(trip_title, "ТЕКУЩАЯ ПОЕЗДКА");
        lv_obj_set_x(trip_title, 12);
        lv_obj_set_y(trip_title, 28);
        lv_obj_set_width(trip_title, 456);
        lv_obj_set_style_text_font(trip_title, ui_medium, 0);
        lv_obj_set_style_text_color(trip_title, COLOR_TEXT, 0);
        lv_obj_set_flag(trip_title, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * stats_card_0 = lv_obj_create(lv_obj_0);
        lv_obj_set_name(stats_card_0, "stats_card_0");
        lv_obj_set_x(stats_card_0, 12);
        lv_obj_set_y(stats_card_0, 59);
        lv_obj_set_width(stats_card_0, 108);
        lv_obj_set_height(stats_card_0, 62);
        lv_obj_set_flag(stats_card_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(stats_card_0, &card, 0);
        lv_obj_t * stats_caption_0 = lv_label_create(stats_card_0);
        lv_obj_set_name(stats_caption_0, "stats_caption_0");
        lv_label_set_text(stats_caption_0, "МАКС. КМ/Ч");
        lv_obj_set_x(stats_caption_0, 0);
        lv_obj_set_y(stats_caption_0, 5);
        lv_obj_set_width(stats_caption_0, 106);
        lv_obj_set_style_text_font(stats_caption_0, ui_small, 0);
        lv_obj_set_style_text_color(stats_caption_0, COLOR_TEXT, 0);
        lv_obj_set_flag(stats_caption_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(stats_caption_0, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * stats_value_0 = lv_label_create(stats_card_0);
        lv_obj_set_name(stats_value_0, "stats_value_0");
        lv_label_set_text(stats_value_0, "46");
        lv_obj_set_x(stats_value_0, 0);
        lv_obj_set_y(stats_value_0, 26);
        lv_obj_set_width(stats_value_0, 106);
        lv_obj_set_style_text_font(stats_value_0, ui_medium, 0);
        lv_obj_set_style_text_color(stats_value_0, COLOR_TEXT, 0);
        lv_obj_set_flag(stats_value_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(stats_value_0, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * stats_card_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_name(stats_card_1, "stats_card_1");
        lv_obj_set_x(stats_card_1, 128);
        lv_obj_set_y(stats_card_1, 59);
        lv_obj_set_width(stats_card_1, 108);
        lv_obj_set_height(stats_card_1, 62);
        lv_obj_set_flag(stats_card_1, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(stats_card_1, &card, 0);
        lv_obj_t * stats_caption_1 = lv_label_create(stats_card_1);
        lv_obj_set_name(stats_caption_1, "stats_caption_1");
        lv_label_set_text(stats_caption_1, "ПРОБЕГ, КМ");
        lv_obj_set_x(stats_caption_1, 0);
        lv_obj_set_y(stats_caption_1, 5);
        lv_obj_set_width(stats_caption_1, 106);
        lv_obj_set_style_text_font(stats_caption_1, ui_small, 0);
        lv_obj_set_style_text_color(stats_caption_1, COLOR_TEXT, 0);
        lv_obj_set_flag(stats_caption_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(stats_caption_1, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * stats_value_1 = lv_label_create(stats_card_1);
        lv_obj_set_name(stats_value_1, "stats_value_1");
        lv_label_set_text(stats_value_1, "24.8");
        lv_obj_set_x(stats_value_1, 0);
        lv_obj_set_y(stats_value_1, 26);
        lv_obj_set_width(stats_value_1, 106);
        lv_obj_set_style_text_font(stats_value_1, ui_medium, 0);
        lv_obj_set_style_text_color(stats_value_1, COLOR_TEXT, 0);
        lv_obj_set_flag(stats_value_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(stats_value_1, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * stats_card_2 = lv_obj_create(lv_obj_0);
        lv_obj_set_name(stats_card_2, "stats_card_2");
        lv_obj_set_x(stats_card_2, 244);
        lv_obj_set_y(stats_card_2, 59);
        lv_obj_set_width(stats_card_2, 108);
        lv_obj_set_height(stats_card_2, 62);
        lv_obj_set_flag(stats_card_2, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(stats_card_2, &card, 0);
        lv_obj_t * stats_caption_2 = lv_label_create(stats_card_2);
        lv_obj_set_name(stats_caption_2, "stats_caption_2");
        lv_label_set_text(stats_caption_2, "РАСХОД, Втч");
        lv_obj_set_x(stats_caption_2, 0);
        lv_obj_set_y(stats_caption_2, 5);
        lv_obj_set_width(stats_caption_2, 106);
        lv_obj_set_style_text_font(stats_caption_2, ui_small, 0);
        lv_obj_set_style_text_color(stats_caption_2, COLOR_TEXT, 0);
        lv_obj_set_flag(stats_caption_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(stats_caption_2, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * stats_value_2 = lv_label_create(stats_card_2);
        lv_obj_set_name(stats_value_2, "stats_value_2");
        lv_label_set_text(stats_value_2, "132.4");
        lv_obj_set_x(stats_value_2, 0);
        lv_obj_set_y(stats_value_2, 26);
        lv_obj_set_width(stats_value_2, 106);
        lv_obj_set_style_text_font(stats_value_2, ui_medium, 0);
        lv_obj_set_style_text_color(stats_value_2, COLOR_TEXT, 0);
        lv_obj_set_flag(stats_value_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(stats_value_2, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * stats_card_3 = lv_obj_create(lv_obj_0);
        lv_obj_set_name(stats_card_3, "stats_card_3");
        lv_obj_set_x(stats_card_3, 360);
        lv_obj_set_y(stats_card_3, 59);
        lv_obj_set_width(stats_card_3, 108);
        lv_obj_set_height(stats_card_3, 62);
        lv_obj_set_flag(stats_card_3, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(stats_card_3, &card, 0);
        lv_obj_t * stats_caption_3 = lv_label_create(stats_card_3);
        lv_obj_set_name(stats_caption_3, "stats_caption_3");
        lv_label_set_text(stats_caption_3, "ЧАС : МИН");
        lv_obj_set_x(stats_caption_3, 0);
        lv_obj_set_y(stats_caption_3, 5);
        lv_obj_set_width(stats_caption_3, 106);
        lv_obj_set_style_text_font(stats_caption_3, ui_small, 0);
        lv_obj_set_style_text_color(stats_caption_3, COLOR_TEXT, 0);
        lv_obj_set_flag(stats_caption_3, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(stats_caption_3, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * stats_value_3 = lv_label_create(stats_card_3);
        lv_obj_set_name(stats_value_3, "stats_value_3");
        lv_label_set_text(stats_value_3, "01:23");
        lv_obj_set_x(stats_value_3, 0);
        lv_obj_set_y(stats_value_3, 26);
        lv_obj_set_width(stats_value_3, 106);
        lv_obj_set_style_text_font(stats_value_3, ui_medium, 0);
        lv_obj_set_style_text_color(stats_value_3, COLOR_TEXT, 0);
        lv_obj_set_flag(stats_value_3, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(stats_value_3, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * chart_caption = lv_label_create(lv_obj_0);
        lv_obj_set_name(chart_caption, "chart_caption");
        lv_label_set_text(chart_caption, "СКОРОСТЬ · ПОСЛЕДНЯЯ МИНУТА");
        lv_obj_set_x(chart_caption, 14);
        lv_obj_set_y(chart_caption, 136);
        lv_obj_set_width(chart_caption, 450);
        lv_obj_set_style_text_font(chart_caption, ui_small, 0);
        lv_obj_set_style_text_color(chart_caption, COLOR_MUTED, 0);
        lv_obj_set_flag(chart_caption, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * speed_chart = lv_chart_create(lv_obj_0);
        lv_obj_set_name(speed_chart, "speed_chart");
        lv_obj_set_x(speed_chart, 12);
        lv_obj_set_y(speed_chart, 162);
        lv_obj_set_width(speed_chart, 456);
        lv_obj_set_height(speed_chart, 141);
        lv_chart_set_type(speed_chart, LV_CHART_TYPE_LINE);
        lv_chart_set_point_count(speed_chart, 60);
        lv_chart_set_hor_div_line_count(speed_chart, 5);
        lv_chart_set_ver_div_line_count(speed_chart, 6);
        lv_obj_set_style_bg_color(speed_chart, lv_color_hex(0x071622), 0);
        lv_obj_set_style_bg_opa(speed_chart, (255 * 100 / 100), 0);
        lv_obj_set_style_border_color(speed_chart, lv_color_hex(0x294e63), 0);
        lv_obj_set_style_border_width(speed_chart, 1, 0);
        lv_obj_set_style_radius(speed_chart, 10, 0);
        lv_obj_set_style_line_color(speed_chart, lv_color_hex(0x294456), 0);
        lv_obj_set_style_line_opa(speed_chart, (255 * 70 / 100), 0);
        lv_obj_set_style_pad_all(speed_chart, 10, 0);
        lv_obj_set_style_width(speed_chart, 0, LV_PART_INDICATOR);
        lv_obj_set_style_height(speed_chart, 0, LV_PART_INDICATOR);
        lv_obj_set_style_line_width(speed_chart, 2, LV_PART_ITEMS);
        lv_chart_set_axis_min_value(speed_chart, LV_CHART_AXIS_PRIMARY_Y, 0);
        lv_chart_set_axis_max_value(speed_chart, LV_CHART_AXIS_PRIMARY_Y, 100);
        lv_chart_series_t * lv_chart_series_0 = lv_chart_add_series(speed_chart, COLOR_CYAN, LV_CHART_AXIS_PRIMARY_Y);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

