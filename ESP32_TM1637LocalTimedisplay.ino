
/*Pintu Shaw - Digitalab.org
Complete project details can be access from ...
Permission is herby granted, free of charge to use it for any project
 */
#include <WiFi.h>
#include "time.h"
#include <TM1637Display.h>

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// tm1637 pin 
#define CLK 4
#define DIO 5


TM1637Display display(CLK,DIO);

//Timezone (Indian Standard Time /IST)
// Change as per your timezone 
const char* TZ_INFO = "IST-5:30";

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
    delay(1000);
    display.setBrightness(7);
    display.clear();
    initWifi();
    initTime();
}
void loop() {
  // put your main code here, to run repeatedly:
  
    displayTime();
    delay(1000);
}
