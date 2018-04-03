#include "ColorSensor.h"
#include <MyEncoders.h>
#include <MyServos.h>
#include <MySharpSensor.h>
#include <Adafruit_RGBLCDShield.h>

#define K_FRONT 0.90
#define K 0.35
#define FRONT_GOAL_DISTANCE 5.0
#define SIDE_GOAL_DISTANCE 5.50
#define MAX_SPEED 6.00
#define RED 0
#define BLUE 1
#define NEITHER 2
#define COLOR_TOLERANCE 20
#define BUFFER_SIZE 4
#define LONG_RANGE 50
#define SHORT_RANGE 12

bool is_running = false;
uint8_t buttons;
int count;
float velocity = 6.00;
float temp = 0.00;
unsigned long lastRead = 0;
short red_line_count = 0;
short blue_line_count = 0;
short current_line = NEITHER;
short i = 0;
short color_buffer[BUFFER_SIZE];
int calibrated_color_hues[2];
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup() {
  initDistanceSensors();
  initEncoders();
  setupServo();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  Serial.begin(9600);
  setSpeedsIPS(0.0, 0.0);
  initColorSensor(lcd);
  calibrateColorSensor(calibrated_color_hues);
}

void loop() {
  if (!is_running) {
    buttons = lcd.readButtons();
    if (buttons & BUTTON_SELECT) {
      is_running = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Press select");
      lcd.setCursor(0, 1);
      lcd.print("to begin.");
      //calibrate(&lcd);
    }
  } else
    buttons = lcd.readButtons();

  // Follow side Wall
  //----------------------------------------------------------------------------------------------------------------------------
  if (buttons & BUTTON_LEFT or buttons & BUTTON_RIGHT) {
    float current_color = 0;
    bool is_left;
    int front_repeat = 0;
    if (buttons & BUTTON_LEFT)
      is_left = true;
    else
      is_left = false;
    lcd.clear();
lcd.print("L");
    //lcd.setCursor(0, 0);
    // lcd.print("Following Side");

    unsigned long lastCo = 0;

    while (true) {



Serial.println(millis());
      current_color = getColor();

      // Update Circular Buffer with current color
      if (is_red(current_color, COLOR_TOLERANCE))
        color_buffer[i] = RED;
      else if (is_blue(current_color, COLOR_TOLERANCE))
        color_buffer[i] = BLUE;
      else
        color_buffer[i] = NEITHER;

      i++;
      if (i == BUFFER_SIZE)
        i = 0;


      // Count color occurances in buffer
      int color_count[3] = {0};
      for (int j = 0; j < BUFFER_SIZE; j++) {
        if (color_buffer[j] == RED)
          color_count[RED]++;
        else if (color_buffer[j] == BLUE)
          color_count[BLUE]++;
        else
          color_count[NEITHER]++;
      }


      // Find majority color, increment that line unless we're currently on a line
      if (color_count[NEITHER] == BUFFER_SIZE - 1) {
        lcd.setBacklight(0x7);
        if (current_line != NEITHER) {
          current_line = NEITHER;
          // On leaving a line change, 1 in 5 chance to change from left to right wallfollow, or 1 in 2 chance to switch from right to left
          int temp = random(0,6);
          if (is_left and temp == 0){
            is_left = false;
            lcd.clear();
            lcd.print("R");
          } else if (!is_left and temp <= 3){
            is_left = true;
            lcd.clear();
            lcd.print("L");
          }
        }
      } else if (color_count[BLUE] == BUFFER_SIZE - 1) {  // Lighter tolerance since blue seems harder to detect
        lcd.setBacklight(0x4);
        if (current_line != BLUE) {
          current_line = BLUE;
          blue_line_count++;
        }
      } else if (color_count[RED] == BUFFER_SIZE - 1) {
        lcd.setBacklight(0x1);
        if (current_line != RED) {
          current_line = RED;
          red_line_count++;
        }
      }







      if (millis() - lastCo > 30) {

        lastCo = millis();

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
        if (is_left) {
          takeNewMeasurement(SLEFT);
          sidereading = shortToInches(getCombinedDistace(SLEFT));
        }
        else {
          takeNewMeasurement(SRIGHT);
          sidereading = shortToInches(getCombinedDistace(SRIGHT));
        }
        if (sidereading < 0 or sidereading > SIDE_GOAL_DISTANCE * 1.5)
          // In this case, we lost track of wall we're following, so set our 'distance' to that wall to a large value
          sidereading = SIDE_GOAL_DISTANCE * 1.5;


        // As we approach front wall, gradually slow down
        corrected_speed = frontreading / 2;
        if (corrected_speed > MAX_SPEED)
          corrected_speed = MAX_SPEED;
        int prev_repeat = front_repeat;

        // When we're close to the wall, gradually curve away
        if (frontreading < FRONT_GOAL_DISTANCE * 1.5) {
          front_repeat++;
          if (front_repeat >= 3) {
            avoid_front = -(10 / frontreading);
            if (!is_left)
              avoid_front *= -1;
          }
        } else {
          avoid_front = 1000.0;  // Large val that will be discarded in min();
          if (!is_left) avoid_front = -1000;
          front_repeat = 0;
        }
        avoid_side = (sidereading - SIDE_GOAL_DISTANCE) * K;   // From side inches reading, get radians curve value
        if (!is_left)
          avoid_side *= -1;


        // If we're avoiding an obstacle in front, keep turning for a while even after the obstacle is no longer detected
        // This should hopefully turn us enough that we don't drive into corners where our blind-spot is
        if (front_repeat == 0 and prev_repeat > 5) {
          delay(250);
          front_repeat = 0;
        }

        float curve = min(avoid_side, avoid_front);
        if (!is_left) {
          curve = max(avoid_side, avoid_front);
        }
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
        //lcd.setCursor(0, 1);
        //lcd.print(avoid_side);
      }
    }
  }
}
