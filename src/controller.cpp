#include <Arduino.h>
#include "controller.h"
#include "common.h"

SwitchMode currentMode = MODE_EMPTY;
SwitchMode lastMode = MODE_EMPTY;

void setupController() {
  // Set up switch pins with internal pull-ups
  pinMode(yellowbuttonPin, INPUT_PULLUP);
  pinMode(greenbuttonPin, INPUT_PULLUP);
  
  // Initialize LED to a default state (yellow for MODE_NEITHER)
  digitalWrite(ledPin, HIGH);
}

void loopController() {
    // --- Controller (Sender) Code ---
  // Read switch states (LOW means pressed due to pull-ups)
  bool yellowbuttonPressed = (digitalRead(yellowbuttonPin) == LOW);
  bool greenbuttonPressed = (digitalRead(greenbuttonPin) == LOW);

  digitalWrite(ledPin, HIGH);

  if (greenbuttonPressed) {
    currentMode = MODE_GREEN;
  }
  else if (yellowbuttonPressed) {
    currentMode = MODE_YELLOW;
  }

  // Update only if the mode has changed
  if (currentMode != lastMode)
  {
    lastMode = currentMode;
    char modeChar;
    switch (currentMode)
    {
      case MODE_YELLOW:
        modeChar = 'Y';
        break;
      case MODE_GREEN:
        modeChar = 'G';
        break;
    }

    // Update the RGB LED locally
    updateRGBLED(modeChar);
    digitalWrite(ledPin, LOW);
    sendCommand(modeChar);
    // Debug output
    printf("Controller mode: %c\n", modeChar);
    delay(100);
  }

  while (RFSerial.available()) {
    char c = RFSerial.read();
    if (c == 'X') {
      updateRGBLED(c);
      lastMode = MODE_EMPTY;
      currentMode = MODE_EMPTY;
    }
  }

  digitalWrite(ledPin, HIGH);
  delay(100); // Short delay for debouncing and to reduce serial output spam
}

void sendCommand(char modeChar) {
  // Send the mode over RF
  char checksum = modeChar ^ 0xAA;
  RFSerial.write(0x7E);
  RFSerial.write(modeChar);
  RFSerial.write(checksum);
  RFSerial.write(0x7F);
}
