//implement in this document the functions from MyServos.h
#include "MyServos.h"
#include <Servo.h>
#include "MyEncoders.h"
#include <Arduino.h>
#include <Adafruit_RGBLCDShield.h>

Servo LServo;  // Declare Left servo
Servo RServo;  // Declare right servo

float tempspeed[2] = {0};

namespace {
  //this is a nameless namespace
  //put here variables only viewable to this module

   // Filled by calibrate(), Table corresponds to -200 to 200 in 10 increments.
   // [?][0] = IPS of Left Wheel
   // [?][1] = RPS of Left Wheel
   // [?][2] = IPS of Right Wheel
   // [?][3] = RPS of Right Wheel
   // This is a bit strange but I'm unclear on how much memory the arduino can handle
   float speedtable[40][4];   


   const int MINSPEED = -200;
   const int MAXSPEED = 200;
   const float WHEEL_DIAM = 2.61;   // in inches
   const float WHEEL_DIST = 3.95;   // space between wheels in inches
   const int HOLE_NUMBER = 64;
   const float WHEEL_CIRCUM = 8.2;
   const float MAXSPEED_REVPERSEC = 0.8;


   enum speedtableIndexes{IPS_LEFT,
         RPS_LEFT,
         IPS_RIGHT,
         RPS_RIGHT
   };
}

void setupServo() {

	LServo.attach(2);  // Attach right servo to pin 2
	RServo.attach(3);  // Attach right servo to pin 3

}

void setSpeeds(int microsLeft, int microsRight) {

   if (microsLeft < -200)
      microsLeft = -200;
   if (microsRight < -200)
      microsRight = -200;
   if (microsLeft > 200)
      microsLeft = 200;
   if (microsRight > 200)
      microsRight = 200;

	microsLeft += 1500;
	microsRight = -microsRight;
	microsRight += 1500;



	LServo.writeMicroseconds(microsLeft);
	RServo.writeMicroseconds(microsRight);

}

void setSpeedsRPS(float rpsLeft, float rpsRight){
// RPS = Revolutions per Second

	int microsLeft = -1;
	int microsRight = -1;
	float temp_L = 0;
	float temp_R = 0;
	bool found_left = false;
	bool found_right = false;

	if (rpsLeft == 0) {
		found_left = true;
		microsLeft = 0;
	}
	if (rpsRight == 0) {
		found_right = true;
		microsRight = 0;
	}


	for (int i = -20; i <= 20; i++) {

		if (found_left == false) {
			// Exact RPS match?  Return this speed
			if (speedtable[i + 20][RPS_LEFT] == rpsLeft) {
				microsLeft = i * 10;
				found_left = true;
				
			}
			else {
				if (speedtable[i + 20][RPS_LEFT] > rpsLeft) {
					// target RPS is somewhere between temp_L and speedtable[i][1]
					microsLeft = ((rpsLeft - temp_L) / (speedtable[i + 20][RPS_LEFT]) + (i - 1)) * 10;
					found_left = true; 
				}
				else {
					// Target RPS hasn't been narrowed down yet.  Hold most recent IPS and check the next.
					temp_L = speedtable[i + 20][RPS_LEFT];
				}
			}
		}
	}


	for (int i = -20; i <= 20; i++) {

		if (found_right == false) {
			// Exact RPS match?  Return this speed
			if (speedtable[i + 20][RPS_RIGHT] == rpsRight) {
				microsRight = i * 10;
				found_right = true;
			}
			else {
				if (speedtable[i + 20][RPS_RIGHT] > rpsRight) {
					// target RPS is somewhere between temp_R and speedtable[i][IPS_RIGHT]
					microsRight = ((rpsRight - temp_R) / (speedtable[i + 20][RPS_RIGHT]) + (i - 1)) * 10;
					found_right = true;
				}
				else {
					// Target RPS hasn't been narrowed down yet.  Hold most recent RPS and check the next.
					temp_R = speedtable[i + 20][RPS_RIGHT];
				}
			}
		}
	}


	if (microsLeft == -1)
		microsLeft = 0;
	if (microsRight == -1)
		microsRight = 0;

	Serial.print("I believe left speed is ");
	Serial.print(microsLeft);

	Serial.print(", I believe right speed is ");
	Serial.println(microsRight);
	setSpeeds(microsLeft, microsRight);

}

void setSpeedsIPS(float ipsLeft, float ipsRight){
// IPS = Inches per second
	int microsLeft = -1;
	int microsRight = -1;
	float temp_L = 0;
	float temp_R = 0;
	bool found_left = false;
	bool found_right = false;

	if (ipsLeft == 0) {
		found_left = true;
		microsLeft = 0;
	}
	if (ipsRight == 0) {
		found_right = true;
		microsRight = 0;
	}


	for (int i = -20; i <= 20; i++) {

		if (found_left == false) {
			// Exact IPS match?  Return this speed
			if (speedtable[i+20][IPS_LEFT] == ipsLeft) {
				microsLeft = i * 10;
				found_left = true;
			}
			else {
				if (speedtable[i+20][IPS_LEFT] > ipsLeft) {
					// target IPS is somewhere between temp_L and speedtable[i][1]
					microsLeft = ((ipsLeft - temp_L) / (speedtable[i+20][IPS_LEFT] - temp_L) * 10) + ((i - 1) * 10);
					found_left = true;
				}
				else {
					// Target IPS hasn't been narrowed down yet.  Hold most recent IPS and check the next.
					temp_L = speedtable[i+20][IPS_LEFT];
				}
			}
		}
	}


	for (int i = -20; i <= 20; i++) {

		if (found_right == false) {
			// Exact IPS match?  Return this speed
			if (speedtable[i + 20][IPS_RIGHT] == ipsRight) {
				microsRight = i * 10;
				found_right = true;
			}
			else {
				if (speedtable[i + 20][IPS_RIGHT] > ipsRight) {
					// target IPS is somewhere between temp_R and speedtable[i][IPS_RIGHT]
					microsRight = ((ipsLeft - temp_R) / (speedtable[i + 20][IPS_RIGHT] - temp_R) * 10) + ((i - 1) * 10);
					found_right = true;
				}
				else {
					// Target IPS hasn't been narrowed down yet.  Hold most recent IPS and check the next.
					temp_R = speedtable[i + 20][IPS_RIGHT];
				}
			}
		}
	}


	if (microsLeft == -1)
		microsLeft = 0;
	if (microsRight == -1)
		microsRight = 0;

	setSpeeds(microsLeft, microsRight);
}

void setSpeedsvs(float v, float w){
// v is velocity in inches per second
// w is angular velocity in radians per second

}

void calibrate(Adafruit_RGBLCDShield* lcd){

	lcd->setCursor(0, 0);
	lcd->print("Calibrating.");
	lcd->setCursor(0,1);
	lcd->print("Samples: ");

	short int samplesL = 0;
	short int samplesR = 0;
	unsigned long counts[2] = { 0 };
	double speed[2] = { 0 };
	double totalspdL = 0;
	double totalspdR = 0;
	unsigned long last_time = 0;

	for (int i = -20; i < 20; i++) {
		lcd->setCursor(9, 1);
		lcd->print(i + 20);
		lcd->setCursor(11, 1);
		lcd->print("/40");


		if (i == 0) {
			speedtable[20][0] = 0;
			speedtable[20][1] = 0;
			speedtable[20][2] = 0;
			speedtable[20][3] = 0;
			i++;
		}
		setSpeeds(i * 10, i * 10);

		unsigned long start_time = time;
		while ((time - start_time) < 100)
			time = millis();				// wait about 100 ms before taking readings
		samplesL = 0;
		samplesR = 0;
		totalspdL = 0;
		totalspdR = 0;

		short int desired_sample_count = 50;
		if (i > -3 and i < 3)
			desired_sample_count = 20;


		while (samplesL <= desired_sample_count and samplesR <= desired_sample_count) {
			getCounts(counts);
			Serial.print("");

			if (counts[0] > 0) {
				getSpeeds(speed);
				totalspdL += speed[0];
				samplesL++;
				resetCounts();
			}

			if (counts[1] > 0) {
				getSpeeds(speed);
				totalspdR += speed[1];
				samplesR++;
				resetCounts();
			}
		}
		if (i < 0) {
			speedtable[i + 20][0] = -(totalspdL / samplesL);
			speedtable[i + 20][2] = -(totalspdR / samplesR);
		}
		else {
			speedtable[i + 20][0] = totalspdL / samplesL;
			speedtable[i + 20][2] = totalspdR / samplesR;
		}

		if (i < 0) {
			speedtable[i + 20][1] = speedtable[i + 20][0] / WHEEL_CIRCUM;
			speedtable[i + 20][3] = speedtable[i + 20][2] / WHEEL_CIRCUM;
		}
		else {
			speedtable[i + 20][1] = speedtable[i + 20][0] / WHEEL_CIRCUM;
			speedtable[i + 20][3] = speedtable[i + 20][2] / WHEEL_CIRCUM;
		}


	}


	for (int i = 0; i < 40; i++) {
		Serial.print(i - 20);
		Serial.print("\tIPS Left/Right: ");
		Serial.print(speedtable[i][0]);
		Serial.print(",");
		Serial.print(speedtable[i][2]);
		Serial.print("\tRPS Left/Right: ");
		Serial.print(speedtable[i][1]);
		Serial.print(",");
		Serial.println(speedtable[i][3]);
	}

}
