
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <WiFi.h>
#include "HTTPClient.h"
#include "Arduino_JSON.h"
#include "Roadway50pt7b.h"
#include "FreeSerif20pt7b.h"
#include "FreeSerif12pt7b.h"


//Setup TFT display
#define TFT_DC 4
#define TFT_CS 5
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//Setup Internet
WiFiClient wifi;
HTTPClient http;
String jsonBuffer;

const char* ssid = "jrkdwk";
const char* password = "xxnn349z";

void setup() {

    Serial.begin(115200);
    delay(10);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    tft.begin();
    tft.fillScreen(ILI9341_BLACK); //fill screen
    tft.setRotation(3); //rotate screen
}

  void loop() {
  
      if (WiFi.status() == WL_CONNECTED) {
      String serverPath = "http://strathmore.duckdns.org:1880/lake2";

      jsonBuffer = httpGETRequest(serverPath.c_str());
      JSONVar data = JSON.parse(jsonBuffer);

      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(data) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      
//      Serial.println(data); // print to serial for debugging
      
      String time[5];
      String icon[5];
      int hour[5];
      int lake_temp = data["Clear Lake"];
      const char* currently = data["Currently"];

      for (int j=0; j<5; j++) {
        hour[j] = data["hourly"][j]["temp"];
        time[j] = data["hourly"][j]["time"];
        icon[j] = data["hourly"][j]["icon"];
      }

// Output Current Temp and Conditions
//      tft.fillScreen(ILI9341_BLACK);
      tft.setFont(&Roadway50pt7b);
      tft.setCursor(25, 100); //(x,y)
      tft.setTextColor(0x435c); // RGB565 hex color code
      tft.drawRect(25, 10, 150, 100, 0x0000);

      tft.print(lake_temp);
//      tft.println(" F");
      tft.setCursor(110, 95);
      tft.println("o"); // char(247) if degree symbol in fornt library
      tft.setCursor(25,150);
      tft.setFont(&FreeSerif20pt7b); // return to default font: setFont()
      tft.drawRect(25, 150, 150, 50, 0x0000);
      tft.println(currently);

// draw icon

//      drawBitmap(65, 70, clear_day, 195, 146,GREEN);

// Output 5 Hour Forecast
      
      tft.setFont(&FreeSerif12pt7b);
      tft.drawRect(25, 180, 215, 60, 0x0000);
      for (int i = 0; i < 5; i++) {
        int x = 25 + 55 * i;
        tft.setCursor(x, 195); //was 190
        tft.print(hour[i]);
        tft.setCursor((x + 26), 187); // was 182
        tft.print("o");
        tft.setCursor(x,220);
        tft.print(time[i]);
        Serial.println(icon[i]);
      }
    }
    else {
      Serial.println("WiFi Disconnected");
    }

  Serial.println("re-load weather data every 10 minutes");
  delay(600000); // 10 minute delay
}

String httpGETRequest(const char* serverName) {

  // Your Domain name with URL path or IP address with path
  http.begin(wifi, serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void drawBitmap(int16_t x, int16_t y,
 const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++) {
      if(i & 7) byte <<= 1;
      else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if(byte & 0x80) tft.drawPixel(x+i, y+j, color);
    }
  }
 }
