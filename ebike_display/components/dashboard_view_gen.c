/**
 * @file dashboard_view_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "dashboard_view_gen.h"
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

lv_obj_t * dashboard_view_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if EBIKE_DISPLAY_CHECK_COMPILE_TARGET(EBIKE_DISPLAY_TARGET_ALL)
    if (ebike_display_check_target(EBIKE_DISPLAY_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "dashboard_view_#");
        lv_obj_set_width(lv_obj_0, 480);
        lv_obj_set_height(lv_obj_0, 320);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_text_font(lv_obj_0, ui_small, 0);

        lv_obj_add_style(lv_obj_0, &clear, 0);
        lv_obj_t * background = lv_image_create(lv_obj_0);
        lv_obj_set_name(background, "background");
        lv_image_set_src(background, ui_img_background);
        lv_obj_set_x(background, 0);
        lv_obj_set_y(background, 0);
        lv_obj_set_width(background, 480);
        lv_obj_set_height(background, 320);
        lv_image_set_inner_align(background, LV_IMAGE_ALIGN_STRETCH);
        lv_obj_set_flag(background, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * status_left = lv_image_create(lv_obj_0);
        lv_obj_set_name(status_left, "status_left");
        lv_image_set_src(status_left, ui_img_left);
        lv_obj_set_x(status_left, 6);
        lv_obj_set_y(status_left, 3);
        lv_obj_set_flag(status_left, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_image_recolor(status_left, lv_color_hex(0x354b5a), 0);
        lv_obj_set_style_image_recolor_opa(status_left, (255 * 100 / 100), 0);

        lv_obj_t * status_position = lv_image_create(lv_obj_0);
        lv_obj_set_name(status_position, "status_position");
        lv_image_set_src(status_position, ui_img_position);
        lv_obj_set_x(status_position, 78);
        lv_obj_set_y(status_position, 3);
        lv_obj_set_flag(status_position, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_image_recolor(status_position, lv_color_hex(0x31f29a), 0);
        lv_obj_set_style_image_recolor_opa(status_position, (255 * 100 / 100), 0);

        lv_obj_t * status_low_beam = lv_image_create(lv_obj_0);
        lv_obj_set_name(status_low_beam, "status_low_beam");
        lv_image_set_src(status_low_beam, ui_img_low_beam);
        lv_obj_set_x(status_low_beam, 150);
        lv_obj_set_y(status_low_beam, 3);
        lv_obj_set_flag(status_low_beam, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_image_recolor(status_low_beam, lv_color_hex(0xe3f3ff), 0);
        lv_obj_set_style_image_recolor_opa(status_low_beam, (255 * 100 / 100), 0);

        lv_obj_t * status_high_beam = lv_image_create(lv_obj_0);
        lv_obj_set_name(status_high_beam, "status_high_beam");
        lv_image_set_src(status_high_beam, ui_img_high_beam);
        lv_obj_set_x(status_high_beam, 222);
        lv_obj_set_y(status_high_beam, 3);
        lv_obj_set_flag(status_high_beam, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_image_recolor(status_high_beam, lv_color_hex(0x354b5a), 0);
        lv_obj_set_style_image_recolor_opa(status_high_beam, (255 * 100 / 100), 0);

        lv_obj_t * status_brake = lv_image_create(lv_obj_0);
        lv_obj_set_name(status_brake, "status_brake");
        lv_image_set_src(status_brake, ui_img_brake);
        lv_obj_set_x(status_brake, 294);
        lv_obj_set_y(status_brake, 3);
        lv_obj_set_flag(status_brake, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_image_recolor(status_brake, lv_color_hex(0x354b5a), 0);
        lv_obj_set_style_image_recolor_opa(status_brake, (255 * 100 / 100), 0);

        lv_obj_t * status_warning = lv_image_create(lv_obj_0);
        lv_obj_set_name(status_warning, "status_warning");
        lv_image_set_src(status_warning, ui_img_warning);
        lv_obj_set_x(status_warning, 366);
        lv_obj_set_y(status_warning, 3);
        lv_obj_set_flag(status_warning, LV_OBJ_FLAG_CLICKABLE, true);
        lv_obj_set_style_image_recolor(status_warning, lv_color_hex(0x354b5a), 0);
        lv_obj_set_style_image_recolor_opa(status_warning, (255 * 100 / 100), 0);
        lv_obj_add_screen_create_event(status_warning, LV_EVENT_CLICKED, diagnostics_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        lv_obj_t * status_right = lv_image_create(lv_obj_0);
        lv_obj_set_name(status_right, "status_right");
        lv_image_set_src(status_right, ui_img_right);
        lv_obj_set_x(status_right, 438);
        lv_obj_set_y(status_right, 3);
        lv_obj_set_flag(status_right, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_image_recolor(status_right, lv_color_hex(0x354b5a), 0);
        lv_obj_set_style_image_recolor_opa(status_right, (255 * 100 / 100), 0);

        lv_obj_t * temperatures = lv_obj_create(lv_obj_0);
        lv_obj_set_name(temperatures, "temperatures");
        lv_obj_set_x(temperatures, 8);
        lv_obj_set_y(temperatures, 123);
        lv_obj_set_width(temperatures, 112);
        lv_obj_set_height(temperatures, 100);
        lv_obj_set_flag(temperatures, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(temperatures, &card, 0);
        lv_obj_t * motor_caption = lv_label_create(temperatures);
        lv_obj_set_name(motor_caption, "motor_caption");
        lv_label_set_text(motor_caption, "МОТОР");
        lv_obj_set_x(motor_caption, 37);
        lv_obj_set_y(motor_caption, 4);
        lv_obj_set_width(motor_caption, 75);
        lv_obj_set_style_text_font(motor_caption, ui_small, 0);
        lv_obj_set_style_text_color(motor_caption, COLOR_TEXT, 0);
        lv_obj_set_flag(motor_caption, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * motor_value = lv_label_create(temperatures);
        lv_obj_set_name(motor_value, "motor_value");
        lv_label_set_text(motor_value, "48°C");
        lv_obj_set_x(motor_value, 37);
        lv_obj_set_y(motor_value, 20);
        lv_obj_set_width(motor_value, 75);
        lv_obj_set_style_text_font(motor_value, ui_medium, 0);
        lv_obj_set_style_text_color(motor_value, COLOR_TEXT, 0);
        lv_obj_set_flag(motor_value, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * motor_icon = lv_image_create(temperatures);
        lv_obj_set_name(motor_icon, "motor_icon");
        lv_image_set_src(motor_icon, ui_img_thermometer);
        lv_obj_set_x(motor_icon, 8);
        lv_obj_set_y(motor_icon, 7);
        lv_obj_set_flag(motor_icon, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_image_recolor(motor_icon, lv_color_hex(0xff9c60), 0);
        lv_obj_set_style_image_recolor_opa(motor_icon, (255 * 100 / 100), 0);

        lv_obj_t * controller_caption = lv_label_create(temperatures);
        lv_obj_set_name(controller_caption, "controller_caption");
        lv_label_set_text(controller_caption, "КОНТР.");
        lv_obj_set_x(controller_caption, 37);
        lv_obj_set_y(controller_caption, 53);
        lv_obj_set_width(controller_caption, 75);
        lv_obj_set_style_text_font(controller_caption, ui_small, 0);
        lv_obj_set_style_text_color(controller_caption, COLOR_TEXT, 0);
        lv_obj_set_flag(controller_caption, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * controller_value = lv_label_create(temperatures);
        lv_obj_set_name(controller_value, "controller_value");
        lv_label_set_text(controller_value, "38°C");
        lv_obj_set_x(controller_value, 37);
        lv_obj_set_y(controller_value, 69);
        lv_obj_set_width(controller_value, 75);
        lv_obj_set_style_text_font(controller_value, ui_medium, 0);
        lv_obj_set_style_text_color(controller_value, COLOR_TEXT, 0);
        lv_obj_set_flag(controller_value, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * controller_icon = lv_image_create(temperatures);
        lv_obj_set_name(controller_icon, "controller_icon");
        lv_image_set_src(controller_icon, ui_img_thermometer);
        lv_obj_set_x(controller_icon, 8);
        lv_obj_set_y(controller_icon, 56);
        lv_obj_set_flag(controller_icon, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_image_recolor(controller_icon, lv_color_hex(0xff9c60), 0);
        lv_obj_set_style_image_recolor_opa(controller_icon, (255 * 100 / 100), 0);

        lv_obj_t * speed_arc = lv_arc_create(lv_obj_0);
        lv_obj_set_name(speed_arc, "speed_arc");
        lv_obj_set_x(speed_arc, 125);
        lv_obj_set_y(speed_arc, 35);
        lv_obj_set_width(speed_arc, 221);
        lv_obj_set_height(speed_arc, 221);
        lv_arc_set_rotation(speed_arc, 150);
        lv_arc_set_bg_start_angle(speed_arc, 0);
        lv_arc_set_bg_end_angle(speed_arc, 240);
        lv_arc_set_min_value(speed_arc, 0);
        lv_arc_set_max_value(speed_arc, 100);
        lv_arc_bind_value(speed_arc, &ui_speed_value);
        lv_obj_set_flag(speed_arc, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_pad_all(speed_arc, 4, 0);
        lv_obj_set_style_arc_color(speed_arc, lv_color_hex(0x233f51), 0);
        lv_obj_set_style_arc_width(speed_arc, 8, 0);
        lv_obj_add_style(speed_arc, &arc_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(speed_arc, &arc_knob, LV_PART_KNOB);

        lv_obj_t * speed_ticks = lv_scale_create(lv_obj_0);
        lv_obj_set_name(speed_ticks, "speed_ticks");
        lv_obj_set_x(speed_ticks, 125);
        lv_obj_set_y(speed_ticks, 35);
        lv_obj_set_width(speed_ticks, 221);
        lv_obj_set_height(speed_ticks, 221);
        lv_scale_set_mode(speed_ticks, LV_SCALE_MODE_ROUND_INNER);
        lv_scale_set_total_tick_count(speed_ticks, 51);
        lv_scale_set_major_tick_every(speed_ticks, 10);
        lv_scale_set_min_value(speed_ticks, 0);
        lv_scale_set_max_value(speed_ticks, 100);
        lv_scale_set_angle_range(speed_ticks, 240);
        lv_scale_set_rotation(speed_ticks, 150);
        lv_scale_set_label_show(speed_ticks, true);
        lv_obj_set_style_pad_all(speed_ticks, 14, 0);
        lv_obj_set_style_arc_opa(speed_ticks, (255 * 0 / 100), 0);
        lv_obj_set_style_line_width(speed_ticks, 0, 0);
        lv_obj_set_flag(speed_ticks, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(speed_ticks, &minor_ticks, LV_PART_ITEMS);
        lv_obj_add_style(speed_ticks, &major_ticks, LV_PART_INDICATOR);

        lv_obj_t * speed_value = lv_label_create(lv_obj_0);
        lv_obj_set_name(speed_value, "speed_value");
        lv_label_set_text(speed_value, "42");
        lv_obj_set_x(speed_value, 142);
        lv_obj_set_y(speed_value, 80);
        lv_obj_set_width(speed_value, 186);
        lv_obj_set_style_text_font(speed_value, ui_speed, 0);
        lv_obj_set_style_text_color(speed_value, COLOR_TEXT, 0);
        lv_obj_set_flag(speed_value, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(speed_value, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_bind_text(speed_value, &ui_speed_value, NULL);

        lv_obj_t * speed_units = lv_label_create(lv_obj_0);
        lv_obj_set_name(speed_units, "speed_units");
        lv_label_set_text(speed_units, "КМ/Ч");
        lv_obj_set_x(speed_units, 176);
        lv_obj_set_y(speed_units, 165);
        lv_obj_set_width(speed_units, 120);
        lv_obj_set_style_text_font(speed_units, ui_small, 0);
        lv_obj_set_style_text_color(speed_units, COLOR_TEXT, 0);
        lv_obj_set_flag(speed_units, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(speed_units, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * current_mode = lv_obj_create(lv_obj_0);
        lv_obj_set_name(current_mode, "current_mode");
        lv_obj_set_x(current_mode, 184);
        lv_obj_set_y(current_mode, 202);
        lv_obj_set_width(current_mode, 104);
        lv_obj_set_height(current_mode, 29);
        lv_obj_set_flag(current_mode, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_border_color(current_mode, COLOR_CYAN, 0);
        lv_obj_set_style_border_width(current_mode, 2, 0);
        lv_obj_add_style(current_mode, &card, 0);
        lv_obj_t * mode_badge = lv_label_create(current_mode);
        lv_obj_set_name(mode_badge, "mode_badge");
        lv_label_set_text(mode_badge, "Drive");
        lv_obj_set_x(mode_badge, 0);
        lv_obj_set_y(mode_badge, 0);
        lv_obj_set_width(mode_badge, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(mode_badge, ui_medium, 0);
        lv_obj_set_style_text_color(mode_badge, COLOR_TEXT, 0);
        lv_obj_set_flag(mode_badge, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(mode_badge, LV_ALIGN_CENTER);
        lv_label_bind_text(mode_badge, &ui_mode_text, NULL);

        lv_obj_t * hazard = lv_button_create(lv_obj_0);
        lv_obj_set_name(hazard, "hazard");
        lv_obj_set_x(hazard, 351);
        lv_obj_set_y(hazard, 145);
        lv_obj_set_width(hazard, 121);
        lv_obj_set_height(hazard, 78);
        lv_obj_set_flag(hazard, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_border_width(hazard, 2, 0);
        lv_obj_add_style(hazard, &button, 0);
        lv_obj_t * hazard_text = lv_label_create(hazard);
        lv_obj_set_name(hazard_text, "hazard_text");
        lv_label_set_text(hazard_text, "");
        lv_obj_set_x(hazard_text, 0);
        lv_obj_set_y(hazard_text, 0);
        lv_obj_set_width(hazard_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(hazard_text, ui_medium, 0);
        lv_obj_set_style_text_color(hazard_text, COLOR_TEXT, 0);
        lv_obj_set_flag(hazard_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(hazard_text, LV_ALIGN_CENTER);
        lv_obj_set_style_text_align(hazard_text, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * hazard_icon = lv_image_create(hazard);
        lv_obj_set_name(hazard_icon, "hazard_icon");
        lv_image_set_src(hazard_icon, ui_img_hazard);
        lv_obj_set_x(hazard_icon, 0);
        lv_obj_set_y(hazard_icon, 0);
        lv_obj_set_flag(hazard_icon, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_image_recolor(hazard_icon, lv_color_hex(0xd1aa76), 0);
        lv_obj_set_style_image_recolor_opa(hazard_icon, (255 * 100 / 100), 0);
        lv_obj_set_align(hazard_icon, LV_ALIGN_CENTER);

        lv_obj_add_subject_toggle_event(hazard, &ui_hazard, LV_EVENT_CLICKED);
        lv_obj_bind_state_if_eq(hazard, &ui_hazard, LV_STATE_CHECKED, 1);
        lv_obj_add_style(hazard, &limit_selected, LV_STATE_CHECKED);

        lv_obj_t * footer = lv_obj_create(lv_obj_0);
        lv_obj_set_name(footer, "footer");
        lv_obj_set_x(footer, 4);
        lv_obj_set_y(footer, 250);
        lv_obj_set_width(footer, 472);
        lv_obj_set_height(footer, 70);
        lv_obj_set_flag(footer, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(footer, &card, 0);

        lv_obj_t * select_N = lv_button_create(lv_obj_0);
        lv_obj_set_name(select_N, "select_N");
        lv_obj_set_x(select_N, 8);
        lv_obj_set_y(select_N, 254);
        lv_obj_set_width(select_N, 52);
        lv_obj_set_height(select_N, 62);
        lv_obj_set_flag(select_N, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(select_N, &button, 0);
        lv_obj_t * select_N_text = lv_label_create(select_N);
        lv_obj_set_name(select_N_text, "select_N_text");
        lv_label_set_text(select_N_text, "N");
        lv_obj_set_x(select_N_text, 0);
        lv_obj_set_y(select_N_text, 0);
        lv_obj_set_width(select_N_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(select_N_text, ui_medium, 0);
        lv_obj_set_style_text_color(select_N_text, COLOR_TEXT, 0);
        lv_obj_set_flag(select_N_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(select_N_text, LV_ALIGN_CENTER);
        lv_obj_set_style_text_align(select_N_text, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_add_style(select_N, &selected, LV_STATE_CHECKED);
        lv_obj_bind_state_if_eq(select_N, &ui_mode, LV_STATE_CHECKED, 0);
        lv_obj_add_subject_set_int_event(select_N, &ui_mode, LV_EVENT_CLICKED, 0);
        lv_obj_add_subject_set_string_event(select_N, &ui_mode_text, LV_EVENT_CLICKED, "N");

        lv_obj_t * select_Drive = lv_button_create(lv_obj_0);
        lv_obj_set_name(select_Drive, "select_Drive");
        lv_obj_set_x(select_Drive, 66);
        lv_obj_set_y(select_Drive, 254);
        lv_obj_set_width(select_Drive, 72);
        lv_obj_set_height(select_Drive, 62);
        lv_obj_set_flag(select_Drive, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(select_Drive, &button, 0);
        lv_obj_t * select_Drive_text = lv_label_create(select_Drive);
        lv_obj_set_name(select_Drive_text, "select_Drive_text");
        lv_label_set_text(select_Drive_text, "Drive");
        lv_obj_set_x(select_Drive_text, 0);
        lv_obj_set_y(select_Drive_text, 0);
        lv_obj_set_width(select_Drive_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(select_Drive_text, ui_medium, 0);
        lv_obj_set_style_text_color(select_Drive_text, COLOR_TEXT, 0);
        lv_obj_set_flag(select_Drive_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(select_Drive_text, LV_ALIGN_CENTER);
        lv_obj_set_style_text_align(select_Drive_text, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_add_style(select_Drive, &selected, LV_STATE_CHECKED);
        lv_obj_bind_state_if_eq(select_Drive, &ui_mode, LV_STATE_CHECKED, 1);
        lv_obj_add_subject_set_int_event(select_Drive, &ui_mode, LV_EVENT_CLICKED, 1);
        lv_obj_add_subject_set_string_event(select_Drive, &ui_mode_text, LV_EVENT_CLICKED, "Drive");

        lv_obj_t * select_Sport = lv_button_create(lv_obj_0);
        lv_obj_set_name(select_Sport, "select_Sport");
        lv_obj_set_x(select_Sport, 144);
        lv_obj_set_y(select_Sport, 254);
        lv_obj_set_width(select_Sport, 72);
        lv_obj_set_height(select_Sport, 62);
        lv_obj_set_flag(select_Sport, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(select_Sport, &button, 0);
        lv_obj_t * select_Sport_text = lv_label_create(select_Sport);
        lv_obj_set_name(select_Sport_text, "select_Sport_text");
        lv_label_set_text(select_Sport_text, "Sport");
        lv_obj_set_x(select_Sport_text, 0);
        lv_obj_set_y(select_Sport_text, 0);
        lv_obj_set_width(select_Sport_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(select_Sport_text, ui_medium, 0);
        lv_obj_set_style_text_color(select_Sport_text, COLOR_TEXT, 0);
        lv_obj_set_flag(select_Sport_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(select_Sport_text, LV_ALIGN_CENTER);
        lv_obj_set_style_text_align(select_Sport_text, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_add_style(select_Sport, &selected, LV_STATE_CHECKED);
        lv_obj_bind_state_if_eq(select_Sport, &ui_mode, LV_STATE_CHECKED, 2);
        lv_obj_add_subject_set_int_event(select_Sport, &ui_mode, LV_EVENT_CLICKED, 2);
        lv_obj_add_subject_set_string_event(select_Sport, &ui_mode_text, LV_EVENT_CLICKED, "Sport");

        lv_obj_t * footer_separator = lv_obj_create(lv_obj_0);
        lv_obj_set_name(footer_separator, "footer_separator");
        lv_obj_set_x(footer_separator, 225);
        lv_obj_set_y(footer_separator, 257);
        lv_obj_set_width(footer_separator, 1);
        lv_obj_set_height(footer_separator, 56);
        lv_obj_set_style_bg_color(footer_separator, lv_color_hex(0x447b98), 0);
        lv_obj_set_style_bg_opa(footer_separator, (255 * 100 / 100), 0);
        lv_obj_set_style_border_width(footer_separator, 0, 0);
        lv_obj_set_style_pad_all(footer_separator, 0, 0);
        lv_obj_set_flag(footer_separator, LV_OBJ_FLAG_SCROLLABLE, false);

        lv_obj_t * limit_buttons = lv_obj_create(lv_obj_0);
        lv_obj_set_name(limit_buttons, "limit_buttons");
        lv_obj_set_x(limit_buttons, 235);
        lv_obj_set_y(limit_buttons, 254);
        lv_obj_set_width(limit_buttons, 237);
        lv_obj_set_height(limit_buttons, 62);
        lv_obj_set_flag(limit_buttons, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_layout(limit_buttons, LV_LAYOUT_FLEX, 0);
        lv_obj_set_style_flex_flow(limit_buttons, LV_FLEX_FLOW_ROW, 0);
        lv_obj_set_style_pad_column(limit_buttons, 6, 0);
        lv_obj_add_style(limit_buttons, &clear, 0);
        lv_obj_t * limit_25 = lv_button_create(limit_buttons);
        lv_obj_set_name(limit_25, "limit_25");
        lv_obj_set_width(limit_25, 0);
        lv_obj_set_height(limit_25, lv_pct(100));
        lv_obj_set_flex_grow(limit_25, 1);
        lv_obj_set_flag(limit_25, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(limit_25, &button, 0);
        lv_obj_t * limit_25_text = lv_label_create(limit_25);
        lv_obj_set_name(limit_25_text, "limit_25_text");
        lv_label_set_text(limit_25_text, "25");
        lv_obj_set_x(limit_25_text, 0);
        lv_obj_set_y(limit_25_text, 0);
        lv_obj_set_width(limit_25_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(limit_25_text, ui_medium, 0);
        lv_obj_set_style_text_color(limit_25_text, COLOR_TEXT, 0);
        lv_obj_set_flag(limit_25_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(limit_25_text, LV_ALIGN_CENTER);
        lv_obj_set_style_text_align(limit_25_text, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_add_style(limit_25, &limit_selected, LV_STATE_CHECKED);
        lv_obj_bind_state_if_eq(limit_25, &ui_limit, LV_STATE_CHECKED, 25);
        lv_obj_add_subject_set_int_event(limit_25, &ui_limit, LV_EVENT_CLICKED, 25);

        lv_obj_t * limit_50 = lv_button_create(limit_buttons);
        lv_obj_set_name(limit_50, "limit_50");
        lv_obj_set_width(limit_50, 0);
        lv_obj_set_height(limit_50, lv_pct(100));
        lv_obj_set_flex_grow(limit_50, 1);
        lv_obj_set_flag(limit_50, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(limit_50, &button, 0);
        lv_obj_t * limit_50_text = lv_label_create(limit_50);
        lv_obj_set_name(limit_50_text, "limit_50_text");
        lv_label_set_text(limit_50_text, "50");
        lv_obj_set_x(limit_50_text, 0);
        lv_obj_set_y(limit_50_text, 0);
        lv_obj_set_width(limit_50_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(limit_50_text, ui_medium, 0);
        lv_obj_set_style_text_color(limit_50_text, COLOR_TEXT, 0);
        lv_obj_set_flag(limit_50_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(limit_50_text, LV_ALIGN_CENTER);
        lv_obj_set_style_text_align(limit_50_text, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_add_style(limit_50, &limit_selected, LV_STATE_CHECKED);
        lv_obj_bind_state_if_eq(limit_50, &ui_limit, LV_STATE_CHECKED, 50);
        lv_obj_add_subject_set_int_event(limit_50, &ui_limit, LV_EVENT_CLICKED, 50);

        lv_obj_t * limit_70 = lv_button_create(limit_buttons);
        lv_obj_set_name(limit_70, "limit_70");
        lv_obj_set_width(limit_70, 0);
        lv_obj_set_height(limit_70, lv_pct(100));
        lv_obj_set_flex_grow(limit_70, 1);
        lv_obj_set_flag(limit_70, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(limit_70, &button, 0);
        lv_obj_t * limit_70_text = lv_label_create(limit_70);
        lv_obj_set_name(limit_70_text, "limit_70_text");
        lv_label_set_text(limit_70_text, "70");
        lv_obj_set_x(limit_70_text, 1);
        lv_obj_set_y(limit_70_text, 0);
        lv_obj_set_width(limit_70_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(limit_70_text, ui_medium, 0);
        lv_obj_set_style_text_color(limit_70_text, COLOR_TEXT, 0);
        lv_obj_set_flag(limit_70_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(limit_70_text, LV_ALIGN_CENTER);
        lv_obj_set_style_text_align(limit_70_text, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_add_style(limit_70, &limit_selected, LV_STATE_CHECKED);
        lv_obj_bind_state_if_eq(limit_70, &ui_limit, LV_STATE_CHECKED, 70);
        lv_obj_add_subject_set_int_event(limit_70, &ui_limit, LV_EVENT_CLICKED, 70);

        lv_obj_t * limit_0 = lv_button_create(limit_buttons);
        lv_obj_set_name(limit_0, "limit_0");
        lv_obj_set_width(limit_0, 0);
        lv_obj_set_height(limit_0, lv_pct(100));
        lv_obj_set_flex_grow(limit_0, 1);
        lv_obj_set_flag(limit_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(limit_0, &button, 0);
        lv_obj_t * limit_0_text = lv_label_create(limit_0);
        lv_obj_set_name(limit_0_text, "limit_0_text");
        lv_label_set_text(limit_0_text, "MAX");
        lv_obj_set_x(limit_0_text, 0);
        lv_obj_set_y(limit_0_text, 0);
        lv_obj_set_width(limit_0_text, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(limit_0_text, ui_medium, 0);
        lv_obj_set_style_text_color(limit_0_text, COLOR_TEXT, 0);
        lv_obj_set_flag(limit_0_text, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(limit_0_text, LV_ALIGN_CENTER);
        lv_obj_set_style_text_align(limit_0_text, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_add_style(limit_0, &limit_selected, LV_STATE_CHECKED);
        lv_obj_bind_state_if_eq(limit_0, &ui_limit, LV_STATE_CHECKED, 0);
        lv_obj_add_subject_set_int_event(limit_0, &ui_limit, LV_EVENT_CLICKED, 0);

        lv_obj_t * battery_card = lv_obj_create(lv_obj_0);
        lv_obj_set_name(battery_card, "battery_card");
        lv_obj_set_x(battery_card, 8);
        lv_obj_set_y(battery_card, 35);
        lv_obj_set_width(battery_card, 112);
        lv_obj_set_height(battery_card, 79);
        lv_obj_set_flag(battery_card, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_clip_corner(battery_card, true, 0);
        lv_obj_add_style(battery_card, &card, 0);
        lv_obj_t * battery_pages = lv_tabview_create(battery_card);
        lv_obj_set_name(battery_pages, "battery_pages");
        lv_obj_set_x(battery_pages, 0);
        lv_obj_set_y(battery_pages, 0);
        lv_obj_set_width(battery_pages, lv_pct(100));
        lv_obj_set_height(battery_pages, lv_pct(100));
        lv_tabview_set_tab_bar_position(battery_pages, LV_DIR_BOTTOM);
        lv_obj_set_style_radius(battery_pages, 10, 0);
        lv_obj_set_style_clip_corner(battery_pages, true, 0);
        lv_obj_set_flag(battery_pages, LV_OBJ_FLAG_SCROLL_ELASTIC, false);
        lv_obj_set_flag(battery_pages, LV_OBJ_FLAG_GESTURE_BUBBLE, false);
        lv_obj_add_style(battery_pages, &clear, 0);
        lv_obj_t * battery_dots = lv_tabview_get_tab_bar(battery_pages);
        lv_obj_set_name(battery_dots, "battery_dots");
        lv_obj_set_height(battery_dots, 10);
        lv_obj_set_style_pad_all(battery_dots, 0, 0);
        lv_obj_set_style_bg_opa(battery_dots, (255 * 0 / 100), 0);
        lv_obj_set_style_border_width(battery_dots, 0, 0);
        lv_obj_set_style_flex_main_place(battery_dots, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_cross_place(battery_dots, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_column(battery_dots, 12, 0);
        lv_obj_set_width(battery_dots, lv_pct(100));
        lv_obj_t * battery_page_0 = lv_tabview_add_tab(battery_pages, "");
        lv_obj_set_name(battery_page_0, "battery_page_0");
        lv_obj_set_flag(battery_page_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(battery_page_0, LV_OBJ_FLAG_GESTURE_BUBBLE, false);
        lv_obj_set_style_bg_opa(battery_page_0, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(battery_page_0, COLOR_BG, 0);
        lv_obj_add_style(battery_page_0, &clear, 0);
        lv_obj_t * battery_title = lv_label_create(battery_page_0);
        lv_obj_set_name(battery_title, "battery_title");
        lv_label_set_text(battery_title, "ЗАРЯД");
        lv_obj_set_x(battery_title, 36);
        lv_obj_set_y(battery_title, 5);
        lv_obj_set_width(battery_title, 70);
        lv_obj_set_style_text_font(battery_title, ui_small, 0);
        lv_obj_set_style_text_color(battery_title, COLOR_TEXT, 0);
        lv_obj_set_flag(battery_title, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(battery_title, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * battery_value_0 = lv_label_create(battery_page_0);
        lv_obj_set_name(battery_value_0, "battery_value_0");
        lv_label_set_text(battery_value_0, "82%");
        lv_obj_set_x(battery_value_0, 36);
        lv_obj_set_y(battery_value_0, 19);
        lv_obj_set_width(battery_value_0, 70);
        lv_obj_set_style_text_font(battery_value_0, ui_value, 0);
        lv_obj_set_style_text_color(battery_value_0, COLOR_TEXT, 0);
        lv_obj_set_flag(battery_value_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(battery_value_0, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * battery_fill = lv_bar_create(battery_page_0);
        lv_obj_set_name(battery_fill, "battery_fill");
        lv_obj_set_x(battery_fill, 14);
        lv_obj_set_y(battery_fill, 21);
        lv_obj_set_width(battery_fill, 14);
        lv_obj_set_height(battery_fill, 33);
        lv_bar_set_min_value(battery_fill, 0);
        lv_bar_set_max_value(battery_fill, 100);
        lv_bar_bind_value(battery_fill, &ui_battery);
        lv_obj_set_style_bg_color(battery_fill, lv_color_hex(0x152a38), 0);
        lv_obj_add_style(battery_fill, &battery_fill_style, LV_PART_INDICATOR);

        lv_obj_t * battery_frame = lv_image_create(battery_page_0);
        lv_obj_set_name(battery_frame, "battery_frame");
        lv_image_set_src(battery_frame, ui_img_battery);
        lv_obj_set_x(battery_frame, 7);
        lv_obj_set_y(battery_frame, 12);
        lv_obj_set_flag(battery_frame, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_image_recolor(battery_frame, lv_color_hex(0xbdd9e8), 0);
        lv_obj_set_style_image_recolor_opa(battery_frame, (255 * 100 / 100), 0);

        lv_obj_t * battery_page_1 = lv_tabview_add_tab(battery_pages, "");
        lv_obj_set_name(battery_page_1, "battery_page_1");
        lv_obj_set_flag(battery_page_1, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(battery_page_1, LV_OBJ_FLAG_GESTURE_BUBBLE, false);
        lv_obj_set_style_bg_opa(battery_page_1, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(battery_page_1, COLOR_BG, 0);
        lv_obj_add_style(battery_page_1, &clear, 0);
        lv_obj_t * battery_title_1 = lv_label_create(battery_page_1);
        lv_obj_set_name(battery_title_1, "battery_title_1");
        lv_label_set_text(battery_title_1, "НАПРЯЖЕНИЕ");
        lv_obj_set_x(battery_title_1, 0);
        lv_obj_set_y(battery_title_1, 2);
        lv_obj_set_width(battery_title_1, lv_pct(100));
        lv_obj_set_style_text_font(battery_title_1, ui_small, 0);
        lv_obj_set_style_text_color(battery_title_1, COLOR_TEXT, 0);
        lv_obj_set_flag(battery_title_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(battery_title_1, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * battery_value_1 = lv_label_create(battery_page_1);
        lv_obj_set_name(battery_value_1, "battery_value_1");
        lv_label_set_text(battery_value_1, "72.0");
        lv_obj_set_x(battery_value_1, 0);
        lv_obj_set_y(battery_value_1, 13);
        lv_obj_set_width(battery_value_1, lv_pct(100));
        lv_obj_set_style_text_font(battery_value_1, ui_value, 0);
        lv_obj_set_style_text_color(battery_value_1, COLOR_TEXT, 0);
        lv_obj_set_flag(battery_value_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(battery_value_1, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_height(battery_value_1, 44);

        lv_obj_t * battery_units_1 = lv_label_create(battery_page_1);
        lv_obj_set_name(battery_units_1, "battery_units_1");
        lv_label_set_text(battery_units_1, "В");
        lv_obj_set_x(battery_units_1, 0);
        lv_obj_set_y(battery_units_1, 49);
        lv_obj_set_width(battery_units_1, lv_pct(100));
        lv_obj_set_style_text_font(battery_units_1, ui_small, 0);
        lv_obj_set_style_text_color(battery_units_1, COLOR_TEXT, 0);
        lv_obj_set_flag(battery_units_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(battery_units_1, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * battery_page_2 = lv_tabview_add_tab(battery_pages, "");
        lv_obj_set_name(battery_page_2, "battery_page_2");
        lv_obj_set_flag(battery_page_2, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(battery_page_2, LV_OBJ_FLAG_GESTURE_BUBBLE, false);
        lv_obj_set_style_bg_opa(battery_page_2, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(battery_page_2, COLOR_BG, 0);
        lv_obj_add_style(battery_page_2, &clear, 0);
        lv_obj_t * battery_title_2 = lv_label_create(battery_page_2);
        lv_obj_set_name(battery_title_2, "battery_title_2");
        lv_label_set_text(battery_title_2, "МОЩНОСТЬ");
        lv_obj_set_x(battery_title_2, 0);
        lv_obj_set_y(battery_title_2, 2);
        lv_obj_set_width(battery_title_2, lv_pct(100));
        lv_obj_set_style_text_font(battery_title_2, ui_small, 0);
        lv_obj_set_style_text_color(battery_title_2, COLOR_TEXT, 0);
        lv_obj_set_flag(battery_title_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(battery_title_2, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * battery_value_2 = lv_label_create(battery_page_2);
        lv_obj_set_name(battery_value_2, "battery_value_2");
        lv_label_set_text(battery_value_2, "590");
        lv_obj_set_x(battery_value_2, 0);
        lv_obj_set_y(battery_value_2, 13);
        lv_obj_set_width(battery_value_2, lv_pct(100));
        lv_obj_set_style_text_font(battery_value_2, ui_value, 0);
        lv_obj_set_style_text_color(battery_value_2, COLOR_TEXT, 0);
        lv_obj_set_flag(battery_value_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(battery_value_2, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_height(battery_value_2, 44);

        lv_obj_t * battery_units_2 = lv_label_create(battery_page_2);
        lv_obj_set_name(battery_units_2, "battery_units_2");
        lv_label_set_text(battery_units_2, "Вт");
        lv_obj_set_x(battery_units_2, 0);
        lv_obj_set_y(battery_units_2, 49);
        lv_obj_set_width(battery_units_2, lv_pct(100));
        lv_obj_set_style_text_font(battery_units_2, ui_small, 0);
        lv_obj_set_style_text_color(battery_units_2, COLOR_TEXT, 0);
        lv_obj_set_flag(battery_units_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(battery_units_2, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * battery_dot_0 = lv_tabview_get_tab_button(battery_pages, 0);
        lv_obj_set_name(battery_dot_0, "battery_dot_0");
        lv_obj_set_width(battery_dot_0, 5);
        lv_obj_set_height(battery_dot_0, 5);
        lv_obj_set_flex_grow(battery_dot_0, 0);
        lv_obj_set_ext_click_area(battery_dot_0, 8);
        lv_obj_set_style_bg_color(battery_dot_0, lv_color_hex(0x375469), 0);
        lv_obj_set_style_bg_opa(battery_dot_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(battery_dot_0, 100, 0);
        lv_obj_set_style_border_width(battery_dot_0, 0, 0);
        lv_obj_set_style_pad_all(battery_dot_0, 0, 0);
        lv_obj_set_style_shadow_width(battery_dot_0, 0, 0);
        lv_obj_set_style_bg_color(battery_dot_0, COLOR_CYAN, LV_STATE_CHECKED);
        lv_obj_set_style_width(battery_dot_0, 16, LV_STATE_CHECKED);
        lv_obj_t * battery_dot_1 = lv_tabview_get_tab_button(battery_pages, 1);
        lv_obj_set_name(battery_dot_1, "battery_dot_1");
        lv_obj_set_width(battery_dot_1, 5);
        lv_obj_set_height(battery_dot_1, 5);
        lv_obj_set_flex_grow(battery_dot_1, 0);
        lv_obj_set_ext_click_area(battery_dot_1, 8);
        lv_obj_set_style_bg_color(battery_dot_1, lv_color_hex(0x375469), 0);
        lv_obj_set_style_bg_opa(battery_dot_1, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(battery_dot_1, 100, 0);
        lv_obj_set_style_border_width(battery_dot_1, 0, 0);
        lv_obj_set_style_pad_all(battery_dot_1, 0, 0);
        lv_obj_set_style_shadow_width(battery_dot_1, 0, 0);
        lv_obj_set_style_bg_color(battery_dot_1, COLOR_CYAN, LV_STATE_CHECKED);
        lv_obj_set_style_width(battery_dot_1, 16, LV_STATE_CHECKED);
        lv_obj_t * battery_dot_2 = lv_tabview_get_tab_button(battery_pages, 2);
        lv_obj_set_name(battery_dot_2, "battery_dot_2");
        lv_obj_set_width(battery_dot_2, 5);
        lv_obj_set_height(battery_dot_2, 5);
        lv_obj_set_flex_grow(battery_dot_2, 0);
        lv_obj_set_ext_click_area(battery_dot_2, 8);
        lv_obj_set_style_bg_color(battery_dot_2, lv_color_hex(0x375469), 0);
        lv_obj_set_style_bg_opa(battery_dot_2, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(battery_dot_2, 100, 0);
        lv_obj_set_style_border_width(battery_dot_2, 0, 0);
        lv_obj_set_style_pad_all(battery_dot_2, 0, 0);
        lv_obj_set_style_shadow_width(battery_dot_2, 0, 0);
        lv_obj_set_style_bg_color(battery_dot_2, COLOR_CYAN, LV_STATE_CHECKED);
        lv_obj_set_style_width(battery_dot_2, 16, LV_STATE_CHECKED);

        lv_obj_t * trip_card = lv_obj_create(lv_obj_0);
        lv_obj_set_name(trip_card, "trip_card");
        lv_obj_set_x(trip_card, 351);
        lv_obj_set_y(trip_card, 35);
        lv_obj_set_width(trip_card, 121);
        lv_obj_set_height(trip_card, 102);
        lv_obj_set_flag(trip_card, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_clip_corner(trip_card, true, 0);
        lv_obj_add_style(trip_card, &card, 0);
        lv_obj_t * trip_pages = lv_tabview_create(trip_card);
        lv_obj_set_name(trip_pages, "trip_pages");
        lv_obj_set_x(trip_pages, 0);
        lv_obj_set_y(trip_pages, 0);
        lv_obj_set_width(trip_pages, lv_pct(100));
        lv_obj_set_height(trip_pages, lv_pct(100));
        lv_tabview_set_tab_bar_position(trip_pages, LV_DIR_BOTTOM);
        lv_obj_set_style_radius(trip_pages, 10, 0);
        lv_obj_set_style_clip_corner(trip_pages, true, 0);
        lv_obj_set_flag(trip_pages, LV_OBJ_FLAG_SCROLL_ELASTIC, false);
        lv_obj_set_flag(trip_pages, LV_OBJ_FLAG_GESTURE_BUBBLE, false);
        lv_obj_add_style(trip_pages, &clear, 0);
        lv_obj_t * trip_dots = lv_tabview_get_tab_bar(trip_pages);
        lv_obj_set_name(trip_dots, "trip_dots");
        lv_obj_set_height(trip_dots, 10);
        lv_obj_set_style_pad_all(trip_dots, 0, 0);
        lv_obj_set_style_bg_opa(trip_dots, (255 * 0 / 100), 0);
        lv_obj_set_style_border_width(trip_dots, 0, 0);
        lv_obj_set_style_flex_main_place(trip_dots, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_cross_place(trip_dots, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_column(trip_dots, 12, 0);
        lv_obj_set_width(trip_dots, 119);
        lv_obj_t * trip_page_0 = lv_tabview_add_tab(trip_pages, "");
        lv_obj_set_name(trip_page_0, "trip_page_0");
        lv_obj_set_flag(trip_page_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(trip_page_0, LV_OBJ_FLAG_GESTURE_BUBBLE, false);
        lv_obj_set_style_bg_opa(trip_page_0, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(trip_page_0, COLOR_BG, 0);
        lv_obj_add_style(trip_page_0, &clear, 0);
        lv_obj_t * trip_title_0 = lv_label_create(trip_page_0);
        lv_obj_set_name(trip_title_0, "trip_title_0");
        lv_label_set_text(trip_title_0, "ПОЕЗДКА");
        lv_obj_set_x(trip_title_0, 0);
        lv_obj_set_y(trip_title_0, 5);
        lv_obj_set_width(trip_title_0, 119);
        lv_obj_set_style_text_font(trip_title_0, ui_small, 0);
        lv_obj_set_style_text_color(trip_title_0, COLOR_TEXT, 0);
        lv_obj_set_flag(trip_title_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(trip_title_0, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * trip_value_0 = lv_label_create(trip_page_0);
        lv_obj_set_name(trip_value_0, "trip_value_0");
        lv_label_set_text(trip_value_0, "24.8");
        lv_obj_set_x(trip_value_0, 0);
        lv_obj_set_y(trip_value_0, 28);
        lv_obj_set_width(trip_value_0, 119);
        lv_obj_set_style_text_font(trip_value_0, ui_value, 0);
        lv_obj_set_style_text_color(trip_value_0, COLOR_TEXT, 0);
        lv_obj_set_flag(trip_value_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(trip_value_0, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * trip_units_0 = lv_label_create(trip_page_0);
        lv_obj_set_name(trip_units_0, "trip_units_0");
        lv_label_set_text(trip_units_0, "КМ");
        lv_obj_set_x(trip_units_0, 0);
        lv_obj_set_y(trip_units_0, 65);
        lv_obj_set_width(trip_units_0, 119);
        lv_obj_set_style_text_font(trip_units_0, ui_small, 0);
        lv_obj_set_style_text_color(trip_units_0, COLOR_TEXT, 0);
        lv_obj_set_flag(trip_units_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(trip_units_0, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * trip_page_1 = lv_tabview_add_tab(trip_pages, "");
        lv_obj_set_name(trip_page_1, "trip_page_1");
        lv_obj_set_flag(trip_page_1, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(trip_page_1, LV_OBJ_FLAG_GESTURE_BUBBLE, false);
        lv_obj_set_style_bg_opa(trip_page_1, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(trip_page_1, COLOR_BG, 0);
        lv_obj_add_style(trip_page_1, &clear, 0);
        lv_obj_t * trip_title_1 = lv_label_create(trip_page_1);
        lv_obj_set_name(trip_title_1, "trip_title_1");
        lv_label_set_text(trip_title_1, "ОБЩИЙ");
        lv_obj_set_x(trip_title_1, 0);
        lv_obj_set_y(trip_title_1, 5);
        lv_obj_set_width(trip_title_1, 119);
        lv_obj_set_style_text_font(trip_title_1, ui_small, 0);
        lv_obj_set_style_text_color(trip_title_1, COLOR_TEXT, 0);
        lv_obj_set_flag(trip_title_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(trip_title_1, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * trip_value_1 = lv_label_create(trip_page_1);
        lv_obj_set_name(trip_value_1, "trip_value_1");
        lv_label_set_text(trip_value_1, "1524.7");
        lv_obj_set_x(trip_value_1, 0);
        lv_obj_set_y(trip_value_1, 28);
        lv_obj_set_width(trip_value_1, 119);
        lv_obj_set_style_text_font(trip_value_1, ui_value, 0);
        lv_obj_set_style_text_color(trip_value_1, COLOR_TEXT, 0);
        lv_obj_set_flag(trip_value_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(trip_value_1, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * trip_units_1 = lv_label_create(trip_page_1);
        lv_obj_set_name(trip_units_1, "trip_units_1");
        lv_label_set_text(trip_units_1, "КМ");
        lv_obj_set_x(trip_units_1, 0);
        lv_obj_set_y(trip_units_1, 65);
        lv_obj_set_width(trip_units_1, 119);
        lv_obj_set_style_text_font(trip_units_1, ui_small, 0);
        lv_obj_set_style_text_color(trip_units_1, COLOR_TEXT, 0);
        lv_obj_set_flag(trip_units_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(trip_units_1, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * trip_page_2 = lv_tabview_add_tab(trip_pages, "");
        lv_obj_set_name(trip_page_2, "trip_page_2");
        lv_obj_set_flag(trip_page_2, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(trip_page_2, LV_OBJ_FLAG_GESTURE_BUBBLE, false);
        lv_obj_set_style_bg_opa(trip_page_2, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(trip_page_2, COLOR_BG, 0);
        lv_obj_add_style(trip_page_2, &clear, 0);
        lv_obj_t * trip_title_2 = lv_label_create(trip_page_2);
        lv_obj_set_name(trip_title_2, "trip_title_2");
        lv_label_set_text(trip_title_2, "ВРЕМЯ");
        lv_obj_set_x(trip_title_2, 0);
        lv_obj_set_y(trip_title_2, 5);
        lv_obj_set_width(trip_title_2, 119);
        lv_obj_set_style_text_font(trip_title_2, ui_small, 0);
        lv_obj_set_style_text_color(trip_title_2, COLOR_TEXT, 0);
        lv_obj_set_flag(trip_title_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(trip_title_2, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * trip_value_2 = lv_label_create(trip_page_2);
        lv_obj_set_name(trip_value_2, "trip_value_2");
        lv_label_set_text(trip_value_2, "01:23");
        lv_obj_set_x(trip_value_2, 0);
        lv_obj_set_y(trip_value_2, 28);
        lv_obj_set_width(trip_value_2, 119);
        lv_obj_set_style_text_font(trip_value_2, ui_value, 0);
        lv_obj_set_style_text_color(trip_value_2, COLOR_TEXT, 0);
        lv_obj_set_flag(trip_value_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(trip_value_2, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * trip_units_2 = lv_label_create(trip_page_2);
        lv_obj_set_name(trip_units_2, "trip_units_2");
        lv_label_set_text(trip_units_2, "ЧАС : МИН");
        lv_obj_set_x(trip_units_2, 0);
        lv_obj_set_y(trip_units_2, 65);
        lv_obj_set_width(trip_units_2, 119);
        lv_obj_set_style_text_font(trip_units_2, ui_small, 0);
        lv_obj_set_style_text_color(trip_units_2, COLOR_TEXT, 0);
        lv_obj_set_flag(trip_units_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_text_align(trip_units_2, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * trip_dot_0 = lv_tabview_get_tab_button(trip_pages, 0);
        lv_obj_set_name(trip_dot_0, "trip_dot_0");
        lv_obj_set_width(trip_dot_0, 5);
        lv_obj_set_height(trip_dot_0, 5);
        lv_obj_set_flex_grow(trip_dot_0, 0);
        lv_obj_set_ext_click_area(trip_dot_0, 8);
        lv_obj_set_style_bg_color(trip_dot_0, lv_color_hex(0x375469), 0);
        lv_obj_set_style_bg_opa(trip_dot_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(trip_dot_0, 100, 0);
        lv_obj_set_style_border_width(trip_dot_0, 0, 0);
        lv_obj_set_style_pad_all(trip_dot_0, 0, 0);
        lv_obj_set_style_shadow_width(trip_dot_0, 0, 0);
        lv_obj_set_style_bg_color(trip_dot_0, COLOR_CYAN, LV_STATE_CHECKED);
        lv_obj_set_style_width(trip_dot_0, 16, LV_STATE_CHECKED);
        lv_obj_t * trip_dot_1 = lv_tabview_get_tab_button(trip_pages, 1);
        lv_obj_set_name(trip_dot_1, "trip_dot_1");
        lv_obj_set_width(trip_dot_1, 5);
        lv_obj_set_height(trip_dot_1, 5);
        lv_obj_set_flex_grow(trip_dot_1, 0);
        lv_obj_set_ext_click_area(trip_dot_1, 8);
        lv_obj_set_style_bg_color(trip_dot_1, lv_color_hex(0x375469), 0);
        lv_obj_set_style_bg_opa(trip_dot_1, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(trip_dot_1, 100, 0);
        lv_obj_set_style_border_width(trip_dot_1, 0, 0);
        lv_obj_set_style_pad_all(trip_dot_1, 0, 0);
        lv_obj_set_style_shadow_width(trip_dot_1, 0, 0);
        lv_obj_set_style_bg_color(trip_dot_1, COLOR_CYAN, LV_STATE_CHECKED);
        lv_obj_set_style_width(trip_dot_1, 16, LV_STATE_CHECKED);
        lv_obj_t * trip_dot_2 = lv_tabview_get_tab_button(trip_pages, 2);
        lv_obj_set_name(trip_dot_2, "trip_dot_2");
        lv_obj_set_width(trip_dot_2, 5);
        lv_obj_set_height(trip_dot_2, 5);
        lv_obj_set_flex_grow(trip_dot_2, 0);
        lv_obj_set_ext_click_area(trip_dot_2, 8);
        lv_obj_set_style_bg_color(trip_dot_2, lv_color_hex(0x375469), 0);
        lv_obj_set_style_bg_opa(trip_dot_2, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(trip_dot_2, 100, 0);
        lv_obj_set_style_border_width(trip_dot_2, 0, 0);
        lv_obj_set_style_pad_all(trip_dot_2, 0, 0);
        lv_obj_set_style_shadow_width(trip_dot_2, 0, 0);
        lv_obj_set_style_bg_color(trip_dot_2, COLOR_CYAN, LV_STATE_CHECKED);
        lv_obj_set_style_width(trip_dot_2, 16, LV_STATE_CHECKED);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

