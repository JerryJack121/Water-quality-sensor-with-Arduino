#include <Arduino.h>
#include "wiring_private.h"
//#include "SoftwareSerial.h"

unsigned char item[8] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x39}; //16進位制測溫命令String data = ""; // 接收到的16進位制字串
//SoftwareSerial pHSerial(8, 7);                                            // RX, TX
Uart pHSerial (&sercom0, 5, 6, SERCOM_RX_PAD_1, UART_TX_PAD_0);
int DE_RE = 3;

float getpH(String pH); // 函數宣告

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
}
void loop()
{
  delay(2000);
  Serial.println("開始執行");
    String data = "";
    digitalWrite(DE_RE, HIGH);
//    pHSerial.write(item[0]); // write輸出
//     delay(10) ;
//    pHSerial.write(item[1]); // write輸出
//    delay(10) ;
    for (int i = 0; i < 8; i++)
    {   
                              // 傳送測溫命令
        pHSerial.write(item[i]); // write輸出
        Serial.print(item[i], HEX);
        delay(10) ;
    }
    Serial.println();
    Serial.println("寫入");
    digitalWrite(DE_RE, LOW);
    Serial.println("讀取資料");
    while (pHSerial.available()) //從串列埠中讀取資料
    {
        Serial.println("讀到值了!");
        unsigned char in = (unsigned char)pHSerial.read(); // read讀取
        Serial.print(in, HEX);
        Serial.print(",");
        data += in;
        data += ',';
    }
    Serial.println();
    Serial.print("鹽度:");
    Serial.println(data);
//    Serial.println(getpH(data));
    Serial.println("---------------");
}

//解讀
float getpH(String pH)
{
    int commaPosition = -1;
    String info[9]; // 用字串陣列儲存
    Serial.println(pH);
    for (int i = 0; i < 11; i++)
    {
        commaPosition = pH.indexOf(',');
        if (commaPosition != -1)
        {
            info[i] = pH.substring(0, commaPosition);

            pH = pH.substring(commaPosition + 1, pH.length());
        }
        else
        {
            if (pH.length() > 0)
            { // 最後一個會執行這個
                info[i] = pH.substring(0, commaPosition);
            }
        }
    }

    return (info[3].toInt() * 256 + info[4].toInt() * 16);
}
