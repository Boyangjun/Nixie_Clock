//lby
#include <ShiftRegister74HC595.h>
#include "WiFiEsp.h"
#include "WiFiEspUdp.h"

#include<Adafruit_NeoPixel.h>
#define PIN 8

// create a global shift register object
// parameters: (number of shift registers, data pin, clock pin, latch pin)
ShiftRegister74HC595 sr (8, 5, 6, 7); 

Adafruit_NeoPixel strip = Adafruit_NeoPixel(6, PIN, NEO_RGB+NEO_KHZ800);
uint8_t brightness = 0;
uint8_t fadeAmount = 5;
uint8_t rb[7][3] = { {255, 0, 0}, {255, 125, 0}, {255, 255, 0}, {0, 255, 0}, {0, 155, 255}, {0, 0, 255}, {139, 0, 255}};


#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(4, 3); // RX, TX
#endif

char ssid[] = "lby";            // your network SSID (name)
char pass[] = "12345678";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

//IPAddress ip = "192.43.244.18";

char timeServer[] = "183.230.40.42";  // NTP server
//char timeServer[] = "0.cn.pool.ntp.org";  // NTP server
unsigned int localPort = 2390;        // local port to listen for UDP packets

const int NTP_PACKET_SIZE = 48;  // NTP timestamp is in the first 48 bytes of the message
const int UDP_TIMEOUT = 2000;    // timeout in miliseconds to wait for an UDP packet to arrive

byte packetBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets

WiFiEspUDP Udp;

uint8_t pinValues[8] = { B00000001, B00000100, B01110000, B00000000, B00000001, B00011100, B01000000, B00000000 };//初始化595寄存器，效果为显示6个1
 
void setup() {
  
  sr.setAll(pinValues);//显示6个1
  
  Serial.begin(9600);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  
  
  strip.begin();
  strip.setBrightness(100);//初始化RGB

  //初始化效果，紫灯闪烁
  for(int j = 0; j < 5; j++){
  for(int i = 0; i < 10; i++){
    for(int k = 0; k < 6; k++)
      strip.setPixelColor(k, 0, brightness, brightness);
    strip.show();
    brightness = brightness + fadeAmount;
    if(brightness <= 0 || brightness >= 50)
      fadeAmount = -fadeAmount;
    delay(40); 
    }
  }

  //Wi-Fi初始化，联网
  WiFi.init(&Serial1);
  
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
  Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  //联网完成后，显示彩虹灯
  for(int M = 0; M < 6; M++){
  for (uint8_t j = 0; j < 6; j++) {
  for (uint8_t k = 0; k < 6; k++)
    strip.setPixelColor(k, 0);
      for (int8_t i = 0; i <= j; i++){
        strip.setPixelColor(i, strip.Color(rb[(j-i) % 7][0], rb[(j-i) % 7][1], rb[(j-i) % 7][2]));
        strip.setBrightness(40);
      }
      strip.show();
  }
  }
  // you're connected now, so print out the data
  Serial.println("You're connected to the network");

  Udp.begin(localPort);
}

void loop() {
  
  sr.setAll(pinValues);
  
  //delay(10);

//----------------------------联网部分-----------------------------------------

  sendNTPpacket(timeServer); // send an NTP packet to a time server
  
  // wait for a reply for UDP_TIMEOUT miliseconds
  unsigned long startMs = millis();
  while (!Udp.available() && (millis() - startMs) < UDP_TIMEOUT) {}

  Serial.println(Udp.parsePacket());
  if (Udp.parsePacket()) {
    Serial.println("packet received");
    // We've received a packet, read the data from it into the buffer
    Udp.read(packetBuffer, NTP_PACKET_SIZE);

    // the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = ");
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);


    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    //小时
    int hour = ((epoch  % 86400L) / 3600 + 8) % 24;
    int minute;
    int second;
    Serial.print(hour); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
      minute =0;
    }
    //分钟
    minute = (epoch  % 3600) / 60;
    Serial.print(minute); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
      second = 0;
    }
    //秒
    second = epoch % 60;
    Serial.println(second); // print the second
    UpdateDisplay(hour, minute, second, pinValues);
  }
  // wait ten seconds before asking for the time again
  delay(100);
  
  //delay(10);
}

//每次获取到时间后，设置各个寄存器状态
void UpdateDisplay(int hour, int minute, int second, uint8_t pinValues[8])
{
  int hour_shi = hour / 10 % 10;
  int hour_ge =  hour / 1  % 10;

  int minute_shi = minute / 10 % 10;
  int minute_ge =  minute / 1  % 10;

  int second_shi = second / 10 % 10;
  int second_ge =  second / 1  % 10;

//  if(minute_ge == 0){
//    for(int q = 1; q < 50; q++){
//      sr.setAllLow();
//      delay(20);
//    }
//    for(int q = 1; q < 50; q++){
//      for (int i = 0; i < 8; i++) {
//        sr.set(i, HIGH); // set single pin HIGH
//        delay(50); 
//      }
//    }
//  }

  switch(hour_shi){
    case 1:
      //doto
      pinValues[0] = pinValues[0] & B00000000;
      pinValues[1] = pinValues[1] & B11111100;
      pinValues[0] = pinValues[0] | B00000001;
      break;
    case 2:
      //doto
      pinValues[0] = pinValues[0] & B00000000;
      pinValues[1] = pinValues[1] & B11111100;
      pinValues[0] = pinValues[0] | B00000010;
      break;
    case 3:
      //doto
      pinValues[0] = pinValues[0] & B00000000;
      pinValues[1] = pinValues[1] & B11111100;
      pinValues[0] = pinValues[0] | B00000100;
      break;
    case 4:
      //doto
      pinValues[0] = pinValues[0] & B00000000;
      pinValues[1] = pinValues[1] & B11111100;
      pinValues[0] = pinValues[0] | B00001000;
      break;
    case 5:
      //doto
      pinValues[0] = pinValues[0] & B00000000;
      pinValues[1] = pinValues[1] & B11111100;
      pinValues[0] = pinValues[0] | B00010000;
      break;
    case 6:
      //doto
      pinValues[0] = pinValues[0] & B00000000;
      pinValues[1] = pinValues[1] & B11111100;
      pinValues[0] = pinValues[0] | B00100000;
      break;
    case 7:
      //doto
      pinValues[0] = pinValues[0] & B00000000;
      pinValues[1] = pinValues[1] & B11111100;
      pinValues[0] = pinValues[0] | B01000000;
      break;
    case 8:
      //doto
      pinValues[0] = pinValues[0] & B00000000;
      pinValues[1] = pinValues[1] & B11111100;
      pinValues[0] = pinValues[0] | B10000000;
      break;   
    case 9:
      //doto
      pinValues[1] = pinValues[1] & B11111100;
      pinValues[0] = pinValues[0] & B00000000;
      pinValues[1] = pinValues[1] | B00000001;
      break;
    case 0:
      //doto
      pinValues[1] = pinValues[1] & B11111100;
      pinValues[0] = pinValues[0] & B00000000;
      pinValues[1] = pinValues[1] | B00000010;
      break;           
  }
  //sr.setAll(pinValues);
  switch(hour_ge){
    case 1:
      //doto
      pinValues[1] = pinValues[1] & B00000011;
      pinValues[2] = pinValues[2] & B11110000;
      pinValues[1] = pinValues[1] | B00000100;
      break;
    case 2:
      //doto
      pinValues[1] = pinValues[1] & B00000011;
      pinValues[2] = pinValues[2] & B11110000;
      pinValues[1] = pinValues[1] | B00001000;
      break;
    case 3:
      //doto
      pinValues[1] = pinValues[1] & B00000011;
      pinValues[2] = pinValues[2] & B11110000;
      pinValues[1] = pinValues[1] | B00010000;
      break;
    case 4:
      //doto
      pinValues[1] = pinValues[1] & B00000011;
      pinValues[2] = pinValues[2] & B11110000;
      pinValues[1] = pinValues[1] | B00100000;
      break;
    case 5:
      //doto
      pinValues[1] = pinValues[1] & B00000011;
      pinValues[2] = pinValues[2] & B11110000;
      pinValues[1] = pinValues[1] | B01000000;
      break;
    case 6:
      //doto
      pinValues[1] = pinValues[1] & B00000011;
      pinValues[2] = pinValues[2] & B11110000;
      pinValues[1] = pinValues[1] | B10000000;
      break;
    case 7:
      //doto
      pinValues[1] = pinValues[1] & B00000011;
      pinValues[2] = pinValues[2] & B11110000;
      pinValues[2] = pinValues[2] | B00000001;
      break;
    case 8:
      //doto
      pinValues[1] = pinValues[1] & B00000011;
      pinValues[2] = pinValues[2] & B11110000;
      pinValues[2] = pinValues[2] | B00000010;
      break;   
    case 9:
      //doto
      pinValues[1] = pinValues[1] & B00000011;
      pinValues[2] = pinValues[2] & B11110000;
      pinValues[2] = pinValues[2] | B00000100;
      break;
    case 0:
      //doto
      pinValues[1] = pinValues[1] & B00000011;
      pinValues[2] = pinValues[2] & B11110000;
      pinValues[2] = pinValues[2] | B00001000;
      break;           
  }
  //sr.setAll(pinValues);
  switch(minute_shi){
    case 1:
      //doto
      pinValues[2] = pinValues[2] & B00111111;
      pinValues[3] = pinValues[3] & B00000000;
      pinValues[2] = pinValues[2] | B01000000;
      break;
    case 2:
      //doto
      pinValues[2] = pinValues[2] & B00111111;
      pinValues[3] = pinValues[3] & B00000000;
      pinValues[2] = pinValues[2] | B10000000;
      break;
    case 3:
      //doto
      pinValues[2] = pinValues[2] & B00111111;
      pinValues[3] = pinValues[3] & B00000000;
      pinValues[3] = pinValues[3] | B00000001;
      break;
    case 4:
      //doto
      pinValues[2] = pinValues[2] & B00111111;
      pinValues[3] = pinValues[3] & B00000000;
      pinValues[3] = pinValues[3] | B00000010;
      break;
    case 5:
      //doto
      pinValues[2] = pinValues[2] & B00111111;
      pinValues[3] = pinValues[3] & B00000000;
      pinValues[3] = pinValues[3] | B00000100;
      break;
    case 6:
      //doto
      pinValues[2] = pinValues[2] & B00111111;
      pinValues[3] = pinValues[3] & B00000000;
      pinValues[3] = pinValues[3] | B00001000;
      break;
    case 7:
      //doto
      pinValues[2] = pinValues[2] & B00111111;
      pinValues[3] = pinValues[3] & B00000000;
      pinValues[3] = pinValues[3] | B00010000;
      break;
    case 8:
      //doto
      pinValues[2] = pinValues[2] & B00111111;
      pinValues[3] = pinValues[3] & B00000000;
      pinValues[3] = pinValues[3] | B00100000;
      break;   
    case 9:
      //doto
      pinValues[2] = pinValues[2] & B00111111;
      pinValues[3] = pinValues[3] & B00000000;
      pinValues[3] = pinValues[3] | B01000000;
      break;
    case 0:
      //doto
      pinValues[2] = pinValues[2] & B00111111;
      pinValues[3] = pinValues[3] & B00000000;
      pinValues[3] = pinValues[3] | B10000000;
      break;           
  }
  //sr.setAll(pinValues);
  switch(minute_ge){
    case 1:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B00000001;
      break;
    case 2:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B00000010;
      break;
    case 3:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B00000100;
      break;
    case 4:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B00001000;
      break;
    case 5:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B00010000;
      break;
    case 6:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B00100000;
      break;
    case 7:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B01000000;
      break;
    case 8:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B10000000;
      break;   
    case 9:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[5] = pinValues[5] | B00000001;
      break;
    case 0:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[5] = pinValues[5] | B00000010;
      break;           
  }
  //sr.setAll(pinValues);
  switch(second_shi){
    case 1:
      //doto
      pinValues[5] = pinValues[5] & B00001111;
      pinValues[6] = pinValues[6] & B11000000;
      pinValues[5] = pinValues[5] | B00010000;
      break;
    case 2:
      //doto
      pinValues[5] = pinValues[5] & B00001111;
      pinValues[6] = pinValues[6] & B11000000;
      pinValues[5] = pinValues[5] | B00100000;
      break;
    case 3:
      //doto
      pinValues[5] = pinValues[5] & B00001111;
      pinValues[6] = pinValues[6] & B11000000;
      pinValues[5] = pinValues[5] | B01000000;
      break;
    case 4:
      //doto
      pinValues[5] = pinValues[5] & B00001111;
      pinValues[6] = pinValues[6] & B11000000;
      pinValues[5] = pinValues[5] | B10000000;
      break;
    case 5:
      //doto
      pinValues[5] = pinValues[5] & B00001111;
      pinValues[6] = pinValues[6] & B11000000;
      pinValues[6] = pinValues[6] | B00000001;
      break;
    case 6:
      //doto
      pinValues[5] = pinValues[5] & B00001111;
      pinValues[6] = pinValues[6] & B11000000;
      pinValues[6] = pinValues[6] | B00000010;
      break;
    case 7:
      //doto
      pinValues[5] = pinValues[5] & B00001111;
      pinValues[6] = pinValues[6] & B11000000;
      pinValues[6] = pinValues[6] | B00000100;
      break;
    case 8:
      //doto
      pinValues[5] = pinValues[5] & B00001111;
      pinValues[6] = pinValues[6] & B11000000;
      pinValues[6] = pinValues[6] | B00001000;
      break;   
    case 9:
      //doto
      pinValues[5] = pinValues[5] & B00001111;
      pinValues[6] = pinValues[6] & B11000000;
      pinValues[6] = pinValues[6] | B00010000;
      break;
    case 0:
      //doto
      pinValues[5] = pinValues[5] & B00001111;
      pinValues[6] = pinValues[6] & B11000000;
      pinValues[6] = pinValues[6] | B00100000;
      break;           
  }
  //sr.setAll(pinValues);
  switch(minute_ge){
    case 1:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B00000001;
      break;
    case 2:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B00000010;
      break;
    case 3:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B00000100;
      break;
    case 4:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B00001000;
      break;
    case 5:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B00010000;
      break;
    case 6:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B00100000;
      break;
    case 7:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B01000000;
      break;
    case 8:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[4] = pinValues[4] | B10000000;
      break;   
    case 9:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[5] = pinValues[5] | B00000001;
      break;
    case 0:
      //doto
      pinValues[4] = pinValues[4] & B00000000;
      pinValues[5] = pinValues[5] & B11111100;
      pinValues[5] = pinValues[5] | B00000010;
      break;           
  }
  //sr.setAll(pinValues);
  switch(second_ge){
    case 1:
      //doto
      pinValues[6] = pinValues[6] & B00111111;
      pinValues[7] = pinValues[7] & B00000000;
      pinValues[6] = pinValues[6] | B01000000;
      break;
    case 2:
      //doto
      pinValues[6] = pinValues[6] & B00111111;
      pinValues[7] = pinValues[7] & B00000000;
      pinValues[6] = pinValues[6] | B10000000;
      break;
    case 3:
      //doto
      pinValues[6] = pinValues[6] & B00111111;
      pinValues[7] = pinValues[7] & B00000000;
      pinValues[7] = pinValues[7] | B00000001;
      break;
    case 4:
      //doto
      pinValues[6] = pinValues[6] & B00111111;
      pinValues[7] = pinValues[7] & B00000000;
      pinValues[7] = pinValues[7] | B00000010;
      break;
    case 5:
      //doto
      pinValues[6] = pinValues[6] & B00111111;
      pinValues[7] = pinValues[7] & B00000000;
      pinValues[7] = pinValues[7] | B00000100;
      break;
    case 6:
      //doto
      pinValues[6] = pinValues[6] & B00111111;
      pinValues[7] = pinValues[7] & B00000000;
      pinValues[7] = pinValues[7] | B00001000;
      break;
    case 7:
      //doto
      pinValues[6] = pinValues[6] & B00111111;
      pinValues[7] = pinValues[7] & B00000000;
      pinValues[7] = pinValues[7] | B00010000;
      break;
    case 8:
      //doto
      pinValues[6] = pinValues[6] & B00111111;
      pinValues[7] = pinValues[7] & B00000000;
      pinValues[7] = pinValues[7] | B00100000;
      break;   
    case 9:
      //doto
      pinValues[6] = pinValues[6] & B00111111;
      pinValues[7] = pinValues[7] & B00000000;
      pinValues[7] = pinValues[7] | B01000000;
      break;
    case 0:
      //doto
      pinValues[6] = pinValues[6] & B00111111;
      pinValues[7] = pinValues[7] & B00000000;
      pinValues[7] = pinValues[7] | B10000000;
      break;           
  }
  sr.setAll(pinValues);
}

void sendNTPpacket(char *ntpSrv)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)

  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(ntpSrv, 123); //NTP requests are to port 123

  Udp.write(packetBuffer, NTP_PACKET_SIZE);

  Udp.endPacket();
}
