// DualL298N.cpp
#include "DualL298N.h"

DualL298N::DualL298N(int in1, int in2, int ena, int in3, int in4, int enb) {
    _in1 = in1;
    _in2 = in2;
    _ena = ena;
    _in3 = in3;
    _in4 = in4;
    _enb = enb;

    _currentSpeedA = 0; 
    _currentSpeedB = 0;  
}

void DualL298N::begin() {
    pinMode(_in1, OUTPUT);
    pinMode(_in2, OUTPUT);
    pinMode(_ena, OUTPUT);
    pinMode(_in3, OUTPUT);
    pinMode(_in4, OUTPUT);
    pinMode(_enb, OUTPUT);

    digitalWrite(_ena, HIGH); 
    digitalWrite(_enb, HIGH); 
}

void DualL298N::setMotorDirection(int inPin1, int inPin2, bool direction) {
    if (direction) {
        digitalWrite(inPin1, HIGH);
        digitalWrite(inPin2, LOW);
    } else {
        digitalWrite(inPin1, LOW);
        digitalWrite(inPin2, HIGH);
    }
}

void DualL298N::gradualSpeedChange(int &currentSpeedA, int targetSpeedA, int &currentSpeedB, int targetSpeedB, int stepDelay) {
    bool directionA = targetSpeedA >= 0;                    
    bool directionB = targetSpeedB >= 0;                     

    setMotorDirection(_in1, _in2, directionA);               
    setMotorDirection(_in3, _in4, directionB);           

    /*while (currentSpeedA != targetSpeedA || currentSpeedB != targetSpeedB) {
        
        if (currentSpeedA < targetSpeedA) {
            currentSpeedA++;
        } else if (currentSpeedA > targetSpeedA) {
            currentSpeedA--;
        }

        if (currentSpeedB < targetSpeedB) {
            currentSpeedB++;
        } else if (currentSpeedB > targetSpeedB) {
            currentSpeedB--;
        }

      
        int pwmValueA = map(abs(currentSpeedA), 0, 100, 0, 255);
        int pwmValueB = map(abs(currentSpeedB), 0, 100, 0, 255);

        analogWrite(_ena, pwmValueA); 
        analogWrite(_enb, pwmValueB); 

        delay(stepDelay);             
    }*/

    int pwmValueA = map(abs(targetSpeedA), 0, 100, 0, 255);
    int pwmValueB = map(abs(targetSpeedB), 0, 100, 0, 255);

        analogWrite(_ena, pwmValueA); 
        analogWrite(_enb, pwmValueB); 
}

void DualL298N::setMotorASpeed(int speed, int stepDelay) {
      speed=constrain(speed,-100,100);
    gradualSpeedChange(_currentSpeedA, speed, _currentSpeedB, _currentSpeedB, stepDelay); 
}

void DualL298N::stopMotorA() {
    _currentSpeedA = 0;
    analogWrite(_ena, 0);
}

void DualL298N::setMotorBSpeed(int speed, int stepDelay) {
    speed=constrain(speed,-100,100);
    gradualSpeedChange(_currentSpeedA, _currentSpeedA, _currentSpeedB, speed, stepDelay); 
}

void DualL298N::stopMotorB() {
    _currentSpeedB = 0;
    analogWrite(_enb, 0); 
}

void DualL298N::setSpeedBoth(int speedA, int speedB, int stepDelay) {
      speedA=constrain(speedA,-100,100);
      speedB=constrain(speedB,-100,100);
    gradualSpeedChange(_currentSpeedA, speedA, _currentSpeedB, speedB, stepDelay); 
}

void DualL298N::stopAll(){
    _currentSpeedA = 0;
    analogWrite(_ena, 0); 
    _currentSpeedB = 0;
    analogWrite(_enb, 0); 

}
