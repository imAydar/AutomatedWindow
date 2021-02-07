/*********
  Руи Сантос (Rui Santos)
  Более подробно о проекте на: http://randomnerdtutorials.com  
*********/

// подключаем библиотеку «ESP8266WiFi»:
#include <ESP8266WiFi.h>
#include "DHT.h"

// оставляем раскомментированной строчку, 
// соответствующую модели вашего датчика:
//#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321  float h = dht.readHumidity() считываем температуру в Цельсиях (по умолчанию): float t = dht.readTemperature();

// вписываем здесь данные для своей WiFi-сети:
const char* ssid = "TakieDela";
const char* password = "20978686";

// веб-сервер на порте 80:
WiFiServer server(80);

// датчик DHT:
const int DHTPin = 4;
// инициализируем датчик DHT:
DHT dht(DHTPin, DHTTYPE);

// временные переменные:
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];

// этот блок будет запускаться только при загрузке ESP:
void setup() {
  // инициализируем последовательный порт (в отладочных целях):
  Serial.begin(115200);
  delay(10);

  dht.begin();
  
  // подключаемся к WiFi-сети:
  Serial.println();
  Serial.print("Connecting to ");  //  "Подключаемся к "
  Serial.println(ssid); 
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
             //  "Подключение к WiFi выполнено"
  
  // запускаем веб-сервер:
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
              // "Веб-сервер запущен. Ждем IP-адрес ESP..."
  delay(10000);
  
  // печатаем IP-адрес ESP:
  Serial.println(WiFi.localIP());
}

// этот блок будет запускаться снова и снова:
void loop() {
  // начинаем прослушку новых клиентов:
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New client");  //  "Новый клиент"
    // создаем переменную типа «boolean»,
    // чтобы определить конец HTTP-запроса:
    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n' && blank_line) {
            // данные от датчика могут запаздывать на 2 секунды
            // (это очень медленный датчик):
           
            // считываем температуру в Фаренгейтах
            // (isFahrenheit = true):
            float f = dht.readTemperature(true);
            // проверяем, прочитались ли данные от датчика, 
            // и если нет, то выходим и начинаем заново:
            if (isnan(h) || isnan(t) || isnan(f)) {
              Serial.println("Failed to read from DHT sensor!");
                          // "Не удалось прочитать 
                          // данные от датчика DHT"
              strcpy(celsiusTemp,"Failed");
              strcpy(fahrenheitTemp, "Failed");
              strcpy(humidityTemp, "Failed");         
            }
            else{
              // рассчитываем градусы в Цельсиях и Фаренгейтах,
              // а также влажность:
              float hic = dht.computeHeatIndex(t, h, false);       
              dtostrf(hic, 6, 2, celsiusTemp);             
              float hif = dht.computeHeatIndex(f, h);
              dtostrf(hif, 6, 2, fahrenheitTemp);         
              dtostrf(h, 6, 2, humidityTemp);

              // все эти Serial.print() ниже можно удалить,
              // т.к. они для отладочных целей:
              Serial.print("Humidity: ");  //  "Влажность: "
              Serial.print(h);
              Serial.print(" %\t Temperature: ");  //  "Температура: "
              Serial.print(t);
              Serial.print(" *C ");
              Serial.print(f);
              Serial.print(" *F\t Heat index: ");
                             //  "Тепловой индекс: "
              Serial.print(hic);
              Serial.print(" *C ");
              Serial.print(hif);
              Serial.print(" *F");
              Serial.print("Humidity: ");  //  "Влажность: "
              Serial.print(h);
              Serial.print(" %\t Temperature: ");  //  "Температура: "
              Serial.print(t);
              Serial.print(" *C ");
              Serial.print(f);
              Serial.print(" *F\t Heat index: ");
                             //  "Тепловой индекс: "
              Serial.print(hic);
              Serial.print(" *C ");
              Serial.print(hif);
              Serial.println(" *F"); 
            }
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            // веб-страница, отображающая температуру и влажность:
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head></head><body><h1>ESP8266 - Temperature and Humidity</h1><h3>Temperature in Celsius: ");
            client.println(celsiusTemp);
            client.println("*C</h3><h3>Temperature in Fahrenheit: ");
            client.println(fahrenheitTemp);
            client.println("*F</h3><h3>Humidity: ");
            client.println(humidityTemp);
            client.println("%</h3><h3>");
            client.println("</body></html>");     
            break;
        }
        if (c == '\n') {
          // если обнаружен переход на новую строку:
          blank_line = true;
        }
        else if (c != '\r') {
          // если в текущей строчке найден символ: 
          blank_line = false;
        }
      }
    }  
    // закрываем соединение с клиентом:
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
               //  "Клиент отключен."
  }
}
