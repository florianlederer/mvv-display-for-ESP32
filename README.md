# mvv display for ESP32
In diesem Projekt wird eine Echtzteitanzeige gebaut mit der die nächsten Abfahrten des Münchner Nahverkehrs auf einem kleinem Display angezeigt werden. Es handelt sich hierbei um kein Projekt der MVV. Akutell gibt es zwei Varianten des Projektes. Die neuere Version 2 unterscheidet sich vom Vorgänger durch ein größeres Farb Display. Damit können weitere Informationen wie z.B. Gleis und Wagonzahl angezeigt werden. Aus diesem Grund wird empfohlen die neuere Variante zu verwenden.

## Benötigtes Version 1 OLED Display (obsolet):
- ESP32 mit OLED Display: https://www.amazon.de/dp/B076P8GRWV/
- Micro USB Kabel
- USB Netzteil
- Computer zum Programmieren

## Benötigtes Version 2 LCD Display (empfohlen):
- ESP32 TTGO-T-Display:  https://www.amazon.de/dp/B07VNG9D52/
- USB-C Kabel
- USB Netzteil
- Computer zum Programmieren

## Anleitung:
1.  Arduino IDE installieren: https://www.arduino.cc/en/main/software
1.  ESP32 Board installieren: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
1.  Tools -> Manage Libraries.. anklicken und nach den folgenden Libraries suchen und installieren: 
    1. "Arduinojson" (by Benoit Blanchon)
    1. "ArduinoWebsockets" (by Gil Maimon)
    1. "ESP8266 and ESP32 OLED driver for SSD1306 displays" (by ThingPulse, Fabrice Weinberg) (Benötigt für Version 1)
    1. "TFT_eSPI" (by Bodmer) (Benötigt für Version 2)
1. Den Arduino Library Ordner öffnen und die "TFT_eSPI" wie hier in Schritt 2 erklärt, modifizieren: https://github.com/Xinyuan-LilyGO/TTGO-T-Display (Benötigt für Version 2)
1. Installiere die "Button2" Library: Das folgende ZIP-Archiv (https://github.com/lennarthennigs/Button2/zipball/master) herunterladen. Unter "Sketch > Include Library > Add .ZIP Library..." das heruntergeladene File auswählen um die Bibliothek zu installieren. (Benötigt für Version 2)
1.  Tools -> Board -> "ESP32 Dev Module" auswählen.
1.  Tools ->  Port anschauen und die Enträge merken falls nicht ausgeblendet.
1.  ESP32 mit dem Kabel mit dem Computer verbinden und erneut unter Tools ->  Port schauen nun müsste ein neuer Port hinzugekommen sein/ Port ist nicht mehr ausgeblendet. Diesen nun auswählen.
1. Dieses Repo als ZIP herunterladen und MVV_ESP_XXXX/MVV_ESP_XXXX.ino (Version 1: OLED, Version 2: LCD)  in der Arduino IDE öffnen.
1. MVV_ESP_XXXX/config.h.dist in config.h umbennenen und minderstens eine eigene Konfiguration eintragen: 
    1. Zunächst müssen wifi_name und wifi_pass angegeben werden.
    1. bei type kann entweder mvg_api oder geops_api (ohne Anführungszeichen) eingetragen werden.
    1. Unter bahnhof muss für die mvg_api die id eingetragen werden, für die geops_api muss die entsprechende uid eingefügt werden. Zum Herausfinden dieser Nummern gibt es im Ordner API entsprechende Listen.
    1. Ein `*` bei include_type zeigt alle verfügbaren Verkehrsmittel an (mögliche Filter: "SBAHN", "UBAHN", "TRAM", "BUS",    REGIONAL_BUS) 
    1. Ein `*` bei include_line zeigt alle verfügbaren Linien an (mögliche Filter: z.B. "S8", "U3", "17")
    1. exclude_destinations kann bei Bedarf leer gelassen werden (mögliche Filter: z.B. "Feldmoching", "Pasing")
    1. HINWEIS: Filter funktionieren aktuell nur mit der mvg_api und werden von der geops_api ignoriert.
1. Sketch -> Verify/Compile und schauen ob irgendwelche Fehler in der Konsole auftreten. Gegebenfalls die Libraries überprüfen.
1. Sketch -> Upload
1. Nach kurzer Zeit sollten auf dem Display die aktuellen Abfahrten angezeigt werden.
1. Zum Debuggen kann der Serielle Monitor unter Tools -> Serial Monitor aufgerufen werden.

## MVG API:
Die MVG API liefert Infos für alle Verkehrsmittel der MVG (Bus/Sbahn/Ubahn/Tram). Sie stellt die Grundlage für die MVG Website sowie die MVG Naviagtor App dar. Allerdings sind die Echtzeitinformationen besonders bei Störungen nicht optimal. Aus diesem Grund  kann ach die geops-API verwendet werden, die unteranderem in der München Navigator APP oder auf s-bahn-muenchen-live.de zum Einsatz kommt. Hier bekommt man sehr genaue Echtzeitinformationen, allerdings ausschließlich für die Sbahn.

### Server
URL                     | Port     | Protokoll
------------------------| -------- | --------
https://www.mvg.de/api | 443      | HTTP

### Kommandos
Die wichtigesten Kommandos für dieses Projekt sind hier beispielhaft aufgelistet: 

Kommando                                     | Funktion 
----------------------------------           |---------------------------------------------------------------------------
/fahrinfo/location/queryWeb?q=               | Liefert Infos zu allen  Bahnhöfen (z.B. id)
/fahrinfo/location/queryWeb?q=Marienplatz    | Liefert Infos zu allen Bahnhöfen am Marienplatz
/fahrinfo/departure/de:09162:2?footway=0     | Liefert die aktuellen Abfahrtzeiten am Marienplatz (id=de:09162:2)

Der Server antwortet immer im JSON Format.

Eine Beispielantwort eines location Paketes sieht folgendermaßen aus:
```json
{
  "locations" : [ {
    "type" : "station",
    "latitude" : 48.13725,
    "longitude" : 11.57542,
    "id" : "de:09162:2",
    "divaId" : 2,
    "place" : "München",
    "name" : "Marienplatz",
    "hasLiveData" : true,
    "hasZoomData" : true,
    "products" : [ "SBAHN", "UBAHN", "BUS" ],
    "aliases" : "Rathaus Bf. Bahnhof München Muenchen Munchen",
    "link" : "MP",
    "lines" : {
      "tram" : [ ],
      "nachttram" : [ ],
      "sbahn" : [ ],
      "ubahn" : [ ],
      "bus" : [ ],
      "nachtbus" : [ ],
      "otherlines" : [ ]
    }
  }, {
    "type" : "station",
    "latitude" : 48.40054,
    "longitude" : 11.74386,
    "id" : "de:09178:2851",
    "divaId" : 2851,
    "place" : "Freising",
    "name" : "Marienplatz",
    "hasLiveData" : false,
    "hasZoomData" : false,
    "products" : [ "BUS" ],
    "aliases" : "FS",
    "lines" : {
      "tram" : [ ],
      "nachttram" : [ ],
      "sbahn" : [ ],
      "ubahn" : [ ],
      "bus" : [ ],
      "nachtbus" : [ ],
      "otherlines" : [ ]
    }
  }]
}
```

Eine Beispielantwort eines departure Paketes sieht folgendermaßen aus:
```json
{
  "servingLines" : [ {
    "destination" : "Ostbahnhof",
    "sev" : false,
    "partialNet" : "mvv",
    "product" : "SBAHN",
    "lineNumber" : "S1",
    "divaId" : "01001"
  }, {
    "destination" : "Leuchtenbergring",
    "sev" : false,
    "partialNet" : "ddb",
    "product" : "SBAHN",
    "lineNumber" : "S1",
    "divaId" : "92M01"
  }, {
    "destination" : "Ostbahnhof",
    "sev" : false,
    "partialNet" : "ddb",
    "product" : "SBAHN",
    "lineNumber" : "S2",
    "divaId" : "92M02"
  }, {
    "destination" : "Holzkirchen",
    "sev" : false,
    "partialNet" : "ddb",
    "product" : "SBAHN",
    "lineNumber" : "S3",
    "divaId" : "92M03"
  }, {
    "destination" : "Ostbahnhof",
    "sev" : false,
    "partialNet" : "ddb",
    "product" : "SBAHN",
    "lineNumber" : "S4",
    "divaId" : "92M04"
  }, {
    "destination" : "Ostbahnhof",
    "sev" : false,
    "partialNet" : "ddb",
    "product" : "SBAHN",
    "lineNumber" : "S6",
    "divaId" : "92M06"
  }, {
    "destination" : "Höhenkirchen-Siegertsbrunn",
    "sev" : false,
    "partialNet" : "ddb",
    "product" : "SBAHN",
    "lineNumber" : "S7",
    "divaId" : "92M07"
  }, {
    "destination" : "Flughafen München",
    "sev" : false,
    "partialNet" : "ddb",
    "product" : "SBAHN",
    "lineNumber" : "S8",
    "divaId" : "92M08"
  }, {
    "destination" : "Fürstenried West",
    "sev" : false,
    "partialNet" : "swm",
    "product" : "UBAHN",
    "lineNumber" : "U3",
    "divaId" : "010U3"
  }, {
    "destination" : "Harras",
    "sev" : false,
    "partialNet" : "swm",
    "product" : "UBAHN",
    "lineNumber" : "U6",
    "divaId" : "010U6"
  }, {
    "destination" : "Forstenrieder Park",
    "sev" : false,
    "partialNet" : "swm",
    "product" : "BUS",
    "lineNumber" : "132",
    "divaId" : "03132"
  } ],
  "departures" : [ {
    "departureTime" : 1577118720000,
    "product" : "SBAHN",
    "label" : "S6",
    "destination" : "Zorneding",
    "live" : false,
    "delay" : 1,
    "cancelled" : false,
    "lineBackgroundColor" : "#03a074",
    "departureId" : "35e650addb0e2620032f5f79e27d4d08#1577118720000#de:09162:2",
    "sev" : false,
    "platform" : "1"
  }, {
    "departureTime" : 1577118840000,
    "product" : "UBAHN",
    "label" : "U6",
    "destination" : "Garching, Forschungszentrum",
    "live" : false,
    "delay" : 0,
    "cancelled" : false,
    "lineBackgroundColor" : "#0472b3",
    "departureId" : "f0aefa74fcb536b1b073913a5f9bbc84#1577118840000#de:09162:2",
    "sev" : false,
    "platform" : "Gleis 1"
  }]
}
```
## Geops API Sbahn München:
Diese API kommt unter anderem in der München Navigator APP oder auf s-bahn-muenchen-live.de zum Einsatz und liefert sehr genaue Echtzeitinformationen, allerdings ausschließlich für die Sbahn. Alle nachfolgenden Erkenntnisse stammen aus öffentlichen Quellen oder aus Mitschnitten und Analysen des Netzwerktraffics.

### Funktionsweise
In jedem Zug befindet sich eine Art GPS Tracker. Circa alle 10 Sekunden werden die aktuellen GPS-Koordinaten sowie Informationen zum  Zuglauf und zum Status von jeder aktiven Sbahn an einen Server verschickt. Diese Daten werden ausgewertet und die daraus berechneten Abfahrtszeiten sowie die Zugpositionen auf der MVV-Netzkarte über die API verbereitet. 

### Server
URL                                                                                             | Port     | Protokoll
------------------------------------------------------------------------------------------------| -------- | --------
wss://api.geops.io/realtime-ws/v1/?key=5cc87b12d7c5370001c1d655306122aa0a4743c489b497cb1afbec9b | 443      | Websocket

### Kommandos
Die Kommandos die an den Server geschickt werden sind in der redis-websocket-api definiert. Die wichtigesten Kommandos für dieses Projekt mit ihren Keys sind hier aufgelistet: 

Kommando              | Funktion 
----------------------|---------------------------------------------------------------------------
PING                  | Überprüfen der Verbindung
GET healthcheck       | Auskunft über den Serverstatus
GET newsticker        | Liefert Zusatzmeldungen 
GET trajectory        | Liefert Live Infos zu allen aktiven Sbahnen (z.B. GPS Koordinaten)
GET station           | Liefert Infos zu allen Bahnöfen. (z.B. uic Nummer)
GET timetable_8098263 | Liefert die aktuellen Abfahrtzeiten am Hauptbahnhof (uic=8098263)
SUB timetable_8098263 | Abboniert die Abfahrtzeiten am Hauptbahnhof (uic=8098263)
DEL timetable_8098263 | Deabboniert die Abfahrtzeiten am Hauptbahnhof (uic=8098263)

Während GET einmalig die nächsten Abfahrten liefert, bekommt der Client mit SUB immer automatisch eine Aktualisierung. Die uic Bahnhofsnummer ist nicht identisch mit der id der MVG API. Im Ordner API gibt es eine Liste mit allen Bahnhöfen und ihren uic Nummern. Der Server antwortet immer im JSON Format.  

Damit eine Verbindung länger erhalten bleibt, muss in gewissen Abständen (ca. 10s) ein PING Kommando an der Server geschickt werden.

Zum Ausprobieren der Kommandos kann der Client WS-GUII verwendet werden. Hierfür muss nur die oben aufgeführte Server URL eingetragen werden und ein Kommando abgschickt werden: [https://github.com/kbjr/ws-gui](https://github.com/kbjr/ws-gui)

Ein Beispielpaket eines timetabeles Paketes einer Sbahn sieht folgendermaßen aus:


```json
{
   "timestamp":1576000939716.51,
   "content":{
      "ris_estimated_time":1576002060000.0,
      "created_at":1575990536037.232,
      "min_arrival_time":1576001823000.0,
      "train_type":2,
      "fzo_estimated_time":1576002222000.0,
      "next_stoppoints":[
         "MDA",
         "MKFS",
         "MMAL",
         "MAUG",
         "MOZ",
         "ML",
         "MMHG",
         "MMDN",
         "MHAB",
         "MHT",
         "MKA",
         "MMP",
         "MIT",
         "MRP",
         "MOP"
      ],
      "train_id":139923201210800,
      "no_stop_between":null,
      "line":{
         "text_color":"#ffffff",
         "color":"#8bbd4d",
         "id":2,
         "name":"S2"
      },
      "state":null,
      "new_to":null,
      "updated_at":1575998600946.968,
      "no_stop_till":null,
      "platform":"1",
      "formation":null,
      "at_station_ds100":"MDA",
      "ris_aimed_time":1576002060000.0,
      "to":[
         "M\u00fcnchen Ost"
      ],
      "has_fzo":true,
      "train_number":6015,
      "time":1576002060000.0,
      "call_id":37049115
   },
   "source":"timetable_8000261",
   "client_reference":null
}
```
Die ris_aimed_time stellet die geplante Abfahrtszeit nach Fahrplan dar. Der Wert time gibt die live berechnete Abfahrtszeit, die auch in der App verwendet wird, an.
Interessant sind die ris_estimated_time, min_arrival_time und fzo_estimated_time zu der aktuell weitere Erkenntnisse fehlen... Ideen?

Weitere sehr Interessante Infos sind train_type mit der Wagonanzahl sowie at_station_ds100 mit der aktuellen Stationsposition.

Ein Besipielpaket eines trajectory Paketes einer Sbahn sieht folgendermaßen aus:
```json
{
   "content":{
      "properties":{
         "vehicle_number":"282",
         "event_timestamp":1576233062000.0,
         "line":{
            "id":3,
            "name":"S3",
            "text_color":"#ffffff",
            "color":"#8d1890"
         },
         "event":"FA",
         "time_since_update":1000,
         "stop_point_ds100":"MLW",
         "time_intervals":[
            [
               1576233073171.119,
               0.0,
               -0.4944043532027426
            ],
            [
               1576233082546.119,
               0.25,
               -0.4686027216471579
            ],
            [
               1576233091921.119,
               0.5,
               -0.4647913562689031
            ],
            [
               1576233101296.119,
               0.75,
               -0.43077358214065276
            ],
            [
               1576233110671.119,
               1.0,
               -0.43077358214065276
            ],
            [
               253402300800000.0,
               1,
               -0.43077358214065276
            ]
         ],
         "original_train_number":6353,
         "position_correction":2,
         "aimed_time_offset":80.0,
         "state":"DRIVING",
         "train_number":6353,
         "ride_state":"K",
         "delay":2.432588,
         "transmitting_vehicle":"948004232823",
         "timestamp":1576233073182.2168,
         "raw_coordinates":[
            11.457110249894782,
            48.151313408014126
         ],
         "calls_stack":[
            "MMAM",
            "MMAG",
            "MMA",
            "MGLD",
            "MESG",
            "MOL",
            "MGRZ",
            "MLO",
            "MLW",
            "MP",
            "ML",
            "MMHG",
            "MMDN",
            "MHAB",
            "MHT",
            "MKA",
            "MMP",
            "MIT",
            "MRP",
            "MOP",
            "MMAR",
            "MGI",
            "MFG",
            "MFP",
            "MUH",
            "MTU",
            "MFU",
            "MDS",
            "MSR",
            "MOTF",
            "MHO"
         ],
         "rake":"948004232278;0;0;0;948004232823;0;0;0",
         "train_id":139921365582400
      },
      "geometry":{
         "coordinates":[
            [
               11.4581872405487,
               48.1509589922501
            ],
            [
               11.4582242,
               48.1509457
            ],
            [
               11.4589674,
               48.1506947
            ],
            [
               11.4590367041999,
               48.1506715148803
            ]
         ],
         "type":"LineString"
      },
      "type":"Feature"
   },
   "client_reference":null,
   "source":"trajectory",
   "timestamp":1576233076278.496
}
```
Besonders interessant sind hier die raw_coordinates die Longitude und Latitude binhalten. Ebenfalls interessant ist der state der scheinbar zwischen "DRIVING" und "BORDING" wechselt.

## Quellen:
[https://www.mvg.de](https://www.mvg.de/)  
[https://s-bahn-muenchen-live.de](https://s-bahn-muenchen-live.de)  
[https://geops.ch/sbahnm-live](https://geops.ch/sbahnm-live)  
[https://geops.ch/blog/zuege-echtzeit](https://geops.ch/blog/zuege-echtzeit)  
[https://github.com/geops/redis-websocket-api](https://github.com/geops/redis-websocket-api)

## Jodel:
[https://shared.jodel.com/58NiubGCf2?channel=other](https://shared.jodel.com/58NiubGCf2?channel=other)
