#ifndef WIFI_STA_H
#define WIFI_STA_H

#include <string>

bool
isConnected();

void
wifiStaInit();

void
wifiStaDeinit();

std::string
wifiGetSSID();

std::int8_t
wifiGetRSSI();

#endif // WIFI_STA_H
