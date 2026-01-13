#pragma once

#ifndef NO_WIFI_CLIENT

#include <DNSServer.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "MyArduinoOTA.h"
#include "WebOTA.h"

extern DNSServer dnsServer;
extern WiFiMulti wifiMulti;

void setup_wifi();

#endif
