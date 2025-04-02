#include <Arduino.h>

// Define the GPIO pins for the switches
#define leftSwitchPin 3   // Connect left switch here
#define rightSwitchPin 4  // Connect right switch here
#define ledPin 8

// Enumeration for the switch modes
enum SwitchMode {
  MODE_LEFT,
  MODE_NEITHER,
  MODE_RIGHT
};

SwitchMode currentMode = MODE_NEITHER;

void setup() {
  Serial.begin(9600);
  
  // Configure the switch pins as inputs with internal pull-ups enabled
  pinMode(leftSwitchPin, INPUT_PULLUP);
  pinMode(rightSwitchPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Read the states (LOW means pressed because of the pull-up resistor)
  bool leftPressed  = (digitalRead(leftSwitchPin) == LOW);
  bool rightPressed = (digitalRead(rightSwitchPin) == LOW);

  // Determine the mode based on the readings
  if (leftPressed && !rightPressed) {
    currentMode = MODE_LEFT;
  } else if (!leftPressed && rightPressed) {
    currentMode = MODE_RIGHT;
  } else if (!leftPressed && !rightPressed) {
    currentMode = MODE_NEITHER;
  }
  
  // Print the current mode to the Serial Monitor
  switch (currentMode) {
    case MODE_LEFT:
      digitalWrite(ledPin, HIGH);
      Serial.println("Mode: left switch pressed");
      break;
    case MODE_NEITHER:
      digitalWrite(ledPin, LOW);
      Serial.println("Mode: middle");
      break;
    case MODE_RIGHT:
      digitalWrite(ledPin, HIGH);
      Serial.println("Mode: Right switch pressed");
      break;
  }
  delay(100); // Short delay for debounce and to avoid spamming the serial output
}
















// #define LED_PIN 8       // Onboard LED on ESP32-C3 SuperMini
// #define SWITCH_PIN 3    // Connect your two-way switch here

// void setup() {
//   pinMode(LED_PIN, OUTPUT);           // Set the LED pin as output
//   pinMode(SWITCH_PIN, INPUT_PULLUP);  // Set the switch pin as input with pull-up
// }

// void loop() {
//   // Read the switch state (LOW when pressed due to pull-up)
//   if (digitalRead(SWITCH_PIN) == LOW) {
//     digitalWrite(LED_PIN, HIGH);  // Switch is pressed: turn LED on
//   } else {
//     digitalWrite(LED_PIN, LOW);   // Switch is not pressed: turn LED off
//   }
// }
