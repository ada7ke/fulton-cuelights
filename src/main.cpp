#include <Arduino.h>

// Define the GPIO pins for the switches
#define leftSwitchPin 3   // Connect left switch here
#define rightSwitchPin 4  // Connect right switch here
#define ledPin 8          // Existing single-color LED

// Define the GPIO pins for the RGB LED
#define rPin 2    // Connect red pin of RGB LED here
#define gPin 1   // Connect green pin of RGB LED here
#define bPin 0   // Connect blue pin of RGB LED here

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
  
  // Configure the LED pins as outputs
  pinMode(ledPin, OUTPUT);
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
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
  
  // Control the single LED and print the current mode to the Serial Monitor
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
      Serial.println("Mode: right switch pressed");
      break;
  }
  
  // Control the RGB LED based on the current mode
  // For a common cathode RGB LED: HIGH turns the LED on.
  switch (currentMode) {
    case MODE_LEFT:
      // Red 
      digitalWrite(rPin, HIGH);
      digitalWrite(gPin, LOW);
      digitalWrite(bPin, LOW);
      break;
    case MODE_NEITHER:
      // yelow
      digitalWrite(rPin, HIGH);
      digitalWrite(gPin, HIGH);
      digitalWrite(bPin, LOW);
      break;
    case MODE_RIGHT:
      // green
      digitalWrite(rPin, LOW);
      digitalWrite(gPin, HIGH);
      digitalWrite(bPin, LOW);
      break;
  }
  
  delay(100); // Short delay for debounce and to avoid spamming the serial output
}
