#pragma once
#include <stdint.h>
#include <stdbool.h>

struct Settings {
    char  wifi_ssid[64];
    char  wifi_password[64];
    char  city_name[64];
    float lat;
    float lon;
    int   utc_offset_seconds;   // from Open-Meteo, updated on each weather fetch
};

Settings& settings_get();
void      settings_load();
void      settings_save();
bool      settings_has_wifi();
bool      settings_has_location();
