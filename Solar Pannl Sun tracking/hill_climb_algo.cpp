#include <Stepper.h>
#include <LiquidCrystal.h>

const int stepsPerRevolution = 2048; 

// Stepper motor initialized on your working Arduino Mega pins
Stepper myStepper(stepsPerRevolution, 50, 52, 51, 53);

// Initialize the LCD library with your custom pin mapping:
// (RS, Enable, D4, D5, D6, D7)
LiquidCrystal lcd(37, 36, 35, 34, 33, 32);
const int ldrLeftPin = A1;   // Left photoresistor analog input
const int ldrRightPin = A2;  // Right photoresistor analog input

const int threshold = 15;    // Deadband threshold to prevent jitter/oscillation
int currentPosition = 0;     // Track relative stepper position
const int maxTravel = 1024;  // Limit sweep range (half rotation)

void setup() {
  lcd.begin(16, 2);
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(25, OUTPUT);
  
  lcd.setCursor(0, 0);
  lcd.print("Solar Optimizer ");
  lcd.setCursor(0, 1);
  lcd.print("Searching...    ");
  delay(1000);
}

void loop() {
  // 1. Sample the objective function (Light Intensity readings)
  int leftVal = analogRead(ldrLeftPin);
  int rightVal = analogRead(ldrRightPin);
  
  // 2. Compute gradient (difference between sensors)
  int gradient = rightVal - leftVal;

  // 3. Hill-Climbing / Gradient Ascent Logic
  if (abs(gradient) > threshold) {
    if (gradient > 0 && currentPosition < maxTravel) {
      // Right side is brighter -> Step clockwise
      myStepper.step(1);
      currentPosition++;
      updateLCD("Climbing: RIGHT", leftVal, rightVal);
    } 
    else if (gradient < 0 && currentPosition > -maxTravel) {
      // Left side is brighter -> Step counter-clockwise
      myStepper.step(-1);
      currentPosition--;
      updateLCD("Climbing: LEFT ", leftVal, rightVal);
    }
  } else {
    // Peak reached (gradient approx 0) -> Hold position
    updateLCD("Peak Locked    ", leftVal, rightVal);
    // Cut coil power to save energy when stationary
    digitalWrite(22, LOW);
    digitalWrite(24, LOW);
    digitalWrite(23, LOW);
    digitalWrite(25, LOW);
  }

  // Small sampling delay for stability
  delay(20);
}

void updateLCD(String stateMsg, int lVal, int rVal) {
  lcd.setCursor(0, 0);
  lcd.print(stateMsg);
  
  lcd.setCursor(0, 1);
  lcd.print("L:");
  lcd.print(lVal);
  lcd.print(" R:");
  lcd.print(rVal);
  lcd.print("   "); // Clear trailing digits
}