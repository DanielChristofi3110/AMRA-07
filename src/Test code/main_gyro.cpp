#include "DualL298N.h"
#include <Wire.h>
#include <MPU6050.h>

DualL298N motorDriver(2, 3, 9, 5, 4, 10);
MPU6050 mpu;

unsigned long lastTime = 0;
float timeStep = 0.01; // Time step in seconds
float velocityX = 0, velocityY = 0, velocityZ = 0; // Velocity in X, Y, Z axes
float distanceX = 0, distanceY = 0, distanceZ = 0; // Distance traveled in X, Y, Z axes
int stab_time = 60000;

float pitch = 0.0, roll = 0.0, yaw = 0.0; // Orientation angles
float filterCoefficient = 0.98; // Complementary filter coefficient

int a_d = 1, b_d = 1, s = 80, a_o = 0, b_o = 8;

void setup() {
    Serial.begin(9600);
    motorDriver.begin();
    Wire.begin();
    mpu.initialize();

    if (!mpu.testConnection()) {
        Serial.println("MPU6050 connection failed!");
        while (1);
    } else {
        Serial.println("MPU6050 connected.");
    }

    Serial.println("Stabilizing...");
    delay(stab_time); // Wait for stabilization
    Serial.println("Stabilization complete.");
}

void loop() {
    // Variables to store raw accelerometer and gyroscope values
    int16_t accelX_raw, accelY_raw, accelZ_raw;
    int16_t gyroX_raw, gyroY_raw, gyroZ_raw;

    // Read raw accelerometer and gyroscope values
    mpu.getMotion6(&accelX_raw, &accelY_raw, &accelZ_raw, &gyroX_raw, &gyroY_raw, &gyroZ_raw);

    // Convert raw values to 'g' and 'deg/s'
    float accelX = accelX_raw / 16384.0;
    float accelY = accelY_raw / 16384.0;
    float accelZ = accelZ_raw / 16384.0;
    float gyroX = gyroX_raw / 131.0; // deg/s for roll
    float gyroY = gyroY_raw / 131.0; // deg/s for pitch
    float gyroZ = gyroZ_raw / 131.0; // deg/s for yaw

    // Calculate the time difference
    unsigned long currentTime = millis();
    float deltaTime = (currentTime - lastTime) / 1000.0;
    lastTime = currentTime;

    // Calculate pitch and roll from the accelerometer
    float accelPitch = atan2(accelY, sqrt(accelX * accelX + accelZ * accelZ)) * 180 / PI;
    float accelRoll = atan2(-accelX, accelZ) * 180 / PI;

    // Integrate gyroscope data to update pitch and roll
    float gyroPitchDelta = gyroY * deltaTime;
    float gyroRollDelta = gyroX * deltaTime;

    // Complementary filter to combine accelerometer and gyroscope data for pitch and roll
    pitch = filterCoefficient * (pitch + gyroPitchDelta) + (1 - filterCoefficient) * accelPitch;
    roll = filterCoefficient * (roll + gyroRollDelta) + (1 - filterCoefficient) * accelRoll;

    // Complementary filter for yaw, integrating gyroZ over time with slight correction (as discussed before)
    float gyroYawDelta = gyroZ * deltaTime;
    yaw = filterCoefficient * (yaw + gyroYawDelta) + (1 - filterCoefficient) * atan2(accelX, accelY) * 180 / PI;

    // Print pitch, roll, and yaw
    Serial.print("Pitch: ");
    Serial.print(pitch);
    Serial.print(" Roll: ");
    Serial.print(roll);
    Serial.print(" Yaw: ");
    Serial.println(yaw);

    // Integrate acceleration to get velocity
    velocityX += accelX * deltaTime;
    velocityY += accelY * deltaTime;
    velocityZ += accelZ * deltaTime;

    // Integrate velocity to get distance
    distanceX += velocityX * deltaTime;
    distanceY += velocityY * deltaTime;
    distanceZ += velocityZ * deltaTime;

    // Print the calculated distance
    Serial.print(" Distance X: ");
    Serial.print(distanceX);
    Serial.print(" Y: ");
    Serial.print(distanceY);
    Serial.print(" Z: ");
    Serial.println(distanceZ);

    delay(10); // Adjust for the required sample rate

    /* Motor control code
    motorDriver.stopMotorA();
    motorDriver.stopMotorB();
    motorDriver.setSpeedBoth(s + a_o, s + b_o, a_d);
    delay(1000);
    motorDriver.stopAll();
    motorDriver.setSpeedBoth(-(s + a_o), -(s + b_o), a_d);
    delay(1000);
    motorDriver.stopAll();
    motorDriver.setSpeedBoth((s + a_o), -(s + b_o), a_d);
    delay(1000);
    motorDriver.stopAll();
    motorDriver.setSpeedBoth(-(s + a_o), (s + b_o), a_d);
    delay(1000);
    */
}
