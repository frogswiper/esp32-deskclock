#include "ui_forecast.h"
#include "../board_config.h"
#include "../weather.h"
#include <Arduino.h>

static lv_obj_t* g_rows[7] = {};   // day label, desc label, temp label per row

// Each row has 3 children: day | condition | hi/lo
static void set_row(int i, const char* day, const char* cond, const char* temps) {
    if (!g_rows[i]) return;
    lv_obj_t* lbl_day  = lv_obj_get_child(g_rows[i], 0);
    lv_obj_t* lbl_cond = lv_obj_get_child(g_rows[i], 1);
    lv_obj_t* lbl_temp = lv_obj_get_child(g_rows[i], 2);
    lv_label_set_text(lbl_day,  day);
    lv_label_set_text(lbl_cond, cond);
    lv_label_set_text(lbl_temp, temps);
}

void ui_forecast_build(lv_obj_t* parent) {
    lv_obj_set_style_bg_color(parent, lv_color_white(), 0);
    lv_obj_set_style_pad_all(parent, 0, 0);

    const int row_h = CONTENT_HEIGHT / 7;

    for (int i = 0; i < 7; i++) {
        lv_obj_t* row = lv_obj_create(parent);
        g_rows[i] = row;

        lv_obj_set_size(row, DISPLAY_WIDTH, row_h);
        lv_obj_set_pos(row, 0, i * row_h);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_pad_ver(row, 4, 0);
        lv_obj_set_style_pad_hor(row, 8, 0);
        lv_obj_set_style_radius(row, 0, 0);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        // Alternating background
        lv_color_t bg = (i % 2 == 0) ? lv_color_white() : lv_color_hex(0xF5F7FA);
        lv_obj_set_style_bg_color(row, bg, 0);

        // Day label (fixed width 58px)
        lv_obj_t* lbl_day = lv_label_create(row);
        lv_obj_set_width(lbl_day, 58);
        lv_label_set_text(lbl_day, "--");
        lv_obj_set_style_text_font(lbl_day, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(lbl_day, lv_color_hex(0x333333), 0);

        // Condition label (fills remaining space, truncates if needed)
        lv_obj_t* lbl_cond = lv_label_create(row);
        lv_obj_set_flex_grow(lbl_cond, 1);
        lv_label_set_long_mode(lbl_cond, LV_LABEL_LONG_CLIP);
        lv_label_set_text(lbl_cond, "--");
        lv_obj_set_style_text_font(lbl_cond, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(lbl_cond, lv_color_hex(0x666666), 0);

        // Hi/Lo label (fixed width 82px, right-aligned text)
        lv_obj_t* lbl_temp = lv_label_create(row);
        lv_obj_set_width(lbl_temp, 84);
        lv_label_set_text(lbl_temp, "--");
        lv_obj_set_style_text_font(lbl_temp, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(lbl_temp, lv_color_hex(0x3D7FEA), 0);
        lv_obj_set_style_text_align(lbl_temp, LV_TEXT_ALIGN_RIGHT, 0);
    }

    ui_forecast_refresh();
}

void ui_forecast_refresh() {
    const WeatherData& wd = weather_get();

    for (int i = 0; i < 7; i++) {
        if (i < wd.daily_count) {
            char temps[24];
            snprintf(temps, sizeof(temps), "%.0f\xc2\xb0 / %.0f\xc2\xb0",
                     wd.daily[i].temp_max, wd.daily[i].temp_min);
            set_row(i,
                    wd.daily[i].day,
                    wmo_description(wd.daily[i].wmo_code),
                    temps);
        } else {
            set_row(i, "--", "--", "--");
        }
    }
}
