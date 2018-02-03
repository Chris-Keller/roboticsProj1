#include <Adafruit_RGBLCDShield.h>
#include <EllipseParams.h>
#include <ForwardParams.h>
#include <MyEncoders.h>
#include <MyServos.h>
#include <SShapeParams.h>
#include <Wire.h>

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
      calibrate(&lcd);
    }
  }
  
  if (is_running){
    setSpeeds(0,0);    // Set velocity to left and right Servo 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Ready!");
        delay(3000);

  lcd.clear();
  lcd.print("Speed 3.00");
  setSpeedsIPS(3.00,3.00);
  delay(50000);
  }


}

