#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include "config.h"

WiFiClientSecure client;
char clientByte;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);
  Serial.println("");

  WiFi.begin(TRGT_SSID, TRGT_PASS);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected!\n    IP: ");
  Serial.println(WiFi.localIP());

  client.setFingerprint(TRGT_SHA_FINGERPR);
  Serial.println("Attempting to connect");
  client.connect("gutenberg.org", 443);
  while (!client.connected()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected to gutenberg");
  client.println("GET  /files/60321/60321-h/60321-h.htm HTTP/1.0\n");
}

void loop() {
  if (client.available()) {
    clientByte = client.read();
    Serial.print(clientByte);
  }
}
