

#ifndef __COLOR_SENSOR__
#define __COLOR_SENSOR__

#include <Adafruit_RGBLCDShield.h>

// COLOR SENSOR
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8


//int testGlobalVariable = 4;

//initialize color sensor
void initColorSensor(Adafruit_RGBLCDShield &_lcd);

//gets raw measurements and return average
void getRawValues(int values[3],int sample);

//calibrate fuction
void calibrateColorSensor(int calibrated_color_hues[4]);

//I will assume rgbs in the range 0-1
void raw2rgb(int raw[3], float rgb[3]);

float rgb2hsi(float rgb[3]);

//sample programs
bool testColorRecognition();

bool viewMeasures();

// Our additions
int getColor();
bool is_red(int h, short COLOR_TOLERANCE);
bool is_blue(int h, short COLOR_TOLERANCE);
#endif
