#include <Arduino.h>
#include "common.h"

bool mode_r = false;
bool last_r = false;

void updateRGBLED(Mode mode)
{  
  switch (mode)
  {
    case Mode::G:
      analogWrite(rPin, 0);
      analogWrite(gPin, 255);
      analogWrite(bPin, 0);
      break;
    case Mode::Y:
      analogWrite(rPin, 255);
      analogWrite(gPin, 100);
      analogWrite(bPin, 0);
      break;
    case Mode::X:
      analogWrite(rPin, 0);
      analogWrite(gPin, 0);
      analogWrite(bPin, 0);
      break;
    default:
      analogWrite(rPin, 0);
      analogWrite(gPin, 0);
      analogWrite(bPin, 255);
      break;
  }
}

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

char toChar(Mode mode) {
  return static_cast<char>(mode);
}

bool isValidModeByte(uint8_t b) {
  return (b == 'X' || b == 'R' || b == 'Y' || b == 'G');
}