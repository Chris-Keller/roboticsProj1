#include <EllipseParams.h>
#include <ForwardParams.h>
#include <MyEncoders.h>
#include <MyServos.h>
#include <SShapeParams.h>
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>

float testsp[2] = {0};
bool is_running = false;
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup() {
  initEncoders();
  setupServo();
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  lcd.setCursor(4,0);
}

void loop() {      // Main loop auto-repeats

  time = millis();

  if (!is_running){
   uint8_t buttons = lcd.readButtons();
    if (buttons & BUTTON_SELECT){
      is_running = true;
    }
  }
  
  if (is_running){
    setSpeeds(10,10);    // Set velocity to left and right Servo 

  if (time % 50 == 0){
  Serial.print("Time: ");
  Serial.print(time);
  Serial.print("\n");
  Serial.print("Speed: l/r ");
  getSpeeds(testsp);
  Serial.print(testsp[0]);
  Serial.print(" ");
  Serial.print(testsp[1]);
  Serial.print("\n");
  }
 
  } 


}

