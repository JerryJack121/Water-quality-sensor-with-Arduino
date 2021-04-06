#include <SPI.h>
#include <WiFiNINA.h>
#include <Arduino.h>
#include "wiring_private.h"


// 設定網路基地台SSID跟密碼
const char* ssid      = "Jack的zenfone5z";
const char* password  = "b10500042";
int status = WL_IDLE_STATUS;     // the Wifi radio's status

// 請更換成 Thing Speak WRITE API KEY
const char* apiKey = "L81YAAPKTNFAS5QG";
const char* resource = "/update?api_key=";

// Thing Speak API server 
const char* server = "api.thingspeak.com";

// 設定虛擬序列埠
Uart pHSerial (&sercom0, 5, 6, SERCOM_RX_PAD_1, UART_TX_PAD_0);
int DE_RE = 3;  // 致能腳位

// 設定虛擬序列埠
void SERCOM0_Handler()
{
    pHSerial.IrqHandler();
}

// 函數宣告
float getpH(String pH); 
float command(unsigned char item[]);

void setup() {
  // Initializing serial port for debugging purposes
  pinPeripheral(5, PIO_SERCOM_ALT);
  pinPeripheral(6, PIO_SERCOM_ALT);
  Serial.begin(9600);
  pHSerial.begin(9600);
  pinMode(DE_RE, OUTPUT);
  //  digitalWrite(DE_RE,LOW);    
//  while (!Serial) ; // 等待序列埠連線
  Serial.println("序列埠連線完成");

  Serial.println();
  Serial.println();


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
  
  // 命令字串
  unsigned char item1[] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x71, 0xCB};
  unsigned char item2[] = {0x02, 0x04, 0x00, 0x00, 0x00, 0x02, 0x71, 0xF8};
  unsigned char item3[] = {0x03, 0x04, 0x00, 0x00, 0x00, 0x02, 0x70, 0x29};
  unsigned char item4[] = {0x04, 0x04, 0x00, 0x06, 0x00, 0x02, 0x91, 0x9F};

  float ph, con, do_, orp;
  
  Serial.println("\nPH值");
  ph = command(item1);
  delay(2000);
  
  Serial.println("\n鹽度CON");
  con = command(item2);
  delay(2000);
  Serial.println("\nDO溶氧量");
  do_ = command(item3);
  delay(2000);
  Serial.println("\nORP電極");
  orp = command(item4);
  delay(2000);

//  連線到server
  Serial.print("Connecting to "); 
  Serial.println(server);
  
  WiFiClient client;
// 使用 80 Port 連線
  if (client.connect(server, 80)) {
    Serial.println(F("connected"));
  } 
  else  {
    Serial.println(F("connection failed"));
    return;
  }
// 上傳thingspeak
  client.print(String("GET ") + resource + apiKey +
                "&field1=" + ph + 
                "&field2=" + con +
                "&field3=" + do_ +
                "&field4=" + orp +
                  " HTTP/1.1\r\n" +
                  "Host: " + server + "\r\n" + 
                  "Connection: close\r\n\r\n");
  
  delay(15000); // thingspeak每筆資料間隔至少15秒
  client.flush();   // No Socket available
  client.stop();
  }


// 傳送指令給感測器，並呼叫getdata解讀回傳值
float command(unsigned char item[]){
    String ans = "";
    unsigned long runTime1, runTime2;
    int count = 0;
    
    Serial.println("---------------");
    //高電位-寫入模式
    digitalWrite(DE_RE, HIGH);
    Serial.println("寫入");

    // 傳送測溫命令
    for (int i = 0; i < 8; i++)
    {                     
        pHSerial.write(item[i]); // 依序寫入指令
        Serial.print(item[i], HEX);
        delay(10) ;
    }
    Serial.println();
    
    //高電位-讀取模式
    digitalWrite(DE_RE, LOW);
    Serial.println("讀取");
   
     runTime1 = millis();      //讀取 Arduino 板執行時間
     
  // 等待感測器回應，若太久沒有回應則退出
    while(!pHSerial.available())
    {
      runTime2 = millis();
      if((runTime2-runTime1)>= 3000)
        break;
    }
    // 讀取感測器回傳指令
    while (pHSerial.available()) 
    { 
        count += 1;
        unsigned char in = (unsigned char)pHSerial.read(); 
//        Serial.print(in, HEX);
        ans += in;
        ans += ',';
        delay(100);
    }
    Serial.println(ans);
    // 判斷感測器是否有回應   
    if (count != 0){
      if (count!=(5+2*(int(item[5])))) // 若回傳長度不對，則回傳錯誤
      {
        Serial.println("回傳長度錯誤");
        return command(item);
      }
      else{
//            Serial.println("解讀數值");
           float info = getdata(ans);
            Serial.println(info);
            return info;
          }
    }
    else
    {
      Serial.println("感測器無回應");
//      return -1.0;
      return command(item);
    }
    
  }

//解讀回傳指令
float getdata(String ans)
{
    int commaPosition;
    String info[9]; // 用字串陣列儲存
    for (int i = 0; i < 11; i++)
    {
        commaPosition = ans.indexOf(','); // 尋找字串中逗號的位置
        if (commaPosition != -1)
        {
            info[i] = ans.substring(0, commaPosition);
            ans = ans.substring(commaPosition + 1, ans.length());
        }
    }
    if (info[2].toInt() == 2)
      return (info[3].toInt() * 256 + info[4].toInt() * 16);
    else if (info[2].toInt() == 4){
      Serial.print("小數點位數:");
      Serial.println(info[5].toInt());
      Serial.print("單位代號:");
      Serial.println(info[6].toInt(),HEX);
      return ((info[3].toInt() * 256 + info[4].toInt())/pow(10, info[5].toInt()));
      }
}
