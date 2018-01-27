#include <EllipseParams.h>
#include <ForwardParams.h>
#include <MyEncoders.h>
#include <MyServos.h>
#include <SShapeParams.h>

#include <Wire.h>

#include "MyServos.h"
#include <Adafruit_RGBLCDShield.h>



bool is_running = false;
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup() {
  setupServo();
  
  lcd.begin(16, 2);
  lcd.setCursor(4,0);
}

void loop() {      // Main loop auto-repeats

  if (!is_running){
   uint8_t buttons = lcd.readButtons();
    if (buttons & BUTTON_SELECT){
      is_running = true;
    }
  }
  
  if (is_running){
    setSpeeds(200,200);    // Set velocity to left and right Servo
    delay(100);                        
 
  } 
}

