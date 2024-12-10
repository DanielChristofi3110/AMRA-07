#include <Wire.h>
#include "MPU6050.h"
#include "DualL298N.h"
#include "PID_v1.h"

DualL298N motorDriver(7, 6, 9, 4, 5, 10);

MPU6050 mpu;

//way points

int way_angle[3] = {0,120,120};
float way_dist[3] = {500,500,500};// distance in mm
float dm1=0,dm2=0;
int c_way=0,finish_way=2;



// Timers
unsigned long timer = 0;
float timeStep = 0.01, TimeSinceStart = 0;
const float time_to_stab = 1;
float c_time_to_stab=0;

// Encoder variables
volatile unsigned int encoderCount1 = 0; // Count pulses for encoder 1
volatile unsigned int encoderCount2 = 0; // Count pulses for encoder 2
unsigned long lastRPMCalcTime = 0;
double rpm1 = 0, rpm2 = 0;
const int encoderPPR = 20; // Pulses per revolution 



// Target yaw angle and current yaw
double t_yaw=0;
double yaw = 0;
double yawPIDOutput, rotationPIDOutput;

// PID constants for yaw control
double yawKp = 0.75, yawKi = 1.0, yawKd = 0.1; 

// PID constants for rotation control
double rotKp = 1, rotKi = 0, rotKd = 0; 

//motors
int motorA=0,motorB=0;


//  PID controllers
PID yawPID(&yaw, &yawPIDOutput, &t_yaw, yawKp, yawKi, yawKd, DIRECT);
PID rotationPID(&yaw, &rotationPIDOutput, &t_yaw, rotKp, rotKi, rotKd, DIRECT);

// States
enum _state {
    forward,
    rotating,
    finish,
    stand
};

_state State = stand;


// Interrupt service routines for encoders
void encoder1ISR() {
  encoderCount1++;
  if(State == forward)
  dm1+=10.362;
}

void encoder2ISR() {
  encoderCount2++;
  if(State == forward)
  dm2+=10.362;
}

void setup() 
{
  Serial.begin(115200);

  // Initialize MPU6050
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

  pinMode(13, OUTPUT);

  for (int i = 0; i < 20; i++) {
    digitalWrite(13, HIGH);
    Serial.println(i);
    delay(500);
    if(i>40){
    digitalWrite(13, LOW);
    }
    delay(500);
  }

  // Calibrate gyroscope
  mpu.calibrateGyro();

  // Set PID parameters
  yawPID.SetMode(AUTOMATIC);
  yawPID.SetOutputLimits(-30, 30); // Yaw PID output range
  rotationPID.SetMode(AUTOMATIC);
  rotationPID.SetOutputLimits(-25, 25); // Rotation speed range for fine-tuning

  motorDriver.begin();
  yaw = way_angle[c_way]; // Initialize yaw
  c_time_to_stab=time_to_stab;

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

void printTotalDistance() {
  float totalDistance = (dm1 + dm2) / 2.0; // Average distance of both wheels
  Serial.print("Distance_Traveled_(mm) = ");
  Serial.println(totalDistance);
}

void resetDistance() {
  dm1 = 0;
  dm2 = 0;
}

void loop()
{
  //calculateRPM(); // Update RPM values

  timer = millis();

  // Read normalized values
  Vector norm = mpu.readNormalizeGyro();

  // Calculate Yaw
  yaw = yaw + norm.ZAxis * timeStep;
  int acc = 10; // Tolerance around the target yaw




  
  if(State!=finish){


     /**/
    // Serial.print("State = ");
    // Serial.println(State);
     //Serial.print("Yaw = ");
    // Serial.println(yaw);

    // Serial.print("Way dist = ");
    // Serial.println(way_dist[c_way]);
    // Serial.print("Way angle = ");
    // Serial.println(way_angle[c_way]);

    // Serial.print("MotorA = ");
    // Serial.println(motorA);
    // Serial.print("MotorB = ");
    // Serial.println(motorB);

     //Serial.print("PID ROTATE = ");
    
      //Serial.print("PID YAW = ");
     //Serial.println(yawPIDOutput);
    
     
      //printTotalDistance();


    
     // Serial.println(yawPIDOutput);

       Serial.print("[");
       Serial.print(State);
       Serial.print(",");
       Serial.print(yaw);
       Serial.print(",");
       Serial.print(way_dist[c_way]);
       Serial.print(",");
       Serial.print(way_angle[c_way]);
       Serial.print(",");
       Serial.print(motorA);
       Serial.print(",");
       Serial.print(motorB);
       Serial.print(",");
       Serial.print(yawPIDOutput);
       Serial.print(",");
       Serial.print(rotationPIDOutput);
       Serial.println("]");

     
     
  if ((abs(yaw) > acc) || (c_time_to_stab>0)) {

    State=rotating;

    c_time_to_stab-=timeStep;

    rotationPID.Compute();

    // Serial.print("Rot pid");
    

     motorA = -(float)(rotationPIDOutput)*1.2;
     motorB = (float)(rotationPIDOutput)*1.0; 

    
   // Serial.println(rotationPIDOutput);
    motorDriver.setSpeedBoth(motorA, motorB, 0);
 
  } else if(c_time_to_stab<=0) {
     yawPID.Compute();
     State=forward;
     
   
    
     motorA = (32) -yawPIDOutput;
     motorB = (30) + yawPIDOutput; 
    
  motorDriver.setSpeedBoth(motorA, motorB, 0);
  //motorDriver.stopAll();
   
   //way_dist[c_way]-= timeStep;
    if (way_dist[c_way] <= ((dm1 + dm2) / 2.0)) {
       if(c_way<finish_way){
        resetDistance();
         c_way++;
          
          yaw = way_angle[c_way]; // Reset yaw for another turn
          c_time_to_stab=time_to_stab;
         


       }else{
        State = finish;
        motorDriver.stopAll();
       }

      
     
    }

  }

 
  // Wait to full timeStep period
  delay((timeStep * 1000) - (millis() - timer));
  }else{
    Serial.println("FIN");
     motorDriver.stopAll();

    digitalWrite(13, HIGH);
  
    delay(200);
    digitalWrite(13, LOW);
    delay(200);
    
    
    }
  
 
}
