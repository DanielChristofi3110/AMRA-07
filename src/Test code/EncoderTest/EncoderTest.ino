#include "DualL298N.h"

DualL298N motorDriver(7, 6, 9, 4, 5, 10);

// Define pins for encoder OUT signals
const int encoder1Pin = 2; // Interrupt pin 0
const int encoder2Pin = 3; // Interrupt pin 1

// Variables to track encoder counts
volatile long encoder1Count = 0;
volatile long encoder2Count = 0;

// Timer variables
unsigned long lastTime = 0;
unsigned long interval = 1000; // 1 second interval

float rpm1 = 0.0;
float rpm2 = 0.0;

// Number of subdivisions (pulses) per revolution
const int subdivisions = 20;

// Interrupt service routines for encoders
void encoder1ISR() {
  encoder1Count++; // Increment count for each pulse
}

void encoder2ISR() {
  encoder2Count++; // Increment count for each pulse
}

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Set encoder pins as input
  pinMode(encoder1Pin, INPUT);
  pinMode(encoder2Pin, INPUT);

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(encoder1Pin), encoder1ISR, RISING); // Trigger on rising edge
  attachInterrupt(digitalPinToInterrupt(encoder2Pin), encoder2ISR, RISING); // Trigger on rising edge

  // Debugging message
  Serial.println("Setup complete. Starting RPM measurement...");
}

void loop() {

  unsigned long currentTime = millis();

  motorDriver.setSpeedBoth(40, 40,0);

  // Calculate RPM every 1 second
  if (currentTime - lastTime >= interval) {
    lastTime = currentTime;

    // Calculate RPM by dividing counts by the number of subdivisions
    rpm1 = (encoder1Count * 60.0) / (interval / 1000.0) / subdivisions;
    rpm2 = (encoder2Count * 60.0) / (interval / 1000.0) / subdivisions;

    // Debugging: Print raw counts and calculated RPM
    Serial.print("Encoder 1 Count: ");
    Serial.print(encoder1Count);
    Serial.print(" | RPM: ");
    Serial.println(rpm1);

    Serial.print("Encoder 2 Count: ");
    Serial.print(encoder2Count);
    Serial.print(" | RPM: ");
    Serial.println(rpm2);

    // Reset counts
    encoder1Count = 0;
    encoder2Count = 0;
  }
}
