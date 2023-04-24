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
boolean bucketHasWater = true;

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
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // sets rtc to time when code is compiled
  isntWet();
  sendData();
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
  { // turns lights on from 7AM to 7PM
    return true;
  }
  return false;
}

//to fix later
void checkUpdates()
{
  if (Serial.available() > 0)
  {
    Serial.print("OK");
    Serial.println();
    String output;
    output = Serial.readString();
    if (output.substring(0, 1) == "1")
    {
      int arr[5];
      String str = output.substring(1,str.length());
      int index = 0;
      str.remove(0,1);
      str.remove(str.length()-1);
      while(str.length() > 0){
        int pos = str.indexOf(",");
        if(pos == -1){
          arr[index] = str.toInt();
          break;
        }
        String substr = str.substring(0, pos);
        arr[index] = substr.toInt();
        str.remove(0, pos + 1);
        index++;
      }
      memcpy(timings, arr, sizeof(arr[0])*5);
    }
    else if (output.substring(0, 1) == "2"){
      int arr[2];
      String str = output.substring(1,str.length());
      int index = 0;
      str.remove(0,1);
      str.remove(str.length()-1);
      while(str.length() > 0){
        int pos = str.indexOf(",");
        if(pos == -1){
          arr[index] = str.toInt();
          break;
        }
        String substr = str.substring(0, pos);
        arr[index] = substr.toInt();
        str.remove(0, pos + 1);
        index++;
      }
      rtc.setHours(arr[0]);
      rtc.setMinutes(arr[1]);
    }
  }
}

void sendData()
{
  Serial.print("[");
  Serial.print(sens1);
  Serial.print(",");
  Serial.print(sens2);
  Serial.print(",");
  Serial.print(rtc.now().hour());
  Serial.print(",");
  Serial.print(rtc.now().minute()); 
  Serial.print(",");
  Serial.print(bucketHasWater);
  Serial.print("]");
  Serial.println();
}

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
  checkUpdates();

  if ((time.minute() == 30 || time.minute() == 0) && time.second() > 57){
    sendData();
  }
  delay(1000);
  lcd.clear();
}
