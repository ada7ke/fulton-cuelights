#include <Arduino.h>
#include "common.h"

bool mode_r = false;
bool last_r = false;

// LED activity pulse
bool activityPulseActive = false;
unsigned long activityPulseUntil = 0;

void pulseActivityLed(uint16_t ms = 10) {
  digitalWrite(LED_PIN, LOW);
  activityPulseActive = true;
  activityPulseUntil = millis() + ms;
}

void serviceActivityLed() {
  if (activityPulseActive && (long)(millis() - activityPulseUntil) >= 0) {
    digitalWrite(LED_PIN, HIGH);
    activityPulseActive = false;
  }
}

char toChar(Mode mode) {
  return static_cast<char>(mode);
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
  uint8_t payload[5] = { frame.device, frame.red, frame.yellow, frame.green, frame.brightness };
  uint8_t crc = crc8(payload, 5);

  // printf("Red: %u, Yellow: %u, Green: %u, Brightness: %u\n",
  //        frame.red, frame.yellow, frame.green, frame.brightness);
  // printf("Sending frame: START %02X %02X %02X %02X %02X CRC %02X END\n",
  //        payload[0], payload[1], payload[2], payload[3], payload[4], crc);

  uint8_t raw[8] = {
    FRAME_START,
    payload[0],
    payload[1],
    payload[2],
    payload[3],
    payload[4],
    crc,
    FRAME_END
  };

  RFSerial.write(raw, 8);
}

bool readFrame(Frame &out)
{
  static uint8_t state = 0;
  static uint8_t buf[8];
  static uint8_t crc;

  while (RFSerial.available())
  {
    uint8_t byte = RFSerial.read();

    if (state == 0 && byte != FRAME_START)
      continue; // wait for start byte

    buf[state++] = byte;

    if (state == 8) {
      state = 0;

      if (buf[7] != FRAME_END) return false;

      crc = crc8(&buf[1], 5);
      if (crc != buf[6]) return false;

      out.device = buf[1];
      out.red = buf[2];
      out.yellow = buf[3];
      out.green = buf[4];
      out.brightness = buf[5];

      // printf("Red: %u, Yellow: %u, Green: %u, Brightness: %u\n",
             // out.red, out.yellow, out.green, out.brightness);
      // printf("Received frame: START %02X %02X %02X %02X %02X CRC %02X END\n",
             // buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);
      return true;
    }
  }
  return false;
}
