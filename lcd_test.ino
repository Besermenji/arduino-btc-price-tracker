#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

// UNO+WiFi R3 uses standard Arduino UNO I2C pins
// SDA = A4, SCL = A5

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

void setup() {
  Serial.begin(9600);
  Serial.println("UNO+WiFi R3 LCD1602 I2C Test");
  
  Wire.begin();
  
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("UNO+WiFi R3");
  lcd.setCursor(0, 1);
  lcd.print("LCD Test OK!");
  
  Serial.println("LCD initialized successfully on UNO+WiFi R3");
  Serial.println("I2C pins: SDA=A4, SCL=A5");
}

void loop() {
  delay(1000);
}
