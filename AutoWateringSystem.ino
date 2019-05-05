#include "DHT.h"
#include "Wire.h"
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define DS3231_I2C_ADDRESS 0x68
#define DHTPIN 10    
#define DHTTYPE DHT11   // DHT 11 
#define pinMotor  11
//#define pinRainsensor 6
#define waterflow 1.5
//-- VARIABLES initialize --

int Temperature =30;
int Moisture= 70;
float AmmountWater=0.5;
float temp_setup = 0;
float humi_setup = 0;
int a[5], c;

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial ArduinoUno(3,2);

byte check = 0;
int mark;
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

byte nowHour,nowMinute,nowSecond;
int out=LOW;

void setup() {
  Serial.begin(115200); 
  Serial.println("Đề tài : Hệ Thống Tưới Cây Tự Động -- Nhóm 18");
  lcd.begin(16, 2);
  pinMode(pinMotor, OUTPUT);
  dht.begin();
  Wire.begin();
  mark=0; 
  ArduinoUno.begin(115200);
}

void loop() {

  float H,T;
  int hour, minute;
  Readsensor(H,T);
  readDS3231time(minute,hour);
  SetupHT(H,T,hour,minute);     //Ham gui du lieu cho nodemcu
  
  DisplayonLCD(H,T);
  AutoWateringMode(H,T);
  Alarm(16,10);
  //SetupHT();
}

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void readDS3231time(byte & second,byte & minute,byte & hour,byte & dayOfWeek,byte & dayOfMonth,byte & month,byte & year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  second = bcdToDec(Wire.read() & 0x7f);
  minute = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read() & 0x3f);
  dayOfWeek = bcdToDec(Wire.read());
  dayOfMonth = bcdToDec(Wire.read());
  month = bcdToDec(Wire.read());
  year = bcdToDec(Wire.read());
}
void readDS3231time(byte & second,byte & minute,byte & hour)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  second = bcdToDec(Wire.read() & 0x7f);
  minute = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read() & 0x3f);
}

void readDS3231time(int & hour,int & minute)
{
  int second;
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  second = bcdToDec(Wire.read() & 0x7f);
  minute = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read() & 0x3f);
}
void displayTime()
{
  String Weekdays[]={"Chủ nhật","Thứ hai", "Thứ ba", "Thứ tư", "Thứ năm", "Thứ sáu", "Thứ bảy"};
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
  // send it to the serial monitor
  Serial.print(hour, DEC);  Serial.print(":");
  // convert the byte variable to a decimal number when displayed
  if (minute<10)  Serial.print("0");
  Serial.print(minute, DEC);  Serial.print(":");
  if (second<10) Serial.print("0");
  Serial.print(second, DEC);  Serial.print(" ");
  Serial.print(Weekdays[dayOfWeek-1]);Serial.print(", ");
  Serial.print(dayOfMonth, DEC);  Serial.print("/");
  Serial.print(month, DEC);  Serial.print("/");
  Serial.println(year, DEC); 
}
byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}
bool setTimer(byte hourSet, byte minuteSet)
{
  int hour, minute;
  readDS3231time(hour, minute);
  if(hourSet==hour && minuteSet==minute) return true;
  return false;
}

void Readsensor(float &H, float &T)
{
   H = dht.readHumidity();
   T = dht.readTemperature();
   
}
void AutoWateringMode(float & H, float & T)
{
   // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(T) || isnan(H))   {   return;  } 
   if(H < Moisture || T > Temperature ) 
     {
        digitalWrite(pinMotor,HIGH);   
     }
    else  digitalWrite(pinMotor,LOW);
}

void Alarm (int hour, int minute)
{

  if(setTimer(hour,minute)==true && check==0)
    {
      Serial.println("Bao thuc tuoi cay");
      out=HIGH;
      digitalWrite(pinMotor, out);   
      check=1;
      mark=0;
      }
     if(out==HIGH)
     {
      int t=roundf(AmmountWater*60/waterflow);
      if(mark==0) 
             {
              readDS3231time(nowSecond, nowMinute, nowHour);
              mark=1;
             }
       if(WateringTime(t,nowHour,nowMinute,nowSecond)==true)
                {
                  Serial.println("Dung tuoi cay");
                  out=LOW;
                  digitalWrite(pinMotor, out);
                  mark=0;
                }  }}
bool WateringTime(int t,byte startHour, byte startMinute, byte startSecond)
{
  byte finishHour,finishMinute,finishSecond;
  finishSecond=startSecond+t%3600%60;
  finishMinute=startMinute+t%3600/60+finishSecond/60;
  finishHour=(startHour+t/3600+finishMinute/60)%24;
  finishMinute%=60;
  finishSecond%=60;
  byte nowHour,nowMinute,nowSecond;
  readDS3231time(nowSecond, nowMinute, nowHour);
  if(finishHour==nowHour && finishMinute==nowMinute && finishSecond==nowSecond) return true;
  else return false;
}

void DisplayonLCD (float & H, float & T)
{
  //Display time
   byte second, minute, hour;
  readDS3231time(second, minute, hour);
  lcd.setCursor(4, 0);
  String strHour="",strMinute="",strSecond="";
   strHour+=hour;
   strMinute+=minute;
   strSecond+=second;
  if(hour<10) {strHour="";strHour=strHour+'0'+hour;}
  if(minute<10) {strMinute="";strMinute=strMinute+'0'+minute;}
  if(second<10) {strSecond="";strSecond=strSecond+'0'+second;}
  String time="";
  time=time+ strHour+':'+strMinute+':'+strSecond;
  lcd.print(time);
  //Display tempature and humidity
  lcd.setCursor(1, 1);
  String Index="";
  Index=Index+T+"*C"+' '+H+'%';
  lcd.print(Index); 
}

void SetupHT(float & H, float & T, int & minute, int & hour)
{
  //Hien tai: hour=45, minute=165 vi chua cam dong ho
   Serial.print("Do am: ");
   Serial.println(H);
   Serial.print("Nhiet do: ");
   Serial.println(T);
   Serial.print("Gio: ");
   Serial.print(hour);
   Serial.print("      phut: ");
   Serial.println(minute);
   ArduinoUno.print(H);
   ArduinoUno.println("h");
   ArduinoUno.print(T);
   ArduinoUno.println("h");
   ArduinoUno.print(hour);
   ArduinoUno.println("h");
   ArduinoUno.print(minute);
   ArduinoUno.println("h");
   ArduinoUno.flush();
   Serial.println("Da gui");
  
  delay(1000);
}


