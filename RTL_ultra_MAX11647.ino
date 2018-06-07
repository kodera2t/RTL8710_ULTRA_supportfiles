/***************************************************************************
 RTL8710 ULTRA 2ch A/D converter sample program
 ADC: Maxim MAX11647
 Two input voltage will be shown on OLED display
 ***************************************************************************/
#include <ESP_SSD1306.h>    
#include <Adafruit_GFX.h>
#include <SPI.h> 
#include <Wire.h>
#define adc_ad  0b0110110
byte read_byte1, read_byte2;
/* Assign a unique ID to this sensor at the same time */
ESP_SSD1306 oled(16);    // I2C declaration

void setup(void) 
{
  pinMode(11,OUTPUT);
  Wire.begin();
  oled.begin();
      oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);  
    oled.display();
}

void loop(void) 
{
  unsigned int read_value,read_value2;
  float ch1,ch2;
  oled.setCursor(0,0);
  oled.print("MAX11647 2ch ADC"); 
  init_adch0();
  i2c_read(adc_ad);
  read_value=(read_byte1<<8|read_byte2);
  oled.setCursor(0,8);
  oled.print("CH1: ");
  oled.print(read_value);
  ch1=(float)read_value/1024.0*3.3; 
  oled.setCursor(64,8);
  oled.print(ch1);
  oled.print(" V");
  init_adch1();
  i2c_read(adc_ad);
  read_value2=(read_byte1<<8|read_byte2);
  oled.setCursor(0,16);
  oled.print("CH2: ");
  oled.print(read_value2);
  ch2=(float)read_value2/1024.0*3.3;
  oled.setCursor(64,16);
  oled.print(ch2);
  oled.print(" V");   
  oled.display();
  delay(1000);
  oled.clearDisplay();
}

void init_adch0(void){
  i2c_write(adc_ad,0b10001000,0b01100001);
}
void init_adch1(void){
  i2c_write(adc_ad,0b10001000,0b01100011);
}

void i2c_write(int device_address, int memory_address, int value)
{
  Wire.beginTransmission(device_address);
  Wire.write(memory_address);
  Wire.write(value);
  Wire.endTransmission();
}

void i2c_read(int device_address)
{
Wire.beginTransmission(device_address);
Wire.requestFrom(device_address, 2);
read_byte1 = (Wire.read())&0b11;
read_byte2 = Wire.read();
Wire.endTransmission(true);
}


