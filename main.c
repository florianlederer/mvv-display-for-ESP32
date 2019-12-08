#include <WiFi.h>
#include <HTTPClient.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
 
#include <Arduino.h>
#include "Wire.h"
#include "SSD1306.h"
 
#include "time.h"
 
const char* ssid = "XXX";
const char* password =  "XXX";
 
//https://www.mvg.de/dienste/abfahrtszeiten.html
const char* url =  "https://www.mvg.de/api/fahrinfo/departure/de:09162:6?footway=0";
 
SSD1306 display (0x3c, 4, 15);
 
StaticJsonDocument<20000> doc;
 
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
 
void setup() {
 
  Serial.begin(115200);
 
  pinMode (16, OUTPUT);
  digitalWrite (16, LOW); // set GPIO16 low to reset OLED
  delay (50);
  digitalWrite (16, HIGH); // while OLED is running, GPIO16 must go high
 
  delay(4000);   //Delay needed before calling the WiFi.begin
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
 
  display.init ();
  display.flipScreenVertically ();
  display.setFont (ArialMT_Plain_10);
  delay(1000);
 
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
 
}
 
void loop() {
 
 
  if (WiFi.status() == WL_CONNECTED) {
 
    HTTPClient http;
 
    http.begin(url);
    int httpResponseCode = http.GET();
 
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("NEW DATA");
      Serial.println(httpResponseCode);
      Serial.println(response);
 
      Serial.println("Parsing Json...");
      //Parsing Json
      DeserializationError error = deserializeJson(doc, response);
      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
      }
      else
      {
        Serial.println("No Errors");
        String servings = doc["departures"][0];
        Serial.println(servings);
 
        time_t now;
        time(&now);
        Serial.println(now);
 
        display.clear();
        unsigned int departures_length = doc["departures"].size();
        unsigned int i = 0;
        unsigned int cnt = 0;
 
        while (i < departures_length && cnt < 4)
        {
          if (doc["departures"][i]["product"] == "SBAHN")
          {
            //Calc minutes until departure
            unsigned long departure = doc["departures"][i]["departureTime"].as<long long>() / 1000; //ms to seconds
            Serial.println(departure);
 
            unsigned long minutes = 0;
            if ( departure > now)
            {
              unsigned long wait = departure - now;
              Serial.println(wait);
              minutes = wait / 60;
              if (wait % 60 > 30) ++minutes;
              minutes +=  doc["departures"][i]["delay"].as<int>();
            }
            Serial.println(minutes);
 
            String label = doc["departures"][i]["label"].as<String>();
            String destination  = doc["departures"][i]["destination"].as<String>();
            String full_name = label + " " + destination;
 
            display.setTextAlignment (TEXT_ALIGN_LEFT);
            display.setFont (ArialMT_Plain_10);
            display.drawString( 0 , cnt * 16, full_name);
 
            display.setFont (ArialMT_Plain_16);
            display.setTextAlignment (TEXT_ALIGN_RIGHT);
            display.drawString( 128, cnt * 16, String(minutes));
            ++cnt;
          }
          ++i;
        }
        display.display();
      }
 
    } else {
      Serial.print("Error: Couldn't send GET: ");
      Serial.println(httpResponseCode);
    }
    http.end();
 
  } else {
 
    Serial.println("Error in WiFi connection");
 
  }
 
  delay(30000);  //Send a request every 30 seconds
 
}
