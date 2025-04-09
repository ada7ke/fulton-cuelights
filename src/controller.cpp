#include <Arduino.h>
#include "controller.h"
#include "common.h"

SwitchMode currentMode = MODE_NEITHER;
SwitchMode lastMode = MODE_NEITHER;

void setupController() {
  // Set up switch pins with internal pull-ups
  pinMode(leftSwitchPin, INPUT_PULLUP);
  pinMode(rightSwitchPin, INPUT_PULLUP);
  
  // Initialize LED to a default state (yellow for MODE_NEITHER)
  digitalWrite(ledPin, HIGH);
}

void loopController() {
    // --- Controller (Sender) Code ---
  // Read switch states (LOW means pressed due to pull-ups)
  bool leftPressed = (digitalRead(leftSwitchPin) == LOW);
  bool rightPressed = (digitalRead(rightSwitchPin) == LOW);

  digitalWrite(ledPin, HIGH);

  // Determine mode based on switch states
  if (leftPressed && !rightPressed)
  {
    currentMode = MODE_LEFT;
  }
  else if (!leftPressed && rightPressed)
  {
    currentMode = MODE_RIGHT;
  }
  else if (!leftPressed && !rightPressed)
  {
    currentMode = MODE_NEITHER;
  }

  // Update only if the mode has changed
  if (currentMode != lastMode)
  {
    lastMode = currentMode;
    char modeChar;
    switch (currentMode)
    {
    case MODE_LEFT:
      modeChar = 'L';
      break;
    case MODE_NEITHER:
      modeChar = 'N';
      break;
    case MODE_RIGHT:
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
