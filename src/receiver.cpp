#include "receiver.h"
#include "common.h"

static State state = WAIT_START;
static uint8_t modeByte;
static uint8_t redByte;
static uint8_t recievedChecksum;
static unsigned long lastMessage = 0;

void setupReceiver()
{
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  updateLEDs(Mode::X, false);
  digitalWrite(ledPin, HIGH);
}

void loopReceiver()
{
  while (RFSerial.available()) {
    uint8_t byteIn = RFSerial.read();
    // printf("Read: 0x%02X\n", byteIn);
    digitalWrite(ledPin, LOW);
    Mode mode;

    switch (state) {
      case WAIT_START:
        if (byteIn == FRAME_START) {
          state = READ_MODE;
          printf("Frame start detected\n");
        }
        break;

      case READ_MODE:
        modeByte = byteIn;
        state = READ_RED;
        printf("Mode byte: 0x%02X\n", modeByte);
        break;

      case READ_RED:
        redByte = byteIn;
        state = READ_CHECKSUM;
        printf("Red byte: 0x%02X\n", redByte);
        break;

      case READ_CHECKSUM:
        recievedChecksum = byteIn;
        state = WAIT_END;
        printf("Received checksum: 0x%02X\n", recievedChecksum);
        break;

      case WAIT_END:
        printf("End byte: 0x%02X\n", byteIn);
        if (byteIn == FRAME_END) {
          uint8_t data[2] = { modeByte, redByte };
          uint8_t expectedCRC = crc8(data, 2);
          printf("Expected CRC: 0x%02X\n", expectedCRC);
          if (recievedChecksum == expectedCRC && isValidModeByte(modeByte)) {
            mode = static_cast<Mode>(modeByte);
            bool mode_r = (redByte != 0);
            printf("Command: %c\n", toChar(mode));
            updateLEDs(mode, mode_r);
            RFSerial.write(modeByte); // echo unframed byte for now
          } else {
            printf("Checksum error or invalid mode byte. Received: 0x%02X, Expected: 0x%02X\n", 
                    recievedChecksum, expectedCRC);
            RFSerial.write(static_cast<uint8_t>(Mode::B));
          }
        }
        state = WAIT_START;
        break;
    }
    delay(10);
  }

  if (millis() - lastMessage > 500) {
    updateLEDs(Mode::X, false);
    lastMessage = millis();
    RFSerial.write(static_cast<uint8_t>(Mode::B));
    printf("No message timeout, turning off LEDs\n");
  }

  digitalWrite(ledPin, HIGH);
  delay(100);
}

void updateLEDs(Mode mode, bool mode_r) {
  auto setLEDs = [](uint8_t r, uint8_t y, uint8_t g) {
    analogWrite(redLED, r);
    analogWrite(yellowLED, y);
    analogWrite(greenLED, g);
  };

  uint8_t r = mode_r ? 10 : 0;
  uint8_t y = (mode == Mode::Y) ? 10 : 0;
  uint8_t g = (mode == Mode::G) ? 10 : 0;

  setLEDs(r, y, g);
}
