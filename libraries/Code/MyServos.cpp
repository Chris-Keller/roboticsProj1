//implement in this document the functions from MyServos.h
#include "MyServos.h"
#include <Servo.h>
#include "MyEncoders.h"

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


   /****************************************************

               MANUALLY CHECK WHEEL DISTANCE
                    AND DIAMETER VALUES!


   *****************************************************/

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


   for (int i = 0; i <= 40; i++){

      if (found_left == false){
         // Exact RPS match?  Return this speed
         if (speedtable[i][RPS_LEFT] == rpsLeft){
            microsLeft = i * 10;
            found_left = true;
         } else {
            if (speedtable[i][RPS_LEFT] > rpsLeft){
               // target IPS is somewhere between temp_L and speedtable[i][1]
               microsLeft = ((rpsLeft - temp_L) / (speedtable[i][RPS_LEFT] - temp_L)   *  10) + ((i-1) * 10);
               found_left = true;
            } else {
               // Target IPS hasn't been narrowed down yet.  Hold most recent IPS and check the next.
               temp_L = speedtable[i][RPS_LEFT];
            }
         }
      }

      if (found_right == false){
         // Exact RPS match?  Return this speed
         if (speedtable[i][RPS_RIGHT] == rpsRight){
            microsRight = i * 10;
            found_right = true;
         } else {
            if (speedtable[i][RPS_RIGHT] > rpsRight){
               // target IPS is somewhere between temp_L and speedtable[i][1]
               microsRight = ((rpsRight - temp_R) / (speedtable[i][RPS_RIGHT] - temp_R)   *  10) + ((i-1) * 10);
               found_right = true;
            } else {
               // Target IPS hasn't been narrowed down yet.  Hold most recent IPS and check the next.
               temp_R = speedtable[i][RPS_RIGHT];
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
}

void setSpeedsvs(float v, float w){
// v is velocity in inches per second
// w is angular velocity in radians per second

}

void calibrate(){
	// Filled by calibrate(), Table corresponds to -200 to 200 in 10 increments.
	// [?][0] = IPS of Left Wheel
	// [?][1] = RPS of Left Wheel
	// [?][2] = IPS of Right Wheel
	// [?][3] = RPS of Right Wheel
	// This is a bit strange but I'm unclear on how much memory the arduino can handle
   // Store the IPS and RPS of each wheel for various input values into speedtable for use in setting accurate speeds in other functions
	short int samplesL = 0;
	short int samplesR = 0;
	unsigned long counts[2] = { 0 };
	unsigned long countL = 0;
	unsigned long countR = 0;
	float speed[2] = { 0 };
	float totalspdL = 0;
	float totalspdR = 0;
	unsigned long start_time = time;

	for (int i = -20; i < 20; i++) {

		setSpeeds(i * 10, i * 10);
		while ((time - start_time) < 100);				// wait about 100 ms
		while (samplesL <= 10 && samplesR <= 10) {
			getCounts(counts);
			if (countL != counts[0]) {
				getSpeeds(speed);
				totalspdL += speed[0];
				samplesL++;
				countL = counts[0];
			}

			if (countR != counts[1]) {
				getSpeeds(speed);
				totalspdR += speed[1];
				samplesR++;
				countR = counts[1];
			}
		}

		speedtable[i][0] = totalspdL / samplesL;
		speedtable[i][1] = speedtable[i][0] / WHEEL_CIRCUM;
		speedtable[i][2] = totalspdR / samplesR;
		speedtable[i][3] = speedtable[i][2] / WHEEL_CIRCUM;

	}

/*************************************************************

      DOUBLE CHECK UNITS ON THE IPS / WHEEL_CIRCUM OPERATION

*************************************************************/

}
