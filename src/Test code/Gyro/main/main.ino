/*
    MPU6050 Triple Axis Gyroscope & Accelerometer. Pitch & Roll & Yaw Gyroscope Example.
    Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/3-osiowy-zyroskop-i-akcelerometr-mpu6050.html
    GIT: https://github.com/jarzebski/Arduino-MPU6050
    Web: http://www.jarzebski.pl
    (c) 2014 by Korneliusz Jarzebski
*/

#include <Wire.h>
#include "MPU6050.h"
#include "DualL298N.h"
#include "PID_v1.h"

DualL298N motorDriver(7, 6, 9, 4, 5, 10); 

MPU6050 mpu;




// Timers
unsigned long timer = 0;
float timeStep = 0.01, TimeSinceStart=0;

// Pitch, Roll and Yaw values
float pitch = 0;
float roll = 0;

// Target yaw angle and current yaw
double targetYaw = 0;
double yaw = 0;
double tyaw=-90;
double pidOutput;

// PID constants
double Kp = 1, Ki = 0, Kd = 0; // Tune as needed

// Create PID controller
PID yawPID(&yaw, &pidOutput, &targetYaw, Kp, Ki, Kd, DIRECT);

void setup() 
{
  Serial.begin(115200);

  // Initialize MPU6050
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

  for(int i=0; i<20; i++){
     Serial.println(i);
   delay(1000);
   }  
  // Calibrate gyroscope. The calibration must be at rest.
  // If you don't want calibrate, comment this line.
  mpu.calibrateGyro();
 

  // Set threshold sensivty. Default 3.
  // If you don't want use threshold, comment this line or set 0.
  // mpu.setThreshold(3);
   // Set PID parameters
  yawPID.SetMode(AUTOMATIC);
  yawPID.SetOutputLimits(-30, 30); // Adjust limits for motor control range
  motorDriver.begin();
  yaw=tyaw;
}

void loop()
{
  timer = millis();

  // Read normalized values
  Vector norm = mpu.readNormalizeGyro();

  // Calculate Pitch, Roll and Yaw
  pitch = pitch + norm.YAxis * timeStep;
  roll = roll + norm.XAxis * timeStep;
  yaw = yaw + norm.ZAxis * timeStep;

  // Output raw
 // Serial.print(" Pitch = ");
  //Serial.print(pitch);
  //Serial.print(" Roll = ");
 // Serial.print(roll);  
  Serial.print(" Yaw = ");
  Serial.println(yaw);
  Serial.print(" TimeSinceStart = ");
  Serial.println(TimeSinceStart);
 
   //Serial.print(" Yaw = ");
 //motorDriver.setSpeedBoth(-95,95,0);
 int acc=10;

 if(yaw>=0+acc){
 motorDriver.setSpeedBoth(+35,-35,0);
 Serial.println("Left");

}else if(yaw<=0-acc){
 motorDriver.setSpeedBoth(-35,+35,0);
Serial.println("Right");
 
 }else{
 yawPID.Compute();
  motorDriver.setSpeedBoth(45-pidOutput,45+pidOutput,0);
 // motorDriver.stopAll();
  Serial.println(pidOutput);

  Serial.println("frw");
  tyaw=-tyaw;
  TimeSinceStart+=timeStep;

  if(TimeSinceStart>2){
    TimeSinceStart=0;
    yaw=-180;
    
    }
  //delay(1000);
  // yaw=tyaw;
   //mpu.calibrateGyro();

 }

  // Wait to full timeStep period
  delay((timeStep*1000) - (millis() - timer));
}
