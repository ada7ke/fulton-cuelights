#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>

#define rPin 2 
#define gPin 1
#define bPin 0
#define ledPin 8

#define RFSerial Serial1

enum class Mode : uint8_t
{
  X = 'X',
  R = 'R',
  Y = 'Y',
  G = 'G',
};

void updateRGBLED(Mode mode);
uint8_t crc8(const uint8_t* data, size_t len);
