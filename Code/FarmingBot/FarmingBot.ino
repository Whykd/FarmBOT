#include <RTClib.h>// library to run RTC
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//Farmbot code V2.0
//ALL PINS ARE ON PCB DESIGN

#define pump 3  //pump relay pin 
#define lights 4   //light relay pin

const int threshold = 700; //threshold to change when pump is on or off


RTC_DS1307 rtc; //init RTC obj
LiquidCrystal_I2C lcd(0x3F, 16, 2); //init LCD obj

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(pump, OUTPUT);
  pinMode(lights, OUTPUT);
  Serial.begin(57600);
  digitalWrite(pump, HIGH);
  digitalWrite(lights, HIGH);
rtc.begin();
//if (! rtc.begin()) {
// Serial.println("Couldn't find RTC");
//  while (1) delay(10);
//}.
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //sets rtc to time when code is compiled
}




boolean isntWet(){
  int sens1 = analogRead(A0);
  Serial.println(sens1);
  int sens2 = analogRead(A1);
  Serial.println(sens2);
  float avg = (sens1+sens2)/2;
  Serial.println(avg);
  lcd.print("AVG: "); 
  lcd.print(avg);            
         
  
  return !(avg > threshold); //if avg is greater than threshold, return true
}

boolean isNight(){
  int hour = rtc.now().hour();
  if (hour > 19 || hour < 6){ //turns lights on from 7PM to 6AM
   return true;
  }
  return false;
}


void loop() {

  bool isNotWet = isntWet();
  bool night = isNight();
  if (isNotWet){
    digitalWrite(pump, LOW); //turns pump on
  }
  else{
    digitalWrite(pump, HIGH); //turns pump off
  }
  if (night){
    digitalWrite(lights, LOW); //turns lights on
  }
  else{
    digitalWrite(lights, HIGH); //turns lights off
  }
  delay(1000); //wait 10 seconds
  lcd.clear();
  Serial.println("i made it here");
}
