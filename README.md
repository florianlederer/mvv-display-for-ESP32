# mvv display for ESP32
Zeigt die nächsten Abfahrten der MVV auf einem kleinem Farb LCD Display an. Es handelt sich hierbei um kein Projekt der MVV.
 
## Benötigtes:
- TTGO-T-Display:  [https://www.amazon.de/dp/B07VNG9D52/](https://www.amazon.de/dp/B07VNG9D52/)
- USB-C Kabel
- USB Netzteil
- Computer zum Programmieren

## Anleitung:
1.  Arduino IDE installieren: https://www.arduino.cc/en/main/software
1.  ESP32 Board installieren: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
1.  Tools -> Manage Libraries.. anklicken und nach den folgenden Libraries suchen und installieren:
    1. "ESP8266 and ESP32 OLED driver for SSD1306 displays" (by ThingPulse, Fabrice Weinberg) 
    1. "Arduinojson" (by Benoit Blanchon)
    1. "ArduinoWebsockets" (by Gil Maimon)
    1. "TFT_eSPI" (by Bodmer)
1. Den Arduino Library Ordner öffnen und die "TFT_eSPI" wie hier in Schritt 2 erklärt, modifizieren: https://github.com/Xinyuan-LilyGO/TTGO-T-Display
1. Installiere die "Button2" Library: Das folgende ZIP-Archiv (https://github.com/lennarthennigs/Button2/zipball/master) herunterladen. Unter "Sketch > Include Library > Add .ZIP Library..." das heruntergeladene File auswählen um die Bibliothek zu installieren.

1.  Tools -> Board -> "ESP32 Dev Module" auswählen.
1.  Tools ->  Port anschauen und die Enträge merken falls nicht ausgeblendet.
1.  ESP32 mit dem Kabel mit dem Computer verbinden und erneut unter Tools ->  Port schauen nun müsste ein neuer Port hinzugekommen sein/ Port ist nicht mehr ausgeblendet. Diesen nun auswählen.
1. Dieses Repo als ZIP herunterladen und MVV_ESP/MVV_ESP.ino in der Arduino IDE öffnen.
1. MVV_ESP/config.h.dist in config.h umbennenen und minderstens eine eigene Konfiguration eintragen. 
    1. Die Seite https://www.mvg.de/dienste/abfahrtszeiten.html im Firefox Webbrowser öffnen und anschließend STRG + SHIFT + E drücken. Jetzt sollte ein neues Fenster aufgegangen sein welches den Netzwerkverkehr mitschneidet.
    1. Nun den gewünschten Bahnhof ganz normal eingeben und sich die aktuellen Abfahrten anzeigen lassen.
    1. Im in Schritt 8.i geöffneten Fenster bei Type nach dem/den Paketen mit json suchen und die Bahnhof nun in der Spalte File rauslesen.
    1. Die ermittelte Bahnhofs ID in die config.h  unter Bahnhof eintragen
    1. Ein `*` bei include_type zeigt alle verfügbaren Verkehrsmittel an (mögliche Filter: "SBAHN", "UBAHN", "TRAM", "BUS",    REGIONAL_BUS) 
    1. Ein `*` bei include_line zeigt alle verfügbaren Linien an (mögliche Filter: z.B. "S8", "U3", "17")
    1. exclude_destinations kann bei Bedarf leer gelassen werden (mögliche Filter: z.B. "Feldmoching", "Pasing")
    1. Werte für include_type, include_lines und exclude_destinations können auch dem JSON der API entnommen werden
1. Sketch -> Verify/Compile und schauen ob irgendwelche Fehler in der Konsole auftreten. Gegebenfalls die Libraries überprüfen.
1. Sketch -> Upload
1. Nach kurzer Zeit sollten auf dem Display die aktuellen Abfahrten angezeigt werden.
1. Zum Debuggen kann der Serielle Monitor unter Tools -> Serial Monitor aufgerufen werden.

## MVG API:
Aktuell wird die MVG API verwendet. Diese liefert Infos für alle Verkehrsmittel der MVG (Bus/Sbahn/Ubahn/Tram). Sie stellt die Grundlage für die MVG Website sowie die MVG Naviagtor App dar. Allerdings sind die Echtzeitinformationen besonders bei Störungen nicht optimal. Aus diesem Grund gibt es einen neuen Branch der sich mit der geops-API, die unteranderem in der München Navigator APP oder auf s-bahn-muenchen-live.de zum Einsatz kommt, beschäftigt. Hier bekommt man sehr genaue Echtzeitinformationen, allerdings ausschließlich für die Sbahn.

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

Während GET einmalig die nächsten Abfahrten liefert, bekommt der Client mit SUB immer automatisch eine Aktualisierung. Die uic Bahnhofsnummer ist nicht identisch mit der id der MVG API. Im folgenden Kapitel gibt es eine Liste mit allen Bahnhöfen und ihren uic Nummern. Der Server antwortet immer im JSON Format.  

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

### UIC Nummern:
Die folgende Liste gibt Auskunft über die uic Nummer eines Bahnhofes. (Stand: 22.12.2019)  

Bahnhof               | uic 
----------------------|---------------------------------------------------------------------------
Altenerding | 8000524
Altomünster | 8000556
Arnbach | 8000603
Aufhausen(b Erding) | 8000653
Aying | 8000675
Bachern | 8000685
Baierbrunn | 8000781
Baldham | 8000785
Buchenau(Oberbay) | 8001229
Buchenhain | 8001231
Dachau Bahnhof | 8001354
Dachau Stadt | 8001355
Deisenhofen | 8001404
Dürrnhaar | 8001578
Ebenhausen-Schäftlarn | 8001621
Ebersberg(Oberbay) | 8001634
Eching | 8001647
Eglharting | 8001682
Eichenau(Oberbay) | 8001702
Erding | 8001825
Erdweg | 8001829
Esting | 8001996
Fasanenpark | 8001963
Feldafing | 8001970
Feldkirchen(b München) | 8001973
Freising | 8002078
Fürstenfeldbruck | 8002141
Furth(b Deisenhofen) | 8002161
Gauting | 8002198
Geisenbrunn | 8002210
Geltendorf | 8000119
Germering-Unterpfaffenhofen | 8006006
Gernlinden | 8002247
Gilching-Argelsried | 8002275
Gräfelfing | 8002339
Grafing Bahnhof | 8002347
Grafing Stadt | 8002348
Grafrath | 8002351
Gröbenzell | 8002377
Gronsdorf | 8002383
Großhelfendorf | 8002420
Großhesselohe Isartalbf | 8002422
Grub(Oberbay) | 8002435
Haar | 8002491
Hallbergmoos | 8002534
Harthaus | 8002610
Hebertshausen | 8006189
Heimstetten | 8002715
Herrsching | 8002792
Hohenbrunn | 8002940
Höhenkirchen-Siegertsbrunn | 8002894
Hohenschäftlarn | 8002955
Höllriegelskreuth | 8002899
Holzkirchen | 8002980
Icking | 8003039
Ismaning | 8003092
Kirchseeon | 8003290
Kleinberghofen | 8003317
Kreuzstraße | 8003438
Lochham | 8003720
Lohhof | 8003735
Maisach | 8003824
Malching(Oberbay) | 8003828
Mammendorf | 8004204
Markt Indersdorf | 8003072
Markt Schwaben | 8003879
München-Allach | 8004140
München-Aubing | 8004141
München-Berg am Laim | 8004142
München-Daglfing | 8004143
München Donnersbergerbrücke | 8004128
München-Englschalking | 8004144
München-Fasanerie | 8004145
München-Fasangarten | 8004146
München-Feldmoching | 8004147
München Flughafen Besucherpark | 8004167
München Flughafen Terminal | 8004168
München-Freiham | 8004181
München-Giesing | 8004148
München Hackerbrücke | 8004129
München Harras | 8004130
München Hbf (tief) | 8098263
München Heimeranplatz | 8005419
München Hirschgarten | 8004179
München Isartor | 8004131
München-Johanneskirchen | 8004149
München-Karlsfeld | 8004150
München Karlsplatz | 8004132
München-Laim | 8004151
München-Langwied | 8004152
München Leienfelsstr. | 8004133
München Leuchtenbergring | 8004134
München-Lochhausen | 8004153
München Marienplatz | 8004135
München-Mittersendling | 8004154
München-Moosach | 8004155
München-Neuaubing | 8004156
München-Neuperlach Süd | 8006696
München-Obermenzing | 8004157
München Ost | 8000262
München-Pasing | 8004158
München-Perlach | 8004159
München-Riem | 8004160
München Rosenheimer Platz | 8004136
München Siemenswerke | 8004137
München-Solln | 8004161
München St.Martin-Str. | 8004138
München-Trudering | 8004162
München-Untermenzing | 8004139
München-Westkreuz | 8004163
Neubiberg | 8004252
Neufahrn(b Freising) | 8004279
Neugilching | 8004249
Niederroth | 8004404
Oberschleißheim | 8004580
Olching | 8004667
Ottenhofen(Oberbay) | 8004723
Otterfing | 8004726
Ottobrunn | 8004733
Peiß | 8004761
Petershausen(Obb) | 8004775
Planegg | 8004827
Poing | 8004854
Possenhofen | 8004874
Puchheim | 8004893
Pullach | 8004899
Pulling(b Freising) | 8004900
Röhrmoos | 8005127
Sauerlach | 8005299
Schöngeising | 8005406
Schwabhausen(b Dachau) | 8005442
Seefeld-Hechendorf | 8005504
Starnberg | 8005676
Starnberg Nord | 8005675
Steinebach | 8005699
St Koloman | 8005652
Stockdorf | 8005735
Taufkirchen | 8005831
Türkenfeld | 8005920
Tutzing | 8005927
Unterföhring | 8005986
Unterhaching | 8005991
Unterschleißheim | 8006688
Vaterstetten | 8006059
Vierkirchen-Esterhofen | 8001922
Wächterhof | 8006131
Weßling(Oberbay) | 8006359
Wolfratshausen | 8006550
Zorneding | 8006671
### Library:
Es wird die ArduinoWebockets Library benötigt: [https://github.com/gilmaimon/ArduinoWebsockets](https://github.com/gilmaimon/ArduinoWebsockets)

### Quellen:
[https://s-bahn-muenchen-live.de](https://s-bahn-muenchen-live.de)  
[https://geops.ch/sbahnm-live](https://geops.ch/sbahnm-live)  
[https://geops.ch/blog/zuege-echtzeit](https://geops.ch/blog/zuege-echtzeit)  
[https://github.com/geops/redis-websocket-api](https://github.com/geops/redis-websocket-api)
