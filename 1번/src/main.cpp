#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

int mode;
const char*   ssid = "iptime";
const char*   password = "jhs2031%";

ESP8266WebServer server(80);

void on()
{
  mode = 1; 
  digitalWrite(15, mode);
  String message = "relay is on\n";
  server.send(404, "text/plain", message);
}

void off()
{
  mode = 0;
  digitalWrite(15, mode);
  String message = "relay is off\n";
  server.send(404, "text/plain", message);
}

void toggle() 
{
  mode = !mode;
  digitalWrite(15, mode);
  String message = "relay is toggled\n";
  server.send(404, "text/plain", message);
}

void handleRoot(){
  String message = (server.method() == HTTP_GET)?"GET":"POST";
  message += " " + server.uri() + "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + " : " + server.arg(i) + "\n";
  }
  message += "\nHello from ESP8266!\n";
  server.send(200, "text/plain", message);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  server.send(404, "text/plain", message);
}

void setup() {
  pinMode(15, OUTPUT);
  Serial.begin(115200);
  while(!Serial);

//WAIT FOR CONNECTION
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting...");

  while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to "); Serial.println(ssid);
  Serial.println("IP address : "); Serial.println(WiFi.localIP());

  if (MDNS.begin("정재영")) {
      Serial.println("MDNS rsponder started");
  }

  server.on("/", handleRoot);
  server.on("inline", [](){
      server.send(200, "text/plain", "Hello from the inline function\n");
  });

  server.on("/on", on);
  server.on("/off", off);
  server.on("/toggle", toggle);
  
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  MDNS.update();
  server.handleClient();
}