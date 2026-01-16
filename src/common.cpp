#include <Arduino.h>
#include "common.h"

bool mode_r = false;
bool last_r = false;

char toChar(Mode mode) {
  return static_cast<char>(mode);
}

bool isValidModeByte(uint8_t byte) {
  return (byte == 'X' || byte == 'R' || byte == 'Y' || byte == 'G' || byte == 'B');
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

void sendFrame(const Frame& frame)
{
  uint8_t payload[4] = { frame.device, frame.mode, frame.red, frame.brightness };
  uint8_t crc = crc8(payload, 4);

  printf("Mode: %c\n Red: %c\n Brightness: %u\n", static_cast<char>(frame.mode), static_cast<uint8_t>(frame.red), frame.brightness);
  printf("Sending frame: START %02X %02X %02X %02X CRC %02X END\n",
         payload[0], payload[1], payload[2], payload[3], crc);

  uint8_t raw[7] = {
    FRAME_START,
    payload[0],
    payload[1],
    payload[2],
    payload[3],
    crc,
    FRAME_END
  };

  RFSerial.write(raw, 7);
}

bool readFrame(Frame &out)
{
  static uint8_t state = 0;
  static uint8_t buf[7];
  static uint8_t crc;

  while (RFSerial.available())
  {
    uint8_t byte = RFSerial.read();

    if (state == 0 && byte != FRAME_START)
      continue; // wait for start byte

    buf[state++] = byte;

    if (state == 7) {
      state = 0;

      if (buf[6] != FRAME_END) return false;

      crc = crc8(&buf[1], 4);
      if (crc != buf[5]) return false;

      out.device = buf[1];
      out.mode   = buf[2];
      out.red    = buf[3];
      out.brightness = buf[4];

      printf("Mode: %c\n Red: %c\n Brightness: %u\n", static_cast<char>(out.mode), static_cast<uint8_t>(out.red), out.brightness);
      printf("Received frame: START %02X %02X %02X %02X CRC %02X END\n",
             buf[1], buf[2], buf[3], buf[4], buf[5]);
      return true;
    }
    delay(10);
  }
  return false;
}
