// MyArduinoOTA.cpp

#ifndef NO_WIFI_CLIENT

#include <Arduino.h>
#include "MyArduinoOTA.h"
#include "dataConfig.h"

bool otaEnabling = false;
bool otaEnabled = false;
bool otaStarted = false;

void setupOTA() {
    Serial.println("Setting up OTA.");
    if (OTA_PASSWORD != nullptr && OTA_PASSWORD[0] != '\0') {
        ArduinoOTA.setPassword(OTA_PASSWORD);
    }
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";
        Serial.println("Start updating " + type);
        otaStarted = true;
    });
    ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); otaStarted = false; });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
    });
    ArduinoOTA.begin();
}

void enableArduinoOTA() {
    Serial.println("Enabling OTA");
    otaEnabling = true;
    setupOTA();
    otaEnabled = true;
    Serial.println("OTA Enabled");
}

#endif