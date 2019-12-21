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
1.  Tools -> Manage Libraries.. anklicken und nach den folgenden Libraries suchen
- "ESP8266 and ESP32 OLED driver for SSD1306 displays" (by ThingPulse, Fabrice Weinberg) 
- "Arduinojson" (by Benoit Blanchon)
- "ArduinoWebsockets" (by Gil Maimon)
- "TFT_eSPI" (by Bodmer)
suchen und installieren.
1. 
- Installiere die "Button2" Library
Download das folgene ZIP-Archiv (https://github.com/lennarthennigs/Button2/zipball/master), 
wähle unter "Sketch > Include Library > Add .ZIP Library..." den heruntergeladenen File aus um die Bibliothek zu verlinken.

1.  Tools -> Board -> "TTGO T1" auswählen.
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
1. Nach kurzer Zeit sollten auf dem Display die aktuellen Abfahrten angezeigt werden.
1. Zum Debuggen kann der Serielle Monitor unter Tools -> Serial Monitor aufgerufen werden.

## API:
Aktuell wird die MVG API verwendet. Diese liefert Infos für alle Verkehrsmittel der MVG (Bus/Sbahn/Ubahn/Tram). Sie stellt die Grundlage für die MVG Website sowie die MVG Naviagtor App dar. Allerdings sind die Echtzeitinformationen besonders bei Störungen nicht optimal. Aus diesem Grund gibt es einen neuen Branch der sich mit der geops-API, die unteranderem in der München Navigator APP oder auf s-bahn-muenchen-live.de zum Einsatz kommt, beschäftigt. Hier bekommt man sehr genaue Echtzeitinformationen, allerdings ausschließlich für die Sbahn.
