/**
 * @file ebike_display_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "ebike_display_gen.h"

#if defined(LV_USE_XML) && LV_USE_XML
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void check_font(lv_font_t ** font, const char * name);

/**********************
 *  STATIC VARIABLES
 **********************/

static uint32_t ebike_display_target = EBIKE_DISPLAY_TARGET_ALL;

/*----------------
 * Translations
 *----------------*/

#ifndef LV_EDITOR_PREVIEW
    static const char * translation_languages[] = {"ru", NULL};
    static const char * translation_tags[] = {"app_title", NULL};
    static const char * translation_texts[] = {
        "Велодисплей", /* app_title */
    };
#endif

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

/*----------------
 * Fonts
 *----------------*/

lv_font_t * ui_small;
extern lv_font_t ui_small_data;
lv_font_t * ui_medium;
extern lv_font_t ui_medium_data;
lv_font_t * ui_value;
extern lv_font_t ui_value_data;
lv_font_t * ui_speed;
extern lv_font_t ui_speed_data;

/*----------------
 * Images
 *----------------*/

/* Targets: any */
const void * ui_img_background = NULL;
extern const void * ui_img_background_data;
const void * ui_img_left = NULL;
extern const void * ui_img_left_data;
const void * ui_img_right = NULL;
extern const void * ui_img_right_data;
const void * ui_img_position = NULL;
extern const void * ui_img_position_data;
const void * ui_img_low_beam = NULL;
extern const void * ui_img_low_beam_data;
const void * ui_img_high_beam = NULL;
extern const void * ui_img_high_beam_data;
const void * ui_img_brake = NULL;
extern const void * ui_img_brake_data;
const void * ui_img_warning = NULL;
extern const void * ui_img_warning_data;
const void * ui_img_hazard = NULL;
extern const void * ui_img_hazard_data;
const void * ui_img_battery = NULL;
extern const void * ui_img_battery_data;
const void * ui_img_thermometer = NULL;
extern const void * ui_img_thermometer_data;
const void * ui_img_gauge_active = NULL;
extern const void * ui_img_gauge_active_data;
const void * ui_img_gauge_idle = NULL;
extern const void * ui_img_gauge_idle_data;
const void * ui_img_gauge_bezel = NULL;
extern const void * ui_img_gauge_bezel_data;
const void * ui_img_switch_track = NULL;
extern const void * ui_img_switch_track_data;
const void * ui_img_switch_knob = NULL;
extern const void * ui_img_switch_knob_data;

/*----------------
 * Global styles
 *----------------*/

lv_style_t card;
lv_style_t button;
lv_style_t selected;
lv_style_t limit_selected;
lv_style_t battery_fill_style;
lv_style_t arc_indicator;
lv_style_t arc_knob;
lv_style_t minor_ticks;
lv_style_t major_ticks;
lv_style_t half_major_ticks;
lv_style_t inner_minor_ticks;
lv_style_t inner_major_ticks;
lv_style_t dot_selected_0;
lv_style_t dot_selected_1;
lv_style_t dot_selected_2;
lv_style_t clear;
lv_style_t switch_track;
lv_style_t switch_indicator;
lv_style_t switch_on;
lv_style_t switch_knob;

/*----------------
 * Subjects
 *----------------*/

lv_subject_t ui_speed_value;
lv_subject_t ui_battery;
lv_subject_t ui_mode;
lv_subject_t ui_limit;
lv_subject_t ui_meter;
lv_subject_t ui_hazard;
lv_subject_t ui_blink_on;
lv_subject_t ui_blink_off;
lv_subject_t ui_hold;
lv_subject_t ui_double;
lv_subject_t ui_flip;
lv_subject_t ui_motion;
lv_subject_t ui_cruise;
lv_subject_t ui_safe;
lv_subject_t ui_mode_text;
lv_subject_t ui_notice;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ebike_display_init_gen(const char * asset_path)
{
    /* When running from the editor the theme set from the XML should overwrite this */
#if !defined(LV_EDITOR_PREVIEW)
#if LV_USE_THEME_SIMPLE
    lv_display_t * disp = lv_display_get_default();
    lv_theme_t * th = lv_theme_simple_init(disp);
    lv_display_set_theme(disp, th);
#else
    LV_LOG_WARN("Simple theme is selected in project.xml but LV_USE_THEME_SIMPLE is disabled");
#endif
#endif /*LV_EDITOR_PREVIEW*/


    /*----------------
     * Fonts
     *----------------*/

    /* Targets: any */

    #if EBIKE_DISPLAY_CHECK_COMPILE_TARGET(EBIKE_DISPLAY_TARGET_ALL)
    if (ebike_display_check_target(EBIKE_DISPLAY_TARGET_ALL)) {
        if (!ui_small) {
            /* ui_small */
            /* get font 'ui_small' from a C array */
            ui_small = &ui_small_data;

        }
        if (!ui_medium) {
            /* ui_medium */
            /* get font 'ui_medium' from a C array */
            ui_medium = &ui_medium_data;

        }
        if (!ui_value) {
            /* ui_value */
            /* get font 'ui_value' from a C array */
            ui_value = &ui_value_data;

        }
        if (!ui_speed) {
            /* ui_speed */
            /* get font 'ui_speed' from a C array */
            ui_speed = &ui_speed_data;

        }
    }
    #endif

    /*----------------
     * Images
     *----------------*/

    /* Targets: any */
    #if EBIKE_DISPLAY_CHECK_COMPILE_TARGET(EBIKE_DISPLAY_TARGET_ALL)
    if (ebike_display_check_target(EBIKE_DISPLAY_TARGET_ALL)) {
        /* ui_img_background */
        if (!ui_img_background) {
            ui_img_background = &ui_img_background_data;
        }
        /* ui_img_left */
        if (!ui_img_left) {
            ui_img_left = &ui_img_left_data;
        }
        /* ui_img_right */
        if (!ui_img_right) {
            ui_img_right = &ui_img_right_data;
        }
        /* ui_img_position */
        if (!ui_img_position) {
            ui_img_position = &ui_img_position_data;
        }
        /* ui_img_low_beam */
        if (!ui_img_low_beam) {
            ui_img_low_beam = &ui_img_low_beam_data;
        }
        /* ui_img_high_beam */
        if (!ui_img_high_beam) {
            ui_img_high_beam = &ui_img_high_beam_data;
        }
        /* ui_img_brake */
        if (!ui_img_brake) {
            ui_img_brake = &ui_img_brake_data;
        }
        /* ui_img_warning */
        if (!ui_img_warning) {
            ui_img_warning = &ui_img_warning_data;
        }
        /* ui_img_hazard */
        if (!ui_img_hazard) {
            ui_img_hazard = &ui_img_hazard_data;
        }
        /* ui_img_battery */
        if (!ui_img_battery) {
            ui_img_battery = &ui_img_battery_data;
        }
        /* ui_img_thermometer */
        if (!ui_img_thermometer) {
            ui_img_thermometer = &ui_img_thermometer_data;
        }
        /* ui_img_gauge_active */
        if (!ui_img_gauge_active) {
            ui_img_gauge_active = &ui_img_gauge_active_data;
        }
        /* ui_img_gauge_idle */
        if (!ui_img_gauge_idle) {
            ui_img_gauge_idle = &ui_img_gauge_idle_data;
        }
        /* ui_img_gauge_bezel */
        if (!ui_img_gauge_bezel) {
            ui_img_gauge_bezel = &ui_img_gauge_bezel_data;
        }
        /* ui_img_switch_track */
        if (!ui_img_switch_track) {
            ui_img_switch_track = &ui_img_switch_track_data;
        }
        /* ui_img_switch_knob */
        if (!ui_img_switch_knob) {
            ui_img_switch_knob = &ui_img_switch_knob_data;
        }
    }
    #endif

    /*----------------
     * Global styles
     *----------------*/

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&card);
        lv_style_init(&button);
        lv_style_init(&selected);
        lv_style_init(&limit_selected);
        lv_style_init(&battery_fill_style);
        lv_style_init(&arc_indicator);
        lv_style_init(&arc_knob);
        lv_style_init(&minor_ticks);
        lv_style_init(&major_ticks);
        lv_style_init(&half_major_ticks);
        lv_style_init(&inner_minor_ticks);
        lv_style_init(&inner_major_ticks);
        lv_style_init(&dot_selected_0);
        lv_style_init(&dot_selected_1);
        lv_style_init(&dot_selected_2);
        lv_style_init(&clear);
        lv_style_init(&switch_track);
        lv_style_init(&switch_indicator);
        lv_style_init(&switch_on);
        lv_style_init(&switch_knob);

        lv_style_set_bg_color(&card, COLOR_BG);
        lv_style_set_bg_opa(&card, (255 * 92 / 100));
        lv_style_set_border_color(&card, lv_color_hex(0x315870));
        lv_style_set_border_width(&card, 1);
        lv_style_set_radius(&card, 12);
        lv_style_set_pad_all(&card, 0);
        lv_style_set_bg_color(&button, lv_color_hex(0x112b3d));
        lv_style_set_bg_grad_color(&button, lv_color_hex(0x050e18));
        lv_style_set_bg_grad_dir(&button, LV_GRAD_DIR_VER);
        lv_style_set_bg_opa(&button, (255 * 100 / 100));
        lv_style_set_border_color(&button, lv_color_hex(0x37647e));
        lv_style_set_border_width(&button, 1);
        lv_style_set_radius(&button, 10);
        lv_style_set_pad_all(&button, 0);
        lv_style_set_shadow_width(&button, 0);
        lv_style_set_bg_color(&selected, lv_color_hex(0x084b69));
        lv_style_set_border_color(&selected, COLOR_CYAN);
        lv_style_set_border_width(&selected, 2);
        lv_style_set_bg_color(&limit_selected, lv_color_hex(0x491b20));
        lv_style_set_border_color(&limit_selected, COLOR_RED);
        lv_style_set_border_width(&limit_selected, 2);
        lv_style_set_bg_color(&battery_fill_style, lv_color_hex(0x31f29a));
        lv_style_set_arc_color(&arc_indicator, COLOR_RED);
        lv_style_set_arc_width(&arc_indicator, 13);
        lv_style_set_arc_image_src(&arc_indicator, ui_img_gauge_active);
        lv_style_set_bg_opa(&arc_knob, (255 * 0 / 100));
        lv_style_set_border_width(&arc_knob, 0);
        lv_style_set_shadow_width(&arc_knob, 0);
        lv_style_set_length(&minor_ticks, 7);
        lv_style_set_radial_offset(&minor_ticks, -3);
        lv_style_set_line_width(&minor_ticks, 1);
        lv_style_set_line_color(&minor_ticks, lv_color_hex(0xe2eff8));
        lv_style_set_line_opa(&minor_ticks, (255 * 85 / 100));
        lv_style_set_length(&major_ticks, 20);
        lv_style_set_line_width(&major_ticks, 2);
        lv_style_set_line_color(&major_ticks, lv_color_hex(0xffffff));
        lv_style_set_text_font(&major_ticks, ui_small);
        lv_style_set_text_color(&major_ticks, lv_color_hex(0xbcd8ea));
        lv_style_set_length(&half_major_ticks, 13);
        lv_style_set_line_width(&half_major_ticks, 2);
        lv_style_set_line_color(&half_major_ticks, lv_color_hex(0xffffff));
        lv_style_set_length(&inner_minor_ticks, 5);
        lv_style_set_line_width(&inner_minor_ticks, 1);
        lv_style_set_line_color(&inner_minor_ticks, lv_color_hex(0x6c91a8));
        lv_style_set_line_opa(&inner_minor_ticks, (255 * 55 / 100));
        lv_style_set_length(&inner_major_ticks, 6);
        lv_style_set_line_width(&inner_major_ticks, 2);
        lv_style_set_line_color(&inner_major_ticks, lv_color_hex(0xe9f4ff));
        lv_style_set_text_font(&inner_major_ticks, ui_small);
        lv_style_set_text_color(&inner_major_ticks, lv_color_hex(0xbcd8ea));
        lv_style_set_bg_color(&dot_selected_0, COLOR_CYAN);
        lv_style_set_width(&dot_selected_0, 16);
        lv_style_set_bg_color(&dot_selected_1, COLOR_CYAN);
        lv_style_set_width(&dot_selected_1, 16);
        lv_style_set_bg_color(&dot_selected_2, COLOR_CYAN);
        lv_style_set_width(&dot_selected_2, 16);
        lv_style_set_bg_opa(&clear, (255 * 0 / 100));
        lv_style_set_border_width(&clear, 0);
        lv_style_set_pad_all(&clear, 0);
        lv_style_set_shadow_width(&clear, 0);
        lv_style_set_radius(&clear, 0);
        lv_style_set_radius(&switch_track, 100);
        lv_style_set_bg_opa(&switch_track, (255 * 0 / 100));
        lv_style_set_border_width(&switch_track, 0);
        lv_style_set_pad_all(&switch_track, 0);
        lv_style_set_shadow_width(&switch_track, 0);
        lv_style_set_bg_image_src(&switch_track, ui_img_switch_track);
        lv_style_set_bg_image_recolor(&switch_track, lv_color_hex(0x243c4d));
        lv_style_set_bg_image_recolor_opa(&switch_track, (255 * 100 / 100));
        lv_style_set_radius(&switch_indicator, 100);
        lv_style_set_bg_opa(&switch_indicator, (255 * 0 / 100));
        lv_style_set_border_width(&switch_indicator, 0);
        lv_style_set_bg_image_recolor(&switch_on, COLOR_CYAN);
        lv_style_set_radius(&switch_knob, 100);
        lv_style_set_bg_opa(&switch_knob, (255 * 0 / 100));
        lv_style_set_border_width(&switch_knob, 0);
        lv_style_set_pad_all(&switch_knob, -4);
        lv_style_set_shadow_width(&switch_knob, 0);
        lv_style_set_bg_image_src(&switch_knob, ui_img_switch_knob);
        lv_style_set_bg_image_recolor(&switch_knob, COLOR_TEXT);
        lv_style_set_bg_image_recolor_opa(&switch_knob, (255 * 100 / 100));

        style_inited = true;
    }

    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_int(&ui_speed_value, 42);
    lv_subject_init_int(&ui_battery, 82);
    lv_subject_init_int(&ui_mode, 1);
    lv_subject_init_int(&ui_limit, 50);
    lv_subject_init_int(&ui_meter, 0);
    lv_subject_init_int(&ui_hazard, 0);
    lv_subject_init_int(&ui_blink_on, 450);
    lv_subject_init_int(&ui_blink_off, 450);
    lv_subject_init_int(&ui_hold, 2000);
    lv_subject_init_int(&ui_double, 300);
    lv_subject_init_int(&ui_flip, 0);
    lv_subject_init_int(&ui_motion, 0);
    lv_subject_init_int(&ui_cruise, 0);
    lv_subject_init_int(&ui_safe, 1);
    static char ui_mode_text_buf[UI_SUBJECT_STRING_LENGTH];
    static char ui_mode_text_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&ui_mode_text,
                           ui_mode_text_buf,
                           ui_mode_text_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "Drive"
                          );
    static char ui_notice_buf[UI_SUBJECT_STRING_LENGTH];
    static char ui_notice_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&ui_notice,
                           ui_notice_buf,
                           ui_notice_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "ПРЕДПРОСМОТР · тестовые показания"
                          );

    /*----------------
     * Translations
     *----------------*/

    #ifndef LV_EDITOR_PREVIEW
        lv_translation_add_static(translation_languages, translation_tags, translation_texts);
        lv_translation_set_language(translation_languages[0]);
    #endif

#if defined(LV_USE_XML) && LV_USE_XML
    /* Register widgets */

    /* Check all fonts / default if needed. This prevents fonts that are used in one target but
       defined in another from causing assertion failures during rendering of the Preview. */
    check_font(&ui_small, "ui_small");
    check_font(&ui_medium, "ui_medium");
    check_font(&ui_value, "ui_value");
    check_font(&ui_speed, "ui_speed");

    /* Register fonts */
    lv_xml_register_font(NULL, "ui_small", ui_small);
    lv_xml_register_font(NULL, "ui_medium", ui_medium);
    lv_xml_register_font(NULL, "ui_value", ui_value);
    lv_xml_register_font(NULL, "ui_speed", ui_speed);

    /* Register subjects */
    lv_xml_register_subject(NULL, "ui_speed_value", &ui_speed_value);
    lv_xml_register_subject(NULL, "ui_battery", &ui_battery);
    lv_xml_register_subject(NULL, "ui_mode", &ui_mode);
    lv_xml_register_subject(NULL, "ui_limit", &ui_limit);
    lv_xml_register_subject(NULL, "ui_meter", &ui_meter);
    lv_xml_register_subject(NULL, "ui_hazard", &ui_hazard);
    lv_xml_register_subject(NULL, "ui_blink_on", &ui_blink_on);
    lv_xml_register_subject(NULL, "ui_blink_off", &ui_blink_off);
    lv_xml_register_subject(NULL, "ui_hold", &ui_hold);
    lv_xml_register_subject(NULL, "ui_double", &ui_double);
    lv_xml_register_subject(NULL, "ui_flip", &ui_flip);
    lv_xml_register_subject(NULL, "ui_motion", &ui_motion);
    lv_xml_register_subject(NULL, "ui_cruise", &ui_cruise);
    lv_xml_register_subject(NULL, "ui_safe", &ui_safe);
    lv_xml_register_subject(NULL, "ui_mode_text", &ui_mode_text);
    lv_xml_register_subject(NULL, "ui_notice", &ui_notice);

    /* Register callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if defined(LV_USE_XML) && LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "ui_img_background", ui_img_background);
    lv_xml_register_image(NULL, "ui_img_left", ui_img_left);
    lv_xml_register_image(NULL, "ui_img_right", ui_img_right);
    lv_xml_register_image(NULL, "ui_img_position", ui_img_position);
    lv_xml_register_image(NULL, "ui_img_low_beam", ui_img_low_beam);
    lv_xml_register_image(NULL, "ui_img_high_beam", ui_img_high_beam);
    lv_xml_register_image(NULL, "ui_img_brake", ui_img_brake);
    lv_xml_register_image(NULL, "ui_img_warning", ui_img_warning);
    lv_xml_register_image(NULL, "ui_img_hazard", ui_img_hazard);
    lv_xml_register_image(NULL, "ui_img_battery", ui_img_battery);
    lv_xml_register_image(NULL, "ui_img_thermometer", ui_img_thermometer);
    lv_xml_register_image(NULL, "ui_img_gauge_active", ui_img_gauge_active);
    lv_xml_register_image(NULL, "ui_img_gauge_idle", ui_img_gauge_idle);
    lv_xml_register_image(NULL, "ui_img_gauge_bezel", ui_img_gauge_bezel);
    lv_xml_register_image(NULL, "ui_img_switch_track", ui_img_switch_track);
    lv_xml_register_image(NULL, "ui_img_switch_knob", ui_img_switch_knob);
#endif

#if !defined(LV_USE_XML) || LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manually from XML using lv_xml_create() */
#endif
}

void ebike_display_set_target(uint32_t target)
{
    ebike_display_target = target;
}

uint32_t ebike_display_get_target(void)
{
    return ebike_display_target;
}

bool ebike_display_check_target(uint32_t target)
{
    return (ebike_display_target & target) ? true : false;
}

/* Callbacks */

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void check_font(lv_font_t ** font, const char * name)
{
    if (!(*font)) {
        *font = (lv_font_t *)LV_FONT_DEFAULT;
        LV_LOG_WARN("font `%s` was not set. Using `LV_FONT_DEFAULT` instead", name);
    }
}