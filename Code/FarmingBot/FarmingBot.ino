#include <RTClib.h>            //Adafruit lib
#include <LiquidCrystal_I2C.h> //By Marco Shwartz
#include <Wire.h>

// Farmbot code V2.0
// ALL PINS ARE ON PCB DESIGN

// define timing array
// lights on outlet 2
// pump outlet 3
#define pump 4        // pump relay pin
#define lights 3      // light relay pin
#define threshold 490 // threshold to change when pump is on or off


RTC_DS1307 rtc;                     // init RTC obj
LiquidCrystal_I2C lcd(0x3F, 16, 2); // init LCD obj
int timings[] = {8, 45, 47, 7, 19};
int sens1 = 0;
int sens2 = 0;
boolean timeout = true;

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
  // if (! rtc.begin()) {
  //  Serial.println("Couldn't find RTC");
  //   while (1) delay(10);
  // }.
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // sets rtc to time when code is compiled
}

boolean isntWet()
{
  sens1 = analogRead(A0);
  Serial.println(sens1);
  sens2 = analogRead(A1);
  Serial.println(sens2);
  float avg = (sens1 + sens2) / 2;
  Serial.println(avg);
  lcd.print("AVG: ");
  lcd.print(avg);
  return (avg > threshold); // if avg is greater than threshold, return true
}

boolean isNight()
{
  int hour = rtc.now().hour();
  if (hour >= timings[3] && hour <= timings[4])
  { // turns lights on from 7AM to 7PM
    Serial.println("day");
    return true;
  }
  return false;
}

//to fix later
//void checkUpdates()
//{
//  if (Serial.available() > 0)
//  {
//    Serial.print("OK");
//    Serial.println();
//    String output;
//    output = Serial.readString();
//    if (output.substring(0, 1) == "1")
//    {
//      timings = output.substring(1, output.length());
//    }
//  }
//}

void sendData()
{
  Serial.print("[");
  Serial.print(sens1);
  Serial.print(",");
  Serial.print(sens2);
  Serial.print("]");
  Serial.println();
}

void loop()
{
  isntWet();
  bool night = isNight();
  if (night)
  {
    digitalWrite(lights, LOW); // turns lights on
  }
  else
  {
    digitalWrite(lights, HIGH); // turns lights off
  }
  DateTime time = rtc.now();
  if (time.hour() == timings[0] && time.minute() >= timings[1] && time.minute() <= timings[2])
  {
    digitalWrite(pump, LOW);
  }
  else
  {
    digitalWrite(pump, HIGH);
  }
  float avg = (sens1 + sens2) / 2;
  lcd.setCursor(0, 0);
  lcd.print("AVG: ");
  lcd.print(avg);
  lcd.setCursor(0, 1);
  lcd.print(rtc.now().hour());
  lcd.print(":");
  lcd.print(rtc.now().minute());
  delay(1000);
  lcd.clear();
  //checkUpdates();

  if ((time.minute() == 30 || time.minute() == 0) && timeout)
  {
    sendData();
    timeout = false;
  }
  else
  {
    timeout = true;
  }
}
