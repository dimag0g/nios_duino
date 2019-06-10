
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  EEPROM.write(0, EEPROM.read(0)+1);
}

void loop() {
  byte error, address;
  int nDevices;
 
  //Serial.println(alt_ticks_per_second());
  Serial.println("Scanning for I2C");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("Device at 0x");
      if (address<16) Serial.print("0");
      Serial.println(address,HEX);
      nDevices++;
    }
  }
  if(nDevices == 0) Serial.println("No devices found");
  
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("1s is ");
  int now = millis();
  delayMicroseconds(1000000);
  Serial.print(millis()-now);
  Serial.println("ms");
  Serial.print("Analog pin 1 value = ");
  Serial.println(analogRead(1));
  Serial.print("Reset counter = ");
  Serial.println(EEPROM.read(0));
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
