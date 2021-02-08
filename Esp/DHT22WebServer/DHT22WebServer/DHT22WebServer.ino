#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "DHT.h"


#define DHTTYPE DHT22 
const int DHTPin = 4;
DHT dht(DHTPin, DHTTYPE);

const char* ssid = "TakieDela";
const char* password = "";
IPAddress ip (192,168,0,178);
IPAddress dns (8, 8, 8, 8);
IPAddress gateway (192, 168, 0, 1);
IPAddress mask (255, 255, 255, 0);

ESP8266WebServer server(80);

void connectWifi(){
  int status = WL_IDLE_STATUS;
  //WiFi.config(ip, gateway, mask, dns, dns);
  // attempt to connect to Wifi network:
    WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, password);
    // wait 10 seconds for connection:
    delay(10000);
  }
  // print your WiFi shield's IP address:
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void handleRoot() {
  // данные от датчика могут запаздывать на 2 секунды
  // (это очень медленный датчик):
  float h = dht.readHumidity();
  // считываем температуру в Цельсиях (по умолчанию):
  float t = dht.readTemperature(); 
  server.send(200, "text/plain", "temp is " + String(t) + " humidity is " + String(h));
}

void handleTemp(){
  float t = dht.readTemperature(); 
  server.send(200, "text/plain", String(t));
}

void handleHumidity(){
  float h = dht.readHumidity();
  server.send(200, "text/plain", String(h));
}

void setUpServer(){
  server.on("/", handleRoot);
  server.on("/temp", handleTemp);
  server.on("/humidity", handleHumidity);
  server.begin();
  Serial.println("HTTP server started");
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  connectWifi();
  setUpServer();
  dht.begin();
}

void loop() {
  if((WiFi.status() != WL_CONNECTED))
    connectWifi();
  server.handleClient();
}
