#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

// UNO+WiFi R3 uses standard Arduino UNO I2C pins
// SDA = A4, SCL = A5

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

const char* apiUrl = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd";

unsigned long lastUpdate = 0;
const unsigned long updateInterval = 30000;

void setup() {
  Serial.begin(115200);
  Serial.println("UNO+WiFi R3 BTC Price Tracker");
  
  Wire.begin();
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("UNO+WiFi R3");
  lcd.setCursor(0, 1);
  lcd.print("BTC Tracker");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi...");
  
  // Initialize WiFi connection
  initializeWiFi();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");
  delay(2000);
  
  lcd.clear();
  updateBitcoinPrice();
}

void loop() {
  if (millis() - lastUpdate >= updateInterval) {
    updateBitcoinPrice();
    lastUpdate = millis();
  }
}

void initializeWiFi() {
  // Send AT commands to ESP8266 module
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
  // Connect to API server
  String cmd = "AT+CIPSTART=\"TCP\",\"api.coingecko.com\",80";
  Serial.println(cmd);
  delay(2000);
  
  if (Serial.find("OK")) {
    // Prepare HTTP request
    String request = "GET /api/v3/simple/price?ids=bitcoin&vs_currencies=usd HTTP/1.1\r\n";
    request += "Host: api.coingecko.com\r\n";
    request += "Connection: close\r\n\r\n";
    
    String cmd = "AT+CIPSEND=";
    cmd += request.length();
    Serial.println(cmd);
    delay(1000);
    
    Serial.print(request);
    delay(2000);
    
    // Read response
    String response = "";
    while (Serial.available()) {
      response += Serial.readString();
    }
    
    // Parse JSON response
    int jsonStart = response.indexOf("{");
    if (jsonStart != -1) {
      String jsonData = response.substring(jsonStart);
      
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, jsonData);
      
      float btcPrice = doc["bitcoin"]["usd"];
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bitcoin Price:");
      lcd.setCursor(0, 1);
      lcd.print("$");
      lcd.print(btcPrice, 2);
      
      Serial.print("BTC Price: $");
      Serial.println(btcPrice, 2);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Error parsing");
      lcd.setCursor(0, 1);
      lcd.print("response data");
    }
    
    Serial.println("AT+CIPCLOSE");
    delay(1000);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi connection");
    lcd.setCursor(0, 1);
    lcd.print("failed");
  }
}
