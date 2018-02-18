#include <Adafruit_RGBLCDShield.h>

#include <EllipseParams.h>
#include <ForwardParams.h>
#include <MyEncoders.h>
#include <MyServos.h>
#include <MySharpSensor.h>
#include <SShapeParams.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup() {
  // put your setup code here, to run once:
 initDistanceSensors();
 initEncoders(); 
 setupServo();
 lcd.begin(16,2);
 Serial.begin(9600);  
 time = 0;
 calibrate(&lcd);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(35);
  time = micros();
  takeNewMeasurement(SSFRONT);
  float reading = shortToInches(getCombinedDistace(SSFRONT)) - 5.00;
  reading *= .4;
  if (reading > 6.00) 
    reading = 6.00;
  if (reading < -5.50)
    reading = -5.50;
  Serial.print("Target: ");
  Serial.println(reading);
  setSpeedsIPS(reading, reading);

}
