#pragma once
#include <lvgl.h>

void ui_clock_build(lv_obj_t* parent);
void ui_clock_tick(lv_timer_t* timer);
void ui_clock_refresh_weather();
