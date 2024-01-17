#include <RTClib.h>            //Adafruit lib
#include <LiquidCrystal_I2C.h> //By Frank de Brabander
#include <Wire.h>

// Farmbot code V1.0.0
// ALL PINS ARE ON PCB DESIGN

// lights on outlet 2
// pump outlet 3
#define pump 4        // pump relay pin
#define lights 3      // light relay pin

RTC_DS1307 rtc;                     // init RTC obj
LiquidCrystal_I2C lcd(0x3F, 16, 2); // init LCD obj
//pump on/off hour, pump on min, pump off min, light on hour, light off hour 
uint8_t timings[] = {9, 35, 37, 7, 19};  // define timing array

void setup()
{
  lcd.init();
  lcd.backlight();
  pinMode(pump, OUTPUT);
  pinMode(lights, OUTPUT);
  Serial.begin(57600);
  digitalWrite(pump, HIGH);
  digitalWrite(lights, HIGH);
  rtc.begin();
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // sets rtc to time when code is compiled
}

boolean isNight()
{
  int hour = rtc.now().hour();
  if (hour >= timings[3] && hour <= timings[4])
  { // turns lights on from start hour to end hour
    return true;
  }
  return false;
}


void loop()
{
  bool night = isNight();
  if (night){
    digitalWrite(lights, LOW); // turns lights on
  }
  else{
    digitalWrite(lights, HIGH); // turns lights off
  }
  DateTime time = rtc.now();
  //runs pump for 2 minutes every other day.
  if (time.hour() == timings[0] && time.minute() >= timings[1] && time.minute() <= timings[2] && time.day()%2 == 0){
    digitalWrite(pump, LOW);
  }
  else{
    digitalWrite(pump, HIGH);
  }
  lcd.setCursor(0, 1);
  lcd.print(rtc.now().hour());
  lcd.print(":");
  if(rtc.now().minute()<10){
    lcd.print("0");
    lcd.print(rtc.now().minute());
  }
  else{
    lcd.print(rtc.now().minute());
  }
  delay(1000);
  lcd.clear();
}
