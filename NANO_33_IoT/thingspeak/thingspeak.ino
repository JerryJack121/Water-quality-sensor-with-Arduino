//#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <SPI.h>
#include <WiFiNINA.h>


// 設定網路基地台SSID跟密碼
const char* ssid      = "Jack的zenfone5z";
const char* password  = "b10500042";
int status = WL_IDLE_STATUS;     // the Wifi radio's status

// 請更換成 Thing Speak WRITE API KEY
const char* apiKey = "L81YAAPKTNFAS5QG";
const char* resource = "/update?api_key=";

// Thing Speak API server 
const char* server = "api.thingspeak.com";


void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(9600);
  delay(10);

  Serial.println("");
  Serial.println("");
  Serial.print("Connecting To: ");
  Serial.println(ssid);


  // 等待連線，並從 Console顯示 IP
  while (status != WL_CONNECTED) {

    Serial.print("Attempting to connect to WPA SSID: ");

    Serial.println(ssid);

    // Connect to WPA/WPA2 network:

    status = WiFi.begin(ssid, password);

    // wait 10 seconds for connection:

    delay(1000);
  }

  Serial.println("");
  Serial.println("WiFi Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  
  int data = 50;
  
  Serial.println(data);

//  連線到server
  Serial.print("Connecting to "); 
  Serial.print(server);
  WiFiClient client;
   // 使用 80 Port 連線
//  if (client.connect(server, 80)) {
//    Serial.println(F("connected"));
//  } 
//  else  {
//    Serial.println(F("connection failed"));
//    return;
//  }
//
//
//  client.print(String("GET ") + resource + apiKey + "&field1=" + data +
//                  " HTTP/1.1\r\n" +
//                  "Host: " + server + "\r\n" + 
//                  "Connection: close\r\n\r\n");
//  delay(15000); // thingspeak每筆資料間隔至少15秒
  
  }
