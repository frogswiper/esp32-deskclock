#include "ui_main.h"
#include "ui_clock.h"
#include "ui_forecast.h"
#include "ui_settings.h"
#include "../board_config.h"

static lv_obj_t* g_tabview = nullptr;
static lv_obj_t* g_nav_btns[3] = {};
static int        g_active_tab  = 0;
static bool       g_save_requested = false;

static const char* TAB_ICONS[]  = {LV_SYMBOL_HOME, LV_SYMBOL_LIST, LV_SYMBOL_SETTINGS};
static const char* TAB_LABELS[] = {"Clock", "Forecast", "Settings"};

static lv_style_t style_nav_bar;
static lv_style_t style_btn_normal;
static lv_style_t style_btn_active;
static bool styles_inited = false;

static void init_styles() {
    if (styles_inited) return;

    lv_style_init(&style_nav_bar);
    lv_style_set_bg_color(&style_nav_bar, lv_color_white());
    lv_style_set_border_color(&style_nav_bar, lv_color_hex(0xE0E0E0));
    lv_style_set_border_width(&style_nav_bar, 1);
    lv_style_set_border_side(&style_nav_bar, LV_BORDER_SIDE_TOP);
    lv_style_set_radius(&style_nav_bar, 0);
    lv_style_set_shadow_width(&style_nav_bar, 0);
    lv_style_set_pad_all(&style_nav_bar, 0);

    lv_style_init(&style_btn_normal);
    lv_style_set_bg_opa(&style_btn_normal, LV_OPA_TRANSP);
    lv_style_set_border_width(&style_btn_normal, 0);
    lv_style_set_shadow_width(&style_btn_normal, 0);
    lv_style_set_radius(&style_btn_normal, 0);
    lv_style_set_text_color(&style_btn_normal, lv_color_hex(0xAAAAAA));

    lv_style_init(&style_btn_active);
    lv_style_set_bg_opa(&style_btn_active, LV_OPA_TRANSP);
    lv_style_set_border_width(&style_btn_active, 0);
    lv_style_set_shadow_width(&style_btn_active, 0);
    lv_style_set_radius(&style_btn_active, 0);
    lv_style_set_text_color(&style_btn_active, lv_color_hex(0x3D7FEA));

    styles_inited = true;
}

static void update_nav_styles() {
    for (int i = 0; i < 3; i++) {
        lv_obj_t* btn = g_nav_btns[i];
        lv_color_t col = (i == g_active_tab)
                         ? lv_color_hex(0x3D7FEA)
                         : lv_color_hex(0xAAAAAA);
        if (i == g_active_tab) {
            lv_obj_add_style(btn, &style_btn_active, 0);
            lv_obj_remove_style(btn, &style_btn_normal, 0);
        } else {
            lv_obj_add_style(btn, &style_btn_normal, 0);
            lv_obj_remove_style(btn, &style_btn_active, 0);
        }
        for (uint32_t c = 0; c < lv_obj_get_child_cnt(btn); c++)
            lv_obj_set_style_text_color(lv_obj_get_child(btn, c), col, 0);
    }
}

static void nav_btn_cb(lv_event_t* e) {
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    g_active_tab = idx;
    lv_tabview_set_act(g_tabview, idx, LV_ANIM_ON);
    update_nav_styles();
}

void ui_main_init() {
    init_styles();

    lv_obj_t* scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_white(), 0);
    lv_obj_set_style_pad_all(scr, 0, 0);

    // Tabview — hidden tab bar, custom nav bar below
    g_tabview = lv_tabview_create(scr, LV_DIR_TOP, 0);
    lv_obj_set_size(g_tabview, DISPLAY_WIDTH, CONTENT_HEIGHT);
    lv_obj_set_pos(g_tabview, 0, 0);
    lv_obj_set_style_bg_color(g_tabview, lv_color_white(), 0);
    lv_obj_set_style_border_width(g_tabview, 0, 0);
    lv_obj_set_style_pad_all(g_tabview, 0, 0);

    lv_obj_t* tab1 = lv_tabview_add_tab(g_tabview, "Clock");
    lv_obj_t* tab2 = lv_tabview_add_tab(g_tabview, "Forecast");
    lv_obj_t* tab3 = lv_tabview_add_tab(g_tabview, "Settings");

    lv_obj_set_scroll_dir(tab1, LV_DIR_VER);
    lv_obj_set_scroll_dir(tab2, LV_DIR_VER);
    lv_obj_set_scroll_dir(tab3, LV_DIR_NONE);

    lv_obj_set_style_pad_all(tab1, 0, 0);
    lv_obj_set_style_pad_all(tab2, 0, 0);
    lv_obj_set_style_pad_all(tab3, 0, 0);

    ui_clock_build(tab1);
    ui_forecast_build(tab2);
    ui_settings_build(tab3);

    // Bottom navigation bar
    lv_obj_t* nav = lv_obj_create(scr);
    lv_obj_add_style(nav, &style_nav_bar, 0);
    lv_obj_set_size(nav, DISPLAY_WIDTH, TAB_BAR_HEIGHT);
    lv_obj_set_pos(nav, 0, CONTENT_HEIGHT);
    lv_obj_set_flex_flow(nav, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(nav, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    for (int i = 0; i < 3; i++) {
        lv_obj_t* btn = lv_btn_create(nav);
        g_nav_btns[i] = btn;
        lv_obj_set_size(btn, DISPLAY_WIDTH / 3, TAB_BAR_HEIGHT);
        lv_obj_add_style(btn, &style_btn_normal, 0);
        lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_row(btn, 2, 0);

        lv_obj_t* icon = lv_label_create(btn);
        lv_label_set_text(icon, TAB_ICONS[i]);
        lv_obj_set_style_text_font(icon, &lv_font_montserrat_20, 0);

        lv_obj_t* lbl = lv_label_create(btn);
        lv_label_set_text(lbl, TAB_LABELS[i]);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);

        lv_obj_add_event_cb(btn, nav_btn_cb, LV_EVENT_CLICKED, (void*)(intptr_t)i);
    }

    g_active_tab = 0;
    update_nav_styles();
}

void ui_main_refresh() {
    ui_clock_refresh_weather();
    ui_forecast_refresh();
}

void ui_main_goto_tab(int idx) {
    if (idx < 0 || idx > 2) return;
    g_active_tab = idx;
    lv_tabview_set_act(g_tabview, idx, LV_ANIM_OFF);
    update_nav_styles();
}

void ui_main_request_save() {
    g_save_requested = true;
}

bool ui_main_consume_save_request() {
    if (!g_save_requested) return false;
    g_save_requested = false;
    return true;
}
