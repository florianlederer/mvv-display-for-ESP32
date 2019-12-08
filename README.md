# mvv display for ESP32
Zeigt die nächsten Abfahrten der MVV auf einem kleinem OLED Display an. Es handelt sich hierbei um kein Projekt der MVV.
 
## Benötigtes:
- ESP32 mit OLED Display: https://www.amazon.de/dp/B076P8GRWV/ref=cm_sw_em_r_mt_dp_U_Md.6DbQCS5WA1
- Micro USB Kabel
- USB Netzteil
- Computer zum Programmieren
 
## Anleitung:
1.  Arduino IDE installieren: https://www.arduino.cc/en/main/software
2.  ESP32 Board installieren: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
3.  TOOLS -> Manage Libraries.. anklicken und nach der "ESP8266 and ESP32 OLED driver for SSD1306 displays" (ThingPulse) suchen und installieren. Anschließend nach der "Arduinojson" suchen und ebenfalls installieren.
4.  Tools -> Board -> "TTGO LoEa-32-OLED V1" auswählen.
5.  Tools ->  Port anschauen und die Enträge merken falls nicht ausgeblendet.
6.  ESP32 mit dem Kabel mit dem Computer verbinden und erneut unter Tools ->  Port schauen nun müsste ein neuer Port hinzugekommen sein/ Port ist nicht mehr ausgeblendet. Diesen nun auswählen.
7.  main.c aus diesem Repo kopieren und in der Arduino IDE einfügen.
8.  Sketch -> Verify/Compile und schauen ob irgendwelche Fehler in der Konsole auftreten. Gegebenfalls die Libraries überprüfen.
9. In den Zeilen const char* ssid = "XXX"; sowie const char* password =  "XXX"; die drei XXX jeweils durch WLAN Namen und Passwort ersetzten.
10. Die Seite https://www.mvg.de/dienste/abfahrtszeiten.html im Firefox Webbrowser öffnen und anschließend STRG + SHIFT + E drücken. Jetzt sollte ein neues Fenster aufgegangen sein welches den Netzwerkverkehr mitschneidet. 
11. Nun den gewünschten Bahnhof ganz normal eingeben und sich die aktuellen Abfahrten anzeigen lassen.
12. Im in Schritt 10 geöffneten Fenster bei Type nach dem/den Paketen mit json suchen und die Bahnhof nun in der Spalte File rauslesen.
13. Das was in File steht muss man nun in der Zeile const char* url =.... am Ende ersetzen.
14. Sketch -> Upload
15. Jetzt sollten auf dem Display die aktuellen Abfahrten der Sbahn angezeigt werden
16. Zum Debuggen kann der Serielle Monitor unter Tools -> Serial Monitor aufgerufen werden.

## Jodel:
[https://shared.jodel.com/58NiubGCf2?channel=other](https://shared.jodel.com/58NiubGCf2?channel=other) 
