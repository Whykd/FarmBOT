#include <RTClib.h>            //Adafruit lib
#include <ArduinoJson.h>       //by bblanchon

bool pumpEnabled = true;
bool lightEnable = true;

bool bypassPumpState = false;
bool bypassLightState = false;

int pHour = 0;
int pMinuteOn = 0;
int pMinuteOff = 0;
int lHourOn = 0;
int lHourOff = 0;

void setup(){

}

void loop(){
DynamicJsonDocument doc(1024); //create a json document
deserilaizeJson(doc, Serial); //deserialize the json document from the serial port
/*
 *
 *  0 - do nothing
 *  1 - toggle pump function
 *  2 - toggle light function
 *  3 - update pump and light schedule from json
 *  4 - toggle light on/off
 *  5 - toggle pump on/off
 *  6 - update rtc time/date
 * 
 */


int action = doc["action"]; //get the action from the json document
  switch (action)
  {
  case 0:
    break;

  case 1:
    pumpEnabled = !pumpEnabled;
    break;

  case 2:
    lightEnable = !lightEnable;
    break;

  case 3:
    pHour = doc["data"]["pHour"];
    pMinuteOn = doc["data"]["pMinuteOn"];
    pMinuteOff = doc["data"]["pMinuteOff"];

    lHourOn = doc["data"]["lHourOn"];
    lHourOff = doc["data"]["lHourOff"];
    break;
  case 4:
    bypassPumpState = !bypassPumpState;
    break;

  case 5:
    bypassLightState = !bypassLightState;
    break;

  case 6:
    //rtc to deal with later
    break;

  default:
    break;
  }

}
