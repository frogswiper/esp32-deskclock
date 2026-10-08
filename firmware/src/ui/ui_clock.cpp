#include "ui_clock.h"
#include "../board_config.h"
LV_FONT_DECLARE(font_clock_92);
#include "../weather.h"
#include "../ntp.h"
#include "../settings.h"
#include <time.h>
#include <Arduino.h>

// ── Widget handles ────────────────────────────────────────────────────────────
static lv_obj_t* lbl_time     = nullptr;
static lv_obj_t* lbl_date     = nullptr;
static lv_obj_t* lbl_city     = nullptr;
static lv_obj_t* lbl_temp     = nullptr;
static lv_obj_t* lbl_desc     = nullptr;
static lv_obj_t* lbl_feels    = nullptr;
static lv_obj_t* lbl_humidity = nullptr;
static lv_obj_t* lbl_wind     = nullptr;

void ui_clock_build(lv_obj_t* parent) {
    lv_obj_set_style_bg_color(parent, lv_color_white(), 0);
    lv_obj_set_style_pad_all(parent, 0, 0);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(parent, 0, 0);

    // ── Clock section (top half) ──────────────────────────────────────────────
    lv_obj_t* clk = lv_obj_create(parent);
    lv_obj_set_size(clk, LV_PCT(100), CONTENT_HEIGHT / 2);
    lv_obj_set_style_bg_color(clk, lv_color_white(), 0);
    lv_obj_set_style_border_width(clk, 0, 0);
    lv_obj_set_style_pad_hor(clk, 4, 0);
    lv_obj_set_style_pad_ver(clk, 6, 0);
    lv_obj_set_flex_flow(clk, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(clk, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(clk, 2, 0);

    lbl_time = lv_label_create(clk);
    lv_label_set_text(lbl_time, "00:00");
    lv_obj_set_style_text_font(lbl_time, &font_clock_92, 0);
    lv_obj_set_style_text_color(lbl_time, lv_color_hex(0x111111), 0);

    lbl_date = lv_label_create(clk);
    lv_label_set_text(lbl_date, "---");
    lv_obj_set_style_text_font(lbl_date, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(lbl_date, lv_color_hex(0x555555), 0);

    lbl_city = lv_label_create(clk);
    lv_label_set_text(lbl_city, settings_get().city_name);
    lv_obj_set_style_text_font(lbl_city, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_city, lv_color_hex(0xAAAAAA), 0);

    // ── Divider ───────────────────────────────────────────────────────────────
    lv_obj_t* div = lv_obj_create(parent);
    lv_obj_set_size(div, LV_PCT(88), 1);
    lv_obj_set_style_bg_color(div, lv_color_hex(0xDDDDDD), 0);
    lv_obj_set_style_border_width(div, 0, 0);
    lv_obj_set_style_pad_all(div, 0, 0);

    // ── Weather section (bottom half) ─────────────────────────────────────────
    lv_obj_t* wx = lv_obj_create(parent);
    lv_obj_set_size(wx, LV_PCT(100), CONTENT_HEIGHT / 2 - 1);
    lv_obj_set_style_bg_color(wx, lv_color_white(), 0);
    lv_obj_set_style_border_width(wx, 0, 0);
    lv_obj_set_style_pad_all(wx, 10, 0);
    lv_obj_set_flex_flow(wx, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(wx, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(wx, 6, 0);

    lbl_temp = lv_label_create(wx);
    lv_label_set_text(lbl_temp, "--\xc2\xb0");
    lv_obj_set_style_text_font(lbl_temp, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(lbl_temp, lv_color_hex(0x111111), 0);

    lbl_desc = lv_label_create(wx);
    lv_label_set_text(lbl_desc, "No weather data");
    lv_obj_set_style_text_font(lbl_desc, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(lbl_desc, lv_color_hex(0x555555), 0);

    lbl_feels = lv_label_create(wx);
    lv_label_set_text(lbl_feels, "");
    lv_obj_set_style_text_font(lbl_feels, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_feels, lv_color_hex(0x888888), 0);

    // Stats row — humidity | wind
    lv_obj_t* row = lv_obj_create(wx);
    lv_obj_set_size(row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lbl_humidity = lv_label_create(row);
    lv_label_set_text(lbl_humidity, "--");
    lv_obj_set_style_text_font(lbl_humidity, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(lbl_humidity, lv_color_hex(0x3D7FEA), 0);

    lbl_wind = lv_label_create(row);
    lv_label_set_text(lbl_wind, "--");
    lv_obj_set_style_text_font(lbl_wind, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(lbl_wind, lv_color_hex(0x3D7FEA), 0);

    // 1-second tick timer
    lv_timer_create(ui_clock_tick, 1000, NULL);
    ui_clock_refresh_weather();
}

void ui_clock_tick(lv_timer_t* /*t*/) {
    if (!lbl_time) return;

    struct tm t = ntp_get_local_time();
    char buf[32];

    snprintf(buf, sizeof(buf), "%02d:%02d", t.tm_hour, t.tm_min);
    lv_label_set_text(lbl_time, buf);

    static const char* days[]   = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    static const char* months[] = {"Jan","Feb","Mar","Apr","May","Jun",
                                    "Jul","Aug","Sep","Oct","Nov","Dec"};
    snprintf(buf, sizeof(buf), "%s, %s %d",
             days[t.tm_wday], months[t.tm_mon], t.tm_mday);
    lv_label_set_text(lbl_date, buf);
}

void ui_clock_refresh_weather() {
    if (!lbl_temp) return;

    // Always refresh city name
    Settings& s = settings_get();
    lv_label_set_text(lbl_city, strlen(s.city_name) > 0 ? s.city_name : "");

    const WeatherData& wd = weather_get();
    if (!wd.is_valid) {
        lv_label_set_text(lbl_temp, "--\xc2\xb0");
        lv_label_set_text(lbl_desc, "No weather data");
        lv_label_set_text(lbl_feels, "");
        lv_label_set_text(lbl_humidity, "--");
        lv_label_set_text(lbl_wind, "--");
        return;
    }

    char buf[64];
    snprintf(buf, sizeof(buf), "%.0f\xc2\xb0", wd.temp);
    lv_label_set_text(lbl_temp, buf);

    lv_label_set_text(lbl_desc, wmo_description(wd.wmo_code));

    snprintf(buf, sizeof(buf), "Feels like %.0f\xc2\xb0", wd.feels_like);
    lv_label_set_text(lbl_feels, buf);

    snprintf(buf, sizeof(buf), "Hum: %d%%", wd.humidity);
    lv_label_set_text(lbl_humidity, buf);

    snprintf(buf, sizeof(buf), "Wind: %.1f m/s", wd.wind_mps);
    lv_label_set_text(lbl_wind, buf);
}
