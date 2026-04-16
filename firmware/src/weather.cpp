#include "weather.h"
#include "settings.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <Arduino.h>

static WeatherData g_wd = {};

const char* wmo_description(int c) {
    if (c == 0)                 return "Clear Sky";
    if (c == 1)                 return "Mainly Clear";
    if (c == 2)                 return "Partly Cloudy";
    if (c == 3)                 return "Overcast";
    if (c == 45 || c == 48)    return "Fog";
    if (c >= 51 && c <= 55)    return "Drizzle";
    if (c >= 61 && c <= 65)    return "Rain";
    if (c == 66 || c == 67)    return "Freezing Rain";
    if (c >= 71 && c <= 75)    return "Snow";
    if (c == 77)                return "Snow Grains";
    if (c >= 80 && c <= 82)    return "Showers";
    if (c == 85 || c == 86)    return "Snow Showers";
    if (c == 95)                return "Thunderstorm";
    if (c == 96 || c == 99)    return "Thunderstorm";
    return "--";
}

static const char* date_dayname(const char* d) {
    int y, mo, day;
    if (sscanf(d, "%d-%d-%d", &y, &mo, &day) != 3) return "?";
    struct tm t = {};
    t.tm_year = y - 1900;
    t.tm_mon  = mo - 1;
    t.tm_mday = day;
    mktime(&t);
    static const char* names[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    return names[t.tm_wday];
}

bool weather_update() {
    Settings& s = settings_get();
    if (s.lat == 0.0f && s.lon == 0.0f) return false;

    char url[512];
    snprintf(url, sizeof(url),
        "https://api.open-meteo.com/v1/forecast"
        "?latitude=%.4f&longitude=%.4f"
        "&current=temperature_2m,relative_humidity_2m,apparent_temperature"
        ",wind_speed_10m,weather_code"
        "&daily=weather_code,temperature_2m_max,temperature_2m_min"
        "&timezone=auto&forecast_days=7&wind_speed_unit=ms",
        s.lat, s.lon);

    HTTPClient http;
    http.begin(url);
    http.setTimeout(10000);
    int rc = http.GET();
    if (rc != 200) { http.end(); return false; }

    String body = http.getString();
    http.end();

    JsonDocument doc;
    if (deserializeJson(doc, body) != DeserializationError::Ok) return false;

    g_wd.utc_offset_seconds = doc["utc_offset_seconds"] | 0;

    JsonObject cur = doc["current"].as<JsonObject>();
    g_wd.temp       = cur["temperature_2m"].as<float>();
    g_wd.feels_like = cur["apparent_temperature"].as<float>();
    g_wd.humidity   = cur["relative_humidity_2m"].as<int>();
    g_wd.wind_mps   = cur["wind_speed_10m"].as<float>();
    g_wd.wmo_code   = cur["weather_code"].as<int>();

    JsonObject daily = doc["daily"].as<JsonObject>();
    JsonArray  times = daily["time"].as<JsonArray>();
    int n = min((int)times.size(), 7);
    g_wd.daily_count = n;
    for (int i = 0; i < n; i++) {
        const char* dt = times[i] | "";
        strlcpy(g_wd.daily[i].day,
                i == 0 ? "Today" : date_dayname(dt),
                sizeof(g_wd.daily[i].day));
        g_wd.daily[i].wmo_code  = daily["weather_code"][i].as<int>();
        g_wd.daily[i].temp_max  = daily["temperature_2m_max"][i].as<float>();
        g_wd.daily[i].temp_min  = daily["temperature_2m_min"][i].as<float>();
    }

    g_wd.is_valid = true;
    return true;
}

const WeatherData& weather_get() { return g_wd; }
