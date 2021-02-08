/************************************************
                      step motor
*************************************************/
int step = 0; //GPIO0---D3 of Nodemcu--Step of stepper motor driver
int dir  = 2; //GPIO2---D4 of Nodemcu--Direction of stepper motor driver
int enable = 4;//GPIO2---D2 enable pin
int maxSteps = 600;
int minSteps = 1;
int delaySteps = 5;//2 is max speed, 3 is not enough when it's windy
bool rotating = false;

void initializeDriver(){
  pinMode(enable, OUTPUT);
  pinMode(step, OUTPUT); //Step pin as output
  pinMode(dir,  OUTPUT); //Direcction pin as output
  digitalWrite(step, LOW); // Currently no stepper motor movement
  digitalWrite(dir, LOW);  
  disableDriver();
}
void go(int steps, int inputDirection){
    if(steps > maxSteps) steps = maxSteps;
    if(steps < minSteps) steps = minSteps;

    digitalWrite(dir, inputDirection);//set direction
    enableDriver();//to prevent both motor and driver from overheat disable it after done
    rotating = true;
    for(int i = 0; i < steps; i++){
      digitalWrite(step, HIGH);
      delay(delaySteps);
      digitalWrite(step, LOW);
      delay(delaySteps);
    }
    disableDriver();
    rotating = false;
}
void enableDriver(){
  digitalWrite(enable, LOW);
}
void disableDriver(){
  digitalWrite(enable, HIGH);
}
/******************************************
              web server
 ******************************************/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "TakieDela";
const char* password = "20978686";
IPAddress ip (192,168,0,177);
IPAddress dns (8, 8, 8, 8);
IPAddress gateway (192, 168, 0, 1);
IPAddress mask (255, 255, 255, 0);

ESP8266WebServer server(80);

String ind = "<!DOCTYPE html><html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1252\"><title>ESP Web Server</title><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><style>html{font-family:Arial;display:inline-block;text-align:center}h2{font-size:2.6rem}body{max-width:600px;margin:0 auto;padding-bottom:10px}button{border-radius:50%;width:70px;height:70px;background-color:#fff}.steps{width:100px;font-size:50px;margin-left:20px;text-align:right}</style></head><body><h2>Automated window</h2><p>Steps<input type=\"number\" value=\"50\" min=\"1\" max=\"600\" class=\"steps\" onkeypress=\"return event.charCode &gt;= 48 &amp;&amp; event.charCode &lt;= 57\"></p><div><button onclick=\"rotate(0)\">Open</button> <button onclick=\"rotate(1)\">Close</button></div><button onclick=\"hold()\">Hold</button><script>function rotate(t){var e=new XMLHttpRequest;e.open(\"GET\",\"/update?direction=\"+t+\"&steps=\"+document.getElementsByClassName(\"steps\")[0].value,!1),e.send()}function hold(){var t=new XMLHttpRequest;t.open(\"GET\",\"/hold\",!1),t.send()}</script></body></html>";

void connectWifi(){
  int status = WL_IDLE_STATUS;
  WiFi.config(ip, gateway, mask, dns, dns);
  // attempt to connect to Wifi network:
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    WiFi.begin(ssid, password);
    // wait 10 seconds for connection:
    delay(10000);
  }

  // print your WiFi shield's IP address:
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
}
void setUpServer(){
  server.on("/",  [] (){
    server.send(200, "text/html",ind );
  });
  
  server.on("/hold", [] (){
    enableDriver();
    server.send(200, "text/plain", "OK");    
  });

  server.on("/update", handleUpdate);
  
  server.begin();
  Serial.println("HTTP server started");
}

void handleUpdate() {
  if (server.arg("direction") != "") {
      String inputDirection = server.arg("direction");
      String inputSteps = server.arg("steps");
      Serial.println("direction - " + inputDirection + " steps - " + inputSteps);
      int count = inputSteps.toInt();  
      go(count, inputDirection.toInt());
    }
    server.send(200, "text/plain", "OK");     
}


void setup(void) {
  Serial.begin(115200);
  initializeDriver();
  connectWifi();
  setUpServer();
}

void loop(void) {
  if(WiFi.status() != WL_CONNECTED)
    connectWifi();
  server.handleClient();
  MDNS.update();
}
