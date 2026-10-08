#pragma once
#include <lvgl.h>

void ui_main_init();
void ui_main_refresh();
void ui_main_goto_tab(int idx);          // 0=clock, 1=forecast, 2=settings
void ui_main_request_save();
bool ui_main_consume_save_request();
