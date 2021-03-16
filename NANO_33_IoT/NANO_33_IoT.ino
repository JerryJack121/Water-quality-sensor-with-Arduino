
#include <Arduino.h>
#include "wiring_private.h"


// 設定虛擬序列埠
Uart pHSerial (&sercom0, 5, 6, SERCOM_RX_PAD_1, UART_TX_PAD_0);
int DE_RE = 3;  // 致能腳位

// 函數宣告
float getpH(String pH); 
void command(unsigned char item[]);

// 設定虛擬序列埠
void SERCOM0_Handler()
{
    pHSerial.IrqHandler();
}


void setup()
{
    pinPeripheral(5, PIO_SERCOM_ALT);
    pinPeripheral(6, PIO_SERCOM_ALT);
    Serial.begin(9600);
    pHSerial.begin(9600);
    pinMode(DE_RE, OUTPUT);
    //  digitalWrite(DE_RE,LOW);    
    while (!Serial) ; // 等待序列埠連線
    Serial.println("序列埠連線完成");
}
void loop()
{
  // 命令字串
  unsigned char item1[] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x71, 0xCB};
  unsigned char item2[] = {0x02, 0x04, 0x00, 0x00, 0x00, 0x02, 0x71, 0xF8};
  unsigned char item3[] = {0x03, 0x04, 0x00, 0x00, 0x00, 0x02, 0x70, 0x29};
  unsigned char item4[] = {0x04, 0x04, 0x00, 0x06, 0x00, 0x02, 0x91, 0x9F};

  Serial.println("PH值");
  command(item1);
  delay(2000);
  Serial.println("鹽度CON");
  command(item2);
  delay(2000);
  Serial.println("DO溶氧量");
  command(item3);
  delay(2000);
  Serial.println("ORP電極");
  command(item4);
  delay(2000);
}

// 傳送指令給感測器，並呼叫getdata解讀回傳值
void command(unsigned char item[]){
    String ans = "";
    unsigned long runTime1, runTime2;
    int count = 0;
    
    //高電位-寫入模式
    digitalWrite(DE_RE, HIGH);
    Serial.println("寫入模式");

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
    Serial.println("讀取模式");
   
     runTime1 = millis();      //讀取 Arduino 板執行時間
     
  // 等待感測器回應，若太久沒有回應則退出
    while(!pHSerial.available()){
      runTime2 = millis();
//      Serial.println(runTime2-runTime1);
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
        delay(10);
    }
    Serial.println(ans);
    // 判斷感測器是否有回應   
    if (count != 0){
      if (count!=(5+2*(int(item[5])))) // 若回傳長度不對，則回傳錯誤
        Serial.println("回傳錯誤");
      else{
            Serial.println("解讀數值");
           float info = getdata(ans);
            Serial.println(info);
          }
    }
    else
      Serial.println("感測器無回應");
      
    Serial.println("---------------");
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
