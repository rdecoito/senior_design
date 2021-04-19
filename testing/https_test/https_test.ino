//#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "config.h"

// test payload: {"IMID": "001","TPMID": "AAA","CPMID": "BBB","has_mm": true,"mm_error": false,"trans_error": false,"im_error": false,"timestamp": "606F34AD","temp": "0141"}

// The SSID and password of the desired WiFi network
#define TRGT_SSID "======== PLACE SSID HERE ==========="
#define TRGT_PASS "======== PLACE PASSWORD HERE ==========="

// Host and port of target endpoint for data
const char awsEndpt[] = "a37tcxmgqf266y-ats.iot.us-east-2.amazonaws.com";
const char endptUrl[] = "/topics/im/1/door_interaction?qos=0";
const int httpsPort = 8443;
const char payload[] = "{\"IMID\": \"001\",\"TPMID\": \"AAA\",\"CPMID\": \"BBB\",\"has_mm\": true,\"mm_error\": false,\"trans_error\": false,\"im_error\": false,\"timestamp\": \"606F34AD\",\"temp\": \"0141\"}";

BearSSL::X509List caCerts(rootCA1);
BearSSL::X509List clientCert(clientPublicCert);
BearSSL::PrivateKey clientKey(clientPrivateKey);
char clientByte;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);
  Serial.println("");



  // CONNECTING TO WIFI
  WiFi.begin(TRGT_SSID, TRGT_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected!\n    IP: ");
  Serial.println(WiFi.localIP());



  // TIME SYNCHRONIZATION
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(NULL);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(NULL);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));



  // Connecting to host and sending PUSH
  BearSSL::WiFiClientSecure client;
  client.setTrustAnchors(&caCerts);
  client.setClientRSACert(&clientCert, &clientKey);
  Serial.println("Attempting to connect");
  if (!client.connect(awsEndpt, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }
  Serial.println("connected");
  client.print(String("POST ") + endptUrl + " HTTP/1.1\r\n"
    + "Host: " + awsEndpt + "\r\n"
    + "User-Agent: BuildFailureDetectorESP8266\r\n"
    + "Connection: close\r\n"
    + "Content-Type: application/json\r\n"
    + "Content-Length: 157\r\n"
    + "\r\n"
    + payload + "\r\n");
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
    Serial.println("Communication success!");
  } else {
    Serial.println("Something failed :(");
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
