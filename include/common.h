#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>

#define rPin 2 
#define gPin 1
#define bPin 0
#define ledPin 8

#define RFSerial Serial1
#define RX_PIN 21 // swap rx and tx pins
#define TX_PIN 20 // swap rx and tx pins

constexpr uint8_t FRAME_START = 0x7E;
constexpr uint8_t FRAME_END   = 0x7F;

extern bool mode_r;
extern bool last_r;
enum class Mode : uint8_t
{
  X = 'X',
  Y = 'Y',
  G = 'G',
  B = 'B',
};

void updateRGBLED(Mode mode);
uint8_t crc8(const uint8_t* data, size_t len);

// Utility helpers for logging and validation
char toChar(Mode mode);
bool isValidModeByte(uint8_t b);
