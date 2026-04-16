#include "ui_settings.h"
#include "ui_main.h"
#include "../settings.h"
#include "../board_config.h"

// ── Custom font — Montserrat 14 with full ASCII + ÆØÅæøå ─────────────────────
LV_FONT_DECLARE(font_montserrat_14_nor);

// ── Custom keyboard maps ───────────────────────────────────────────────────────
static const char* kb_map_lc[] = {
    "q","w","e","r","t","y","u","i","o","p","\n",
    "a","s","d","f","g","h","j","k","l","\n",
    "ABC","z","x","c","v","b","n","m",LV_SYMBOL_BACKSPACE,"\n",
    "1#","\xc3\xa6","\xc3\xb8","\xc3\xa5",","," ",LV_SYMBOL_OK,""
};
static const lv_btnmatrix_ctrl_t kb_ctrl_lc[] = {
    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,
    LV_KEYBOARD_CTRL_BTN_FLAGS|2, 1,1,1,1,1,1,1, LV_KEYBOARD_CTRL_BTN_FLAGS|2,
    LV_KEYBOARD_CTRL_BTN_FLAGS|1, 1,1,1,1, 4, LV_KEYBOARD_CTRL_BTN_FLAGS|2
};

static const char* kb_map_uc[] = {
    "Q","W","E","R","T","Y","U","I","O","P","\n",
    "A","S","D","F","G","H","J","K","L","\n",
    "abc","Z","X","C","V","B","N","M",LV_SYMBOL_BACKSPACE,"\n",
    "1#","\xc3\x86","\xc3\x98","\xc3\x85",","," ",LV_SYMBOL_OK,""
};
static const lv_btnmatrix_ctrl_t kb_ctrl_uc[] = {
    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,
    LV_KEYBOARD_CTRL_BTN_FLAGS|2, 1,1,1,1,1,1,1, LV_KEYBOARD_CTRL_BTN_FLAGS|2,
    LV_KEYBOARD_CTRL_BTN_FLAGS|1, 1,1,1,1, 4, LV_KEYBOARD_CTRL_BTN_FLAGS|2
};

static lv_obj_t* g_ta_ssid    = nullptr;
static lv_obj_t* g_ta_pass    = nullptr;
static lv_obj_t* g_ta_city    = nullptr;
static lv_obj_t* g_lbl_status = nullptr;
static lv_obj_t* g_kb         = nullptr;

static void ta_focus_cb(lv_event_t* e) {
    lv_obj_t* ta = (lv_obj_t*)lv_event_get_target(e);
    lv_keyboard_set_textarea(g_kb, ta);
}

static void save_cb(lv_event_t* /*e*/) {
    Settings& s = settings_get();
    strlcpy(s.wifi_ssid,     lv_textarea_get_text(g_ta_ssid), sizeof(s.wifi_ssid));
    strlcpy(s.wifi_password, lv_textarea_get_text(g_ta_pass), sizeof(s.wifi_password));
    strlcpy(s.city_name,     lv_textarea_get_text(g_ta_city), sizeof(s.city_name));
    settings_save();
    lv_label_set_text(g_lbl_status, "Connecting...");
    ui_main_request_save();
}

void ui_settings_build(lv_obj_t* parent) {
    lv_obj_set_style_bg_color(parent, lv_color_white(), 0);
    lv_obj_set_style_pad_all(parent, 0, 0);

    const int KB_H   = 200;
    const int FORM_H = CONTENT_HEIGHT - KB_H;

    // Scrollable form
    lv_obj_t* form = lv_obj_create(parent);
    lv_obj_set_size(form, DISPLAY_WIDTH, FORM_H);
    lv_obj_set_pos(form, 0, 0);
    lv_obj_set_style_bg_color(form, lv_color_white(), 0);
    lv_obj_set_style_border_width(form, 0, 0);
    lv_obj_set_style_pad_all(form, 10, 0);
    lv_obj_set_style_pad_row(form, 6, 0);
    lv_obj_set_flex_flow(form, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(form, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_scroll_dir(form, LV_DIR_VER);

    // WiFi header
    lv_obj_t* h1 = lv_label_create(form);
    lv_label_set_text(h1, "WiFi");
    lv_obj_set_style_text_font(h1, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(h1, lv_color_hex(0x333333), 0);

    Settings& s = settings_get();

    g_ta_ssid = lv_textarea_create(form);
    lv_textarea_set_one_line(g_ta_ssid, true);
    lv_textarea_set_placeholder_text(g_ta_ssid, "Network name (SSID)");
    lv_obj_set_width(g_ta_ssid, LV_PCT(100));
    lv_obj_set_style_text_font(g_ta_ssid, &font_montserrat_14_nor, 0);
    if (strlen(s.wifi_ssid) > 0) lv_textarea_set_text(g_ta_ssid, s.wifi_ssid);
    lv_obj_add_event_cb(g_ta_ssid, ta_focus_cb, LV_EVENT_FOCUSED, nullptr);

    g_ta_pass = lv_textarea_create(form);
    lv_textarea_set_one_line(g_ta_pass, true);
    lv_textarea_set_placeholder_text(g_ta_pass, "Password");
    lv_textarea_set_password_mode(g_ta_pass, true);
    lv_obj_set_width(g_ta_pass, LV_PCT(100));
    lv_obj_set_style_text_font(g_ta_pass, &font_montserrat_14_nor, 0);
    if (strlen(s.wifi_password) > 0) lv_textarea_set_text(g_ta_pass, s.wifi_password);
    lv_obj_add_event_cb(g_ta_pass, ta_focus_cb, LV_EVENT_FOCUSED, nullptr);

    // Location header
    lv_obj_t* h2 = lv_label_create(form);
    lv_label_set_text(h2, "Location");
    lv_obj_set_style_text_font(h2, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(h2, lv_color_hex(0x333333), 0);

    g_ta_city = lv_textarea_create(form);
    lv_textarea_set_one_line(g_ta_city, true);
    lv_textarea_set_placeholder_text(g_ta_city, "City, e.g. Oslo");
    lv_obj_set_width(g_ta_city, LV_PCT(100));
    lv_obj_set_style_text_font(g_ta_city, &font_montserrat_14_nor, 0);
    if (strlen(s.city_name) > 0) lv_textarea_set_text(g_ta_city, s.city_name);
    lv_obj_add_event_cb(g_ta_city, ta_focus_cb, LV_EVENT_FOCUSED, nullptr);

    // Save button
    lv_obj_t* btn = lv_btn_create(form);
    lv_obj_set_size(btn, LV_PCT(100), 40);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x3D7FEA), 0);
    lv_obj_set_style_radius(btn, 8, 0);
    lv_obj_t* btn_lbl = lv_label_create(btn);
    lv_label_set_text(btn_lbl, "Connect & Save");
    lv_obj_set_style_text_font(btn_lbl, &lv_font_montserrat_16, 0);
    lv_obj_center(btn_lbl);
    lv_obj_add_event_cb(btn, save_cb, LV_EVENT_CLICKED, nullptr);

    // Status
    g_lbl_status = lv_label_create(form);
    lv_label_set_text(g_lbl_status, "");
    lv_obj_set_style_text_font(g_lbl_status, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(g_lbl_status, lv_color_hex(0x666666), 0);
    lv_label_set_long_mode(g_lbl_status, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(g_lbl_status, LV_PCT(100));

    // Keyboard pinned to bottom
    g_kb = lv_keyboard_create(parent);
    lv_obj_set_size(g_kb, DISPLAY_WIDTH, KB_H);
    lv_obj_align(g_kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_map(g_kb, LV_KEYBOARD_MODE_TEXT_LOWER, kb_map_lc, kb_ctrl_lc);
    lv_keyboard_set_map(g_kb, LV_KEYBOARD_MODE_TEXT_UPPER, kb_map_uc, kb_ctrl_uc);
    lv_obj_set_style_text_font(g_kb, &font_montserrat_14_nor, 0);
    lv_keyboard_set_textarea(g_kb, g_ta_ssid);
}

void ui_settings_set_status(const char* msg) {
    if (g_lbl_status) lv_label_set_text(g_lbl_status, msg);
}
