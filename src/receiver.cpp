#include "receiver.h"
#include "common.h"

// setup message receiving
static State state = WAIT_START;
static uint8_t modeByte;
static uint8_t redByte;
static uint8_t recievedChecksum;
static unsigned long lastMessage = 0;

// pwm settings
const int freq = 5000;
const int res = 8;
int brightness = 255;
const int redChannel = 0;
const int yellowChannel = 1;
const int greenChannel = 2;

void setupReceiver()
{
  // Configure PWM channels
  ledcSetup(redChannel, freq, res);
  ledcSetup(yellowChannel, freq, res);
  ledcSetup(greenChannel, freq, res);
  
  // Attach pins to PWM channels
  ledcAttachPin(redLED, redChannel);
  ledcAttachPin(yellowLED, yellowChannel);
  ledcAttachPin(greenLED, greenChannel);

  pinMode(ledPin, OUTPUT);

  updateLEDs(Mode::X, false);
  digitalWrite(ledPin, HIGH);
}

void loopReceiver()
{
  while (RFSerial.available()) {
    uint8_t byteIn = RFSerial.read();
    printf("Read: 0x%02X\n", byteIn);
    Mode mode;

    switch (state) {
      case WAIT_START:
        if (byteIn == FRAME_START) {
          state = READ_MODE;
          digitalWrite(ledPin, LOW);
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
            lastMessage = millis();
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

  if (millis() - lastMessage > 5000) {
    lastMessage = millis();
    updateLEDs(Mode::X, false);
    RFSerial.write(static_cast<uint8_t>(Mode::B));
    printf("No message timeout, turning off LEDs\n");
  }

  digitalWrite(ledPin, HIGH);
  delay(100);
}

void updateLEDs(Mode mode, bool mode_r) {
  auto setLEDs = [](uint8_t r, uint8_t y, uint8_t g) {
    ledcWrite(redChannel, r);
    ledcWrite(yellowChannel, y);
    ledcWrite(greenChannel, g);
  };

  uint8_t r = mode_r ? brightness : 0;
  uint8_t y = (mode == Mode::Y) ? brightness : 0;
  uint8_t g = (mode == Mode::G) ? brightness : 0;

  setLEDs(r, y, g);
}
