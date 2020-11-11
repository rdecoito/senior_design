//#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

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
const char HOME_SSID[] = "SSID";
const char HOME_PASS[] = "PASSWORD";

const char TARGET_IP[] = "10.0.0.15";
const int TARGET_PORT = 8765;

const int LOCAL_PORT = 9876;

WiFiUDP UDP;

char readBuf[1024];

//ICACHE_RAM_ATTR void SPI_cs_int {
//  
//}

void setup() {
  int pktSuccess;
  int pktLength = 0;
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  Serial.println();
  
  //attachInterrupt(digitalPinToInterrupt(15), SPI_cs_int, FALLING)
  WiFi.begin(HOME_SSID, HOME_PASS);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  digitalWrite(LED_BUILTIN, LOW);
  Serial.println('\n'+WiFi.localIP().toString());

  Serial.printf("\nSending data: 'Hello!' to %s:%d\n", TARGET_IP, TARGET_PORT);
  UDP.begin(LOCAL_PORT);
  UDP.beginPacket(TARGET_IP, TARGET_PORT);
  UDP.write("Hello!");
  pktSuccess = UDP.endPacket();

  if (pktSuccess) {
    Serial.println("Packet send success!");
  } else {
    Serial.println("Packet send failure");
  }

  while (!pktLength) {
    pktLength = UDP.parsePacket();
  }
  UDP.read(readBuf, 1024);
  Serial.printf("Received %d bytes:\n%s", pktLength, readBuf);
}

void loop() {
  
}
