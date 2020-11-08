//#include <SPI.h>
#include <ESP8266WiFi.h>

/*
 * Hardcode SSID and Password for prototyping
 * SPI interrupt on CS high
 * Read 1 byte
 * Blink that many times
 * 
 * MOSI: GPIO13
 * MISO: GPIO12
 * CLK: GPIO14
 * CS: GPIO15
 */
const char HOME_SSID[26] = "SSID OF NETWORK";
const char HOME_PASS[23] = "PASS OF NETWORK";

//ICACHE_RAM_ATTR void SPI_cs_int {
//  
//}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);
  Serial.println();
  
  //attachInterrupt(digitalPinToInterrupt(15), SPI_cs_int, FALLING)
  WiFi.begin(HOME_SSID, HOME_PASS);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
}
