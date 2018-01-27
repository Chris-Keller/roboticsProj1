//implement in this document the functions from MyServos.h
#include "MyServos.h"
#include <Servo.h>

Servo LServo;  // Declare Left servo
Servo RServo;  // Declare right servo


namespace {
  //this is a nameless namespace
  //put here variables only viewable to this module
  
}

void setupServo() {

	LServo.attach(2);  // Attach right servo to pin 2
	RServo.attach(3);  // Attach right servo to pin 3

}

void setSpeeds(int microsLeft, int microsRight) {
	//this is a nameless namespace
	//put here variables only viewable to this module

	microsLeft += 1500;
	microsRight = -microsRight;
	microsRight += 1500;

	LServo.writeMicroseconds(microsLeft);
	RServo.writeMicroseconds(microsRight);

}

void setSpeedsRPS(float rpsLeft, float rpsRight){

}
