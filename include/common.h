#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>
//----------------------------------------------------
// Pin Definitions (common pins)
#define rPin 2   // RGB LED: Red channel
#define gPin 1   // RGB LED: Green channel
#define bPin 0   // RGB LED: Blue channel
#define ledPin 8 // Additional single-color LED


// extern HardwareSerial RFSerial;
#define RFSerial Serial1

void updateRGBLED(char mode);
uint8_t crc8(const uint8_t* data, size_t len);
