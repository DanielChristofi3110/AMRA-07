#include "TimerOne.h"
#include "DualL298N.h"

unsigned int counter1 = 0; // Counter for motor 1
unsigned int counter2 = 0; // Counter for motor 2

DualL298N motorDriver(7, 6, 9, 4, 5, 10);



// Speed sensor pins
int sensor1 = 2; // Interrupt pin 0 (Motor 1 sensor)
int sensor2 = 3; // Interrupt pin 1 (Motor 2 sensor)

// Variables for RPM and control
int rpm1 = 0, rpm2 = 0; // Current RPM for both motors
int pwmMotor1 = 50; // Fixed PWM for Motor 1
int pwmMotor2 = 50; // Adjustable PWM for Motor 2
int trg_rpm1 = 300;
int trg_rpm2 = 300;

double d1=0;
double d2=0;

void docount1() {
  counter1++;
  d1+=0.1884/20;
}

void docount2() {
  counter2++;
  d2+=0.1884/20;
}



void setup() {
  Serial.begin(9600);
   motorDriver.begin();    
 
 

}
void loop() {
  // Drive Motor 1 at constant speed
  /*analogWrite(b1a, pwmMotor1); // Set PWM for Motor 1
  digitalWrite(b1b, 1); // Set rotation direction (Clockwise)
  setSpeedBoth

  // Drive Motor 2 with adjusted speed
  analogWrite(c1a, pwmMotor2); // Set PWM for Motor 2
  digitalWrite(c1b, 1); // Set rotation direction (Clockwise)*/
   //analogWrite(9, 255);

  //motorDriver.setSpeedBoth(pwmMotor1,pwmMotor2,0);
 motorDriver.setSpeedBoth(pwmMotor1,0,0);
 Serial.println(pwmMotor1++);
 delay(400);
  
}
