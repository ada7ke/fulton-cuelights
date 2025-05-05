#include "receiver.h"
#include "common.h"

State state = WAIT_START;
int timer = 0;

void setupReceiver()
{
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  analogWrite(redLED, 0);
  analogWrite(yellowLED, 0);
  analogWrite(greenLED, 0);
  updateRGBLED('L');
}

void loopReceiver()
{
  char modeChar;
  char recievedChecksum;

  digitalWrite(ledPin, LOW);

  // sleep timer
  if (timer >= 60000) {
    analogWrite(redLED, 0);
    analogWrite(yellowLED, 0);
    analogWrite(greenLED, 0);
    printf("timer: %i \n", timer);
  }

  while (RFSerial.available()) {
    char c = RFSerial.read();
    printf("Read: %c\n", c);
    digitalWrite(ledPin, HIGH);

    switch (state) {
      case WAIT_START:
        if (c == 0x7E) {
          state = READ_MODE;
        }
        break;

      case READ_MODE:
        modeChar = c;
        state = READ_CHECKSUM;
        break;

      case READ_CHECKSUM:
        recievedChecksum = c;
        state = WAIT_END;
        break;

      case WAIT_END:
        if (c == 0x7F) {
          char expectedChecksum = modeChar ^ 0xAA;
          if (recievedChecksum == expectedChecksum) {
            Serial.print("Command: ");
            Serial.println(modeChar);
            updateRGBLED(modeChar);
            updateLEDs(c);
          }
          else {
            RFSerial.write('X');
            updateRGBLED('X');
          }
          state = WAIT_START;
          timer = 0;
        }
    }
    delay(10);
  } 
  
  timer += 0.1;
  delay(100);
}

void updateLEDs(char c) {
  if (c == 'L') {
    analogWrite(redLED, 20);
    analogWrite(yellowLED, 0);
    analogWrite(greenLED, 0);
  }
  else if (c == 'N') {
    analogWrite(redLED, 0);
    analogWrite(yellowLED, 20);
    analogWrite(greenLED, 0);
  }
  else if (c == 'R') {
    analogWrite(redLED, 0);
    analogWrite(yellowLED, 0);
    analogWrite(greenLED, 20);
  }
}
