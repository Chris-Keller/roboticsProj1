#include <Adafruit_RGBLCDShield.h>
#include "ColorSensor.h"
#include <MySharpSensor.h>
#include <MyServos.h>


#define RED 0
#define BLUE 1
#define NEITHER 2
#define COLOR_TOLERANCE 20
#define BUFFER_SIZE 5
#define ADDITIONAL_THRESHOLD 1
#define PRINT_DELAY 250

#define LONG_RANGE 50
#define SHORT_RANGE 12


Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
int calibrated_color_hues[2];

void setup() {

  lcd.begin(16, 2);
  initColorSensor(lcd);
  setupServo();
  Serial.begin(9600);
  setSpeedsIPS(0, 0);
  calibrateColorSensor(calibrated_color_hues);
}

void loop() {

  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Starting");
  delay(1000);
  lcd.print(".");
  delay(1000);
  lcd.print(".");
  delay(1000);
  lcd.print(".");
  delay(1000);
  lcd.print(".");
  setSpeedsIPS(5.40, 5.40); // This is equal to 0.7 RPM, the min allowable speed
  lcd.clear();
  
  unsigned long lastRead = 0;  
  unsigned long last_print = 0;
  
  short red_line_count = 0;
  short blue_line_count = 0;
  short current_line = NEITHER;
  short i = 0;
  short color_buffer[BUFFER_SIZE];
  short current_color;
  for (int j = 0; j < BUFFER_SIZE; j++)
    color_buffer[j] = NEITHER;
  lcd.print("L    F    R");
  
  while (1) {
    float current_color = getColor();

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
    if (color_count[NEITHER] == BUFFER_SIZE - ADDITIONAL_THRESHOLD) {
      lcd.setBacklight(0x7);
      if (current_line != NEITHER)
        current_line = NEITHER;
    } else if (color_count[BLUE] == BUFFER_SIZE - ADDITIONAL_THRESHOLD) {  // Lighter tolerance since blue seems harder to detect
      lcd.setBacklight(0x4);
      if (current_line != BLUE) {
        current_line = BLUE;
        blue_line_count++;
      }
    } else if (color_count[RED] == BUFFER_SIZE - ADDITIONAL_THRESHOLD) {
      lcd.setBacklight(0x1);
      if (current_line != RED) {
        current_line = RED;
        red_line_count++;
      }
    }
    Serial.println(millis());





    //take sensor measurements
    if (millis() - lastRead > 30) {
      lastRead = millis();
      lcd.setCursor(0, 0);
      takeNewMeasurement(SSFRONT);
      takeNewMeasurement(SLEFT);
      takeNewMeasurement(SRIGHT);

      takeNewMeasurement(SLONG);

      float readingF = shortToInches(getCombinedDistace(SSFRONT));
      float readingL = shortToInches(getCombinedDistace(SLEFT));
      float readingR = shortToInches(getCombinedDistace(SRIGHT));

      // Handle sensor values that are out of range
      if (readingF < 0 or readingF > 12)
        readingF = longToInches(getCombinedDistace(SLONG));
      if (readingF < 0) {
        readingF = LONG_RANGE;
      }
      if (readingL < 0 or readingL > 12)
        readingL = SHORT_RANGE;
      if (readingR < 0 or readingR > 12)
        readingR = SHORT_RANGE;

      // Terminate loop when we're close to a wall
      if (readingF < 5.00) {
        setSpeedsIPS(0, 0);
        break;
      }
      /*
              Serial.println("L     F     R");
              Serial.print(readingL);
              Serial.print(readingF);
              Serial.println(readingR);*/


      // 30ms trigger on this code causes the main loop to run once per 200ms(!!) rather than once per 35ms
      //if (millis() - last_print > PRINT_DELAY){ // Uncomment this and next line for lcd printing   
      //  last_print = millis(); 
      /*    
        lcd.setCursor(0,1);
        lcd.print(readingL);
        lcd.setCursor(6,1);
        lcd.print(readingF);
        lcd.setCursor(12,1);
        lcd.print(readingR);
        */
    

        // Alternative print method
        /* 
        char temp1[3] = {};
        char temp2[3] = {};
        char temp3[3] = {};
        char finalstring[16] = {};
        char spaces[4] = {' ', ' '};
        itoa((int)readingL, temp1, 10);
        itoa((int)readingF, temp2, 10);
        itoa((int)readingR, temp3, 10);
        strcat(finalstring, temp1);
        strcat(finalstring, spaces);
        strcat(finalstring, temp2);
        strcat(finalstring, spaces);
        strcat(finalstring, temp3);
        strcat(finalstring, spaces);
        lcd.setCursor(0,1);
        lcd.print(finalstring);
        */
      //}   // Uncomment this for lcd printing
    }
  }
  
    // Program ended, display results
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Red Lines: ");
    lcd.print(red_line_count);
    lcd.setCursor(0, 1);
    lcd.print("Blue Lines: ");
    lcd.print(blue_line_count);
    Serial.print("Red/Blue Lines: ");
    Serial.print(red_line_count);
    Serial.println(blue_line_count);

    while (true) {
      uint8_t buttons = lcd.readButtons();
      delay(100);
      if (buttons & BUTTON_SELECT ) {
        break;
      }
    }
}
