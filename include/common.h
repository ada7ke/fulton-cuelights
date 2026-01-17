#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>

#define rPin 2 
#define gPin 1
#define bPin 0
#define LED_PIN 8

#define RFSerial Serial1
#define RX_PIN 21 // swap rx and tx pins
#define TX_PIN 20 // swap rx and tx pins

#define FRAME_START 0x7E
#define FRAME_END 0x7F
#define CONTROLLER_ADDRESS 0x01
#define RECEIVER_ADDRESS 0x02

struct Frame {
  uint8_t device;
  uint8_t red;
  uint8_t yellow;
  uint8_t green;
  uint8_t brightness;
};

inline constexpr unsigned long longHoldDuration = 3000UL;

extern bool mode_r;
extern bool last_r;
enum class Mode : uint8_t
{
  X = 'X', // sleep
  Y = 'Y', // yellow
  G = 'G', // green
  T = 'T', // yellow and green
  R = 'R', // failed checksum
  B = 'B', // no message timeout
  W = 'W' // unknown error
};

extern bool modeR;
extern bool modeY;
extern bool modeG;

char toChar(Mode mode);

// Frame protocol:
// [FRAME_START, device, red, yellow, green, brightness, crc, FRAME_END]
// crc is CRC8 over the 5-byte payload: [device, red, yellow, green, brightness]
inline bool isValidBoolByte(uint8_t byte) { return (byte == 0 || byte == 1); }
uint8_t crc8(const uint8_t* data, size_t len);
void sendFrame(const Frame& frame);
bool readFrame(Frame &out);