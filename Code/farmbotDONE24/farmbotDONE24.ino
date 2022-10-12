#include <RTClib.h> // library to run RTC
#include <LiquidCrystal_I2C.h>
#include <Wire.h>



const int pumpRelay = 3;   
const int Sensor= A0; // Soil Sensor input at Analog PIN A0
const int Sensor1 = A1; // Sensor at A1
const int light1 = 4;
const int light2 = 5;
int count = 0;
int value= 0;
int value1 = 0;
int average = 0;
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x3F, 16, 2);
void setup() {
   lcd.init();
   lcd.backlight();
   pinMode(pumpRelay, OUTPUT);
   pinMode(light1, OUTPUT);
   pinMode(light2, OUTPUT);
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
void pump(){   
       //Code for value
   lcd.clear();   
   value= analogRead(Sensor);
   lcd.print("Sensor 1: ");
   lcd.print(value);
   lcd.setCursor(0, 1);
   //Code for value1
   value1 = analogRead(Sensor1); 
   lcd.print("Sensor 2: ");
   lcd.print(value1);
   lcd.setCursor(0, 0);
   average = (value + value1) / 2; 
   
   if(average>380) //340 is moist and 400+ is dry
   {
      digitalWrite(pumpRelay, LOW);
      Serial.println("pump on!");
      delay(10);
   }
   else
   {
      digitalWrite(pumpRelay,HIGH);
   }

}  
   
  
void loop() { 
  DateTime now = rtc.now();
  if (now.hour() <= 8 | now.hour() > 15){
    digitalWrite(light1, HIGH);
    digitalWrite(light2, HIGH);
    Serial.println("off");
    }
  else if (now.hour() > 8 & now.hour()<=15 ){
    digitalWrite(light1, LOW);
    digitalWrite(light2, LOW);
    Serial.println("on");
  }
  delay(1000);
  pump();
}
