#include <RTClib.h>//Adafruit lib
#include <LiquidCrystal_I2C.h> //By Marco Shwartz
#include <Wire.h>

//Farmbot code V2.0
//ALL PINS ARE ON PCB DESIGN

#define pump 4  //pump relay pin 
#define lights 3   //light relay pin
//ights on outlet 2
//pump outlet 3

const int threshold = 490; //threshold to change when pump is on or off


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
         
  
  return (avg > threshold); //if avg is greater than threshold, return true
}

boolean isNight(){
  int hour = rtc.now().hour();
  if (hour >= 7 && hour <= 19){ //turns lights on from 7AM to 7PM
   Serial.println("day");
   return true;
   
  }
  return false;
}


void loop() {
  bool night = isNight();
  if (night){
    digitalWrite(lights, LOW); //turns lights on
  }
  else{
    digitalWrite(lights, HIGH); //turns lights off
  }
  DateTime time = rtc.now();
  if(time.hour() == 8 && time.minute() >= 45 && time.minute() <= 47){
    digitalWrite(pump, LOW); 
  }
  else {
    digitalWrite(pump, HIGH);
  }
  int sens1 = analogRead(A0);
  int sens2 = analogRead(A1);
  float avg = (sens1+sens2)/2;
  lcd.print("AVG: "); 
  lcd.print(avg); 
  delay(1000); 
  lcd.clear();
}
