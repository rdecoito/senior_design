//#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "config.h"
#include "certificates.h"

WiFiClientSecure client;
X509List certList(certificates);
char clientByte;
const char* host = "gutenberg.org";

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);
  Serial.println("");

  WiFi.begin(TRGT_SSID, TRGT_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected!\n    IP: ");
  Serial.println(WiFi.localIP());

  // TIME SYNCHRONIZATION
  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));

  // 
  client.setTrustAnchors(&certList);
  Serial.println("Attempting to connect");
  if !(client.connect(host, 443)) {
    Serial.println("Connection failed");
    return;
  }
  Serial.println("connected to gutenberg");
  client.println("GET  /files/60321/60321-h/60321-h.htm HTTP/1.1\r\n"
    + "Host: " + host + "\r\n" +
    "User-Agent: BuildFailureDetectorESP8266\r\n" +
    "Connection: close\r\n\r\n");
  Serial.println("Request sent");


  Serial.println("Request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("Headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("Reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("Closing connection");
}

void loop() {
//  if (client.available()) {
//    clientByte = client.read();
//    Serial.print(clientByte);
//  }
}
