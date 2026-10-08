#include "wifi_manager.h"
#include "settings.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>

bool wifi_connect_sta() {
    Settings& s = settings_get();
    if (!strlen(s.wifi_ssid)) return false;

    WiFi.mode(WIFI_STA);
    WiFi.begin(s.wifi_ssid, s.wifi_password);

    uint32_t t = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t < 15000) {
        delay(200);
    }
    return WiFi.status() == WL_CONNECTED;
}

bool wifi_is_connected() {
    return WiFi.status() == WL_CONNECTED;
}

void wifi_check_connection() {
    if (WiFi.status() != WL_CONNECTED) {
        Settings& s = settings_get();
        if (strlen(s.wifi_ssid)) WiFi.reconnect();
    }
}

// Geocode city name → lat/lon using Open-Meteo geocoding API (free, no key)
bool wifi_geocode_city(const char* city, float& lat, float& lon) {
    char url[256];
    snprintf(url, sizeof(url),
        "https://geocoding-api.open-meteo.com/v1/search"
        "?name=%s&count=1&language=en&format=json", city);

    HTTPClient http;
    http.begin(url);
    http.setTimeout(10000);
    int rc = http.GET();
    if (rc != 200) { http.end(); return false; }

    String body = http.getString();
    http.end();

    JsonDocument doc;
    if (deserializeJson(doc, body) != DeserializationError::Ok) return false;

    JsonArray results = doc["results"].as<JsonArray>();
    if (!results || results.size() == 0) return false;

    lat = results[0]["latitude"].as<float>();
    lon = results[0]["longitude"].as<float>();
    return true;
}
