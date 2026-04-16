#pragma once
#include <stdbool.h>

bool wifi_connect_sta();       // blocking connect, 15 s timeout
bool wifi_is_connected();
void wifi_check_connection();  // call periodically — reconnects if dropped
bool wifi_geocode_city(const char* city, float& lat, float& lon);
