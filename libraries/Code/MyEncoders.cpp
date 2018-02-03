//implement in this document the functions from MyEncoders.h
#include "MyEncoders.h"
#include <PinChangeInt.h>

#define ENCODER_PORT_L 10  // left wheel encoder
#define ENCODER_PORT_R 11  // right wheel encoder

//needs global variable time

unsigned long time = 0;

namespace {
  //this is a nameless namespace
  //put here variables only viewable to this module
	long dtL = -1;
	long dtR = -1;
	double speedL;
	double speedR;
	int countL = 0;
	int countR = 0;
	const double wheelCircum = 8.2;
	const double changes = 64;
	double arc_per_hole = wheelCircum / changes;
}

void resetCounts() {
	countL = 0;
	countR = 0;
}

void getCounts(unsigned long counts[]) {
	counts[0] = countL;
	counts[1] = countR;
}

void getSpeeds(double speeds[]) {
	//instintanious speed. distance over time.
	speeds[0] = speedL;
	speeds[1] = speedR;

}

void riseAndFallL() {
	countL++;
	time = millis();

	if (dtL == -1) {
		speedL = 1000 * arc_per_hole / (time);
		dtL = time;
	}
	else {
//		Serial.print(time);
//		Serial.print(",");
//		Serial.println(dtL);
		speedL = 1000 * arc_per_hole / (time - dtL);
		dtL = time;
	}
}

void riseAndFallR() {
	countR++;
	time = millis();

	if (dtR == -1) {
		speedR = 1000 * arc_per_hole / (time);
		dtR = time;
	}
	else {
		speedR = 1000 * arc_per_hole / (time - dtR);
		dtR = time;
	}
}

void initEncoders() {
	Serial.begin(115200);

	pinMode(ENCODER_PORT_L, INPUT_PULLUP);
	pinMode(ENCODER_PORT_R, INPUT_PULLUP); // set port as input port

	PCintPort::attachInterrupt(ENCODER_PORT_L, &riseAndFallL, CHANGE);
	PCintPort::attachInterrupt(ENCODER_PORT_R, &riseAndFallR, CHANGE);
}
