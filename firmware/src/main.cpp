#include <Arduino.h>
#include <WiFi.h>
#include "display.h"
#include "settings.h"
#include "wifi_manager.h"
#include "ntp.h"
#include "weather.h"
#include "ui/ui_main.h"
#include "ui/ui_settings.h"

static uint32_t last_weather    = 0;
static uint32_t last_wifi_check = 0;

static const uint32_t WEATHER_INTERVAL_MS  = 30 * 60 * 1000UL;
static const uint32_t WIFI_CHECK_MS        =  5 * 60 * 1000UL;

// ── Called from the main loop when the settings tab requests connect+save ─────
static void handle_save() {
    lv_timer_handler();

    ui_settings_set_status("Connecting to WiFi...");
    lv_timer_handler();

    WiFi.disconnect(true);
    delay(200);

    if (!wifi_connect_sta()) {
        ui_settings_set_status("WiFi failed. Check credentials.");
        return;
    }

    ui_settings_set_status("Syncing time...");
    lv_timer_handler();
    ntp_sync();

    ui_settings_set_status("Looking up location...");
    lv_timer_handler();

    Settings& s = settings_get();
    float lat = s.lat, lon = s.lon;
    if (wifi_geocode_city(s.city_name, lat, lon)) {
        s.lat = lat;
        s.lon = lon;
        settings_save();
    } else {
        ui_settings_set_status("City not found. Try a different name.");
        return;
    }

    ui_settings_set_status("Fetching weather...");
    lv_timer_handler();

    if (weather_update()) {
        int utc_off = weather_get().utc_offset_seconds;
        ntp_apply_offset(utc_off);
        s.utc_offset_seconds = utc_off;
        settings_save();
        ui_main_refresh();
        ui_settings_set_status("Connected!");
    } else {
        ui_settings_set_status("Weather fetch failed.");
    }

    last_weather = millis();
    lv_timer_handler();
}

void setup() {
    Serial.begin(115200);
    Serial.println("[DeskClock] Starting...");

    display_init();
    settings_load();

    // Build the UI immediately so the screen is always live
    ui_main_init();

    if (!settings_has_wifi()) {
        // No credentials yet — go straight to Settings tab
        ui_main_goto_tab(2);
        ui_settings_set_status("Enter WiFi and location to get started.");
        return;
    }

    // Show splash status in clock tab city label area via status label
    ui_settings_set_status("Connecting...");
    lv_timer_handler();

    if (wifi_connect_sta()) {
        Serial.println("[DeskClock] WiFi connected");

        ntp_sync();

        if (settings_has_location()) {
            weather_update();
            if (weather_get().is_valid) {
                int utc_off = weather_get().utc_offset_seconds;
                ntp_apply_offset(utc_off);
                settings_get().utc_offset_seconds = utc_off;
                settings_save();
            }
            ui_main_refresh();
        }
        ui_settings_set_status("");
    } else {
        Serial.println("[DeskClock] WiFi failed");
        ui_settings_set_status("WiFi connection failed.");
    }

    last_weather = millis();
}

void loop() {
    lv_timer_handler();

    uint32_t now = millis();

    // Handle settings save request from UI
    if (ui_main_consume_save_request()) {
        handle_save();
    }

    // Periodic WiFi health check
    if (now - last_wifi_check > WIFI_CHECK_MS) {
        wifi_check_connection();
        last_wifi_check = now;
    }

    // Periodic weather refresh
    if (wifi_is_connected() && settings_has_location()) {
        if (now - last_weather > WEATHER_INTERVAL_MS) {
            if (weather_update()) {
                int utc_off = weather_get().utc_offset_seconds;
                ntp_apply_offset(utc_off);
                settings_get().utc_offset_seconds = utc_off;
                settings_save();
                ui_main_refresh();
            }
            last_weather = now;
        }
    }

    delay(5);
}
