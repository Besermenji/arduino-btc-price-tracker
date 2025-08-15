#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <math.h>
#include "config.h"

// LCD setup
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Helper to show two-line status messages (store text in flash)
static void lcdMsg(const __FlashStringHelper* line1, const __FlashStringHelper* line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

// Simple state variables
unsigned long lastUpdate = 0;
unsigned long lastTimeUpdate = 0;
bool showTime = false;
float lastPrice = 0;
bool firstUpdate = true;

// Communication state
String responseBuffer = "";

void showWelcomeScreen();
void showWiFiConnected();
void initializeWiFi();
void updateBitcoinPrice();
void updateTime();
void showErrorDisplay(String error);
void showRawDebug(String raw);
void showScrollingText(String text, int row);
void showFullResponse(String response);

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.println("UNO+WiFi R3 BTC Price Tracker");
  
  Wire.begin();
  lcd.init();
  lcd.backlight();
  
  // Welcome animation
  showWelcomeScreen();
  
  // Initialize WiFi
  initializeWiFi();
  
  // Show connected status
  showWiFiConnected();
  
  // Start with Bitcoin price display
  lastUpdate = millis();
  showTime = false;
  updateBitcoinPrice();
}

void loop() {
  // Simple alternating display logic - every 30 seconds
  static unsigned long lastToggle = 0;
  
  if (millis() - lastToggle >= 30000) {
    if (showTime) {
      // Currently showing time, switch to BTC
      showTime = false;
      updateBitcoinPrice();
    } else {
      // Currently showing BTC, switch to time
      showTime = true;
      updateTime();
    }
    lastToggle = millis();
  }
}

void showWelcomeScreen() {
  lcd.clear();
  
  // Top row with Bitcoin theme
  lcd.setCursor(0, 0);
  lcd.print("Bitcoin Tracker");
  
  // Bottom row with elegant message
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");
  
  delay(1500);
  
  // Show a cool loading animation
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Initializing...  ");
  
  // Animated dots
  for (int i = 0; i < 3; i++) {
    lcd.setCursor(8 + i, 1);
    lcd.print(".");
    delay(300);
  }
  
  delay(500);
}

void showWiFiConnected() {
  lcd.clear();
  
  // Top row: WiFi status
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");
  
  // Bottom row: Ready message
  lcd.setCursor(0, 1);
  lcd.print("Ready to track!");
  
  delay(1000);
}

void initializeWiFi() {
  // Wait for custom ESP firmware to announce readiness
  lcdMsg(F("WiFi"), F("waiting..."));
  unsigned long t0 = millis();
  bool ready = false;
  String receivedData = "";

  while (millis() - t0 < 15000) { // 15-second window
    if (Serial.available()) {
      String s = Serial.readString();
      receivedData += s;
      if (DEBUG_MODE) {
        Serial.print(F("[DEBUG] ESP data: '"));
        Serial.print(s);
        Serial.println(F("'"));
      }
      if (receivedData.indexOf("ESP8266 Ready") != -1) {
        ready = true;
        break;
      }
    }
    delay(100);
  }

  if (ready) {
    lcdMsg(F("WiFi"), F("Ready"));
    delay(800);
  } else {
    if (DEBUG_MODE) {
      Serial.print(F("[DEBUG] ESP not ready. Received: '"));
      Serial.print(receivedData);
      Serial.println(F("'"));
    }
    showErrorDisplay("ESP not ready");
    delay(2000);
  }
}

void updateBitcoinPrice() {
  // Request price via custom ESP8266 firmware
  responseBuffer = "";
  Serial.println("GET");

  // Read response
  unsigned long startTime = millis();
  responseBuffer = "";
  
  while ((millis() - startTime) < 15000) {
    if (Serial.available()) {
      String chunk = Serial.readString();
      responseBuffer += chunk;
      
      // Check if we have a complete JSON response
      if (responseBuffer.indexOf('{') != -1 && responseBuffer.indexOf('}') != -1) {
        break;
      }
    }
    delay(10);
  }
  
  // Parse JSON response - extract price and change
  if (responseBuffer.length() > 0) {
    int jsonStart = responseBuffer.indexOf('{');
    if (jsonStart != -1) {
      String jsonData = responseBuffer.substring(jsonStart);
      
      // Check if ESP8266 sent an error response
      int errorStart = jsonData.indexOf("\"error\":");
      if (errorStart != -1) {
        // Extract error message
        int errorValueStart = errorStart + 8; // Skip "error":
        int errorValueEnd = jsonData.indexOf("}", errorValueStart);
        
        if (errorValueEnd != -1) {
          String errorStr = jsonData.substring(errorValueStart, errorValueEnd);
          errorStr.trim();
          
          // Remove quotes if present
          if (errorStr[0] == '"' && errorStr[errorStr.length() - 1] == '"') {
            errorStr = errorStr.substring(1, errorStr.length() - 1);
          }
          
          // Show ESP8266 error message
          showErrorDisplay("ESP: " + errorStr);
          return;
        }
      }
      
      // Look for "usd": and "usd_24h_change": patterns
      int usdStart = jsonData.indexOf("\"usd\":");
      int usd_24h_changeStart = jsonData.indexOf("\"usd_24h_change\":");
      
      if (usdStart != -1 && usd_24h_changeStart != -1) {
        // Extract current price
        int valueStart = usdStart + 6; // Skip "usd":
        int valueEnd = jsonData.indexOf(",", valueStart);
        if (valueEnd == -1) valueEnd = jsonData.indexOf("}", valueStart);
        
        // Extract 24h change - more robust approach
        int colonPos = jsonData.indexOf(":", usd_24h_changeStart);
        int changeValueStart = colonPos + 1; // Start right after the colon
        int changeValueEnd = jsonData.indexOf(",", changeValueStart);
        if (changeValueEnd == -1) {
          // If no comma found, look for the closing brace of the bitcoin object
          int bitcoinObjEnd = jsonData.indexOf("}", changeValueStart);
          if (bitcoinObjEnd != -1) {
            changeValueEnd = bitcoinObjEnd;
          }
        }
        
        if (valueEnd != -1 && changeValueEnd != -1) {
          String priceStr = jsonData.substring(valueStart, valueEnd);
          String changeStr = jsonData.substring(changeValueStart, changeValueEnd);
          priceStr.trim();
          changeStr.trim();
          
          // Debug output to Serial if needed
          if (DEBUG_MODE) {
            Serial.print("Price string: '");
            Serial.print(priceStr);
            Serial.println("'");
            Serial.print("Change string: '");
            Serial.print(changeStr);
            Serial.println("'");
          }
          
          float btcPrice = priceStr.toFloat();
          float changePercent = changeStr.toFloat();
          
          // Debug output to Serial if needed
          if (DEBUG_MODE) {
            Serial.print("Parsed price: ");
            Serial.println(btcPrice);
            Serial.print("Parsed change: ");
            Serial.println(changePercent);
            Serial.print("Change is negative: ");
            Serial.println(changePercent < 0 ? "YES" : "NO");
          }
          
          if (btcPrice > 0) {
            // Clean display for LCD1602 (16 chars per line)
            lcd.clear();
            
            // Top line: BTC +0.45% (16 chars max)
            lcd.setCursor(0, 0);
            lcd.print("BTC ");
            
            if (changePercent > 0) {
              lcd.print("+");
              lcd.print(changePercent, 2);
              lcd.print("%");
            } else if (changePercent < 0) {
              lcd.print("-");
              lcd.print(fabs(changePercent), 2);
              lcd.print("%");
            } else {
              lcd.print("0.00%");
            }
            
            // Bottom line: $118748 (16 chars max)
            lcd.setCursor(0, 1);
            lcd.print("$");
            lcd.print(btcPrice);
            
            lastPrice = btcPrice;
            firstUpdate = false;
            return;
          }
        }
      }
    }
    
    // If we get here, something went wrong - provide specific error messages
    if (responseBuffer.length() == 0) {
      showErrorDisplay("Empty response");
    } else if (responseBuffer.indexOf('{') == -1) {
      showErrorDisplay("No JSON: " + responseBuffer.substring(0, 16));
    } else {
      int jsonStart = responseBuffer.indexOf('{');
      String jsonData = responseBuffer.substring(jsonStart);
      
      if (jsonData.indexOf("\"usd\":") == -1) {
        showErrorDisplay("Missing fields: no USD");
      } else if (jsonData.indexOf("\"usd_24h_change\":") == -1) {
        showErrorDisplay("Missing fields: no 24h");
      } else {
        showErrorDisplay("Parse error: no price end");
      }
    }
  }
}

void updateTime() {
  // Request time from ESP8266
  responseBuffer = "";
  Serial.println("TIME");

  // Read response
  unsigned long startTime = millis();
  responseBuffer = "";
  
  while ((millis() - startTime) < 15000) {
    if (Serial.available()) {
      String chunk = Serial.readString();
      responseBuffer += chunk;
      
      // Check if we have a complete JSON response
      if (responseBuffer.indexOf('{') != -1 && responseBuffer.indexOf('}') != -1) {
        break;
      }
    }
    delay(10);
  }
  
  // Parse time response
  if (responseBuffer.length() > 0) {
    int jsonStart = responseBuffer.indexOf('{');
    if (jsonStart != -1) {
      String jsonData = responseBuffer.substring(jsonStart);
      
      // Check if ESP8266 sent an error response
      int errorStart = jsonData.indexOf("\"error\":");
      if (errorStart != -1) {
        // Extract error message
        int errorValueStart = errorStart + 8; // Skip "error":
        int errorValueEnd = jsonData.indexOf("}", errorValueStart);
        
        if (errorValueEnd != -1) {
          String errorStr = jsonData.substring(errorValueStart, errorValueEnd);
          errorStr.trim();
          
          // Remove quotes if present
          if (errorStr[0] == '"' && errorStr[errorStr.length() - 1] == '"') {
            errorStr = errorStr.substring(1, errorStr.length() - 1);
          }
          
          // Show ESP8266 error message
          showErrorDisplay("ESP: " + errorStr);
          return;
        }
      }
      
      // Look for "time": pattern
      int timeStart = jsonData.indexOf("\"time\":");
      if (timeStart != -1) {
        int valueStart = timeStart + 7; // Skip "time":
        int valueEnd = jsonData.indexOf("}", valueStart);
        
        if (valueEnd != -1) {
          String timeStr = jsonData.substring(valueStart, valueEnd);
          timeStr.trim();
          
          // Remove quotes if present
          if (timeStr[0] == '"' && timeStr[timeStr.length() - 1] == '"') {
            timeStr = timeStr.substring(1, timeStr.length() - 1);
          }
          
                     if (timeStr != "--:--") {
            // Display time
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Current Time");
            lcd.setCursor(0, 1);
            lcd.print(timeStr);
            return;
          }
        }
      }
    }
    
    // If we get here, something went wrong
    showErrorDisplay("Time fetch failed");
  }
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

void showRawDebug(String raw){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("RAW resp:");
  lcd.setCursor(0,1);
  if(raw.length()>LCD_COLUMNS) raw=raw.substring(0,LCD_COLUMNS);
  lcd.print(raw);
}

void showScrollingText(String text, int row) {
  if (text.length() <= LCD_COLUMNS) {
    lcd.setCursor(0, row);
    lcd.print(text);
    return;
  }
  
  for (int i = 0; i <= text.length() - LCD_COLUMNS; i++) {
    lcd.setCursor(0, row);
    lcd.print(text.substring(i, i + LCD_COLUMNS));
    delay(300);
  }
}

void showFullResponse(String response) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Response:");
  
  if (response.length() <= LCD_COLUMNS) {
    lcd.setCursor(0, 1);
    lcd.print(response);
  } else {
    showScrollingText(response, 1);
  }
}
