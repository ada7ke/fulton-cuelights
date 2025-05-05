#include <Arduino.h>
#include "controller.h"
#include "common.h"

SwitchMode currentMode = MODE_RED;
SwitchMode lastMode = MODE_RED;

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
    case MODE_RED:
      modeChar = 'L';
      break;
    case MODE_YELLOW:
      modeChar = 'N';
      break;
    case MODE_GREEN:
      modeChar = 'R';
      break;
    }

    // Update the RGB LED locally
    updateRGBLED(modeChar);
    // Send the mode over RF
    RFSerial.write(modeChar);
    // Debug output
    printf("Controller mode: %c\n", modeChar);
    digitalWrite(ledPin, LOW);
    delay(100);
    
  }
  digitalWrite(ledPin, HIGH);
  delay(100); // Short delay for debouncing and to reduce serial output spam
}
