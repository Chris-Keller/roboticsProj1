


#include "ColorSensor.h"
#include <Arduino.h>
#include <MyServos.h>
#include <MySharpSensor.h>

float FRONT_GOAL_DISTANCE = 5.00;
float K_FRONT = 0.90;
float LONG_RANGE = 50.00;
float MAX_SPEED = 6.00;
float pi = 3.14;

namespace{ //this is a nameless namespace

  Adafruit_RGBLCDShield lcd;

  int rawBlack[3] = {415,475,357};
  int rawWhite[3] = {27,32,28};
  int rawRed[3]   = {60,150,127};
  int rawBlue[3]  = {123,90,57};

  float rgbBlack[3] = {0,0,0};
  float rgbWhite[3] = {0,0,0};
  float rgbRed[3] = {0,0,0};
  float rgbBlue[3] = {0,0,0};

  int cRed = 0;
  int cBlue = 0;

}

void getRawValues(int values[3],int samples){

  int s1v = HIGH, s2v=HIGH;
  for(int i=0;i<3;i++){
    values[i]=0;
    switch(i){
      
      case 0:{//red
        digitalWrite(S2,LOW);
        digitalWrite(S3,LOW);
        break;
      }
      case 1:{//green
        digitalWrite(S2,HIGH);
        digitalWrite(S3,HIGH);
        break;
      }
      case 2:{//blue
        digitalWrite(S2,LOW);
        digitalWrite(S3,HIGH);
        break;
      }
    }

    for(int j=0;j<samples;j++)
      values[i]+=pulseIn(sensorOut,HIGH);
    values[i]/=samples;
    
    
  }
  
  
  
}


void initColorSensor(Adafruit_RGBLCDShield &_lcd){

  lcd = _lcd;
//  Serial.println(testVariable);

 //set pins S0-S3 to output and sensorOut to input
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  //Set frequency scaling to 100%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
}

//I will assume rgbs in the range 0-1
void raw2rgb(int raw[3],float rgb[3]){

  for(int i=0;i<3;i++)
    rgb[i] = ((float)(raw[i]-rawBlack[i]))/(rawWhite[i]-rawBlack[i]);
  
}

namespace{

  void waitMessage(const char* messageTop,const char* messageLow){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(messageTop);
    lcd.setCursor(0,1);
    lcd.print(messageLow);

    delay(100);
    while(lcd.readButtons()!= BUTTON_SELECT);
    
    
  }
  
}

void calibrateColorSensor(int hsi_vals_of_bwrb[2]){

  
  waitMessage("Calib black","Press Enter...");
  getRawValues(rawBlack,10);
  waitMessage("Calib white","Press Enter...");
  getRawValues(rawWhite,10);
  waitMessage("Calib red line","Press Enter...");
  getRawValues(rawRed,10);
  waitMessage("Calib blue line","Press Enter...");
  getRawValues(rawBlue,10);
  

  raw2rgb(rawBlack,rgbBlack);
  raw2rgb(rawWhite,rgbWhite);
  raw2rgb(rawRed  ,rgbRed);
  raw2rgb(rawBlue ,rgbBlue);

  cRed = rgb2hsi(rgbRed);
  cBlue = rgb2hsi(rgbBlue);


}

//assume rgb values in the range 0-1
float rgb2hsi(float rgb[3]){ 

// Commented out S and I, since we don't use those - no need to compute them
  float r = rgb[0];
  float g = rgb[1];
  float b = rgb[2];

  float h = 0;
//  float s = 0;
//  float i = 0;

//  i = (r+g+b)/(3);

  if(r == g and g ==b ){
    h = -500;
//    s = 0;
    }
  
  else{
//    s = 1 - 3 *min(r, min(g, b));
    h = acos( (0.5*( (r-g)+(r-b) ) ) / sqrt( (r-g)*(r-g) + (r-b)*(g-b) ) );
    if(b > g)
      h = 2*pi - h;
    if(0 < h and h < 2*pi);
    else h = 0;
  }

  h *= (180.0 / pi);
//  s *= 100;
//  i *= 255;

//  hsi[0] = h;
//  hsi[1] = s;
//  hsi[2] = i;

  return h;

}

namespace{
  unsigned long lastCheck = 0;
  unsigned long lastRead = 0;

  float distanceL1(float c1[3],float c2[3]){
    return abs(c1[0]-c2[0]) + abs(c1[1]-c2[1]) + abs(c1[2]-c2[2]);
  }

  float epsilon = 3*1.0/12;
}

bool testColorRecognition(){
/*
  setSpeedsIPS(5.75,5.75);
  
  uint8_t buttons = lcd.readButtons();

  if (  buttons & BUTTON_SELECT ){ 
    setSpeedsIPS(0,0);
    return true; //exit program
  }
  
  if(millis()-lastCheck > 1){
    
    lastCheck = millis();

    if(millis() - lastRead > 30){
      takeNewMeasurement(SSFRONT);
      takeNewMeasurement(SLONG);
      takeNewMeasurement(SLEFT);
      takeNewMeasurement(SRIGHT); 
    }

    float readingF = shortToInches(getCombinedDistace(SSFRONT));
    float readingL = shortToInches(getCombinedDistace(SLEFT));
    float readingR = shortToInches(getCombinedDistace(SRIGHT));

    lastRead = millis();
    
    if (readingF < 0 or readingF > 12)
      readingF = longToInches(getCombinedDistace(SLONG));

    if(readingF < 0)
      readingF = LONG_RANGE;

    if(readingL < 0 or readingL > 12)
      readingL = LONG_RANGE;

    if(readingR < 0 or readingR > 12)
      readingR = LONG_RANGE;

    int raw[3];
    getRawValues(raw,10);
    
    float rgb[3];
    raw2rgb(raw,rgb);

    float dRed = distanceL1(rgbRed,rgb);
    float dBlue = distanceL1(rgbBlue,rgb);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("L     F     R");
    lcd.setCursor(0,1);
    
    if(dRed < dBlue){
      if(dRed < epsilon){ 
        lcd.print(readingL);
        lcd.setCursor(6,1);
        lcd.print(readingF);
        lcd.setCursor(12,1);
        lcd.print(readingR);
        lcd.setBacklight(0x1);
        
        colorBuffer[tracker] = 1;
        if(tracker < 5)
          tracker++;
        else 
          tracker = 0;

        if(colorBuffer[tracker] != 1 && colorBuffer[(tracker+5)%6] == 1)
          cRed++;
          
        }
        
      else{ 
        lcd.print(readingL);
        lcd.setCursor(6,1);
        lcd.print(readingF);
        lcd.setCursor(12,1);
        lcd.print(readingR);
        lcd.setBacklight(0x7);
        
        colorBuffer[tracker] = 0;
        if(tracker < 5)
          tracker++;
        else 
          tracker = 0;
        }
        
    }
    
    else if(dBlue < epsilon){ 
        lcd.print(readingL);
        lcd.setCursor(6,1);
        lcd.print(readingF);
        lcd.setCursor(12,1);
        lcd.print(readingR);
        lcd.setBacklight(0x4);

        colorBuffer[tracker] = 2;
        if(tracker < 5)
          tracker++;
        else 
          tracker = 0;
          
        if(colorBuffer[tracker] != 2 && colorBuffer[(tracker+5)%6] == 2)
          cBlue++;
      }
      
    else{ 
        lcd.print(readingL);
        lcd.setCursor(6,1);
        lcd.print(readingF);
        lcd.setCursor(12,1);
        lcd.print(readingR); 
        lcd.setBacklight(0x7);

        colorBuffer[tracker] = 0;
        if(tracker < 5)
          tracker++;
        else 
          tracker = 0;
    }

    if(readingF < 5.00){
      while(1){
        
        setSpeedsIPS(0,0);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("cBlue  cRed");
        lcd.setCursor(0,1);
        lcd.print(cBlue);
        lcd.setCursor(7,1);
        lcd.print(cRed);
        
        if (  buttons & BUTTON_SELECT ){ 
            return true; //exit program
          }
          
        delay(100);
        }
      
    }
    
  }
*/
  return false;
  
}

bool viewMeasures(){
  uint8_t buttons = lcd.readButtons();

  if (  buttons & BUTTON_SELECT ) return true; //exit program

  if(millis()-lastCheck > 300){
    lastCheck = millis();

    int raw[3];
    getRawValues(raw,10);
    
    float rgb[3];
    raw2rgb(raw,rgb);

    lcd.setCursor(0,0);
    lcd.print(raw[0]);
    lcd.print(" ");
    lcd.print(raw[1]);
    lcd.print(" ");
    lcd.print(raw[2]);
    lcd.print("     ");
    
    lcd.setCursor(0,1);
    lcd.print(rgb[0]);
    lcd.print(" ");
    lcd.print(rgb[1]);
    lcd.print(" ");
    lcd.print(rgb[2]);
    lcd.print("     ");
    
    
  }

  return false;
}

int getColor(){
  int raw[3] = {0};
  float rgb[3] = {0};
  getRawValues(raw, 7);
  raw2rgb(raw, rgb);  
  
  return rgb2hsi(rgb);
}

bool is_red(int h, short COLOR_TOLERANCE){
  int c = cRed;

//return ((h >= 0 and h < 15) or h > 345);

  if(c > COLOR_TOLERANCE and c < 360-COLOR_TOLERANCE)
    return(h > c - COLOR_TOLERANCE and h < c + COLOR_TOLERANCE);
    
  else if(c < COLOR_TOLERANCE){
    int low = c + COLOR_TOLERANCE;
    int high = c + 360 - COLOR_TOLERANCE;
    return (h >= high or h <= low);
  } 
  
  else if (c > 360 - COLOR_TOLERANCE){
    int low = (c + COLOR_TOLERANCE) % 360;
    int high = c - COLOR_TOLERANCE;
    return (h >= high or h <= low);
  } 
  
  else
    lcd.print("PANIC");
   
  return false;
}


bool is_blue(int h, short COLOR_TOLERANCE){

  int c = cBlue;
  //return ( h < c + COLOR_TOLERANCE and h > c - COLOR_TOLERANCE);

  if(c > COLOR_TOLERANCE and c < 360-COLOR_TOLERANCE)
    return(h > c - COLOR_TOLERANCE * 1.5 and h < c + COLOR_TOLERANCE * 1.5);
    
  else if(c < COLOR_TOLERANCE){
    int low = c + COLOR_TOLERANCE * 1.5;
    int high = c + 360 - COLOR_TOLERANCE * 1.5;
    return (h > high or h < low);
  } 
  
  else if (c > 360 - COLOR_TOLERANCE){
    int low = (c + COLOR_TOLERANCE) % 360;
    int high = c - COLOR_TOLERANCE * 1.5;
    return (h >= high or h <= low);
  } 
  
  else
    lcd.print("PANIC");
  return false;
  
 
}