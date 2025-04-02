#include <Arduino.h>

// Define the GPIO pins for the switches
#define leftSwitchPin 3   // Connect left switch here
#define rightSwitchPin 4  // Connect right switch here
#define ledPin 8          // Existing single-color LED

// Define the GPIO pins for the RGB LED
// Make sure these pins support PWM if you want to adjust brightness.
#define rPin 2    // Connect red pin of RGB LED here
#define gPin 1    // Connect green pin of RGB LED here
#define bPin 0    // Connect blue pin of RGB LED here

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
  
  // Control the RGB LED based on the current mode using PWM values.
  // For a common cathode RGB LED, analogWrite(255) is full brightness.
  switch (currentMode) {
    case MODE_LEFT:
      // Red color
      analogWrite(rPin, 255);
      analogWrite(gPin, 0);
      analogWrite(bPin, 0);
      break;
    case MODE_NEITHER:
      // Yellow: full red, reduced green for a warmer tone
      analogWrite(rPin, 255);
      analogWrite(gPin, 100);  // Adjust this value to get your desired yellow
      analogWrite(bPin, 0);
      break;
    case MODE_RIGHT:
      // Green color
      analogWrite(rPin, 0);
      analogWrite(gPin, 255);
      analogWrite(bPin, 0);
      break;
  }
  
  delay(100); // Short delay for debounce and to avoid spamming the serial output
}
