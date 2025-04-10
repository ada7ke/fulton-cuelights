#include "receiver.h"
#include "common.h"

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
  digitalWrite(ledPin, LOW);
  while (RFSerial.available()) {
    char c = RFSerial.read();
    printf("Read: %c\n", c);
    digitalWrite(ledPin, HIGH); 
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
      
    updateRGBLED(c);
    delay(10);
  } 
  
  delay(100);
}

