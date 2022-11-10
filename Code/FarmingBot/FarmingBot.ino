#include <RTClib.h> // library to run RTC
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//Farmbot code V2.0
//ALL PINS ARE ON PCB DESIGN

const int pump = 3;  //pump relay pin 
const int sensor1 = A0; // Soil Sensor input at Analog PIN A0 and A1
const int sensor2 = A1;
const int lights = 4;   //light relay pin

const int threshold = 500; //threshold to change when pump is on or off


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

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

if (! rtc.begin()) {
  Serial.println("Couldn't find RTC");
  Serial.flush();
  while (1) delay(10);
}
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //sets rtc to time when code is compiled
}




boolean isntWet(){
   int sens1 = analogRead(sensor1);
   int sens2 = analogRead(sensor2);
   int avg = (sens1+sens2)/2;
   lcd.print("Sensor 1: " + sens1);
   lcd.setCursor(0, 1);              //moves cursor to next line
   lcd.print("Sensor 2: " + sens2);
   lcd.setCursor(0, 0);              //moves cursor back up TO FIRST LINE
   
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
  DateTime now = rtc.now(); //current time on RTC
  if (isntWet()){
    digitalWrite(pump, LOW); //turns pump on
  }
  else{
    digitalWrite(pump, HIGH); //turns pump off
  }
  if (isNight()){
    digitalWrite(lights, LOW); //turns lights on
  }
  else{
    digitalWrite(lights, HIGH); //turns lights off
  }
  delay(10000); //wait 10 seconds
  lcd.clear();
}
