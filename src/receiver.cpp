#include "receiver.h"
#include "common.h"

void setupReceiver()
{
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  digitalWrite(redLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(greenLED, LOW);
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
      digitalWrite(redLED, HIGH);
      digitalWrite(yellowLED, LOW);
      digitalWrite(greenLED, LOW);
    }
    else if (c == 'N') {
      digitalWrite(redLED, LOW);
      digitalWrite(yellowLED, HIGH);
      digitalWrite(greenLED, LOW);
    }
    else if (c == 'R') {
      digitalWrite(redLED, LOW);
      digitalWrite(yellowLED, LOW);
      digitalWrite(greenLED, HIGH);
    }
      
    updateRGBLED(c);
    delay(10);
  } 
  
  delay(100);
}

