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
    static const char * translation_languages[] = {"en", "de", NULL};
    static const char * translation_tags[] = {"dog", "cat", "house", NULL};
    static const char * translation_texts[] = {
        "This is a dog", "Das ist ein Hund", /* dog */
        "A curious little cat", "Eine neugierige kleine Katze", /* cat */
        "The house is cozy and warm", "Das Haus ist gemütlich und warm", /* house */
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
const void * dashboard_background = NULL;
extern const void * dashboard_background_data;
const void * icon_left = NULL;
extern const void * icon_left_data;
const void * icon_right = NULL;
extern const void * icon_right_data;
const void * icon_position = NULL;
extern const void * icon_position_data;
const void * icon_low_beam = NULL;
extern const void * icon_low_beam_data;
const void * icon_high_beam = NULL;
extern const void * icon_high_beam_data;
const void * icon_brake = NULL;
extern const void * icon_brake_data;
const void * icon_warning = NULL;
extern const void * icon_warning_data;
const void * icon_hazard = NULL;
extern const void * icon_hazard_data;
const void * icon_battery = NULL;
extern const void * icon_battery_data;
const void * icon_thermometer = NULL;
extern const void * icon_thermometer_data;

/*----------------
 * Global styles
 *----------------*/

lv_style_t card;
lv_style_t button;
lv_style_t selected;
lv_style_t limit_selected;
lv_style_t battery_fill;
lv_style_t arc_indicator;
lv_style_t arc_knob;
lv_style_t minor_ticks;
lv_style_t major_ticks;
lv_style_t dot_selected_0;
lv_style_t dot_selected_1;
lv_style_t dot_selected_2;

/*----------------
 * Subjects
 *----------------*/

lv_subject_t demo_speed;
lv_subject_t demo_battery;
lv_subject_t demo_mode;
lv_subject_t demo_limit;
lv_subject_t demo_meter;
lv_subject_t demo_hazard;
lv_subject_t demo_blink_on;
lv_subject_t demo_blink_off;
lv_subject_t demo_hold;
lv_subject_t demo_double;

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
        /* dashboard_background */
        if (!dashboard_background) {
            dashboard_background = &dashboard_background_data;
        }
        /* icon_left */
        if (!icon_left) {
            icon_left = &icon_left_data;
        }
        /* icon_right */
        if (!icon_right) {
            icon_right = &icon_right_data;
        }
        /* icon_position */
        if (!icon_position) {
            icon_position = &icon_position_data;
        }
        /* icon_low_beam */
        if (!icon_low_beam) {
            icon_low_beam = &icon_low_beam_data;
        }
        /* icon_high_beam */
        if (!icon_high_beam) {
            icon_high_beam = &icon_high_beam_data;
        }
        /* icon_brake */
        if (!icon_brake) {
            icon_brake = &icon_brake_data;
        }
        /* icon_warning */
        if (!icon_warning) {
            icon_warning = &icon_warning_data;
        }
        /* icon_hazard */
        if (!icon_hazard) {
            icon_hazard = &icon_hazard_data;
        }
        /* icon_battery */
        if (!icon_battery) {
            icon_battery = &icon_battery_data;
        }
        /* icon_thermometer */
        if (!icon_thermometer) {
            icon_thermometer = &icon_thermometer_data;
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
        lv_style_init(&battery_fill);
        lv_style_init(&arc_indicator);
        lv_style_init(&arc_knob);
        lv_style_init(&minor_ticks);
        lv_style_init(&major_ticks);
        lv_style_init(&dot_selected_0);
        lv_style_init(&dot_selected_1);
        lv_style_init(&dot_selected_2);

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
        lv_style_set_bg_color(&battery_fill, lv_color_hex(0x31f29a));
        lv_style_set_arc_color(&arc_indicator, COLOR_RED);
        lv_style_set_arc_width(&arc_indicator, 8);
        lv_style_set_bg_opa(&arc_knob, (255 * 0 / 100));
        lv_style_set_border_width(&arc_knob, 0);
        lv_style_set_shadow_width(&arc_knob, 0);
        lv_style_set_length(&minor_ticks, 6);
        lv_style_set_line_width(&minor_ticks, 1);
        lv_style_set_line_color(&minor_ticks, lv_color_hex(0x91b1c6));
        lv_style_set_line_opa(&minor_ticks, (255 * 50 / 100));
        lv_style_set_length(&major_ticks, 13);
        lv_style_set_line_width(&major_ticks, 2);
        lv_style_set_line_color(&major_ticks, lv_color_hex(0xc5dfef));
        lv_style_set_text_font(&major_ticks, ui_small);
        lv_style_set_text_color(&major_ticks, lv_color_hex(0xbcd8ea));
        lv_style_set_bg_color(&dot_selected_0, COLOR_CYAN);
        lv_style_set_width(&dot_selected_0, 16);
        lv_style_set_bg_color(&dot_selected_1, COLOR_CYAN);
        lv_style_set_width(&dot_selected_1, 16);
        lv_style_set_bg_color(&dot_selected_2, COLOR_CYAN);
        lv_style_set_width(&dot_selected_2, 16);

        style_inited = true;
    }

    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_int(&demo_speed, 42);
    lv_subject_init_int(&demo_battery, 82);
    lv_subject_init_int(&demo_mode, 1);
    lv_subject_init_int(&demo_limit, 50);
    lv_subject_init_int(&demo_meter, 0);
    lv_subject_init_int(&demo_hazard, 0);
    lv_subject_init_int(&demo_blink_on, 450);
    lv_subject_init_int(&demo_blink_off, 450);
    lv_subject_init_int(&demo_hold, 2000);
    lv_subject_init_int(&demo_double, 300);

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
    lv_xml_register_subject(NULL, "demo_speed", &demo_speed);
    lv_xml_register_subject(NULL, "demo_battery", &demo_battery);
    lv_xml_register_subject(NULL, "demo_mode", &demo_mode);
    lv_xml_register_subject(NULL, "demo_limit", &demo_limit);
    lv_xml_register_subject(NULL, "demo_meter", &demo_meter);
    lv_xml_register_subject(NULL, "demo_hazard", &demo_hazard);
    lv_xml_register_subject(NULL, "demo_blink_on", &demo_blink_on);
    lv_xml_register_subject(NULL, "demo_blink_off", &demo_blink_off);
    lv_xml_register_subject(NULL, "demo_hold", &demo_hold);
    lv_xml_register_subject(NULL, "demo_double", &demo_double);

    /* Register callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if defined(LV_USE_XML) && LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "dashboard_background", dashboard_background);
    lv_xml_register_image(NULL, "icon_left", icon_left);
    lv_xml_register_image(NULL, "icon_right", icon_right);
    lv_xml_register_image(NULL, "icon_position", icon_position);
    lv_xml_register_image(NULL, "icon_low_beam", icon_low_beam);
    lv_xml_register_image(NULL, "icon_high_beam", icon_high_beam);
    lv_xml_register_image(NULL, "icon_brake", icon_brake);
    lv_xml_register_image(NULL, "icon_warning", icon_warning);
    lv_xml_register_image(NULL, "icon_hazard", icon_hazard);
    lv_xml_register_image(NULL, "icon_battery", icon_battery);
    lv_xml_register_image(NULL, "icon_thermometer", icon_thermometer);
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