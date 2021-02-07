/************************************************
                      step motor
*************************************************/
int step = 0; //GPIO0---D3 of Nodemcu--Step of stepper motor driver
int dir  = 2; //GPIO2---D4 of Nodemcu--Direction of stepper motor driver
int enable = 4;//GPIO2---D2 enable pin
int maxSteps = 600;
int minSteps = 1;
int delaySteps = 2;//2 is max speed
//int _step = 1; // rotate direction.
bool rotating = false;
String steps = "50";

void go(int steps, int inputDirection){
    if(steps > maxSteps) steps = maxSteps;
    if(steps < minSteps) steps = minSteps;

    digitalWrite(dir, inputDirection);//set direction
    enableDriver();//to prevent both motor and driver from overheat disable it after done
    for(int i = 0; i < steps; i++){
      digitalWrite(step, HIGH);
      delay(delaySteps);
      digitalWrite(step, LOW);
      delay(delaySteps);
    }
    disableDriver();
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
const char* password = "YourPass";

ESP8266WebServer server(80);



/*const char index_html[] PROGMEM = R"rawliteral(*/
String ind = "<!DOCTYPE HTML><html>"
"<head>"
"  <title>ESP Web Server</title>"
"  <meta name='viewport' content='width=device-width, initial-scale=1'>"
"  <style>"
"    html {font-family: Arial; display: inline-block; text-align: center;}"
"    h2 {font-size: 2.6rem;}"
"    body {max-width: 600px; margin:0px auto; padding-bottom: 10px;}"
"    .switch {position: relative; display: inline-block; width: 120px; height: 68px} "
"    .switch input {display: none}"
"    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}"
"    .slider:before {position: absolute; content: ''; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}"
"    input:checked+.slider {background-color: #2196F3}"
"    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}"
"  </style>"
"</head>"
"<body>"
"  <h2>ESP Web Server</h2>"
"  <button onclick='logoutButton()'>Logout</button>"
"<p><label class='switch'><input type='checkbox' onchange='toggleCheckbox(this)' id='output'><span class='slider'></span></label></p><input value='50' class='steps' type='text' onkeypress='return event.charCode >= 48 &amp;&amp; event.charCode <= 57' style='width: 300px;font-size: 50px;'>"
"<script>function toggleCheckbox(element) {"
"  var xhr = new XMLHttpRequest();"
"  if(element.checked){ xhr.open(\"GET\", \"/update?direction=1&steps=\" + document.getElementsByClassName(\"steps\")[0].value, false)}"
"  else { xhr.open(\"GET\", \"/update?direction=0&steps=\" + document.getElementsByClassName(\"steps\")[0].value, false); }"
"  xhr.send();"
"}"
"function logoutButton() {"
"  var xhr = new XMLHttpRequest();"
"  xhr.open(\"GET\", \"/logout\", true);"
"  xhr.send();"
"  setTimeout(function(){ window.open(\"/logged-out\",\"_self\"); }, 1000);"
"}"
""
"</script>"
"</body>"
"</html>";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    buttons+= "<p><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"output\"><span class=\"slider\"></span></label></p><input value=\"50\" class=\"steps\" type=\"text\" onkeypress=\"return event.charCode >= 48 &amp;&amp; event.charCode <= 57\" style=\"width: 300px;font-size: 50px;\">";
    return buttons;
  }
  return String();
}


void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}


void setup(void) {
  Serial.begin(115200);

  pinMode(enable, OUTPUT);
  pinMode(step, OUTPUT); //Step pin as output
  pinMode(dir,  OUTPUT); //Direcction pin as output
  digitalWrite(step, LOW); // Currently no stepper motor movement
  digitalWrite(dir, LOW);  
  disableDriver();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/",  [](){
    server.send(200, "text/html",ind );
  });

   server.on("/update",  [] () {
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    
    if (server.arg("direction") != "") {
      String inputDirection = server.arg("direction");
      String inputSteps = server.arg("steps");
      Serial.println("direction - " + inputDirection + " steps - " + inputSteps);
      //_step = inputDirection.toInt();
      int count = inputSteps.toInt();
      rotating = true;
      
      //digitalWrite(dir, _step); //Rotate stepper motor in clock wise direction
      go(count, inputDirection.toInt());
    }
    server.send(200, "text/plain", "OK");     
  });
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
