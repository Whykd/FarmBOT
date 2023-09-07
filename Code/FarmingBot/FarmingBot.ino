#include <RTClib.h>            //Adafruit lib
#include <LiquidCrystal_I2C.h> //By Frank de Brabander
#include <Wire.h>
#include <ArduinoJson.h>       //by bblanchon

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
//pump on/off hour, pump on min, pump off min, light on hour, light off hour 
int timings[] = {9, 35, 37, 7, 19};  
int sens1 = 0;
int sens2 = 0;
boolean timeout = true;
//boolean bucketHasWater = true;

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
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // sets rtc to time when code is compiled
  isntWet();
//  sendData();
}

boolean isntWet()
{
  sens1 = analogRead(A0);
  sens2 = analogRead(A1);
  float avg = (sens1 + sens2) / 2;
  lcd.printstr("AVG: ");
  lcd.print(avg);
  return (avg > threshold); // if avg is greater than threshold, return true
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

//to fix later
//void checkUpdates()
//{
//  if (Serial.available() == 0)
//  {
//    StaticJsonDocument<96> doc;
//
//    DeserializationError error = deserializeJson(doc, Serial);
//
//    if (error) {
//     // add output on screen if error as serial bus is in use
//     return;
//    }
//
//    int timings[0] = doc["wstarth"]; // water start hour
//    int timings[1] = doc["wstartm"]; // water start minute
//    int timings[2] = doc["wendm"];   // water end minute
//    int timings[3] = doc["lstartm"]; // light start minute
//    int timings[4] = doc["lendm"];   // light end minute
//
//    //rtc.adjust(DateTime(23, 6, 26, arr[0], arr[1], 0)); // sets rtc dont forget to add this back you moron (luke)
//  }
//  
//
//}

//void sendData()
//{
//  StaticJsonDocument<24> doc;
//  DateTime now = rtc.now();
//  uint32_t timestamp = now.unixtime());
//
//  doc["sens1"] = sens1;
//  doc["sens2"] = sens2;
//  doc["time"] = timestamp;
//
//  serializeJson(doc, Serial);
//}

void loop()
{
  isntWet();
  bool night = isNight();
  if (night){
    digitalWrite(lights, LOW); // turns lights on
  }
  else{
    digitalWrite(lights, HIGH); // turns lights off
  }
  DateTime time = rtc.now();
  if (time.hour() == timings[0] && time.minute() >= timings[1] && time.minute() <= timings[2]){
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
//  checkUpdates();

  if ((time.minute() == 30 || time.minute() == 0) && time.second() > 58){
    //sendData();
  }
  delay(1000);
  lcd.clear();
}
