#include <Arduino.h>
#include "common.h"

bool mode_r = false;
bool last_r = false;

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
  return (b == 'X' || b == 'R' || b == 'Y' || b == 'G' || b == 'B');
}