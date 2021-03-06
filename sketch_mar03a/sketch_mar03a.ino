#include<String.h>
#include "ESP8266.h"  //引入WiFi库。
#include <SoftwareSerial.h>
#include "HX711.h"

// 配置软串口传送二维码数据，tx 6, rx 5;
SoftwareSerial softSerial(6,5);

// 函数声明
void setColor(int redValue, int blueValue, int greenValue);
String mysubstr(int a,int b,String s);
String mysplit(String s,char target);

// led引脚
int red = 10;
int blue = 9;
int green = 8;

// 初始化WiFi的RX,TX引脚，此处使用的是实体串口，而不是软串口。
ESP8266 wifi(Serial);

String inputString = "";
int sendLength = 0;
//String Weight = "";
float Weight = 0;
void setup() {
    // put your setup code here, to run once:

    // led
    pinMode(red,OUTPUT);
    pinMode(blue,OUTPUT);
    pinMode(green,OUTPUT);
    setColor(255, 255, 255); // 灭灯 
    
    Serial.begin(115200);
    softSerial.begin(9600);

    Init_Hx711(); 
    delay(100);
    Get_Maopi();
    
    // Serial.println("AT+CIPSTART=\"TCP\",\"www.rexjoush.com\",5001");
    Serial.println("AT+CIPSTART=\"TCP\",\"121.4.127.5\",5001");
    Serial.flush();
    delay(2000);
    
}

void loop() {
    // put your main code here, to run repeatedly:
   
    //读取二维码信息
    while(softSerial.available()){
        inputString += char(softSerial.read());
        delay(2); 
    }
    
    //Serial.println(inputString);
    // 获取读取的长度
    sendLength = inputString.length();

    // 读取到了二维码数据，直接上传
    if (sendLength > 0){

        //获取重量传感器数据
        //Weight = String(Get_Weight()/100);
        Weight = Get_Weight()/1000;
        //拼接数据
        inputString = "W" + (String)Weight + "=" + inputString;
        //向服务器发送二维码
        Serial.print("AT+CIPSEND=");
        Serial.println(inputString.length());
        delay(100);   
        //if(Serial.find(">")) 
        //{ 
            Serial.println(inputString);
        //} 
             
        inputString = "";
        wifi.rx_empty();

        // 接受返回数据
        String receive = mysplit(wifi.recvString("=",10000),'=');
        //String receive = wifi.recvString("=",10000);
        //char response[1024];
        //int res_length = wifi.recvPkg(response, 1024, 1024, 10000, 0);
//
//        String res_string = response;
//      
        // Serial.println("resp: " + receive);
        delay(100);
//        if(res_length > 0){
//            Serial.println(res_string);
//        }
        if (receive == "g"){ // 若为个人信息码
            // 亮绿灯
            setColor(0, 255, 0); 
            delay(1000);
            setColor(255, 255, 255); // 灭灯
        }else if (receive == "n"){ // 是库存商品
            // 亮黄灯
            setColor(255, 255, 0); 
            delay(1000);
            setColor(255, 255, 255); // 灭灯
        }else if (receive == "r"){ // 不是库存商品亮红灯
            // 亮红灯
            setColor(0, 255, 255); 
            delay(1000);
            setColor(255, 255, 255); // 灭灯
        }
        else{
            setColor(255, 255, 255); // 灭灯
        }
        receive = ""; 
    }
    
  
    
}

// 颜色设置函数
void setColor(int redValue, int blueValue, int greenValue) {
  analogWrite(red, redValue);
  analogWrite(blue, blueValue);
  analogWrite(green, greenValue);
}
// 分割字符串，以target字符分割，返回‘=’后面的字符串
String mysplit(String s,char target){
    int middle;
    int sum;
    for (int i = 0; s[i] != '\0'; i++){
        if (target == s[i]){
            middle=i+1;
        }
        sum=i;
    }
    return mysubstr(middle,sum,s);
}

// 分割字符串，获取字符串从a到b位置的字串并返回
String mysubstr(int a,int b,String s){
    String result;
    for (int i = a; i <=b; ++i){
        result+=s[i];
    }
    return result;
}
