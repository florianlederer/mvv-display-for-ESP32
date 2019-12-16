//Quellen: 
// https://github.com/Bodmer/TFT_eSPI/blob/master/examples/320%20x%20240/Free_Font_Demo/Free_Font_Demo.ino
// https://github.com/Xinyuan-LilyGO/TTGO-T-Display

#include <TFT_eSPI.h>
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include "Free_Fonts.h" // Include the header file attached to this sketchtft.setFreeFont(FSB9);


#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23

#define TFT_BL          4  // Display backlight control pin
#define ADC_EN          14
#define ADC_PIN         34
#define BUTTON_1        35
#define BUTTON_2        0

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library


//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{   
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

void drawTopLine()
{
    tft.fillRect(0, 0, 240, 8,TFT_YELLOW);
    tft.setTextFont(GLCD);
    tft.setTextColor(0x005, TFT_YELLOW);
    tft.drawString("Linie", 1, 0);
    tft.drawString("Ziel", 33, 0);
    tft.drawString("Gleis", 128, 0);
    tft.drawString("A", 169, 0);
    tft.drawString("B", 184, 0);
    tft.drawString("C", 200, 0);
    tft.drawString("Min", 220, 0);
}

void setupDisplay()
{   tft.init();
    tft.setRotation(3);
    //tft.fillScreen(TFT_BLACK);
    tft.fillScreen(0x005);
    if (TFT_BL > 0) { // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
         pinMode(TFT_BL, OUTPUT); // Set backlight pin to output mode
         digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }
}

void drawDepature(int display_line, String line, String destination, int track, int wagon, int minutes)
{
  char str_buffer[10];
  int line_height = 15;
  int offset = 9;
  int y_display = display_line*line_height+offset;
    
  tft.setFreeFont(FF17);
  tft.setTextDatum(TL_DATUM);

  //line
  tft.fillRoundRect(0, y_display, 28, 15, 7,TFT_MAGENTA);
  tft.setTextColor(0x005);
  tft.drawString(line, 3, y_display); 

  //destination
  tft.setTextColor(TFT_WHITE);
  tft.drawString(destination, 32, y_display);

  //track
  sprintf(str_buffer, "%u", track);
  tft.drawString(str_buffer, 150, y_display);

  //wagon
  tft.drawString("_ _ _", 167, y_display);
  switch(wagon)
  {
    case 3:
      tft.fillRoundRect(195, y_display+4, 14, 11, 3,TFT_WHITE); //rechts
    case 2:
      tft.fillRoundRect(165, y_display+4, 14, 11, 3,TFT_WHITE); //links
    case 1:
      tft.fillRoundRect(180, y_display+4, 14, 11, 3,TFT_WHITE); //mitte
      default:
        break;
  }

  //minutes
  tft.setTextDatum(TR_DATUM);
  sprintf(str_buffer, "%u", minutes);
  tft.drawString(str_buffer, 240,y_display);
  
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Start");
    setupDisplay();


}


void loop()
{   tft.fillScreen(0x005);
    delay(2000);
    drawTopLine();
    drawDepature(0, "S3", "Maisach", 1, 3, 1);
    drawDepature(1, "S3", "Holzkirchen", 1, 2, 10);
    drawDepature(2, "S3", "Deisenhofen", 1, 1, 100);
    drawDepature(3, "S3", "Maisach", 1, 3, 1);
    drawDepature(4, "S3", "Holzkirchen", 1, 2, 10);
    drawDepature(5, "S3", "Deisenhofen", 1, 1, 100);
    drawDepature(6, "S3", "Maisach", 1, 3, 1);
    drawDepature(7, "S3", "Holzkirchen", 1, 2, 10);
    delay(2000);
}
