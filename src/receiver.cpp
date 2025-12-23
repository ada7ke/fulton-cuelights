#include "receiver.h"
#include "common.h"

static State state = WAIT_START;

void setupReceiver()
{
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  updateLEDs(Mode::X);
  digitalWrite(ledPin, HIGH);
}

void loopReceiver()
{
  static uint8_t modeByte;
  static uint8_t recievedChecksum;
  // printf("Waiting for data...\n");

  while (RFSerial.available()) {
    uint8_t byteIn = RFSerial.read();
    printf("Read: 0x%02X\n", byteIn);
    digitalWrite(ledPin, LOW);
    Mode mode;

    switch (state) {
      case WAIT_START:
        if (byteIn == 0x7E) {
          state = READ_MODE;
        }
        break;

      case READ_MODE:
        modeByte = byteIn;
        state = READ_CHECKSUM;
        break;

      case READ_CHECKSUM:
        recievedChecksum = byteIn;
        state = WAIT_END;
        break;

      case WAIT_END:
        if (byteIn == 0x7F) {
          uint8_t data[1] = { modeByte };
          uint8_t expectedCRC = crc8(data, 1); 

          if (recievedChecksum == expectedCRC &&
             (modeByte == 'R' || modeByte == 'Y' ||
              modeByte == 'G' || modeByte == 'X')) {
            
            mode = static_cast<Mode>(modeByte);
            printf("Command: %c\n", static_cast<char>(mode));
            updateLEDs(mode);

            RFSerial.write(modeByte);
          }
        else {
          mode = Mode::X;
        }
        state = WAIT_START;
      }
    }
    delay(10);
  } 

  digitalWrite(ledPin, HIGH);
  delay(100);
}

void updateLEDs(Mode mode) {
  if (mode == Mode::R) {
    analogWrite(redLED, 20);
    analogWrite(yellowLED, 0);
    analogWrite(greenLED, 0);
  }
  else if (mode == Mode::Y) {
    analogWrite(redLED, 0);
    analogWrite(yellowLED, 20);
    analogWrite(greenLED, 0);
  }
  else if (mode == Mode::G) {
    analogWrite(redLED, 0);
    analogWrite(yellowLED, 0);
    analogWrite(greenLED, 20);
  }
  else if (mode == Mode::X) {
    analogWrite(redLED, 0);
    analogWrite(yellowLED, 0);
    analogWrite(greenLED, 0);
  }
}
