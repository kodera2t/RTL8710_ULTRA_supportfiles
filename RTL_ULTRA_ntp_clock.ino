
/*

 Udp NTP Client

 Get the time from a Network Time Protocol (NTP) time server
 Demonstrates use of UDP sendPacket and ReceivePacket
 For more on NTP time servers and the messages needed to communicate with them,
 see http://en.wikipedia.org/wiki/Network_Time_Protocol

 created 4 Sep 2010
 by Michael Margolis
 modified 9 Apr 2012
 by Tom Igoe

 This code is in the public domain.

 */
// Ameba ntp clock written for RTL8710 ULTRA board
// Board support by Realtek version 2.0.4 compatible


#include <ESP_SSD1306.h>    
#include <Adafruit_GFX.h>   
#include <SPI.h>            
#include <Wire.h>
#include <WiFiUdp.h>           
#include <WiFi.h>

#define OLED_RESET  16  // Pin 15 -RESET digital signal
ESP_SSD1306 oled(OLED_RESET); // FOR I2C





char ssid[] = "rtl8710test";  //  your network SSID (name)
char pass[] = "happy2018";       // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServer(210, 173, 160, 27); //adjust for your nearest ntp server address

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

void setup() {
  // Open serial communications and wait for port to open:
//  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }
      oled.begin();
    //   init done
 //   oled.display();
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(32,32);


  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    oled.setCursor(32,32);
    oled.println("WiFi nfound");
    // don't continue:
    //while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
////    Serial.println("Please upgrade the firmware");
    oled.setCursor(32,32);
    oled.println("firm old");
  }
    oled.setCursor(0,0);
    oled.print("Attempting to connect");
    oled.display();
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    oled.setCursor(0,8);
    oled.print("trying");
    oled.setCursor(0,16);
    oled.println(ssid);
    oled.display();
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(5000);
  }
  oled.setCursor(0,24);
  oled.println("Connected");
  oled.display();
  //printWifiStatus();

  //Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);
}

void loop() {
  sendNTPpacket(timeServer); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  //Serial.println(Udp.parsePacket());
  Udp.parsePacket();
  if (Udp.parsePacket()) {
    //Serial.println("packet received");
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    //Serial.print("Seconds since Jan 1 1900 = ");
    //Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    //Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    //Serial.println(epoch);


    // print the hour, minute and second:
      oled.fillRect(0,0,128,64,BLACK);
    oled.setTextSize(2);
    oled.setCursor(0,0);
    oled.print("UTC time");       // UTC is the time at Greenwich Meridian (GMT)
    oled.setTextSize(2);
    oled.setCursor(0,15);
    //Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    //Serial.print(':');
    oled.print((epoch  % 86400L) / 3600);
    oled.print(":");
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      oled.print('0');
    }
    oled.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    oled.print(':');
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      oled.print('0');
    }
    oled.println(epoch % 60); // print the second
  }
  oled.display();
  // wait ten seconds before asking for the time again
  delay(10000);

}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
  //Serial.println("1");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //Serial.println("2");
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  //Serial.println("3");

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  //Serial.println("4");
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  //Serial.println("5");
  Udp.endPacket();
  //Serial.println("6");
 // oled.display();
}


//void printWifiStatus() {
//  // print the SSID of the network you're attached to:
//  Serial.print("SSID: ");
//  Serial.println(WiFi.SSID());
//
//  // print your WiFi shield's IP address:
//  IPAddress ip = WiFi.localIP();
//  Serial.print("IP Address: ");
//  Serial.println(ip);
//
//  // print the received signal strength:
//  long rssi = WiFi.RSSI();
//  Serial.print("signal strength (RSSI):");
//  Serial.print(rssi);
//  Serial.println(" dBm");
//}










