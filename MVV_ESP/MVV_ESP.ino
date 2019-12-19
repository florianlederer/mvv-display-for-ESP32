#include <WiFi.h>
#include <HTTPClient.h>
//mvg api timestamp in ms needs long long
#define ARDUINOJSON_USE_LONG_LONG 1
//geops api timestamp in ms needs double
#define ARDUINOJSON_USE_DOUBLE 1
//geops api uses unicode characters
#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>
#include <Arduino.h>
#include "Wire.h"
#include "SSD1306.h"
#include "time.h"
#include <ArduinoWebsockets.h>
#include <list>
#include <iterator>
using namespace std;

#define MAX_INCLUDE_TYPE 10
#define MAX_INCLUDE_LINE 10
#define MAX_EXCLUDE_DESTINATION 10
#define MAX_JSON_DOCUMENT 20000
#define MAX_DEPARTURE_LIST_LENGTH 100

enum types {
  mvg_api,
  geops_api
};


typedef struct {
  const char *wifi_name;
  const char *wifi_pass;
  const enum types type;
  const char *bahnhof;
  const char *include_type[MAX_INCLUDE_TYPE];
  const char *include_line[MAX_INCLUDE_LINE];
  const char *exclude_destinations[MAX_EXCLUDE_DESTINATION];
} Config;
#include "config.h";

typedef struct {
  unsigned long long aimed_time;
  unsigned long long estimated_time;
  String line;
  String destination;
  int platform;
  int wagon;
} Departure;

list <Departure> departure_list;

websockets::WebsocketsClient client;

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
void drawDeparture(int display_line, const char * line, const char * destination, int track, int wagon, int minutes);
void init_geops_api(Config config);
void handle_geops_api(Config config);

void setup() {

  Serial.begin(115200);

  setup_display();

  delay(4000);   //Delay needed before calling the WiFi.begin

  while (config_number == -1) {
    config_number = connect_wifi();
  }
  Config loaded_config = configs[config_number];

  switch (loaded_config.type) {
    case mvg_api:
      break;
    case geops_api:
      init_geops_api(loaded_config);
      break;
    default:
      Serial.println("Unkown config type");
      break;
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Config loaded_config = configs[config_number];
    switch (loaded_config.type)
    {
      case mvg_api:
        handle_mvg_api(loaded_config);
        break;
      case geops_api:
        handle_geops_api(loaded_config);
        break;
      default:
        Serial.println("Unkown config type");
    }
  }
  else
  {
    Serial.println("Error in WiFi connection");
  }
}

int connect_wifi()
{
  int number_of_networks = WiFi.scanNetworks();
  if (number_of_networks == -1 ) {
    Serial.println("No networks available");
  }
  for (int i = 0; i < number_of_networks; ++i) {
    String ssid = WiFi.SSID(i);
    // is this network in config...?
    for (int j = 0; j < number_of_configs; ++j) {
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
  String url = "https://www.mvg.de/api/fahrinfo/departure/" + String(config.bahnhof) + "?footway=0";
  http.begin(url);
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
        bool interesting_type = false;
        bool interesting_line = false;
        bool interesting_destination = true;

        for (int j = 0; j < MAX_INCLUDE_TYPE; ++j) {
          Serial.println(config.include_type[j]);
          if (config.include_type[j] && strcmp(config.include_type[j], "*") == 0) {
            // We want to see all types
            interesting_type = true;
            break;
          }
          if (config.include_type[j] && strcmp(config.include_type[j], doc["departures"][i]["product"]) == 0) {
            // We want to see this type
            interesting_type = true;
            break;
          }
        }

        for (int j = 0; j < MAX_INCLUDE_LINE; ++j) {
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
        if (interesting_type && interesting_line) {
          for (int j = 0; j < MAX_EXCLUDE_DESTINATION; ++j) {
            if (config.exclude_destinations[j] && strcmp(config.exclude_destinations[j], doc["departures"][i]["destination"]) == 0) {
              interesting_destination = false;
              break;
            }
          }
        }
        if (interesting_type && interesting_line && interesting_destination) {
          //Calc minutes until departure
          unsigned long departure = doc["departures"][i]["departureTime"].as<long long>() / 1000; //ms to seconds
          Serial.println(departure);

          unsigned long minutes = 0;
          if ( departure > now) {
            unsigned long wait = departure - now;
            Serial.println(wait);
            minutes = wait / 60;
            //if (wait % 60 > 30) ++minutes;
            minutes +=  doc["departures"][i]["delay"].as<int>();
          }
          Serial.println(minutes);

          drawDeparture(cnt, doc["departures"][i]["label"].as<String>(), doc["departures"][i]["destination"].as<String>(), 0, 0, minutes);

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
  delay(30000);  //Send a request every 30 seconds
}


void drawDeparture(int display_line, String line, String destination, int track, int wagon, int minutes)
{
  String full_name = line + " " + destination;
  String sliced_name = full_name.substring(0, 19);
  display.setTextAlignment (TEXT_ALIGN_LEFT);
  display.setFont (ArialMT_Plain_10);
  display.drawString(0, display_line * 16, sliced_name);

  display.setFont (ArialMT_Plain_16);
  display.setTextAlignment (TEXT_ALIGN_RIGHT);
  display.drawString( 128, display_line * 16, String(minutes));
}

void init_geops_api(Config config)
{
  bool connected = client.connect("wss://api.geops.io:443/realtime-ws/v1/?key=5cc87b12d7c5370001c1d655306122aa0a4743c489b497cb1afbec9b");
  if (connected) {
    Serial.println("Connecetd!");
    client.send("GET timetable_" + String(config.bahnhof));
    client.send("SUB timetable_" + String(config.bahnhof)); //Subscribe for Departures at Hauptbahnhof
  } else {
    Serial.println("Not Connected!");
  }

  // run callback when messages are received
  client.onMessage([&](websockets::WebsocketsMessage message) {
    Serial.println(message.data());
    Serial.println("Parsing JSON...");
    DeserializationError error = deserializeJson(doc, message.data());
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
    }
    else
    {
      Serial.println("No errors");
      if (doc["source"] == ("timetable_" + String(config.bahnhof)))
      {
        Departure received_departure;
        received_departure.aimed_time= doc["content"]["ris_aimed_time"].as<double>(); 
        received_departure.estimated_time = doc["content"]["time"].as<double>();
        received_departure.line = doc["content"]["line"]["name"].as<String>();
        received_departure.destination = doc["content"]["to"][0].as<String>();
        received_departure.platform = doc["content"]["platform"].as<int>();
        received_departure.wagon = doc["content"]["train_type"].as<int>();

         /*
        Serial.println((unsigned long)(received_departure.aimed_time/1000.));
        Serial.println((unsigned long)(received_departure.estimated_time/1000.));
        Serial.println(received_departure.line);
        Serial.println(received_departure.destination);
        Serial.println(received_departure.platform);
        Serial.println(received_departure.wagon);
        */
        if (departure_list.empty())
        {
          Serial.println("EMPTY");
          departure_list.push_back(received_departure);
        }
        else
        {
          Serial.println("Not empty");
          list<Departure>::iterator it1;
          for (it1 = departure_list.begin(); it1 != departure_list.end(); ++it1)
          { 
            Serial.println("For loop");

            if (it1->aimed_time == received_departure.aimed_time && it1->line == received_departure.line && it1->destination == received_departure.destination) //Departure schon vorhanden => Update
            {
              Serial.println("Update");
              *it1 = received_departure;
              //Sorting to be sure we are sill in correct order
              departure_list.sort([](const Departure & a, const Departure & b) { return a.estimated_time < b.estimated_time; });
              break;
            }
            if ( next(it1, 1) == departure_list.end() && departure_list.size() < MAX_DEPARTURE_LIST_LENGTH) //Departure nicht vorhanden
            {
              Serial.println("Departure nicht vorhanden");
              list<Departure>::iterator it2;
              for (it2 = departure_list.begin(); it2 != departure_list.end(); ++it2)
              {
                if (it2->estimated_time > received_departure.estimated_time) //Element richtig einsortieren
                {
                  Serial.println("Departure insert");
                  departure_list.insert(it2, received_departure);
                  break;
                }
                if (next(it2,1) == departure_list.end()) //Element ganz hinten einfügen da größter Wert
                {
                  Serial.println("Departure push_back");
                  departure_list.push_back(received_departure);
                  break; //needed cause otherwise it2 != departure_list.end() will never be true
                }
              }
            }
          }  
        }  
      }
    }
  });
}

void handle_geops_api(Config config)
{
  if (client.available()) {
    client.poll();
  }

  time_t now;
  time(&now);

  Serial.println("New List");
  display.clear();
  list <Departure> :: iterator it;
  it = departure_list.begin();
  int cnt = 0;
  while ( !departure_list.empty() && it != departure_list.end() && cnt < 4)
  {
    unsigned long estimated_time_s = it->estimated_time/1000;
    unsigned long minutes = 0;

    if (estimated_time_s > now) 
    {
      unsigned long wait = estimated_time_s - now;
      minutes = wait / 60;
      //if (wait % 60 > 30) ++minutes;
    }
    else //abfahrt in der vergangenheit => aus der liste entfernen
    {
      departure_list.erase (it);
    }
    
    if (it != departure_list.end())
    {
      // Serial.print(it->line);
      // Serial.print("\t");
      // Serial.print(it->destination);
      // Serial.print("\t");
      // Serial.println(minutes);
      drawDeparture(cnt, it->line, it->destination, 0, 0, minutes);
    }
    ++cnt;
    ++it;
  }
  display.display();
}