#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <HTTPClient.h>

#define RST_PIN 32
#define SS_PIN 5
#define servo 13

char auth[] = "N1bvQQEmolnesfKtaoBjqH22HAsLAtTP";
//pass wifi
//char ssid[] = "AndroidAP_6995";
//char pass[] = "88888888";
char ssid[] = "PING⁹⁹⁹⁺";
char pass[] = "123456789@";
int value=0;
int dem=0;
//ket noi telegram
String botToken = "7827320526:AAGOSe2ixx8FIcehSuP25BwPlG40CO6Rkw4";  
String chatId = "5948248863"; 
HTTPClient http;

MFRC522 mfrc522(SS_PIN, RST_PIN);

TinyGPSPlus gps;
HardwareSerial mySerial(1);

boolean check=false;
boolean cb=false;
int speed=0;

ESP32PWM pwm;

String urlEncode(const char* msg) {
  const char *hex = "0123456789ABCDEF";
  String encodedMsg = "";

  while (*msg != '\0') {
    if (('a' <= *msg && *msg <= 'z') ||
        ('A' <= *msg && *msg <= 'Z') ||
        ('0' <= *msg && *msg <= '9')) {
      encodedMsg += *msg;
    } else {
      encodedMsg += '%';
      encodedMsg += hex[*msg >> 4];
      encodedMsg += hex[*msg & 15];
    }
    msg++;
  }
  return encodedMsg;
}

void Guitinnhan(String message) {
  String encodedMessage = urlEncode(message.c_str());
  String url = "https://api.telegram.org/bot" + botToken + 
               "/sendMessage?chat_id=" + chatId + 
               "&text=" + encodedMessage; 
  int httpCode = http.GET();
  http.begin(url);
  http.end();
}
void thietlap()
{
  if (mfrc522.PICC_IsNewCardPresent())
  {
   if(mfrc522.PICC_ReadCardSerial())
   {
     dem++;
     if (dem>3) dem=0;
     mfrc522.PICC_HaltA(); 
   }
  }
}
void setup() 
{
  pinMode(22,OUTPUT);
  //RIFD
  SPI.begin();
  mfrc522.PCD_Init();
  // Ket noi Blynk
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED);
  Blynk.config(auth,"blynk-server.com",8080);
  Blynk.connect();
 //GPS
  mySerial.begin(9600, SERIAL_8N1, 16, 17); 
  //Servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3); 
  pwm.attachPin(servo,50,10);
}
BLYNK_WRITE(V5)
{
  value = param.asInt(); 
  dem=value;
}

void loop() 
{
  Blynk.run();
while (mySerial.available()) {
  gps.encode(mySerial.read());  // Giải mã dữ liệu GPS
}
Blynk.virtualWrite(V4,gps.satellites.value());
if (gps.location.isUpdated()) {
  if(gps.location.isValid())
  {
  Blynk.virtualWrite(V0,1,gps.location.lat(),gps.location.lng(),"value");
  Blynk.virtualWrite(V1,gps.speed.kmph());
  Blynk.virtualWrite(V2,gps.location.lng());
  Blynk.virtualWrite(V3,gps.location.lat());
  Blynk.virtualWrite(V4,gps.satellites.value());
  speed=gps.speed.kmph();
  }
}
thietlap();

if(dem%2==0) 
{
  digitalWrite(22,0);Blynk.virtualWrite(V5,0);
  pwm.writeScaled(0.05);
  check=false;
  if(speed>5)
  {
    if(cb==false)
    {
    Guitinnhan("Xe di chuyển bất thường");
    cb=true;
    }
  }
}
else
 {
  digitalWrite(22,1);
  Blynk.virtualWrite(V5,1);
  pwm.writeScaled(0.1);
  if(check==false)
  {
    Guitinnhan("Đã mở khóa");
    check=true;
  }
  cb=false;
}
delay(100);
}
