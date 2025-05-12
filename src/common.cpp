#include <Arduino.h>
#include "common.h"

// Use HardwareSerial (Serial1) for the RF module
// HardwareSerial RFSerial(3);

// Function to update the RGB LED based on mode character
// 'L' = Left (red), 'N' = Neither (yellow), 'R' = Right (green)
void updateRGBLED(char mode)
{
  switch (mode)
  {
    case 'G':
      analogWrite(rPin, 0);
      analogWrite(gPin, 255);
      analogWrite(bPin, 0);
      break;
    case 'Y':
      analogWrite(rPin, 255);
      analogWrite(gPin, 100); // Adjust this value for a warmer yellow tone
      analogWrite(bPin, 0);
      break;
    case 'X':
      analogWrite(rPin, 0);
      analogWrite(gPin, 0);
      analogWrite(bPin, 0);
      break;
    default:
      // Turn off LED for any unknown value
      analogWrite(rPin, 0);
      analogWrite(gPin, 0);
      analogWrite(bPin, 255);
      break;
  }
}

// CRC-8-ATM (polynomial 0x07, init 0x00)
uint8_t crc8(const uint8_t* data, size_t len) {
  uint8_t crc = 0x00;
  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x80)
        crc = (crc << 1) ^ 0x07;
      else
        crc <<= 1;
    }
  }
  return crc;
}