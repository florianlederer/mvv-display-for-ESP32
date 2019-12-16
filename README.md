# mvv display for ESP32
Zeigt die nächsten Abfahrten der MVV auf einem kleinem OLED Display an. Es handelt sich hierbei um kein Projekt der MVV.

## Benötigtes:
- ESP32 mit OLED Display: https://www.amazon.de/dp/B076P8GRWV/
- Micro USB Kabel
- USB Netzteil
- Computer zum Programmieren

## Anleitung:
1.  Arduino IDE installieren: https://www.arduino.cc/en/main/software
1.  ESP32 Board installieren: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
1.  Tools -> Manage Libraries.. anklicken und nach der "ESP8266 and ESP32 OLED driver for SSD1306 displays" (ThingPulse) suchen und installieren. Anschließend nach der "Arduinojson" suchen und ebenfalls installieren.
1.  Tools -> Board -> "TTGO LoRa-32-OLED V1" auswählen.
1.  Tools ->  Port anschauen und die Enträge merken falls nicht ausgeblendet.
1.  ESP32 mit dem Kabel mit dem Computer verbinden und erneut unter Tools ->  Port schauen nun müsste ein neuer Port hinzugekommen sein/ Port ist nicht mehr ausgeblendet. Diesen nun auswählen.
1. MVV_ESP/MVV_ESP.ino aus diesem Repo kopieren und in der Arduino IDE einfügen.
1. config.h.dist kopieren nach config.h und mindestens eine eigene Konfiguration eintragen
    1. Die Seite https://www.mvg.de/dienste/abfahrtszeiten.html im Firefox Webbrowser öffnen und anschließend STRG + SHIFT + E drücken. Jetzt sollte ein neues Fenster aufgegangen sein welches den Netzwerkverkehr mitschneidet.
    1. Nun den gewünschten Bahnhof ganz normal eingeben und sich die aktuellen Abfahrten anzeigen lassen.
    1. Im in Schritt 8.i geöffneten Fenster bei Type nach dem/den Paketen mit json suchen und die Bahnhof nun in der Spalte File rauslesen.
    1. Die ermittelte URL in die config.h eintragen
    1. Ein `*` bei include_type zeigt alle verfügbaren Verkehrsmittel an
    1. Ein `*` bei include_line zeigt alle verfügbaren Linien an
    1. exclude_destinations kann bei Bedarf leer gelassen werden
    1. Werte für include_type, include_lines und exclude_destinations können dem JSON der API entnommen werden
1. Sketch -> Verify/Compile und schauen ob irgendwelche Fehler in der Konsole auftreten. Gegebenfalls die Libraries überprüfen.
1. Sketch -> Upload
1. Jetzt sollten auf dem Display die aktuellen Abfahrten angezeigt werden.
1. Zum Debuggen kann der Serielle Monitor unter Tools -> Serial Monitor aufgerufen werden.

## MVG API:
Aktuell wird die MVG API verwendet. Diese liefert Infos für alle Verkehrsmittel der MVG (Bus/Sbahn/Ubahn/Tram). Sie stellt die Grundlage für die MVG Website sowie die MVG Naviagtor App dar. Allerdings sind die Echtzeitinformationen besonders bei Störungen nicht optimal. Aus diesem Grund gibt es einen neuen Branch der sich mit der geops-API, die unteranderem in der München Navigator APP oder auf s-bahn-muenchen-live.de zum Einsatz kommt, beschäftigt. Hier bekommt man sehr genaue Echtzeitinformationen, allerdings ausschließlich für die Sbahn.
## Geops API Sbahn München:
Diese API kommt unter anderem in der München Navigator APP oder auf s-bahn-muenchen-live.de zum Einsatz und liefert sehr genaue Echtzeitinformationen, allerdings ausschließlich für die Sbahn. Alle nachfolgenden Erkenntnisse stammen aus öffentlichen Quellen oder aus Mitschnitten und Analysen des Netzwerktraffics.

### Funktionsweise
In jedem Zug befindet sich eine Art GPS Tracker. Circa alle 10 Sekunden werden die aktuellen GPS-Koordinaten sowie Informationen zum  Zuglauf und zum Status von jeder aktiven Sbahn an einen Server verschickt. Diese Daten werden ausgewertet und die daraus berechneten Abfahrtszeiten sowie die Zugpositionen auf der MVV-Netzkarte über die API verbereitet. 

### Server
URL                             | Port     | Protokoll
--------------------------------| -------- | --------
wss://tralis.sbahnm.geops.de/ws | 443      | Websocket

### Kommandos
Die Kommandos die an den Server geschickt werden sind in der redis-websocket-api definiert. Die wichtigesten Kommandos für dieses Projekt mit ihren Keys sind hier aufgelistet: 

Kommando              | Funktion 
----------------------|---------------------------------------------------------------------------
PING                  | Überprüfen der Verbindung
GET healthcheck       | Auskunft über den Serverstatus
GET newsticker        | Liefert Zusatzmeldungen 
GET trajectory        | Liefert Live Infos zu allen aktiven Sbahnen (z.B. GPS Koordinaten)
GET station           | Liefert Infos zu allen Bahnöfen. (z.B. uic Nummer)
GET timetable_8000261 | Liefert die aktuellen Abfahrtzeiten am Hauptbahnhof (uic=8000261)
SUB timetable_8000261 | Abboniert die Abfahrtzeiten am Hauptbahnhof (uic=8000261)
DEL timetable_8000261 | Deabboniert die Abfahrtzeiten am Hauptbahnhof (uic=8000261)

Während GET einmalig die nächsten Abfahrten liefert, bekommt der Client mit SUB immer automatisch eine Aktualisierung. Die uid Bahnhofsnummer ist nicht identisch mit der id der MVG API. Der Server antwortet immer im JSON Format. 

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
Die ris_aimed_time stellet scheinbar die geplante Abfahrtszeit nach Fahrplan dar. Interessant wären die ris_estimated_time, min_arrival_time und fzo_estimated_time zu der aktuell weitere Erkenntnisse fehlen... Ideen?

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

### Library:
Es wird die ArduinoWebockets Library benötigt: [https://github.com/gilmaimon/ArduinoWebsockets](https://github.com/gilmaimon/ArduinoWebsockets)

### Quellen:
[https://geops.ch/sbahnm-live](https://geops.ch/sbahnm-live)  
[https://geops.ch/blog/zuege-echtzeit](https://geops.ch/blog/zuege-echtzeit)  
[https://github.com/geops/redis-websocket-api](https://github.com/geops/redis-websocket-api)

## Jodel:
[https://shared.jodel.com/58NiubGCf2?channel=other](https://shared.jodel.com/58NiubGCf2?channel=other)
