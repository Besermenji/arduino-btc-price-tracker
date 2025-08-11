#include <Arduino.h>
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println("I2C Scanner starting...");
  Wire.begin();
}

void loop() {
  byte count = 0;
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("Found I2C device at 0x");
      if (address < 16) Serial.print('0');
      Serial.println(address, HEX);
      count++;
    }
  }
  if (count == 0) Serial.println("No I2C devices found");
  delay(2000);
}
