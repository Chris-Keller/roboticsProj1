
#include "MySharpSensor.h"
#include <Arduino.h>
#include <Adafruit_RGBLCDShield.h>


namespace{
	short const int BUFFER_SIZE = 8;	// Use an even number.  Odd numbers will need ceil() on divisions, probably
	float buffer0[BUFFER_SIZE] = {0};
	float buffer1[BUFFER_SIZE] = {0};
	float buffer2[BUFFER_SIZE + 1] = {0};	// This sensor returns much worse values, this one will use the median of BUFFERSIZE + 1
	float buffer3[BUFFER_SIZE] = {0};
	int count0 = 0;
	int count1 = 0;
	int count2 = 0;
	int count3 = 0;
  
}



float shortToInches(int value){

	float inchess = ((float)value * (5.0/1023));
	inchess = (inchess-0.4) * ((0.25-0.0334)/(2.65-0.4)) + 0.0334;
	inchess = (1/inchess);
	inchess = (inchess * 0.39);

	return inchess;

}


float longToInches(int value){

	float inchesl = ((float)value * (5.0/1023));
	inchesl = (inchesl-0.4) * ((0.05-0.0066)/(2.45-0.4)) + 0.00667;
	inchesl = (1/inchesl);
	inchesl = (inchesl * 0.393);

	return inchesl;

}



void takeNewMeasurement(int sensor){

  //Serial.println("attempting to fill slot");
  
  if(sensor == 0){
	buffer0[count0] = analogRead(A0);
	if(count0 < BUFFER_SIZE - 1)
		count0++;
	else
		count0 = 0;
  }
  else if(sensor == 1){
  	buffer1[count1] = analogRead(A1);
  	if(count1 < BUFFER_SIZE - 1)
		count1++;
	else
		count1 = 0;
  }
  else if(sensor == 2){
	// THIS SENSOR HAS BEEN OBSERVED TO RETURN LOW-OUTLIERS
	buffer2[count2] = analogRead(A2);
  	if(count2 < BUFFER_SIZE)
		count2++;
	else
		count2 = 0;
  }
  else if(sensor == 3){
	buffer3[count3] = analogRead(A3);
    	if(count3 < BUFFER_SIZE - 1)
			count3++;
		else
			count3 = 0;
  }
  /*
  for (int i = 0; i < 7; i++){
	Serial.print(shortToInches(buffer0[i]));
	Serial.print(" ");
	}
  Serial.println("");*/
}


// Our implementation takes BUFFER_SIZE readings, discards the half with highest values, and returns average of remaining.
// NOTE: The RIGHT short sensor uses 2 extra readings due to observed increased volatility
float getCombinedDistace(int sensor){
  //Serial.print("taking averages");
  
  float total = 0;


  if(sensor == 0){
	sort(buffer0);
	for(int i = 0; i<BUFFER_SIZE / 2; i++){
		total += buffer0[i];
	}
  }
  else if(sensor == 1){
  	sort(buffer1);
  	for(int i = 0; i<BUFFER_SIZE / 2; i++){
		total += buffer1[i];
	}
  }
  else if(sensor == 2){
	  // THIS SENSOR HAS BEEN OBSERVED TO RETURN LOW-OUTLIERS
	  // Discard top/bottom value, return average of remaining
  	sort(buffer2);
	return (buffer2[3]);
  }
  else if(sensor == 3){
  	sort(buffer3);
  	for(int i = 0; i<BUFFER_SIZE / 2; i++){
		total += buffer3[i];
	}
  }
  /*
    for (int i = 0; i < 7; i++){
		Serial.print(buffer0[i]);
		Serial.print(" ");
	}
	Serial.print("\t");
	Serial.print(total/3);
    Serial.println("");
	*/
  return (total / (BUFFER_SIZE /2));
}


void initDistanceSensors(){

	float temp0 = analogRead(A0);
	float temp1 = analogRead(A1);
	float temp2 = analogRead(A2);
	float temp3 = analogRead(A3);
	
	for(int i = 0; i < BUFFER_SIZE; i++){
		buffer0[i] = temp0;
		buffer1[i] = temp1;
		buffer2[i] = temp2;
		buffer3[i] = temp3;
	}
  
}

void sort(float * array){

	float temp = 0;
	bool sorted = false;
	int hit = 0;

	while(!sorted){
		hit = 0;
		for(int i=0; i < BUFFER_SIZE - 1; i++){
			if(array[i] > array [i+1]){
				temp = array[i];
				array[i] = array[i+1];
				array[i+1] = temp;
				hit++;
			}
		}
		if(hit == 0)
		sorted = true;

	}
}