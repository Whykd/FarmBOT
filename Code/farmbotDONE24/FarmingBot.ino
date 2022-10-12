#include <RTClib.h> // library to run RTC
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//Farmbot code V2.0
//ALL PINS ARE ON PCB DESIGN

const int pumpRelay = 3;  //pump relay pin 
const int sensor = A0; // Soil Sensor input at Analog PIN A0 and A1
const int sensor1 = A1;
const int lights = 4;   //light relay pin
const int threshold = 0;

RTC_DS1307 rtc; //init RTC obj
LiquidCrystal_I2C lcd(0x3F, 16, 2); //init LCD obj
void setup() {
   lcd.init();
   lcd.backlight();
   pinMode(pumpRelay, OUTPUT);
   pinMode(lights, OUTPUT);
   Serial.begin(57600);

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
   
}
boolean getSensVal(){
   int sens1 = analogRead(sensor);
   int sens2 = analogRead(sensor1);
   int avg = (sens1+sens2)/2;
   
   return avg > threshold; //change to reflect proper values
  
}


void loop() { 
  DateTime now = rtc.now();
  if (getSensVal()){
    digitalWrite(pump, LOW);
  }
}
