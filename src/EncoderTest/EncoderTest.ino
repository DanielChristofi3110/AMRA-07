#include "TimerOne.h"

unsigned int counter1 = 0; // Counter for motor 1
unsigned int counter2 = 0; // Counter for motor 2

// Motor 1 pins (L9110)
int b1a = 8;  
int b1b = 9;

// Motor 2 pins (L9110)
int c1a = 7;  
int c1b = 10;

// Speed sensor pins
int sensor1 = 2; // Interrupt pin 0 (Motor 1 sensor)
int sensor2 = 3; // Interrupt pin 1 (Motor 2 sensor)

// Variables for RPM and control
int rpm1 = 0, rpm2 = 0; // Current RPM for both motors
int pwmMotor1 = 100; // Fixed PWM for Motor 1
int pwmMotor2 = 255; // Adjustable PWM for Motor 2

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

void timerIsr() {
  Timer1.detachInterrupt(); // Stop the timer temporarily

  // Calculate RPM for Motor 1
  rpm1 = (counter1 * 60) / 20; // Assuming 20 holes in the disc
  counter1 = 0; // Reset counter

  // Calculate RPM for Motor 2
  rpm2 = (counter2 * 60) / 20; // Assuming 20 holes in the disc
  counter2 = 0; // Reset counter

  // Display RPMs
  Serial.print("Motor 1 RPM: ");
  Serial.println(rpm1);

  Serial.print("Motor 2 RPM: ");
  Serial.println(rpm2);

  // Simple speed matching adjustment
  int error = rpm1 - rpm2;

  if (error > 5) {
    // Motor 2 is slower; increase its PWM
    pwmMotor1 -= 3;
  } else if (error < -5) {
    // Motor 2 is faster; decrease its PWM
    pwmMotor1 += 3;
  }

  // Constrain PWM values to valid range (0-255)
  pwmMotor2 = constrain(pwmMotor2, 0, 255);

  // Display adjustment
  Serial.print("Error: ");
  Serial.println(error);

  Serial.print("PWM Motor 1: ");
  Serial.println(pwmMotor1);

  Serial.print("PWM Motor 2: ");
  Serial.println(pwmMotor2);

   Serial.print("Dist Motor 1: ");
  Serial.println(d1);

  Serial.print("Dist Motor 2: ");
  Serial.println(d2);


  Serial.println("-------------------------");

  Timer1.attachInterrupt(timerIsr); // Re-enable the timer
}

void setup() {
  Serial.begin(9600);

  // Motor pin modes
  pinMode(b1a, OUTPUT);
  pinMode(b1b, OUTPUT);
  pinMode(c1a, OUTPUT);
  pinMode(c1b, OUTPUT);

  // Speed sensor pin modes
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);

  // Set up Timer1 for 1-second intervals
  Timer1.initialize(1000000); // 1 second
  Timer1.attachInterrupt(timerIsr);

  // Attach interrupts for speed sensors
  attachInterrupt(digitalPinToInterrupt(sensor1), docount1, RISING);
  attachInterrupt(digitalPinToInterrupt(sensor2), docount2, RISING);
}

void loop() {
  // Drive Motor 1 at constant speed
  analogWrite(b1a, pwmMotor1); // Set PWM for Motor 1
  digitalWrite(b1b, 1); // Set rotation direction (Clockwise)

  // Drive Motor 2 with adjusted speed
  analogWrite(c1a, pwmMotor2); // Set PWM for Motor 2
  digitalWrite(c1b, 1); // Set rotation direction (Clockwise)
}
