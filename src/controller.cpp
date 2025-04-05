#include <Arduino.h>
#include "controller.h"
#include "common.h"

SwitchMode currentMode = MODE_NEITHER;
SwitchMode lastMode = MODE_NEITHER;

void setupController() {
  // Set up switch pins with internal pull-ups
  pinMode(leftSwitchPin, INPUT_PULLUP);
  pinMode(rightSwitchPin, INPUT_PULLUP);
  // Set up additional LED pin
  pinMode(ledPin, OUTPUT);
  // Initialize LED to a default state (yellow for MODE_NEITHER)
}

void loopController() {
    // --- Controller (Sender) Code ---
  // Read switch states (LOW means pressed due to pull-ups)
  bool leftPressed = (digitalRead(leftSwitchPin) == LOW);
  bool rightPressed = (digitalRead(rightSwitchPin) == LOW);

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
      digitalWrite(ledPin, HIGH); // Turn on extra LED for left/right modes
      modeChar = 'L';
      break;
    case MODE_NEITHER:
      digitalWrite(ledPin, LOW);
      modeChar = 'N';
      break;
    case MODE_RIGHT:
      digitalWrite(ledPin, HIGH);
      modeChar = 'R';
      break;
    }

    // Update the RGB LED locally
    updateRGBLED(modeChar);
    // Send the mode over RF
    RFSerial.write(modeChar);

    // Debug output
    Serial.print("Controller mode changed: ");
    Serial.println(modeChar);
  }
  delay(100); // Short delay for debouncing and to reduce serial output spam
}

// #include <Arduino.h>

// // --- Pin Definitions ---
// // Three-way switch pins
// #define leftSwitchPin 3    // Connect left switch here
// #define rightSwitchPin 4   // Connect right switch here

// // RGB LED pins (common cathode; HIGH = ON)
// #define rPin 2    // Red channel
// #define gPin 1    // Green channel
// #define bPin 0    // Blue channel

// // RF module UART pins (adjust these pins as needed)
// #define RF_TX_PIN 20  // RX pin to RF module
// #define RF_RX_PIN 21  // TX pin from RF module

// // --- Communication ---
// // We'll use Serial1 for RF module communication.
// HardwareSerial RFSerial(1);

// // --- Mode Definitions ---
// enum SwitchMode {
//   MODE_LEFT,
//   MODE_NEITHER,
//   MODE_RIGHT
// };

// SwitchMode currentMode = MODE_NEITHER;
// SwitchMode lastMode = MODE_NEITHER;  // For change detection

// // Function to update the onboard RGB LED based on mode
// void updateLED(SwitchMode mode) {
//   switch (mode) {
//     case MODE_LEFT:
//       // Red
//       analogWrite(rPin, 255);
//       analogWrite(gPin, 0);
//       analogWrite(bPin, 0);
//       break;
//     case MODE_NEITHER:
//       // Yellow: full red, reduced green for a warmer tone
//       analogWrite(rPin, 255);
//       analogWrite(gPin, 150);  // Adjust for desired yellow hue
//       analogWrite(bPin, 0);
//       break;
//     case MODE_RIGHT:
//       // Green
//       analogWrite(rPin, 0);
//       analogWrite(gPin, 255);
//       analogWrite(bPin, 0);
//       break;
//   }
// }

// void sendMessage(SwitchMode mode) {
//   // Send a corresponding single-character message via RF
//   // 'L' = left, 'N' = neither, 'R' = right
//   char outMsg;
//   switch (currentMode) {
//     case MODE_LEFT:    outMsg = 'L'; break;
//     case MODE_NEITHER: outMsg = 'N'; break;
//     case MODE_RIGHT:   outMsg = 'R'; break;
//   }
//   RFSerial.write(outMsg);
// }

// void setup() {
//   // For debugging on USB Serial
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("Sender starting...");

//   // Begin RF Serial on Serial1 (UART)
//   RFSerial.begin(9600, SERIAL_8N1, RF_RX_PIN, RF_TX_PIN);

//   // Configure switch pins with pull-ups
//   pinMode(leftSwitchPin, INPUT_PULLUP);
//   pinMode(rightSwitchPin, INPUT_PULLUP);
  
//   // Configure RGB LED pins as outputs
//   pinMode(rPin, OUTPUT);
//   pinMode(gPin, OUTPUT);
//   pinMode(bPin, OUTPUT);
  
//   // Initialize LED to middle (MODE_NEITHER)
//   updateLED(MODE_NEITHER);
// }

// void loop() {
//   // Read switches (LOW = pressed because of internal pull-ups)
//   bool leftPressed  = (digitalRead(leftSwitchPin) == LOW);
//   bool rightPressed = (digitalRead(rightSwitchPin) == LOW);

//   // Determine current mode based on switch states
//   if (leftPressed && !rightPressed) {
//     currentMode = MODE_LEFT;
//   } else if (!leftPressed && rightPressed) {
//     currentMode = MODE_RIGHT;
//   } else if (!leftPressed && !rightPressed) {
//     currentMode = MODE_NEITHER;
//   }
  
//   // Only update if the mode has changed
//   if (currentMode != lastMode) {
//     lastMode = currentMode;
//     updateLED(currentMode);  // Update local RGB LED
//     sendMessage(currentMode);
    
//     // For debugging
//     Serial.print("Switch changed, mode: ");
//     if (currentMode == MODE_LEFT) {
//       Serial.println("LEFT");
//     } else if (currentMode == MODE_NEITHER) {
//       Serial.println("NEITHER");
//     } else if (currentMode == MODE_RIGHT) {
//       Serial.println("RIGHT");
//     }
//   }
  
//   delay(100);  // Delay for debouncing and to reduce serial spamming
// }

