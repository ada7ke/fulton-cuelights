#ifndef NO_WIFI_CLIENT

#include <Arduino.h>
#include <WiFi.h>
#include "WifiAP.hpp"

const char *ssid = "XXXX";
const char *password = "XXXXXXXX";
const char *ssid2 = "YYYY";
const char *password2 = "YYYYYYYY";

WiFiMulti wifiMulti;

#ifdef AP_BASE
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
const char *ssid_base = STR(AP_BASE);
#else
const char *ssid_base = "AP_BASE";
#endif

const char *wifiPassword = "cuelights";
DNSServer dnsServer;

// Function to setup WiFi as AP for OTA
void setupWifiAP() {
    printf("Starting WiFi.\n");
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char ssid[32];
    snprintf(ssid, sizeof(ssid), "%s-%02X%02X", ssid_base, mac[4], mac[5]);
    printf("SSID: %s\n", ssid);
    WiFi.mode(WIFI_AP_STA); // Enable both AP and STA modes
    // WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, wifiPassword);


    printf("WiFi AP started.\n");
    printf("IP Address: %s\n", WiFi.softAPIP().toString().c_str());

    dnsServer.start(53, "*", WiFi.softAPIP());
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.setTTL(300);
}


void setup_wifi()
{
    printf("Setting up Wi-Fi...\n");
    // Initialize Wi-Fi as both STA and AP
    setupWifiAP();

    // Initialize Wi-Fi and connect
    wifiMulti.addAP(ssid, password);
    wifiMulti.addAP(ssid2, password2);

    Serial.println("Connecting to Wi-Fi");
    // Check if Wi-Fi is connected
    if (wifiMulti.run() == WL_CONNECTED)
    {
        Serial.println("Connected to Wi-Fi");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        // timeClient.begin();
    }
    else
    {
        Serial.println("Failed to connect to Wi-Fi");
    }

    enableArduinoOTA();
    setupElegantOTA();
}


#endif