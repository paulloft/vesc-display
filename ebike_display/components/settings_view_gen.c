/**
 * @file settings_view_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "settings_view_gen.h"
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

lv_obj_t * settings_view_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if EBIKE_DISPLAY_CHECK_COMPILE_TARGET(EBIKE_DISPLAY_TARGET_ALL)
    if (ebike_display_check_target(EBIKE_DISPLAY_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "settings_view_#");
        lv_obj_set_width(lv_obj_0, 480);
        lv_obj_set_height(lv_obj_0, 320);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_text_font(lv_obj_0, ui_small, 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_0, COLOR_BG, 0);

        lv_obj_add_style(lv_obj_0, &clear, 0);
        lv_obj_t * settings_title = lv_label_create(lv_obj_0);
        lv_obj_set_name(settings_title, "settings_title");
        lv_label_set_text(settings_title, "НАСТРОЙКИ");
        lv_obj_set_x(settings_title, 12);
        lv_obj_set_y(settings_title, 28);
        lv_obj_set_width(settings_title, 300);
        lv_obj_set_style_text_font(settings_title, ui_medium, 0);
        lv_obj_set_style_text_color(settings_title, COLOR_TEXT, 0);
        lv_obj_set_flag(settings_title, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * settings_list = lv_obj_create(lv_obj_0);
        lv_obj_set_name(settings_list, "settings_list");
        lv_obj_set_x(settings_list, 4);
        lv_obj_set_y(settings_list, 57);
        lv_obj_set_width(settings_list, 472);
        lv_obj_set_height(settings_list, 211);
        lv_obj_set_flag(settings_list, LV_OBJ_FLAG_SCROLLABLE, true);
        lv_obj_add_style(settings_list, &clear, 0);
        lv_obj_t * timing_title_0 = lv_label_create(settings_list);
        lv_obj_set_name(timing_title_0, "timing_title_0");
        lv_label_set_text(timing_title_0, "Поворотник: включён");
        lv_obj_set_x(timing_title_0, 12);
        lv_obj_set_y(timing_title_0, 16);
        lv_obj_set_width(timing_title_0, 210);
        lv_obj_set_style_text_font(timing_title_0, ui_small, 0);
        lv_obj_set_style_text_color(timing_title_0, COLOR_TEXT, 0);
        lv_obj_set_flag(timing_title_0, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * timing_value_0 = lv_label_create(settings_list);
        lv_obj_set_name(timing_value_0, "timing_value_0");
        lv_label_set_text(timing_value_0, "450 мс");
        lv_obj_set_x(timing_value_0, 217);
        lv_obj_set_y(timing_value_0, 16);
        lv_obj_set_width(timing_value_0, 105);
        lv_obj_set_style_text_font(timing_value_0, ui_small, 0);
        lv_obj_set_style_text_color(timing_value_0, COLOR_CYAN, 0);
        lv_obj_set_flag(timing_value_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_label_bind_text(timing_value_0, &ui_blink_on, "%d мс");
        lv_obj_set_style_text_align(timing_value_0, LV_TEXT_ALIGN_RIGHT, 0);

        lv_obj_t * timing_minus_0 = lv_button_create(settings_list);
        lv_obj_set_name(timing_minus_0, "timing_minus_0");
        lv_obj_set_x(timing_minus_0, 339);
        lv_obj_set_y(timing_minus_0, 8);
        lv_obj_set_width(timing_minus_0, 52);
        lv_obj_set_height(timing_minus_0, 32);
        lv_obj_set_flag(timing_minus_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(timing_minus_0, &button, 0);
        lv_obj_t * timing_minus_0_text = lv_label_create(timing_minus_0);
        lv_obj_set_name(timing_minus_0_text, "timing_minus_0_text");
        lv_label_set_text(timing_minus_0_text, "-");
        lv_obj_set_x(timing_minus_0_text, 0);
        lv_obj_set_y(timing_minus_0_text, 0);
        lv_obj_set_width(timing_minus_0_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(timing_minus_0_text, ui_medium, 0);
        lv_obj_set_style_text_color(timing_minus_0_text, COLOR_TEXT, 0);
        lv_obj_set_flag(timing_minus_0_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(timing_minus_0_text, LV_ALIGN_CENTER);

        lv_subject_increment_dsc_t * subject_increment_event_0 = lv_obj_add_subject_increment_event(timing_minus_0, &ui_blink_on, LV_EVENT_CLICKED, -50);
        lv_obj_set_subject_increment_event_min_value(timing_minus_0, subject_increment_event_0, 100);
        lv_obj_set_subject_increment_event_max_value(timing_minus_0, subject_increment_event_0, 2000);
        lv_obj_set_subject_increment_event_rollover(timing_minus_0, subject_increment_event_0, false);

        lv_obj_t * timing_plus_0 = lv_button_create(settings_list);
        lv_obj_set_name(timing_plus_0, "timing_plus_0");
        lv_obj_set_x(timing_plus_0, 400);
        lv_obj_set_y(timing_plus_0, 8);
        lv_obj_set_width(timing_plus_0, 52);
        lv_obj_set_height(timing_plus_0, 32);
        lv_obj_set_flag(timing_plus_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(timing_plus_0, &button, 0);
        lv_obj_t * timing_plus_0_text = lv_label_create(timing_plus_0);
        lv_obj_set_name(timing_plus_0_text, "timing_plus_0_text");
        lv_label_set_text(timing_plus_0_text, "+");
        lv_obj_set_x(timing_plus_0_text, 0);
        lv_obj_set_y(timing_plus_0_text, 0);
        lv_obj_set_width(timing_plus_0_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(timing_plus_0_text, ui_medium, 0);
        lv_obj_set_style_text_color(timing_plus_0_text, COLOR_TEXT, 0);
        lv_obj_set_flag(timing_plus_0_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(timing_plus_0_text, LV_ALIGN_CENTER);

        lv_subject_increment_dsc_t * subject_increment_event_1 = lv_obj_add_subject_increment_event(timing_plus_0, &ui_blink_on, LV_EVENT_CLICKED, 50);
        lv_obj_set_subject_increment_event_min_value(timing_plus_0, subject_increment_event_1, 100);
        lv_obj_set_subject_increment_event_max_value(timing_plus_0, subject_increment_event_1, 2000);
        lv_obj_set_subject_increment_event_rollover(timing_plus_0, subject_increment_event_1, false);

        lv_obj_t * timing_title_1 = lv_label_create(settings_list);
        lv_obj_set_name(timing_title_1, "timing_title_1");
        lv_label_set_text(timing_title_1, "Поворотник: выключен");
        lv_obj_set_x(timing_title_1, 12);
        lv_obj_set_y(timing_title_1, 70);
        lv_obj_set_width(timing_title_1, 210);
        lv_obj_set_style_text_font(timing_title_1, ui_small, 0);
        lv_obj_set_style_text_color(timing_title_1, COLOR_TEXT, 0);
        lv_obj_set_flag(timing_title_1, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * timing_value_1 = lv_label_create(settings_list);
        lv_obj_set_name(timing_value_1, "timing_value_1");
        lv_label_set_text(timing_value_1, "450 мс");
        lv_obj_set_x(timing_value_1, 217);
        lv_obj_set_y(timing_value_1, 70);
        lv_obj_set_width(timing_value_1, 105);
        lv_obj_set_style_text_font(timing_value_1, ui_small, 0);
        lv_obj_set_style_text_color(timing_value_1, COLOR_CYAN, 0);
        lv_obj_set_flag(timing_value_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_label_bind_text(timing_value_1, &ui_blink_off, "%d мс");
        lv_obj_set_style_text_align(timing_value_1, LV_TEXT_ALIGN_RIGHT, 0);

        lv_obj_t * timing_minus_1 = lv_button_create(settings_list);
        lv_obj_set_name(timing_minus_1, "timing_minus_1");
        lv_obj_set_x(timing_minus_1, 339);
        lv_obj_set_y(timing_minus_1, 62);
        lv_obj_set_width(timing_minus_1, 52);
        lv_obj_set_height(timing_minus_1, 32);
        lv_obj_set_flag(timing_minus_1, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(timing_minus_1, &button, 0);
        lv_obj_t * timing_minus_1_text = lv_label_create(timing_minus_1);
        lv_obj_set_name(timing_minus_1_text, "timing_minus_1_text");
        lv_label_set_text(timing_minus_1_text, "-");
        lv_obj_set_x(timing_minus_1_text, 0);
        lv_obj_set_y(timing_minus_1_text, 0);
        lv_obj_set_width(timing_minus_1_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(timing_minus_1_text, ui_medium, 0);
        lv_obj_set_style_text_color(timing_minus_1_text, COLOR_TEXT, 0);
        lv_obj_set_flag(timing_minus_1_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(timing_minus_1_text, LV_ALIGN_CENTER);

        lv_subject_increment_dsc_t * subject_increment_event_2 = lv_obj_add_subject_increment_event(timing_minus_1, &ui_blink_off, LV_EVENT_CLICKED, -50);
        lv_obj_set_subject_increment_event_min_value(timing_minus_1, subject_increment_event_2, 100);
        lv_obj_set_subject_increment_event_max_value(timing_minus_1, subject_increment_event_2, 2000);
        lv_obj_set_subject_increment_event_rollover(timing_minus_1, subject_increment_event_2, false);

        lv_obj_t * timing_plus_1 = lv_button_create(settings_list);
        lv_obj_set_name(timing_plus_1, "timing_plus_1");
        lv_obj_set_x(timing_plus_1, 400);
        lv_obj_set_y(timing_plus_1, 62);
        lv_obj_set_width(timing_plus_1, 52);
        lv_obj_set_height(timing_plus_1, 32);
        lv_obj_set_flag(timing_plus_1, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(timing_plus_1, &button, 0);
        lv_obj_t * timing_plus_1_text = lv_label_create(timing_plus_1);
        lv_obj_set_name(timing_plus_1_text, "timing_plus_1_text");
        lv_label_set_text(timing_plus_1_text, "+");
        lv_obj_set_x(timing_plus_1_text, 0);
        lv_obj_set_y(timing_plus_1_text, 0);
        lv_obj_set_width(timing_plus_1_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(timing_plus_1_text, ui_medium, 0);
        lv_obj_set_style_text_color(timing_plus_1_text, COLOR_TEXT, 0);
        lv_obj_set_flag(timing_plus_1_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(timing_plus_1_text, LV_ALIGN_CENTER);

        lv_subject_increment_dsc_t * subject_increment_event_3 = lv_obj_add_subject_increment_event(timing_plus_1, &ui_blink_off, LV_EVENT_CLICKED, 50);
        lv_obj_set_subject_increment_event_min_value(timing_plus_1, subject_increment_event_3, 100);
        lv_obj_set_subject_increment_event_max_value(timing_plus_1, subject_increment_event_3, 2000);
        lv_obj_set_subject_increment_event_rollover(timing_plus_1, subject_increment_event_3, false);

        lv_obj_t * timing_title_2 = lv_label_create(settings_list);
        lv_obj_set_name(timing_title_2, "timing_title_2");
        lv_label_set_text(timing_title_2, "Удержание кнопки");
        lv_obj_set_x(timing_title_2, 12);
        lv_obj_set_y(timing_title_2, 124);
        lv_obj_set_width(timing_title_2, 210);
        lv_obj_set_style_text_font(timing_title_2, ui_small, 0);
        lv_obj_set_style_text_color(timing_title_2, COLOR_TEXT, 0);
        lv_obj_set_flag(timing_title_2, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * timing_value_2 = lv_label_create(settings_list);
        lv_obj_set_name(timing_value_2, "timing_value_2");
        lv_label_set_text(timing_value_2, "2000 мс");
        lv_obj_set_x(timing_value_2, 217);
        lv_obj_set_y(timing_value_2, 124);
        lv_obj_set_width(timing_value_2, 105);
        lv_obj_set_style_text_font(timing_value_2, ui_small, 0);
        lv_obj_set_style_text_color(timing_value_2, COLOR_CYAN, 0);
        lv_obj_set_flag(timing_value_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_label_bind_text(timing_value_2, &ui_hold, "%d мс");
        lv_obj_set_style_text_align(timing_value_2, LV_TEXT_ALIGN_RIGHT, 0);

        lv_obj_t * timing_minus_2 = lv_button_create(settings_list);
        lv_obj_set_name(timing_minus_2, "timing_minus_2");
        lv_obj_set_x(timing_minus_2, 339);
        lv_obj_set_y(timing_minus_2, 116);
        lv_obj_set_width(timing_minus_2, 52);
        lv_obj_set_height(timing_minus_2, 32);
        lv_obj_set_flag(timing_minus_2, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(timing_minus_2, &button, 0);
        lv_obj_t * timing_minus_2_text = lv_label_create(timing_minus_2);
        lv_obj_set_name(timing_minus_2_text, "timing_minus_2_text");
        lv_label_set_text(timing_minus_2_text, "-");
        lv_obj_set_x(timing_minus_2_text, 0);
        lv_obj_set_y(timing_minus_2_text, 0);
        lv_obj_set_width(timing_minus_2_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(timing_minus_2_text, ui_medium, 0);
        lv_obj_set_style_text_color(timing_minus_2_text, COLOR_TEXT, 0);
        lv_obj_set_flag(timing_minus_2_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(timing_minus_2_text, LV_ALIGN_CENTER);

        lv_subject_increment_dsc_t * subject_increment_event_4 = lv_obj_add_subject_increment_event(timing_minus_2, &ui_hold, LV_EVENT_CLICKED, -100);
        lv_obj_set_subject_increment_event_min_value(timing_minus_2, subject_increment_event_4, 500);
        lv_obj_set_subject_increment_event_max_value(timing_minus_2, subject_increment_event_4, 5000);
        lv_obj_set_subject_increment_event_rollover(timing_minus_2, subject_increment_event_4, false);

        lv_obj_t * timing_plus_2 = lv_button_create(settings_list);
        lv_obj_set_name(timing_plus_2, "timing_plus_2");
        lv_obj_set_x(timing_plus_2, 400);
        lv_obj_set_y(timing_plus_2, 116);
        lv_obj_set_width(timing_plus_2, 52);
        lv_obj_set_height(timing_plus_2, 32);
        lv_obj_set_flag(timing_plus_2, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(timing_plus_2, &button, 0);
        lv_obj_t * timing_plus_2_text = lv_label_create(timing_plus_2);
        lv_obj_set_name(timing_plus_2_text, "timing_plus_2_text");
        lv_label_set_text(timing_plus_2_text, "+");
        lv_obj_set_x(timing_plus_2_text, 0);
        lv_obj_set_y(timing_plus_2_text, 0);
        lv_obj_set_width(timing_plus_2_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(timing_plus_2_text, ui_medium, 0);
        lv_obj_set_style_text_color(timing_plus_2_text, COLOR_TEXT, 0);
        lv_obj_set_flag(timing_plus_2_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(timing_plus_2_text, LV_ALIGN_CENTER);

        lv_subject_increment_dsc_t * subject_increment_event_5 = lv_obj_add_subject_increment_event(timing_plus_2, &ui_hold, LV_EVENT_CLICKED, 100);
        lv_obj_set_subject_increment_event_min_value(timing_plus_2, subject_increment_event_5, 500);
        lv_obj_set_subject_increment_event_max_value(timing_plus_2, subject_increment_event_5, 5000);
        lv_obj_set_subject_increment_event_rollover(timing_plus_2, subject_increment_event_5, false);

        lv_obj_t * timing_title_3 = lv_label_create(settings_list);
        lv_obj_set_name(timing_title_3, "timing_title_3");
        lv_label_set_text(timing_title_3, "Двойное нажатие");
        lv_obj_set_x(timing_title_3, 12);
        lv_obj_set_y(timing_title_3, 178);
        lv_obj_set_width(timing_title_3, 210);
        lv_obj_set_style_text_font(timing_title_3, ui_small, 0);
        lv_obj_set_style_text_color(timing_title_3, COLOR_TEXT, 0);
        lv_obj_set_flag(timing_title_3, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * timing_value_3 = lv_label_create(settings_list);
        lv_obj_set_name(timing_value_3, "timing_value_3");
        lv_label_set_text(timing_value_3, "300 мс");
        lv_obj_set_x(timing_value_3, 217);
        lv_obj_set_y(timing_value_3, 178);
        lv_obj_set_width(timing_value_3, 105);
        lv_obj_set_style_text_font(timing_value_3, ui_small, 0);
        lv_obj_set_style_text_color(timing_value_3, COLOR_CYAN, 0);
        lv_obj_set_flag(timing_value_3, LV_OBJ_FLAG_CLICKABLE, false);
        lv_label_bind_text(timing_value_3, &ui_double, "%d мс");
        lv_obj_set_style_text_align(timing_value_3, LV_TEXT_ALIGN_RIGHT, 0);

        lv_obj_t * timing_minus_3 = lv_button_create(settings_list);
        lv_obj_set_name(timing_minus_3, "timing_minus_3");
        lv_obj_set_x(timing_minus_3, 339);
        lv_obj_set_y(timing_minus_3, 170);
        lv_obj_set_width(timing_minus_3, 52);
        lv_obj_set_height(timing_minus_3, 32);
        lv_obj_set_flag(timing_minus_3, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(timing_minus_3, &button, 0);
        lv_obj_t * timing_minus_3_text = lv_label_create(timing_minus_3);
        lv_obj_set_name(timing_minus_3_text, "timing_minus_3_text");
        lv_label_set_text(timing_minus_3_text, "-");
        lv_obj_set_x(timing_minus_3_text, 0);
        lv_obj_set_y(timing_minus_3_text, 0);
        lv_obj_set_width(timing_minus_3_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(timing_minus_3_text, ui_medium, 0);
        lv_obj_set_style_text_color(timing_minus_3_text, COLOR_TEXT, 0);
        lv_obj_set_flag(timing_minus_3_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(timing_minus_3_text, LV_ALIGN_CENTER);

        lv_subject_increment_dsc_t * subject_increment_event_6 = lv_obj_add_subject_increment_event(timing_minus_3, &ui_double, LV_EVENT_CLICKED, -25);
        lv_obj_set_subject_increment_event_min_value(timing_minus_3, subject_increment_event_6, 150);
        lv_obj_set_subject_increment_event_max_value(timing_minus_3, subject_increment_event_6, 600);
        lv_obj_set_subject_increment_event_rollover(timing_minus_3, subject_increment_event_6, false);

        lv_obj_t * timing_plus_3 = lv_button_create(settings_list);
        lv_obj_set_name(timing_plus_3, "timing_plus_3");
        lv_obj_set_x(timing_plus_3, 400);
        lv_obj_set_y(timing_plus_3, 170);
        lv_obj_set_width(timing_plus_3, 52);
        lv_obj_set_height(timing_plus_3, 32);
        lv_obj_set_flag(timing_plus_3, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(timing_plus_3, &button, 0);
        lv_obj_t * timing_plus_3_text = lv_label_create(timing_plus_3);
        lv_obj_set_name(timing_plus_3_text, "timing_plus_3_text");
        lv_label_set_text(timing_plus_3_text, "+");
        lv_obj_set_x(timing_plus_3_text, 0);
        lv_obj_set_y(timing_plus_3_text, 0);
        lv_obj_set_width(timing_plus_3_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(timing_plus_3_text, ui_medium, 0);
        lv_obj_set_style_text_color(timing_plus_3_text, COLOR_TEXT, 0);
        lv_obj_set_flag(timing_plus_3_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(timing_plus_3_text, LV_ALIGN_CENTER);

        lv_subject_increment_dsc_t * subject_increment_event_7 = lv_obj_add_subject_increment_event(timing_plus_3, &ui_double, LV_EVENT_CLICKED, 25);
        lv_obj_set_subject_increment_event_min_value(timing_plus_3, subject_increment_event_7, 150);
        lv_obj_set_subject_increment_event_max_value(timing_plus_3, subject_increment_event_7, 600);
        lv_obj_set_subject_increment_event_rollover(timing_plus_3, subject_increment_event_7, false);

        lv_obj_t * flip_switch_title = lv_label_create(settings_list);
        lv_obj_set_name(flip_switch_title, "flip_switch_title");
        lv_label_set_text(flip_switch_title, "Перевернуть экран");
        lv_obj_set_x(flip_switch_title, 12);
        lv_obj_set_y(flip_switch_title, 221);
        lv_obj_set_width(flip_switch_title, 335);
        lv_obj_set_style_text_font(flip_switch_title, ui_small, 0);
        lv_obj_set_style_text_color(flip_switch_title, COLOR_TEXT, 0);
        lv_obj_set_flag(flip_switch_title, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * flip_switch = lv_switch_create(settings_list);
        lv_obj_set_name(flip_switch, "flip_switch");
        lv_obj_set_x(flip_switch, 390);
        lv_obj_set_y(flip_switch, 216);
        lv_obj_set_width(flip_switch, 66);
        lv_obj_set_height(flip_switch, 34);
        lv_obj_bind_checked(flip_switch, &ui_flip);
        lv_obj_set_style_bg_color(flip_switch, lv_color_hex(0x243c4d), 0);
        lv_obj_set_style_anim_duration(flip_switch, 0, 0);
        lv_obj_add_style(flip_switch, &switch_track, 0);
        lv_obj_add_style(flip_switch, &switch_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(flip_switch, &switch_on, LV_STATE_CHECKED);
        lv_obj_add_style(flip_switch, &switch_knob, LV_PART_KNOB);

        lv_obj_t * control_switch_title = lv_label_create(settings_list);
        lv_obj_set_name(control_switch_title, "control_switch_title");
        lv_label_set_text(control_switch_title, "Управление VESC");
        lv_obj_set_x(control_switch_title, 12);
        lv_obj_set_y(control_switch_title, 271);
        lv_obj_set_width(control_switch_title, 335);
        lv_obj_set_style_text_font(control_switch_title, ui_small, 0);
        lv_obj_set_style_text_color(control_switch_title, COLOR_TEXT, 0);
        lv_obj_set_flag(control_switch_title, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * control_switch = lv_switch_create(settings_list);
        lv_obj_set_name(control_switch, "control_switch");
        lv_obj_set_x(control_switch, 390);
        lv_obj_set_y(control_switch, 266);
        lv_obj_set_width(control_switch, 66);
        lv_obj_set_height(control_switch, 34);
        lv_obj_bind_checked(control_switch, &ui_motion);
        lv_obj_set_style_bg_color(control_switch, lv_color_hex(0x243c4d), 0);
        lv_obj_set_style_anim_duration(control_switch, 0, 0);
        lv_obj_add_style(control_switch, &switch_track, 0);
        lv_obj_add_style(control_switch, &switch_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(control_switch, &switch_on, LV_STATE_CHECKED);
        lv_obj_add_style(control_switch, &switch_knob, LV_PART_KNOB);

        lv_obj_t * cruise_switch_title = lv_label_create(settings_list);
        lv_obj_set_name(cruise_switch_title, "cruise_switch_title");
        lv_label_set_text(cruise_switch_title, "Круиз-контроль");
        lv_obj_set_x(cruise_switch_title, 12);
        lv_obj_set_y(cruise_switch_title, 321);
        lv_obj_set_width(cruise_switch_title, 335);
        lv_obj_set_style_text_font(cruise_switch_title, ui_small, 0);
        lv_obj_set_style_text_color(cruise_switch_title, COLOR_TEXT, 0);
        lv_obj_set_flag(cruise_switch_title, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * cruise_switch = lv_switch_create(settings_list);
        lv_obj_set_name(cruise_switch, "cruise_switch");
        lv_obj_set_x(cruise_switch, 390);
        lv_obj_set_y(cruise_switch, 316);
        lv_obj_set_width(cruise_switch, 66);
        lv_obj_set_height(cruise_switch, 34);
        lv_obj_bind_checked(cruise_switch, &ui_cruise);
        lv_obj_set_style_bg_color(cruise_switch, lv_color_hex(0x243c4d), 0);
        lv_obj_set_style_anim_duration(cruise_switch, 0, 0);
        lv_obj_add_style(cruise_switch, &switch_track, 0);
        lv_obj_add_style(cruise_switch, &switch_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(cruise_switch, &switch_on, LV_STATE_CHECKED);
        lv_obj_add_style(cruise_switch, &switch_knob, LV_PART_KNOB);

        lv_obj_t * safe_switch_title = lv_label_create(settings_list);
        lv_obj_set_name(safe_switch_title, "safe_switch_title");
        lv_label_set_text(safe_switch_title, "Безопасный режим");
        lv_obj_set_x(safe_switch_title, 12);
        lv_obj_set_y(safe_switch_title, 371);
        lv_obj_set_width(safe_switch_title, 335);
        lv_obj_set_style_text_font(safe_switch_title, ui_small, 0);
        lv_obj_set_style_text_color(safe_switch_title, COLOR_TEXT, 0);
        lv_obj_set_flag(safe_switch_title, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * safe_switch = lv_switch_create(settings_list);
        lv_obj_set_name(safe_switch, "safe_switch");
        lv_obj_set_x(safe_switch, 390);
        lv_obj_set_y(safe_switch, 366);
        lv_obj_set_width(safe_switch, 66);
        lv_obj_set_height(safe_switch, 34);
        lv_obj_bind_checked(safe_switch, &ui_safe);
        lv_obj_set_style_bg_color(safe_switch, lv_color_hex(0x243c4d), 0);
        lv_obj_set_style_anim_duration(safe_switch, 0, 0);
        lv_obj_add_style(safe_switch, &switch_track, 0);
        lv_obj_add_style(safe_switch, &switch_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(safe_switch, &switch_on, LV_STATE_CHECKED);
        lv_obj_add_style(safe_switch, &switch_knob, LV_PART_KNOB);

        lv_obj_t * settings_hint = lv_label_create(lv_obj_0);
        lv_obj_set_name(settings_hint, "settings_hint");
        lv_label_set_text(settings_hint, "Изменения действуют сразу");
        lv_obj_set_x(settings_hint, 12);
        lv_obj_set_y(settings_hint, 288);
        lv_obj_set_width(settings_hint, 300);
        lv_obj_set_style_text_font(settings_hint, ui_small, 0);
        lv_obj_set_style_text_color(settings_hint, COLOR_TEXT, 0);
        lv_obj_set_flag(settings_hint, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * save_settings = lv_button_create(lv_obj_0);
        lv_obj_set_name(save_settings, "save_settings");
        lv_obj_set_x(save_settings, 335);
        lv_obj_set_y(save_settings, 278);
        lv_obj_set_width(save_settings, 133);
        lv_obj_set_height(save_settings, 38);
        lv_obj_set_flag(save_settings, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(save_settings, &button, 0);
        lv_obj_t * save_settings_text = lv_label_create(save_settings);
        lv_obj_set_name(save_settings_text, "save_settings_text");
        lv_label_set_text(save_settings_text, "Сохранить");
        lv_obj_set_x(save_settings_text, 0);
        lv_obj_set_y(save_settings_text, 0);
        lv_obj_set_width(save_settings_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(save_settings_text, ui_medium, 0);
        lv_obj_set_style_text_color(save_settings_text, COLOR_TEXT, 0);
        lv_obj_set_flag(save_settings_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(save_settings_text, LV_ALIGN_CENTER);

        lv_obj_add_subject_set_string_event(save_settings, &ui_notice, LV_EVENT_CLICKED, "Изменения сохранены в предпросмотре");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

