#pragma once
#include <stdint.h>
#include <stdbool.h>

struct DayForecast {
    char  day[8];       // "Today", "Mon", "Tue", …
    int   wmo_code;
    float temp_max;
    float temp_min;
};

struct WeatherData {
    bool  is_valid;
    float temp;
    float feels_like;
    int   humidity;
    float wind_mps;
    int   wmo_code;
    DayForecast daily[7];
    int   daily_count;
    int   utc_offset_seconds;
};

bool               weather_update();
const WeatherData& weather_get();
const char*        wmo_description(int code);
