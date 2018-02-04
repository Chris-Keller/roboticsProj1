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
  lcd.print("Speed .30 RPS");
  setSpeedsRPS(.30,.30);
  delay(20000);

    lcd.clear();
  lcd.print("Speed .60 RPS");
  setSpeedsRPS(.60,.60);
  delay(20000);
  }


}

