#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "../config.h"

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266! (blinking LED)");
  int led_state = 0;
  for (int ii=0;ii<10;ii++){
    digitalWrite(LED_BUILTIN,led_state);
    led_state = !led_state;
    delay(100);
  }
}

void led_on(){
  server.send(200, "text/plain", "LED on");
  digitalWrite(LED_BUILTIN,LOW);
}

void led_off(){
  server.send(200, "text/plain", "LED off");
  digitalWrite(LED_BUILTIN,HIGH);
}

void setup(void){
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  WiFi.begin(SSID, PASSWRD);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot); // what happens in the root
  server.on("/on",led_on); // turn led on from webpage /on
  server.on("/off", led_off); // turn led off from webpage /off

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
