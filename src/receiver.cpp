#include "receiver.h"
#include "common.h"

static State state = WAIT_START;

void setupReceiver()
{
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  analogWrite(redLED, 0);
  analogWrite(yellowLED, 0);
  analogWrite(greenLED, 0);
  updateRGBLED('X');
  digitalWrite(ledPin, HIGH);
}

void loopReceiver()
{
  static char mode;
  static char recievedChecksum;
  
  while (RFSerial.available()) {
    char c = RFSerial.read();
    printf("Read: %c\n", c);
    digitalWrite(ledPin, LOW);

    switch (state) {
      case WAIT_START:
        if (c == 0x7E) {
          state = READ_MODE;
        }
        break;

      case READ_MODE:
        mode = c;
        state = READ_CHECKSUM;
        break;

      case READ_CHECKSUM:
        recievedChecksum = c;
        state = WAIT_END;
        break;

      case WAIT_END:
        if (c == 0x7F) {
          uint8_t data[1] = { static_cast<uint8_t>(mode) };
          uint8_t expectedCRC = crc8(data, 1);  // use your CRC-8 function

          if (static_cast<uint8_t>(recievedChecksum) == expectedCRC) {
            printf("Command: %c\n", mode);
            updateRGBLED(mode);
            updateLEDs(mode);
          }
          else {
            mode = 'Z';
            updateRGBLED(mode);
          }

          RFSerial.write(mode);

          state = WAIT_START;
        }
    }
    delay(10);
  } 
  
  digitalWrite(ledPin, HIGH);
  delay(100);
}

void updateLEDs(char c) {
  if (c == 'Y') {
    analogWrite(redLED, 0);
    analogWrite(yellowLED, 20);
    analogWrite(greenLED, 0);
  }
  else if (c == 'G') {
    analogWrite(redLED, 0);
    analogWrite(yellowLED, 0);
    analogWrite(greenLED, 20);
  }
  else if (c == 'X') {
    analogWrite(redLED, 0);
    analogWrite(yellowLED, 0);
    analogWrite(greenLED, 0);
  }
}
