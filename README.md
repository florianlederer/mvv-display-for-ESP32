# mvv display for ESP32
Zeigt abfahrten der MVV auf kleiem dispay.
 
## Benötigtes:
- ESP32 mit OLED Display: https://www.amazon.de/dp/B076P8GRWV/ref=cm_sw_em_r_mt_dp_U_Md.6DbQCS5WA1
- Micro USB Kabel
- USB Netzteil
- Computer zum Programmieren
 
## Anleitung:
1.  Arduino IDE installieren: https://www.arduino.cc/en/main/software
2.  ESP32 Board installieren: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
3.  TOOLS -> Manage Libraries.. anklicken und nach der "ACROBOTIC SSD1306" suchen und installieren. Anschließend nach der "Arduinojson" suchen und ebenfalls installieren.
4.  Tools -> Board -> "TTGO LoEa-32-OLED V1" auswählen
5.  Tools ->  Port anschauen und merken falls nicht ausgeblendet
6.  ESP32 mit dem Kabel mit dem Computer verbinden und erneut unter Tools ->  Port schauen nun müsste ein neuer Port hinzugekommen sein/ Port ist nicht mehr ausgeblendet. Diesen nun auswählen.
7.  Repository clonen und im Editor öffnen.
8.  Auf den Haken links oben klicken und schauen ob irgendwelche Fehler auftreten.
9. In den Zeilen const char* ssid = "XXX"; sowie const char* password =  "XXX"; die drei XXX jeweils durch WLAN Namen und Passwort ersetzten.
10. Die Seite https://www.mvg.de/dienste/abfahrtszeiten.html im Firefox Webbrowser öffnen aschliesend STRG + SHIFT + E drücken.
11. Nun sollte ein neues Fenster aufgegangen sein, welches den Netzwerkverkehr mitschneidet. Nun den gewünschten Bahnhof ganz noormal eingeben und sich die aktuellen Abfahrten anzeigen lassen.
12. Nun im in Schritt 10 geöffneten Fenster bei Type nach dem/den Paketen mit json suchen und die Bahnhof nun in der Spalte File rauslesen.
13. Das was in File steht muss man nun in der Zeile const char* url =.... am Ende ersetzen.
14. Nun Auf den Pfeil links oben  neben dem in Schritt 8 gedrücken haken drücken.
15. Jetzt sollten auf dem Display die aktuellen Abfahrten der Sbahn angezeigt werden

@OJ vom jodel: [https://shared.jodel.com/58NiubGCf2?channel=other](https://shared.jodel.com/58NiubGCf2?channel=other) erstell nen issue, dann übertrage ich dir das repo
