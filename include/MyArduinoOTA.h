// MyArduinoOTA.h

#pragma once
#ifndef NO_WIFI_CLIENT
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoOTA.h>

// OTA WiFi AP credentials and password
extern const char *ssid_base;
extern const char *wifiPassword;

// OTA-related variables
extern bool otaEnabling;
extern bool otaEnabled;
extern bool otaStarted;

// Function prototypes
void setupOTA();
void enableArduinoOTA();

#endif
