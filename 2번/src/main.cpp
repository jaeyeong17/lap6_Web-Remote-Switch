#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

int           switch_pushed = 0;
const int     pushSW = 2;
const char*   ssid = "iptime";
const char*   password = "jhs2031%";

IRAM_ATTR void buttonClicked() {
  Serial.println("pushed");
  switch_pushed = 1;
}

void setup() {

  pinMode(pushSW, INPUT_PULLUP);
  attachInterrupt(pushSW, buttonClicked, FALLING);
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  //WAIT FOR CONNECTION

  while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to "); Serial.println(ssid);
  Serial.println("IP address : "); Serial.println(WiFi.localIP());

  if(MDNS.begin("정재영")) {
      Serial.println("MDNS responder started");
  }
}

void loop() {
  
  MDNS.update();
  WiFiClient client;
  HTTPClient http;

  if(switch_pushed == 1){
    Serial.println("[HTTP] begin...\n");
    if(http.begin(client, "http://192.168.0.25/")) {
      Serial.println("[HTTP] get...\n");
      int httpCode = http.GET();

      if(httpCode > 0) {
        Serial.printf("[HTTP] GET code: %d\n", httpCode);

        if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET failed, error : %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else { 
      Serial.println("[HTTP] Unable to connect\n");
      switch_pushed = 0;
    }
    delay(1000);
  }
}