#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

const char* apiUrl = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd";

unsigned long lastUpdate = 0;
const unsigned long updateInterval = 30000;

float lastPrice = 0;
bool firstUpdate = true;

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

byte dollarSign[8] = {
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100
};

void setup() {
  Serial.begin(115200);
  Serial.println("UNO+WiFi R3 BTC Price Tracker");
  
  Wire.begin();
  lcd.init();
  lcd.backlight();
  
  // Create custom characters
  lcd.createChar(0, bitcoinSymbol);
  lcd.createChar(1, wifiSymbol);
  lcd.createChar(2, arrowUp);
  lcd.createChar(3, arrowDown);
  lcd.createChar(4, dollarSign);
  
  // Welcome animation
  showWelcomeScreen();
  
  // Initialize WiFi
  initializeWiFi();
  
  // Show connected status
  showWiFiConnected();
  
  // First price update
  updateBitcoinPrice();
}

void loop() {
  if (millis() - lastUpdate >= updateInterval) {
    updateBitcoinPrice();
    lastUpdate = millis();
  }
}

void showWelcomeScreen() {
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
  lcd.print(" Connecting... ");
  lcd.write(1);
  lcd.print(" ");
  
  delay(3000);
}

void showWiFiConnected() {
  lcd.clear();
  
  // Top row: WiFi status
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.print(" WiFi Connected!");
  
  // Bottom row: Ready message
  lcd.setCursor(0, 1);
  lcd.print("Ready to track!");
  
  delay(2000);
}

void initializeWiFi() {
  Serial.println("AT+RST");
  delay(2000);
  
  Serial.println("AT+CWMODE=1");
  delay(1000);
  
  String cmd = "AT+CWJAP=\"";
  cmd += ssid;
  cmd += "\",\"";
  cmd += password;
  cmd += "\"";
  Serial.println(cmd);
  delay(5000);
  
  Serial.println("AT+CIPMUX=0");
  delay(1000);
}

void updateBitcoinPrice() {
  String cmd = "AT+CIPSTART=\"TCP\",\"api.coingecko.com\",80";
  Serial.println(cmd);
  delay(2000);
  
  if (Serial.find("OK")) {
    String request = "GET /api/v3/simple/price?ids=bitcoin&vs_currencies=usd HTTP/1.1\r\n";
    request += "Host: api.coingecko.com\r\n";
    request += "Connection: close\r\n\r\n";
    
    String cmd = "AT+CIPSEND=";
    cmd += request.length();
    Serial.println(cmd);
    delay(1000);
    
    Serial.print(request);
    delay(2000);
    
    String response = "";
    while (Serial.available()) {
      response += Serial.readString();
    }
    
    int jsonStart = response.indexOf("{");
    if (jsonStart != -1) {
      String jsonData = response.substring(jsonStart);
      
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, jsonData);
      
      float btcPrice = doc["bitcoin"]["usd"];
      
      // Show price with pretty formatting
      showPriceDisplay(btcPrice);
      
      Serial.print("BTC Price: $");
      Serial.println(btcPrice, 2);
      
      lastPrice = btcPrice;
      firstUpdate = false;
    } else {
      showErrorDisplay("Parse Error");
    }
    
    Serial.println("AT+CIPCLOSE");
    delay(1000);
  } else {
    showErrorDisplay("WiFi Error");
  }
}

void showPriceDisplay(float price) {
  lcd.clear();
  
  // Top row: Bitcoin symbol and price
  lcd.setCursor(0, 0);
  lcd.write(0);
  lcd.print(" Bitcoin Price");
  
  // Bottom row: Price with dollar sign and arrow
  lcd.setCursor(0, 1);
  lcd.write(4); // Dollar sign
  lcd.print(" ");
  
  // Format price nicely
  String priceStr = formatPrice(price);
  lcd.print(priceStr);
  
  // Add arrow indicator if not first update
  if (!firstUpdate) {
    if (price > lastPrice) {
      lcd.print(" ");
      lcd.write(2); // Up arrow
    } else if (price < lastPrice) {
      lcd.print(" ");
      lcd.write(3); // Down arrow
    }
  }
}

String formatPrice(float price) {
  String priceStr = String(price, 2);
  
  if (price >= 100000) {
    // Format as XX,XXX.XX
    int thousands = (int)price / 1000;
    priceStr = String(thousands) + "," + priceStr.substring(priceStr.length() - 4);
  } else if (price >= 10000) {
    // Format as X,XXX.XX
    int thousands = (int)price / 1000;
    priceStr = String(thousands) + "," + priceStr.substring(priceStr.length() - 4);
  }
  
  return priceStr;
}

void showErrorDisplay(String error) {
  lcd.clear();
  
  // Top row: Error symbol and message
  lcd.setCursor(0, 0);
  lcd.print("Error: ");
  lcd.print(error);
  
  // Bottom row: Retry message
  lcd.setCursor(0, 1);
  lcd.print("Retrying...");
}
