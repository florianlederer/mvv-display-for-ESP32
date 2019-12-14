#include <WiFi.h>
#include <HTTPClient.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <Arduino.h>
#include "Wire.h"
#include "SSD1306.h"
#include "time.h"

#define MAX_INCLUDE_LINE 10
#define MAX_EXCLUDE_DESTINATION 10
#define MAX_JSON_DOCUMENT 20000

enum types {
    mvg_api,
    geops_api
};


typedef struct {
    const char *wifi_name;
    const char *wifi_pass;
    const enum types type;
    const char *url;
    const char *include_line[MAX_INCLUDE_LINE];
    const char *exclude_destinations[MAX_EXCLUDE_DESTINATION];
} Config;
#include "config.h";

SSD1306 display (0x3c, 4, 15);

StaticJsonDocument<MAX_JSON_DOCUMENT> doc;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
const int   number_of_configs = sizeof(configs) / sizeof(*configs);
int config_number = -1;

int connect_wifi();
void setup_display();
void handle_mvg_api(Config config);

void setup() {

    Serial.begin(115200);

    setup_display();

    delay(4000);   //Delay needed before calling the WiFi.begin

    while (config_number == -1) {
        config_number = connect_wifi();
    }

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        Config config = configs[config_number];
        switch (config.type) {
            case mvg_api: {
                handle_mvg_api(config);
                break;
            }
            case geops_api: {
                // TODO
                break;
            }
            default: {
                Serial.println("Unkown config type");
            }
        }
    } else {
        Serial.println("Error in WiFi connection");
    }
    delay(30000);  //Send a request every 30 seconds
}

int connect_wifi() 
{
    int number_of_networks = WiFi.scanNetworks();
    if (number_of_networks == -1 ) {
        Serial.println("No networks available");
    }
    for (int i=0; i<number_of_networks; ++i) {
        String ssid = WiFi.SSID(i);
        // is this network in config...?
        for (int j=0; j<number_of_configs; ++j) {
            if (strcmp(ssid.c_str(), configs[j].wifi_name) == 0) {
                // ... yes it is
                WiFi.begin(configs[j].wifi_name, configs[j].wifi_pass);
                while (WiFi.status() != WL_CONNECTED) {
                    delay(1000);
                    Serial.println("Connecting to WiFi...");
                }
                Serial.printf("Connected to the WiFi network: %s\n", ssid.c_str());
                return j;
            }
        }
    }
    return -1;
}

void setup_display() 
{
    pinMode (16, OUTPUT);
    digitalWrite (16, LOW); // set GPIO16 low to reset OLED
    delay (50);
    digitalWrite (16, HIGH); // while OLED is running, GPIO16 must go high
    display.init ();
    display.flipScreenVertically ();
    display.setFont (ArialMT_Plain_10);
    delay(1000);
}

void handle_mvg_api(Config config) 
{
    HTTPClient http;

    http.begin(config.url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("New data");
        Serial.println(httpResponseCode);
        Serial.println(response);

        Serial.println("Parsing JSON...");
        DeserializationError error = deserializeJson(doc, response);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        } else {
            Serial.println("No errors");
            String servings = doc["departures"][0];
            Serial.println(servings);

            time_t now;
            time(&now);
            Serial.println(now);

            display.clear();
            unsigned int departures_length = doc["departures"].size();
            unsigned int i = 0;
            unsigned int cnt = 0;

            while (i < departures_length && cnt < 4) {
                // Extract what we are interested in from response
                // based on config
                bool interesting_line = false;
                bool interesting_destination = true;
                for (int j=0; j<MAX_INCLUDE_LINE; ++j) {
                    Serial.println(config.include_line[j]);
                    if (config.include_line[j] && strcmp(config.include_line[j], "*") == 0) {
                        // We want to see all lines
                        interesting_line = true;
                        break;
                    }
                    if (config.include_line[j] && strcmp(config.include_line[j], doc["departures"][i]["label"]) == 0) {
                        // We want to see this line
                        interesting_line = true;
                        break;
                    }
                }
                if (interesting_line) {
                    for (int j=0; j<MAX_EXCLUDE_DESTINATION; ++j) {
                        if (config.exclude_destinations[j] && strcmp(config.exclude_destinations[j], doc["departures"][i]["destination"]) == 0) {
                            interesting_destination = false;
                            break;
                        }
                    }
                }
                if (interesting_line && interesting_destination) {
                    //Calc minutes until departure
                    unsigned long departure = doc["departures"][i]["departureTime"].as<long long>() / 1000; //ms to seconds
                    Serial.println(departure);

                    unsigned long minutes = 0;
                    if ( departure > now) {
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
}
