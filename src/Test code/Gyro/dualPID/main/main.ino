#include <Wire.h>
#include "MPU6050.h"
#include "DualL298N.h"
#include "PID_v1.h"

DualL298N motorDriver(7, 6, 9, 4, 5, 10);

MPU6050 mpu;

// Timers
unsigned long timer = 0;
float timeStep = 0.01, TimeSinceStart = 0;

// Encoder variables
volatile unsigned int encoderCount1 = 0; // Count pulses for encoder 1
volatile unsigned int encoderCount2 = 0; // Count pulses for encoder 2
unsigned long lastRPMCalcTime = 0;
double rpm1 = 0, rpm2 = 0;
const int encoderPPR = 20; // Pulses per revolution (depends on your encoder)

// Pitch, Roll, and Yaw values
float pitch = 0;
float roll = 0;

// Target yaw angle and current yaw
double targetYaw = 0;
double yaw = 0;
double tyaw = -90;
double yawPIDOutput, rotationPIDOutput;

// PID constants for yaw control
double yawKp = 1, yawKi = 0, yawKd = 0; // Tune as needed

// PID constants for rotation control
double rotKp = 1, rotKi = 0.1, rotKd = 0.05; // Tune as needed

// Create PID controllers
PID yawPID(&yaw, &yawPIDOutput, &targetYaw, yawKp, yawKi, yawKd, DIRECT);
PID rotationPID(&yaw, &rotationPIDOutput, &targetYaw, rotKp, rotKi, rotKd, DIRECT);

// Interrupt service routines for encoders
void encoder1ISR() {
  encoderCount1++;
}

void encoder2ISR() {
  encoderCount2++;
}

void setup() 
{
  Serial.begin(115200);

  // Initialize MPU6050
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

  for (int i = 0; i < 20; i++) {
    Serial.println(i);
    delay(1000);
  }

  // Calibrate gyroscope
  mpu.calibrateGyro();

  // Set PID parameters
  yawPID.SetMode(AUTOMATIC);
  yawPID.SetOutputLimits(-30, 30); // Yaw PID output range
  rotationPID.SetMode(AUTOMATIC);
  rotationPID.SetOutputLimits(10, 50); // Rotation speed range for fine-tuning

  motorDriver.begin();
  yaw = tyaw; // Initialize yaw

  // Initialize encoders
  attachInterrupt(digitalPinToInterrupt(2), encoder1ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(3), encoder2ISR, RISING);
}
void calculateRPM() {
  static unsigned long lastCalcTime = 0;
  unsigned long currentTime = millis();
  unsigned long timeElapsed = currentTime - lastCalcTime;

  if (timeElapsed >= 100) { // Calculate RPM every 100 ms
    rpm1 = (encoderCount1 / (double)encoderPPR) * (60000.0 / timeElapsed); // Convert pulses to RPM
    rpm2 = (encoderCount2 / (double)encoderPPR) * (60000.0 / timeElapsed);
    encoderCount1 = 0; // Reset pulse counters
    encoderCount2 = 0;
    lastCalcTime = currentTime;

    Serial.print("RPM1: ");
    Serial.println(rpm1);
    Serial.print("RPM2: ");
    Serial.println(rpm2);
  }
}

void loop()
{
  calculateRPM(); // Update RPM values

  timer = millis();

  // Read normalized values
  Vector norm = mpu.readNormalizeGyro();

  // Calculate Pitch, Roll, and Yaw
  pitch = pitch + norm.YAxis * timeStep;
  roll = roll + norm.XAxis * timeStep;
  yaw = yaw + norm.ZAxis * timeStep;

  // Output raw values for debugging
  /*Serial.print("Yaw = ");
  Serial.println(yaw);
  Serial.print("TimeSinceStart = ");
  Serial.println(TimeSinceStart);
*/
  int acc = 10; // Tolerance around the target yaw
  int baseSpeed1 = 20; // Minimum motor speed for smooth operation
  int baseSpeed2 = 20;

  if (abs(yaw - targetYaw) > acc) {
    // If yaw error is significant, compute rotation PID
    rotationPID.Compute();

    int speed = rotationPIDOutput; // Dynamically adjust rotation speed
    if (yaw > targetYaw) {
      // Rotate left
      motorDriver.setSpeedBoth(baseSpeed1 + speed, baseSpeed2 - speed, 0);
    //  Serial.println("Rotating Left");
    } else {
      // Rotate right
      motorDriver.setSpeedBoth(baseSpeed1 - speed, baseSpeed2 + speed, 0);
     // Serial.println("Rotating Right");
    }
  } else {
    // Yaw error is small; use yawPID for forward adjustment
    yawPID.Compute();
    motorDriver.setSpeedBoth(45 - yawPIDOutput, 45 + yawPIDOutput, 0);
    Serial.println("Moving Forward");

    TimeSinceStart += timeStep;
    if (TimeSinceStart > 2) {
      TimeSinceStart = 0;
      yaw = -180; // Reset yaw for another turn
    }
  }

  // Wait to full timeStep period
  delay((timeStep * 1000) - (millis() - timer));
}
