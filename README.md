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

## API:
Aktuell wird die MVG API verwendet. Diese liefert Infos für alle Verkehrsmittel der MVG (Bus/Sbahn/Ubahn/Tram). Sie stellt die Grundlage für die MVG Website sowie die MVG Naviagtor App dar. Allerdings sind die Echtzeitinformationen besonders bei Störungen nicht optimal. Aus diesem Grund gibt es einen neuen Branch der sich mit der geops-API, die unteranderem in der München Navigator APP oder auf s-bahn-muenchen-live.de zum Einsatz kommt, beschäftigt. Hier bekommt man sehr genaue Echtzeitinformationen, allerdings ausschließlich für die Sbahn.
