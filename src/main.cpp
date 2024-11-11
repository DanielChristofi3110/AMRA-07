#include "DualL298N.h"

DualL298N motorDriver(2, 3, 9, 5, 4, 10); 

int a_d=1, b_d=1,s=80,a_o=0,b_o=8;
void setup() {
    Serial.begin(9600);       
    motorDriver.begin();     
}

void loop() {
  
  motorDriver.stopMotorA();
  motorDriver.stopMotorB();
  //motorDriver.setMotorASpeed(s+a_o,a_d);
 // motorDriver.setMotorBSpeed(s+b_o,b_d);
  motorDriver.setSpeedBoth(s+a_o,s+b_o,a_d);
  delay(1000);
  motorDriver.stopAll();
 // motorDriver.setMotorASpeed(-s+a_o,a_d);
 // motorDriver.setMotorBSpeed(-s+b_o,b_d);
  motorDriver.setSpeedBoth(-(s+a_o),-(s+b_o),a_d);
   delay(1000);
   
  motorDriver.stopAll();
  //motorDriver.setMotorASpeed(s+a_o,a_d);
  //motorDriver.setMotorBSpeed(-s+b_o,b_d);
   motorDriver.setSpeedBoth((s+a_o),-(s+b_o),a_d);
   delay(1000);
   
  motorDriver.stopAll();
  //motorDriver.setMotorASpeed(-s+a_o,a_d);
  //motorDriver.setMotorBSpeed(s+b_o,b_d);
    motorDriver.setSpeedBoth(-(s+a_o),(s+b_o),a_d);
   delay(1000);
  
}
