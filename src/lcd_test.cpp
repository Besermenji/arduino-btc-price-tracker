#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Custom characters for better visuals
byte bitcoinSymbol[8] = {
  0b00000,
  0b01110,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00000,
  0b00000
};

byte wifiSymbol[8] = {
  0b00000,
  0b01110,
  0b11111,
  0b01110,
  0b00100,
  0b00100,
  0b00000,
  0b00000
};

byte arrowUp[8] = {
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b00100,
  0b00100,
  0b00000,
  0b00000
};

byte arrowDown[8] = {
  0b00000,
  0b00100,
  0b00100,
  0b00100,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

void welcomeAnimation();
void showPrettyFormats();
void scrollText(String text, int row);
void showPriceWithArrows(float price, bool isUp);
void showWiFiStatus();

void setup() {
  Serial.begin(9600);
  Serial.println("UNO+WiFi R3 LCD1602 Pretty Display Test");
  
  Wire.begin();
  
  lcd.init();
  lcd.backlight();
  
  // Create custom characters
  lcd.createChar(0, bitcoinSymbol);
  lcd.createChar(1, wifiSymbol);
  lcd.createChar(2, arrowUp);
  lcd.createChar(3, arrowDown);
  
  // Welcome animation
  welcomeAnimation();
  
  // Show different display styles
  showPrettyFormats();
}

void loop() {
  // Scrolling text demo
  scrollText("Bitcoin Price Tracker Ready!", 0);
  delay(2000);
  
  // Price simulation with arrows
  showPriceWithArrows(45678.90, true);
  delay(3000);
  
  showPriceWithArrows(45234.56, false);
  delay(3000);
  
  // WiFi status display
  showWiFiStatus();
  delay(3000);
}

void welcomeAnimation() {
  lcd.clear();
  
  // Top row with Bitcoin symbol
  lcd.setCursor(0, 0);
  lcd.print("  ");
  lcd.write(0);
  lcd.print(" BTC Tracker ");
  lcd.write(0);
  lcd.print("  ");
  
  // Bottom row with WiFi symbol
  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.write(1);
  lcd.print(" WiFi Ready ");
  lcd.write(1);
  lcd.print(" ");
  
  delay(2000);
}

void showPrettyFormats() {
  lcd.clear();
  
  // Centered title
  lcd.setCursor(2, 0);
  lcd.print("PRICE DISPLAY");
  
  // Bottom row with decorative elements
  lcd.setCursor(0, 1);
  lcd.print("==================");
  
  delay(2000);
  
  // Show price formatting
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Current Price:");
  lcd.setCursor(0, 1);
  lcd.print("$45,678.90");
  
  delay(2000);
}

void scrollText(String text, int row) {
  lcd.setCursor(0, row);
  lcd.print("                "); // Clear row
  
  if (text.length() <= 16) {
    // Center short text
    int padding = (16 - text.length()) / 2;
    lcd.setCursor(padding, row);
    lcd.print(text);
  } else {
    // Scroll long text
    for (int i = 0; i <= text.length() - 16; i++) {
      lcd.setCursor(0, row);
      lcd.print(text.substring(i, i + 16));
      delay(300);
    }
  }
}

void showPriceWithArrows(float price, bool isUp) {
  lcd.clear();
  
  // Top row: Price with arrow
  lcd.setCursor(0, 0);
  lcd.print("Bitcoin: ");
  if (isUp) {
    lcd.write(2); // Up arrow
  } else {
    lcd.write(3); // Down arrow
  }
  lcd.print(" $");
  
  // Format price nicely
  String priceStr = String(price, 2);
  if (price >= 10000) {
    // Add comma for thousands
    priceStr = String((int)price / 1000) + "," + priceStr.substring(priceStr.length() - 4);
  }
  lcd.print(priceStr);
  
  // Bottom row: Status
  lcd.setCursor(0, 1);
  if (isUp) {
    lcd.print("Price Rising ");
    lcd.write(2);
  } else {
    lcd.print("Price Falling");
    lcd.write(3);
  }
}

void showWiFiStatus() {
  lcd.clear();
  
  // Top row: WiFi symbol and status
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.print(" Connected");
  lcd.write(1);
  
  // Bottom row: IP address simulation
  lcd.setCursor(0, 1);
  lcd.print("IP: 192.168.1.100");
}
