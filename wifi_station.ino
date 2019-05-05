#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>


#define FIREBASE_HOST "arduino-8c50c.firebaseio.com"   
#define FIREBASE_AUTH "J7zkUrrpTavPoK2JohrHWx2x6YGdOqeSOpNn0q5j"   
#define WIFI_SSID "Dong123"
#define WIFI_PASSWORD "123456777"

SoftwareSerial NodeMCU(D2,D3);

//Define FirebaseESP8266 data object
FirebaseData firebaseData;
float temp_setup,humi_setup, h, t;
byte hour, minute;

void setup() {

  // Thiết lập truyền dữ liệu nối tiếp ở tốc độ 115200 bits/s
  Serial.begin(115200);
  NodeMCU.begin(115200);

  pinMode(D2,INPUT);
  pinMode(D3,OUTPUT);

  delay(10);
  Serial.print("Connecting to ");

  // Thiết lập ESP8266 ở chế độ station và kết nối đến mạng wifi đã chỉ định
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // Đoạn code in ra dấu . nếu ESP8266 chưa được kết nối
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // In ra dòng "WiFi connected" và địa chỉ IP của ESP8266
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

}
void loop() {
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  //Lay nhiet do do am tu arduino
  if(NodeMCU.available()<=0)Serial.println("Ko duoc");
  if(NodeMCU.available()>0)
  {
      h = NodeMCU.parseFloat();
      t = NodeMCU.parseFloat();
      hour = NodeMCU.parseInt();
      minute = NodeMCU.parseInt();
    
      Serial.println(h);
      Serial.println(t);
      Serial.println(hour);
      Serial.println(minute);
  }
  
  delay(1000);

  // Lay du lieu tu Firebase
  if (Firebase.getFloat(firebaseData, "/setup/temperature"))
    {
      Serial.println("----------Get result-----------");
      temp_setup = firebaseData.floatData();
      
    }
    else
    {
      Serial.println("----------Can't get data--------");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("--------------------------------");
      Serial.println();
    }
  if (Firebase.getFloat(firebaseData, "/setup/humidity"))
    {
      Serial.println("----------Get result-----------");
      humi_setup = firebaseData.floatData();

    }
    else
    {
      Serial.println("----------Can't get data--------");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("--------------------------------");
      Serial.println();
    }
  //Serial.println(temp_setup);
  //Serial.println(humi_setup);
}

