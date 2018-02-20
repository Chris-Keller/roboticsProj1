//implement in this document the functions from MyServos.h
#include "MyServos.h"
#include <Servo.h>
#include "MyEncoders.h"
#include <Arduino.h>
#include <Adafruit_RGBLCDShield.h>
#include <math.h>

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
   float speedtable[41][4];   


   const int MINSPEED = -200;
   const int MAXSPEED = 200;
   const float WHEEL_DIAM = 2.61;   // in inches
   const float WHEEL_DIST = 3.95;   // how far apart wheels are from one another in inches
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

speedtable[0][IPS_LEFT] = -6.00;
speedtable[1][IPS_LEFT] = -5.91;
speedtable[2][IPS_LEFT] = -5.85;
speedtable[3][IPS_LEFT] = -5.80;
speedtable[4][IPS_LEFT] = -5.76;
speedtable[5][IPS_LEFT] = -5.69;
speedtable[6][IPS_LEFT] = -5.59;
speedtable[7][IPS_LEFT] = -5.48;
speedtable[8][IPS_LEFT] = -5.37;
speedtable[9][IPS_LEFT] = -5.19;
speedtable[10][IPS_LEFT] = -4.98;
speedtable[11][IPS_LEFT] = -4.67;
speedtable[12][IPS_LEFT] = -4.33;
speedtable[13][IPS_LEFT] = -3.85;
speedtable[14][IPS_LEFT] = -3.39;
speedtable[15][IPS_LEFT] = -2.78;
speedtable[16][IPS_LEFT] = -2.25;
speedtable[17][IPS_LEFT] = -1.66;
speedtable[18][IPS_LEFT] = -1.13;
speedtable[19][IPS_LEFT] = -0.65;
speedtable[20][IPS_LEFT] = -0.09;
speedtable[21][IPS_LEFT] = 0.00;
speedtable[22][IPS_LEFT] = 0.40;
speedtable[23][IPS_LEFT] = 0.92;
speedtable[24][IPS_LEFT] = 1.50;
speedtable[25][IPS_LEFT] = 2.04;
speedtable[26][IPS_LEFT] = 2.69;
speedtable[27][IPS_LEFT] = 3.16;
speedtable[28][IPS_LEFT] = 3.72;
speedtable[29][IPS_LEFT] = 4.10;
speedtable[30][IPS_LEFT] = 4.50;
speedtable[31][IPS_LEFT] = 4.81;
speedtable[32][IPS_LEFT] = 5.11;
speedtable[33][IPS_LEFT] = 5.29;
speedtable[34][IPS_LEFT] = 5.45;
speedtable[35][IPS_LEFT] = 5.61;
speedtable[36][IPS_LEFT] = 5.74;
speedtable[37][IPS_LEFT] = 5.81;
speedtable[38][IPS_LEFT] = 5.88;
speedtable[39][IPS_LEFT] = 5.93;
speedtable[40][IPS_LEFT] = 6.00;

speedtable[0][IPS_RIGHT] = -6.14;
speedtable[1][IPS_RIGHT] = -6.12;
speedtable[2][IPS_RIGHT] = -6.07;
speedtable[3][IPS_RIGHT] = -6.00;
speedtable[4][IPS_RIGHT] = -5.91;
speedtable[5][IPS_RIGHT] = -5.77;
speedtable[6][IPS_RIGHT] = -5.68;
speedtable[7][IPS_RIGHT] = -5.51;
speedtable[8][IPS_RIGHT] = -5.35;
speedtable[9][IPS_RIGHT] = -5.08;
speedtable[10][IPS_RIGHT] = -4.83;
speedtable[11][IPS_RIGHT] = -4.41;
speedtable[12][IPS_RIGHT] = -4.00;
speedtable[13][IPS_RIGHT] = -3.39;
speedtable[14][IPS_RIGHT] = -2.92;
speedtable[15][IPS_RIGHT] = -2.28;
speedtable[16][IPS_RIGHT] = -1.70;
speedtable[17][IPS_RIGHT] = -1.20;
speedtable[18][IPS_RIGHT] = -0.55;
speedtable[19][IPS_RIGHT] = 0.00;
speedtable[20][IPS_RIGHT] = 0.00;
speedtable[21][IPS_RIGHT] = 0.00;
speedtable[22][IPS_RIGHT] = 1.43;
speedtable[23][IPS_RIGHT] = 1.76;
speedtable[24][IPS_RIGHT] = 2.39;
speedtable[25][IPS_RIGHT] = 2.99;
speedtable[26][IPS_RIGHT] = 3.54;
speedtable[27][IPS_RIGHT] = 4.05;
speedtable[28][IPS_RIGHT] = 4.55;
speedtable[29][IPS_RIGHT] = 4.93;
speedtable[30][IPS_RIGHT] = 5.23;
speedtable[31][IPS_RIGHT] = 5.44;
speedtable[32][IPS_RIGHT] = 5.63;
speedtable[33][IPS_RIGHT] = 5.76;
speedtable[34][IPS_RIGHT] = 5.87;
speedtable[35][IPS_RIGHT] = 5.95;
speedtable[36][IPS_RIGHT] = 6.01;
speedtable[37][IPS_RIGHT] = 6.06;
speedtable[38][IPS_RIGHT] = 6.11;
speedtable[39][IPS_RIGHT] = 6.12;
speedtable[40][IPS_RIGHT] = 6.16;



}

void setSpeeds(int microsLeft, int microsRight) {
/*
Serial.print("Asked to set ");
Serial.print(microsLeft);
Serial.print(", ");
Serial.println(microsRight);*/

	microsLeft *= 0.65;
	microsRight *= 0.65;


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


	for (int i = 0; i <= 40; i++) {

		if (!found_left) {
			// Exact IPS match?  Return this speed
			if (speedtable[i][IPS_LEFT] == ipsLeft) {
				microsLeft = (i-20) * 10;
				found_left = true;
			}
			else {
				if (speedtable[i][IPS_LEFT] > ipsLeft) {
					// target IPS is somewhere between temp_L and speedtable[i][1]
					microsLeft = ((ipsLeft - temp_L) / (speedtable[i][IPS_LEFT] - temp_L) * 10) + ((i-20 - 1) * 10);
					found_left = true;
				}
				else {
					// Target IPS hasn't been narrowed down yet.  Hold most recent IPS and check the next.
					temp_L = speedtable[i][IPS_LEFT];
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
					microsRight = ((ipsRight - temp_R) / (speedtable[i + 20][IPS_RIGHT] - temp_R) * 10) + ((i - 1) * 10);
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
		microsLeft = 200;
	if (microsRight == -1)
		microsRight = 200;
	
	setSpeeds(microsLeft, microsRight);
}

void setSpeedsvw(float v, float w){
// v is velocity in inches per second
// w is angular velocity in radians per second
// If asked to spin a wheel beyond topspeed, just reduce requested speed and repeat.
        if (w > 1.00) 
          w = 1.00;
        if (w < -1.00)
          w = -1.00;

		float Vr = v - w * WHEEL_DIST;
		float Vl = Vr + 2 * w * WHEEL_DIST;
		Serial.print(Vr);
		Serial.print(", ");
		Serial.println(Vl);

	setSpeedsIPS(Vr, Vl);


}

void calibrate(Adafruit_RGBLCDShield* lcd){

	lcd->setCursor(0, 0);
	lcd->print("Calibrating!");
	lcd->setCursor(0,1);
	lcd->print("Samples: ");

	short int samplesL = 0;
	short int samplesR = 0;
	unsigned long counts[2] = { 0 };
	double speed[2] = { 0 };
	double totalspdL = 0;
	double totalspdR = 0;
	unsigned long last_time = 0;

	for (int i = -20; i <= 20; i++) {
		lcd->setCursor(9, 1);
		lcd->print(i + 20);
		lcd->setCursor(11, 1);
		lcd->print("/40");


		setSpeeds(i * 10, i * 10);

		unsigned long start_time = time;
		while ((time - start_time) < 100)
			time = millis();				// wait about 100 ms before taking readings
		samplesL = 0;
		samplesR = 0;
		totalspdL = 0;
		totalspdR = 0;

		short int desired_sample_count = 120;

		time = millis();
		start_time = time;
		while (samplesL <= desired_sample_count and samplesR <= desired_sample_count) {
			if (time - start_time > 60000) {
				break;
			}
			getCounts(counts);
			Serial.print("");
			time = millis();

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




		if (totalspdL != 0.0) {
			if (i < 0) {
				speedtable[i + 20][0] = -(totalspdL / samplesL);
				speedtable[i + 20][2] = -(totalspdR / samplesR);
			}
			else {
				speedtable[i + 20][0] = totalspdL / samplesL;
				speedtable[i + 20][2] = totalspdR / samplesR;
			}
		}
		else {
			speedtable[i + 20][0] = 0;
			speedtable[i + 20][2] = 0;
			speedtable[i + 20][0] = 0;
			speedtable[i + 20][2] = 0;
		}
		if (totalspdR != 0.0) {
			if (i < 0) {
				speedtable[i + 20][1] = speedtable[i + 20][0] / WHEEL_CIRCUM;
				speedtable[i + 20][3] = speedtable[i + 20][2] / WHEEL_CIRCUM;
			}
			else {
				speedtable[i + 20][1] = speedtable[i + 20][0] / WHEEL_CIRCUM;
				speedtable[i + 20][3] = speedtable[i + 20][2] / WHEEL_CIRCUM;
			}
		} 
		else {
			speedtable[i + 20][1] = 0;
			speedtable[i + 20][3] = 0;
			speedtable[i + 20][1] = 0;
			speedtable[i + 20][3] = 0;
		}
	}

	setSpeeds(0, 0);
	Serial.println("\tIPS Left/Right: ");

	for (int i = 0; i <= 40; i++) {
		for (int j = 0; j <= 3; j++) {
			if (isinf(speedtable[i][j]) or isnan(speedtable[i][j]))
				speedtable[i][j] = 0.0;
		}
		Serial.print("speedtable[");
		Serial.print(i);
		Serial.print("][0] = ");
		Serial.print(speedtable[i][0]);
		Serial.println(";");

	}
		for (int i = 0; i <= 40; i++) {
		for (int j = 0; j <= 3; j++) {
			if (isinf(speedtable[i][j]) or isnan(speedtable[i][j]))
				speedtable[i][j] = 0.0;
		}

		Serial.print("speedtable[");
		Serial.print(i);
		Serial.print("][2] = ");
		Serial.print(speedtable[i][2]);
		Serial.println(";");
	}
}
