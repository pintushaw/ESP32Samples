
/*Pintu Shaw - Digitalab.org
Complete project details can be access from ...
Permission is herby granted, free of charge to use it for any project
 */
#include <WiFi.h>
#include "time.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TM1637Display.h>

// Wifi Details
const char* ssid = "";
const char* password = "";

// tm1637 pin 
#define CLK 4
#define DIO 5

//Weather API URL and Key
const char* api_key="xxxxxxxxxxxxxxxxx";
const char* location = "Hyderabad";
String url = "https://api.weatherapi.com/v1/current.json?q=" + String(location) + "&key=" + String(api_key);


TM1637Display display(CLK,DIO);

//Timezone (Indian Standard Time /IST)
// Change as per your timezone 
const char* TZ_INFO = "IST-5:30";

// Create Centigrade symbol
const uint8_t Celsius[]={
  SEG_A | SEG_B | SEG_F|SEG_G,  //circle
  SEG_A | SEG_D | SEG_E | SEG_F //C
};
//Create Fahrenheit symbol
const uint8_t Fahrenheit[]={
  SEG_A | SEG_B | SEG_F| SEG_G,  //circle
  SEG_F | SEG_E | SEG_A | SEG_G //C
};

// Global Variables for temperature and timers
float currentTempCen=9999; // Initial value as abnormal value
float currentTempFh=9999;  // Initial value as abnormal value
unsigned long lastfetchtime=0;
const unsigned long fetchInterval = 300000UL; // 5 Minute in MS


// Initialize Wifi
void initWifi(){
  WiFi.begin(ssid,password);
  while (WiFi.status() !=WL_CONNECTED) {
    delay(1000);
    Serial.println("Wait ! Connecting to wifi...");
    if (WiFi.status()==WL_CONNECT_FAILED) {
      Serial.println("Failed connection..");
      break;
    }

  }
  Serial.println("Wifi Connected - IP is ");
  Serial.println( WiFi.localIP());
}

//
bool updateTemperature(){
  HTTPClient http;
  http.begin(url);

  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {  // check for http code 200 
    String payload = http.getString();
    Serial.println(payload);

    // Parse the JSON
    StaticJsonDocument<1024> doc;   // Change size based on estimated response size
    DeserializationError error=deserializeJson(doc,payload);
    if(error) {
      Serial.print("Json parsig failed...");
      Serial.println(error.c_str());
      http.end();
      return false;
    }
    // Get both temperature
    currentTempCen = doc["current"]["temp_c"];
    currentTempFh = doc["current"]["temp_f"];
    http.end();
    return true;

  } else {
    Serial.print("Error in data request...");
    Serial.println(httpCode);
    http.end();
    return false;
  }
}


// Initialize Time with specified timezone
void initTime() {
  configTime(0,0,"pool.ntp.org");
  setenv("TZ",TZ_INFO,1);
  tzset();
}

//Display time on the screen
void displayTime() {
  // Blinking colon
  static bool colon = false;
  colon = !colon;
  //Get current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }
  int hours = timeinfo.tm_hour; // get hours
  int min = timeinfo.tm_min; //get minutes

  int hourPart = hours*100;    //Move hours to left 2 digits
  int value= hourPart + min ; // Combine init HHMM
  Serial.print("Time is ...");
  Serial.println(value);
  //Code for blinking colon
  uint8_t colonMask;
  if (colon ==true) {
    colonMask= 0b01000000; // turn colon on
  } else {
    colonMask=0;           // turn colon off
  }
  display.showNumberDecEx(
    value,
    colonMask,     // blink colon
    true           // leading zeros 
  );
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    delay(1000);      // Time to initialize
    display.setBrightness(7);
    display.clear();
    initWifi();
    initTime();
    // Initial fetch of temperature
    if (updateTemperature()){
      lastfetchtime=millis();

    }
}
void loop() {
  // put your main code here, to run repeatedly:
  
    displayTime();
    delay(1000);
    displayTime();
    delay(1000);
    displayTime();
    delay(1000);
    displayTime();
    delay(1000);
    displayTime();
    delay(1000);
    // Check if its time to fetch new data
    if(millis() - lastfetchtime >= fetchInterval) {
      if(updateTemperature()) {
        lastfetchtime=millis();
      } else {
        //if failed
        Serial.println("Fetch Failed. keeping old values..");
      }
    }
    // Show Celsius temperature
    Serial.print("Temperature in Celsius : ");
    Serial.println(currentTempCen,2);
    int roundedTempCen=round(currentTempCen);
    display.showNumberDec(roundedTempCen,false,2,0);
    display.setSegments(Celsius,2,2);

    delay(5000);

    // Show Fahrenheit temperature
    Serial.print("Temperature in Fahrenheit : ");
    Serial.println(currentTempFh,2);
    int roundedTempFh=round(currentTempFh);
    display.showNumberDec(roundedTempFh,false,2,0);
    display.setSegments(Fahrenheit,2,2);
    delay(5000);


}
