// DualL298N.h
#ifndef DUAL_L298N_H
#define DUAL_L298N_H

#include <Arduino.h>

class DualL298N {
public:
    DualL298N(int in1, int in2, int ena, int in3, int in4, int enb);  // in1(2) in2(3) fna(1) in3(4) in4(5) fna(6)

    void begin();                                        
    void setMotorASpeed(int speed, int stepDelay = 10); 
    void stopMotorA();                                  
    void setMotorBSpeed(int speed, int stepDelay = 10);  
    void stopMotorB();                                   
    void setSpeedBoth(int speedA, int speedB, int stepDelay = 10); 
    void stopAll();

private:
    int _in1, _in2, _ena;                               
    int _in3, _in4, _enb;                              

    int _currentSpeedA;                                
    int _currentSpeedB;                                

    void setMotorDirection(int inPin1, int inPin2, bool direction);    
    void gradualSpeedChange(int &currentSpeedA, int targetSpeedA, int &currentSpeedB, int targetSpeedB, int stepDelay); // Helper for simultaneous speed change
};

#endif
