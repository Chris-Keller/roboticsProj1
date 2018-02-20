#include <Adafruit_RGBLCDShield.h>

#include <EllipseParams.h>
#include <ForwardParams.h>
#include <MyEncoders.h>
#include <MyServos.h>
#include <MySharpSensor.h>
#include <SShapeParams.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

bool is_running = false;
bool is_front_blocked = false;
bool is_front_blocked_prev = false;
uint8_t buttons;
int count;
float velocity = 6.00;
float K_FRONT = 1.5;
float K = 0.30;
float temp = 0.00;
float FRONT_GOAL_DISTANCE = 5.00;
float SIDE_GOAL_DISTANCE = 5.00;
float AVOID_FRONT = -1.00;
float LONG_RANGE = 20.00;
float MAX_SPEED = 6.00;

void setup() {
  // put your setup code here, to run once:
 initDistanceSensors();
 initEncoders(); 
 setupServo();
 lcd.begin(16,2);
 lcd.setCursor(0,0);
 lcd.print("Press select");
 lcd.setCursor(0,1);
 lcd.print("to initialize");
 Serial.begin(9600);  
 Serial.println("Ready");
 time = 0;
 setSpeedsIPS(0.0,0.0);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(20);

  if (!is_running){
      buttons = lcd.readButtons();
      if (buttons & BUTTON_SELECT){
        is_running = true;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Looking for Wall");
        //calibrate(&lcd);
      }
  } else {
    buttons = lcd.readButtons();



    
// Approach forward wall
//------------------------------------------------------------------------------------------------------------
    if (buttons & BUTTON_UP){
      while (true){
        time = millis();
        takeNewMeasurement(SSFRONT);
        takeNewMeasurement(SLONG);
        float reading = shortToInches(getCombinedDistace(SSFRONT));
        if (reading < 0)
          reading = longToInches(getCombinedDistace(SLONG));
        if (reading < 0)
          reading = LONG_RANGE;

        float corrected_speed = (reading - FRONT_GOAL_DISTANCE) * K_FRONT;
        if (corrected_speed > MAX_SPEED) 
          corrected_speed = MAX_SPEED;
        if (corrected_speed < -MAX_SPEED)
          corrected_speed = -MAX_SPEED;
        if (isnan(corrected_speed) or isinf(corrected_speed))
          lcd.print("PANIC");
        setSpeedsIPS(corrected_speed, corrected_speed);
        
        lcd.setCursor(0,1);
        lcd.print(reading);
      }
    } 


// Follow Left Wall
//----------------------------------------------------------------------------------------------------------------------------    
    else if (buttons & BUTTON_LEFT){
      while (true){
        time = millis();
        
        takeNewMeasurement(SLEFT);
        takeNewMeasurement(SSFRONT);
        takeNewMeasurement(SLONG);
        float leftreading = shortToInches(getCombinedDistace(SLEFT));
        float frontreading = shortToInches(getCombinedDistace(SSFRONT));
        float corrected_speed = MAX_SPEED;
        float avoid_front = 0.0;
        float avoid_left = 0.0;

        // Same as Task 2, go fast toward front wall until 5in mark
        if (frontreading < 0) // sensors often returns crazy negatives if out of range
          frontreading = longToInches(getCombinedDistace(SLONG));
        if (frontreading < 0 or frontreading > 60)
          frontreading = LONG_RANGE;
        if (leftreading < 0 or leftreading > 12)
          // In this case, we lost track of wall we're following, so set our 'distance' to that wall to a large value
          leftreading = SIDE_GOAL_DISTANCE * 2.0;
          
        corrected_speed = frontreading;
        if (corrected_speed > MAX_SPEED) 
          corrected_speed = MAX_SPEED;
        if (corrected_speed < -MAX_SPEED)
          corrected_speed = -MAX_SPEED;

        // When we're close to the wall, curve away
        if (frontreading < FRONT_GOAL_DISTANCE * 1.5){
          avoid_front = AVOID_FRONT;
          corrected_speed = 0.00;
          is_front_blocked = true;
        } else {
          avoid_front = 1000.0;  // Large val that will be discarded in min();
          is_front_blocked = false;
        }
        avoid_left = (leftreading - SIDE_GOAL_DISTANCE) * K;   // From inches reading, get radians curve value



        // If we're avoiding an obstacle in front, keep turning for a while even after the obstacle is no longer detected
        if (!is_front_blocked and is_front_blocked_prev)
          delay(250);
        
        float curve = min(avoid_left, avoid_front);
        setSpeedsvw(corrected_speed, curve);


/*
        Serial.print("L/F Dist: ");
        Serial.print(leftreading);
        Serial.print(",");
        Serial.print(frontreading);
        Serial.print("\tSPD/Curve: ");
        Serial.print(corrected_speed);
        Serial.print(",");
        Serial.println(curve);*/
        lcd.setCursor(0,1);
        lcd.print(avoid_left);
        is_front_blocked_prev = is_front_blocked;

      }
  } else if (buttons & BUTTON_RIGHT){
    // Follow right wall
    while (true){
      time = millis();
      takeNewMeasurement(SRIGHT);
      float reading = shortToInches(getCombinedDistace(SRIGHT)) - 5.00;
      reading *= 0.15;
      if (reading > .50) 
        reading = .50;
      if (reading < -.50)
        reading = -.50;
      setSpeedsvw(6.0, reading);
      lcd.setCursor(0,1);
      lcd.print(reading);
    }
  }






  }
}
