//RTL-ULTRA board BME280 sample program.
//I2C address is 0x76 and please modify it in Adafruit_BME280.h of
//Adafruit_BME280 library


#include <ESP_SSD1306.h>    
#include <Adafruit_GFX.h>   
#include <SPI.h>            
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
//#define BME_SCK 13
//#define BME_MISO 12
//#define BME_MOSI 11
//#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

#define DC_JUMPER 0  // For I2C Communication - this pin pulled high by default
ESP_SSD1306 oled(16);    // I2C declaration
Adafruit_BME280 bme; // I2C

char ssid[] = "rtl8710test";  //  your network SSID (name)
char pass[] = "happy2018";       // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
WiFiServer server(80);
int count=1;
unsigned long time_1;
unsigned long interval = 3600000; //one hour interval
int disp;
boolean disp_index = false;


unsigned long delayTime;

void setup()
{
  pinMode(11,OUTPUT);
  Wire.begin();
  oled.begin();
  bme.begin();  
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);
  oled.setTextSize(1);
  if (WiFi.status() == WL_NO_SHIELD) {
    //Serial.println("WiFi shield not present");
    while (true);       // don't continue
  }
  
  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    //Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    // wait 5 seconds for connection:
    delay(5000);
  }
    printWifiStatus(); 

  server.begin();                           // start the web server on port 80


}


void loop()
{


  float index;


    if(count==1){
    oled.fillRect(0,0,128,32,BLACK);
    oled.setTextSize(1);
    oled.setCursor(0, 0);
    oled.print("BME280 sensornode");
    oled.drawLine(0,7,128,7,WHITE);
    oled.setCursor(0, 8);
    oled.print(bme.readPressure() / 100.0F,1);
    oled.print("hPa  ");
    //oled.setCursor(40, 8);
    oled.print(bme.readTemperature(),1);
    oled.print("deg");
    oled.setCursor(0, 16);
    oled.print("humidity:");
    oled.print(bme.readHumidity());
    oled.print("%");
    oled.setCursor(0, 24);
    oled.print("IP:");
    oled.print(WiFi.localIP());
    oled.display();
    }else{
    oled.fillRect(0,0,128,64,BLACK);
    oled.clearDisplay();
    oled.display();
    }
    count++;


  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    oled.fillRect(32,32,128,64,BLACK);
    oled.setCursor(0,0);
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> turn the LED on pin 11 on<br>");
            client.print("Measured values by BME280 with RTL8710<BR>");
            client.print("Pressure (hpa):");
            client.print(bme.readPressure() / 100.0F);
            client.print("<br>");
            client.print("Temperature (deg):");            
            client.print(bme.readTemperature());
            client.print("<br>");
            client.print("Humidity (%):");            
            client.print(bme.readHumidity());
            client.print("<br>");
               oled.fillRect(0,0,128,64,BLACK); 
            client.print("Click <a href=\"/L\">here</a> turn the LED on pin 11 off<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(11, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(11, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    //Serial.println("client disonnected");
  }




//  delay(3000);
//count++;
}

void printWifiStatus() {
  IPAddress ip = WiFi.localIP();
  oled.setCursor(0,0);
  oled.println("assigned IP:");
  oled.print(ip);
  oled.display();
  long rssi = WiFi.RSSI();
  delay(2000);
}
