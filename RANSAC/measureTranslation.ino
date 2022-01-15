// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>

// Connect a stepper motor with 48 steps per revolution (7.5 degree)
// to motor port #2 (M3 and M4)
AF_Stepper motor(200, 2);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");

  motor.setSpeed(100);  // 10 rpm   
}

void loop()
{
  motor.setSpeed(100);
  Serial.println("Avanza una revolucion.");
  motor.step(2000, FORWARD, DOUBLE); 
  motor.release();
  delay(60000);
  
}
