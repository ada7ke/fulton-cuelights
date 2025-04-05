#pragma once
#include <Arduino.h>
//----------------------------------------------------
// Pin Definitions (common pins)
#define rPin 2   // RGB LED: Red channel
#define gPin 1   // RGB LED: Green channel
#define bPin 0   // RGB LED: Blue channel
#define ledPin 8 // Additional single-color LED

// RF module UART pins (make sure these pins support UART on your ESP32-C3)
#define RF_TX_PIN 21 // RX pin to RF module
#define RF_RX_PIN 20 // TX pin from RF module

extern HardwareSerial RFSerial;

void updateRGBLED(char modeChar);