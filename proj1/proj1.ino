#include <Adafruit_RGBLCDShield.h>
#include <EllipseParams.h>
#include <ForwardParams.h>
#include <MyEncoders.h>
#include <MyServos.h>
#include <SShapeParams.h>
#include <Wire.h>

const double pi = 3.1415926535897;
const float AXIS = 3.95;                         // Distance between wheels
const short int TICKS_PER_ROTATION = 64;
const float WHEEL_CIRCUM = 8.2;
const float TOPSPD = WHEEL_CIRCUM * 0.8;
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
    uint8_t buttons = lcd.readButtons();
    delay(50);






//  Left button means do FORWARD TRAVEL TEST
    if (buttons & BUTTON_LEFT){
      lcd.clear();
      lcd.setCursor(0,0);
  
      float desired_IPS = PARAM_X / PARAM_Y;
      float target_counts = (PARAM_X / WHEEL_CIRCUM) * TICKS_PER_ROTATION;    

      if (desired_IPS > TOPSPD){
        lcd.print("Impossible Speed");
        lcd.setCursor(0,1);
        lcd.print("Requested!");
        delay(5000);
      } else {
        lcd.print("Moving forward");
        lcd.setCursor(0,1);
        lcd.print("For: ");
        lcd.setCursor(5,1);
        lcd.print(PARAM_Y);
        lcd.setCursor(9,1);
        lcd.print("secs");
        delay(500);
        setSpeedsIPS(desired_IPS, desired_IPS);

        resetCounts();
        unsigned long int current_counts[2] = {0};
        lcd.setCursor(0,1);
        lcd.print("Remaining: ");
        while (current_counts[0] < target_counts or current_counts[1] < target_counts){
          getCounts(current_counts);
          lcd.setCursor(11,1);
          lcd.print(target_counts - current_counts[1]);
        }
        setSpeeds(0,0);
      }
    }









//  Right button means do DOUBLE SEMICIRCLE TEST
    if (buttons & BUTTON_RIGHT){
      float desired_left_speed_r1 = 0.0;
      float desired_right_speed_r1 = 0.0;
      float desired_left_speed_r2 = 0.0;
      float desired_right_speed_r2 = 0.0;
      float desired_total_velocity = 0.0;
      bool invalid_operation = false;


      // Find left/right wheel speeds for both semicircles

      // Velocity of bot:
      desired_total_velocity = (((PARAM_R1 * pi) + (PARAM_R2 * pi)) / PARA_Y);  // half circum of r1 + half circum of r2 over requested time
      Serial.println("");
      Serial.print("Total Distance: ");
      Serial.println((((PARAM_R1 * pi) + (PARAM_R2 * pi))));
      Serial.print("Distance on R1: ");
      Serial.println((((PARAM_R1 * pi))));
      Serial.print("Distance on R2: ");
      Serial.println((((PARAM_R2 * pi))));
      Serial.print("Velocity of Bot: ");
      Serial.println(desired_total_velocity);
      if (desired_total_velocity > TOPSPD){
        lcd.print("Impossible Speed");
        lcd.setCursor(0,1);
        lcd.print("Requested!");
        invalid_operation = true;
        delay(5000);
      }

      // Necessary velocity of left/right wheel to cover first circle:
      // Derived from differential drive equations
      // See Kinematics slide 18 
      desired_left_speed_r1 = desired_total_velocity * ((PARAM_R1 / AXIS) + 1) / (PARAM_R1 / AXIS);
      desired_right_speed_r1 = (2 * desired_total_velocity) - desired_left_speed_r1;
      if (desired_left_speed_r1 > TOPSPD or desired_right_speed_r1 > TOPSPD){
        lcd.print("Impossible Speed");
        lcd.setCursor(0,1);
        lcd.print("Requested!");
        invalid_operation = true;
        delay(5000);
      }

      // Necessary velocity of left/right wheel to cover second circle:
      // Derived from differential drive equations
      // See Kinematics slide 18           
      desired_left_speed_r2 = desired_total_velocity * ((PARAM_R2 / AXIS) + 1) / (PARAM_R2 / AXIS);
      desired_right_speed_r2 = (2 * desired_total_velocity) - desired_left_speed_r2;
      if (desired_left_speed_r2 > TOPSPD or desired_right_speed_r2 > TOPSPD){
        lcd.print("Impossible Speed");
        lcd.setCursor(0,1);
        lcd.print("Requested!");
        invalid_operation = true;
        delay(5000);
      }
      Serial.print(desired_left_speed_r1);
      Serial.print(", ");
      Serial.println(desired_right_speed_r1);
      Serial.print(desired_right_speed_r2);
      Serial.print(", ");
      Serial.println(desired_left_speed_r2);
/*
      // hacky way of trying to handle awful speed measurements at low speeds
      if (desired_left_speed_r1 < 3.80 and desired_right_speed_r1 > 4.40)
        desired_left_speed_r1 *= 1.1;
      if (desired_left_speed_r2 < 3.80 and desired_right_speed_r2 > 4.40)
        desired_left_speed_r2 *= 1.1;*/
      if (desired_right_speed_r1 < 3.80 and desired_left_speed_r1 > 4.40)
        desired_right_speed_r1 *= 1.1;/*
      if (desired_right_speed_r2 < 3.80 and desired_left_speed_r1 > 4.40)
        desired_right_speed_r2 *= 1.1;*/

      if (invalid_operation == false){
        setSpeedsIPS(desired_left_speed_r1, desired_right_speed_r1);
  
        unsigned long int current_counts[2] = {0};
        float remaining_counts = 30000;
        float target_counts = (PARAM_R1 * pi / WHEEL_CIRCUM) * TICKS_PER_ROTATION; 
        lcd.setCursor(0,0);
        lcd.print("Going clockwise");
        delay(500);
        lcd.setCursor(0,1);
        lcd.print("Remaining: ");
        resetCounts();
        while (remaining_counts > 1){
          getCounts(current_counts);
          lcd.setCursor(11,1);
          float avg_count = (current_counts[0] + current_counts[1]) / 2;
          remaining_counts = target_counts - avg_count;
          lcd.print(remaining_counts);
        }
        bool start_second = false;
        setSpeeds(0,0);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Waiting...");
  
  
  
        while (start_second == false){
          buttons = lcd.readButtons();
          if (buttons & BUTTON_RIGHT)
            start_second = true;
        }
        setSpeedsIPS(desired_right_speed_r2, desired_left_speed_r2);
  
        current_counts[2] = {0};
        remaining_counts = 30000;
        target_counts = (PARAM_R1 * pi / WHEEL_CIRCUM) * TICKS_PER_ROTATION; 
        lcd.setCursor(0,0);
        lcd.print("Going anticlckws");
        delay(500);
        lcd.setCursor(0,1);
        lcd.print("Remaining: ");
        resetCounts();
        while (remaining_counts > 1){
          getCounts(current_counts);
          lcd.setCursor(11,1);
          short int avg_count = (current_counts[0] + current_counts[1]) / 2;
          remaining_counts = target_counts - avg_count;
          lcd.print(remaining_counts);
        }
      }

      
    }
  }
}

