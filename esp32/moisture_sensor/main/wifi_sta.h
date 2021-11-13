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

#endif // WIFI_STA_H
