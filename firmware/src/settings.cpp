#include "settings.h"
#include <Preferences.h>
#include <string.h>

static Preferences prefs;
static Settings    g_settings;
static const char* NS = "dsk2";

void settings_load() {
    prefs.begin(NS, true);
    prefs.getString("ssid",   g_settings.wifi_ssid,     sizeof(g_settings.wifi_ssid));
    prefs.getString("pass",   g_settings.wifi_password,  sizeof(g_settings.wifi_password));
    prefs.getString("city",   g_settings.city_name,      sizeof(g_settings.city_name));
    g_settings.lat                = prefs.getFloat("lat",    0.0f);
    g_settings.lon                = prefs.getFloat("lon",    0.0f);
    g_settings.utc_offset_seconds = prefs.getInt("utcoff",   0);
    prefs.end();
}

void settings_save() {
    prefs.begin(NS, false);
    prefs.putString("ssid",   g_settings.wifi_ssid);
    prefs.putString("pass",   g_settings.wifi_password);
    prefs.putString("city",   g_settings.city_name);
    prefs.putFloat("lat",     g_settings.lat);
    prefs.putFloat("lon",     g_settings.lon);
    prefs.putInt("utcoff",    g_settings.utc_offset_seconds);
    prefs.end();
}

Settings& settings_get()      { return g_settings; }
bool settings_has_wifi()      { return strlen(g_settings.wifi_ssid) > 0; }
bool settings_has_location()  {
    return strlen(g_settings.city_name) > 0
        && (g_settings.lat != 0.0f || g_settings.lon != 0.0f);
}
