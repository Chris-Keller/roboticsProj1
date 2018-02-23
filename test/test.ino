#include <Adafruit_RGBLCDShield.h>

#include <EllipseParams.h>
#include <ForwardParams.h>
#include <MyEncoders.h>
#include <MyServos.h>
#include <MySharpSensor.h>
#include <SShapeParams.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();


float K_FRONT = .90;
float K = 0.35;
bool is_running = false;
uint8_t buttons;
int count;
float velocity = 6.00;
float temp = 0.00;
float FRONT_GOAL_DISTANCE = 5.00;
float SIDE_GOAL_DISTANCE = 5.00;
float AVOID_FRONT = -PI/2;
float LONG_RANGE = 12.00;
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
        lcd.print("Press button of");
        lcd.setCursor(0,1);
        lcd.print("wall to follow");
        //calibrate(&lcd);
      }
  } else {
    buttons = lcd.readButtons();



    
// Approach forward wall
//------------------------------------------------------------------------------------------------------------
    if (buttons & BUTTON_UP){
        unsigned int start_time = millis();
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Following Front");
      while (true){
        delay(30);
        time = millis();
if (time - start_time > 10000) break;
        takeNewMeasurement(SSFRONT);
        takeNewMeasurement(SLONG);
        float reading = shortToInches(getCombinedDistace(SSFRONT));
        if (reading < 0 or reading > 12)
          reading = LONG_RANGE;

        float corrected_speed = (reading - FRONT_GOAL_DISTANCE) * K_FRONT;
        if (corrected_speed > MAX_SPEED) 
          corrected_speed = MAX_SPEED;
        if (corrected_speed < -MAX_SPEED)
          corrected_speed = -MAX_SPEED;
        if (isnan(corrected_speed) or isinf(corrected_speed))
          lcd.print("PANIC");


        Serial.print((time - start_time));
        Serial.print(" ");
        Serial.println(reading);
        setSpeedsIPS(corrected_speed, corrected_speed);
        
        lcd.setCursor(0,1);
        lcd.print(reading);
      }
    } 


// Follow side Wall
//----------------------------------------------------------------------------------------------------------------------------    
    else if (buttons & BUTTON_LEFT or buttons & BUTTON_RIGHT){
      bool is_left;
      int front_repeat = 0;
      if (buttons & BUTTON_LEFT)
        is_left = true;
      else
        is_left = false;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Following Side");
      
      while (true){
        time = millis();
        delay(20);

        float corrected_speed = MAX_SPEED;
        float avoid_front = 0.0;
        float avoid_side = 0.0;
        float sidereading;
        float frontreading;

        // Get & handle frontal sensor reading
        takeNewMeasurement(SSFRONT);
        takeNewMeasurement(SLONG);
        frontreading = shortToInches(getCombinedDistace(SSFRONT));
        if (frontreading < 0) 
          frontreading = longToInches(getCombinedDistace(SLONG));
        if (frontreading < 0 or frontreading > 60)
          frontreading = LONG_RANGE;


        // Get & handle side sensor readings
        if (is_left){
          takeNewMeasurement(SLEFT);
          sidereading = shortToInches(getCombinedDistace(SLEFT));
        }
        else{
          takeNewMeasurement(SRIGHT);
          sidereading = shortToInches(getCombinedDistace(SRIGHT));
        }
        if (sidereading < 0 or sidereading > 12)
        // In this case, we lost track of wall we're following, so set our 'distance' to that wall to a large value
          sidereading = SIDE_GOAL_DISTANCE * 1.5;

        
        // As we approach front wall, gradually slow down
        corrected_speed = frontreading/2;
        if (corrected_speed > MAX_SPEED) 
          corrected_speed = MAX_SPEED;
        int prev_repeat = front_repeat;
        
        // When we're close to the wall, gradually curve away
        if (frontreading < FRONT_GOAL_DISTANCE * 1.5){
          front_repeat++;
          if (front_repeat >= 1){
            avoid_front = -(10 / frontreading);
          }
        } else {
          avoid_front = 1000.0;  // Large val that will be discarded in min();
          front_repeat = 0;
        }
        avoid_side = (sidereading - SIDE_GOAL_DISTANCE) * K;   // From side inches reading, get radians curve value
        if (!is_left)
          avoid_side *= 0.75;



        // If we're avoiding an obstacle in front, keep turning for a while even after the obstacle is no longer detected
        if (front_repeat == 0 and prev_repeat > 5){
          delay(250);
          front_repeat = 0;
        }
        
        float curve = min(avoid_side, avoid_front);
        if (!is_left)
          curve = -curve;
        setSpeedsvw(corrected_speed, curve);



        Serial.print("L/F Dist: ");
        Serial.print(sidereading);
        Serial.print(",");
        Serial.print(frontreading);
        Serial.print("\tSPD/Curve: ");
        Serial.print(corrected_speed);
        Serial.print(",");
        Serial.print(curve);
        Serial.print("\t");
        Serial.println(front_repeat);
        lcd.setCursor(0,1);
        lcd.print(avoid_side);

      }
  } 
  
  
  
  
//--------------------------------------------------------------------------------------------------------

    else if (buttons & BUTTON_DOWN){ 
      setSpeedsIPS(.450,.45);
      Serial.println("1");
      delay(5000);
      setSpeedsIPS(0.22,0.22);
      Serial.println("2");
      delay(5000);
      setSpeedsIPS(0,0);
    }
    
    


  }  
}
