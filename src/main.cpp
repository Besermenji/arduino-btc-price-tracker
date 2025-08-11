#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
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

// Custom characters
byte bitcoinChar[8] = {0x00, 0x0E, 0x1F, 0x1F, 0x1F, 0x0E, 0x00, 0x00};
byte wifiChar[8] = {0x00, 0x00, 0x0E, 0x11, 0x11, 0x0E, 0x00, 0x00};
byte upArrow[8] = {0x00, 0x04, 0x0E, 0x1F, 0x00, 0x00, 0x00, 0x00};
byte downArrow[8] = {0x00, 0x00, 0x00, 0x1F, 0x0E, 0x04, 0x00, 0x00};
byte dollarChar[8] = {0x00, 0x04, 0x0E, 0x04, 0x04, 0x04, 0x00, 0x00};

// Communication state machine
enum CommunicationState {
  IDLE,
  SENDING_COMMAND,
  WAITING_RESPONSE,
  PROCESSING_RESPONSE
};

CommunicationState currentState = IDLE;
unsigned long lastStateChange = 0;
unsigned long lastPriceUpdate = 0;
bool wifiConnected = false;
bool debugMode = DEBUG_MODE;  // Debug toggle from config

// Debug functions
void debugPrint(const String& message) {
  if (debugMode) {
    Serial.print("[DEBUG] ");
    Serial.println(message);
  }
}

void debugPrintState(const String& operation) {
  if (debugMode) {
    Serial.print("[STATE] ");
    Serial.print(operation);
    Serial.print(" -> State: ");
    switch (currentState) {
      case IDLE: Serial.println("IDLE"); break;
      case SENDING_COMMAND: Serial.println("SENDING_COMMAND"); break;
      case WAITING_RESPONSE: Serial.println("WAITING_RESPONSE"); break;
      case PROCESSING_RESPONSE: Serial.println("PROCESSING_RESPONSE"); break;
    }
  }
}

void debugPrintCommand(const String& command) {
  if (debugMode) {
    Serial.print("[ESP8266] Sending: ");
    Serial.println(command);
  }
}

void debugPrintResponse(const String& response) {
  if (debugMode) {
    Serial.print("[ESP8266] Response: ");
    Serial.println(response);
  }
}

void debugPrintWiFiStatus() {
  if (debugMode) {
    Serial.print("[WiFi] Status: ");
    Serial.println(wifiConnected ? "CONNECTED" : "DISCONNECTED");
  }
}

void debugPrintPriceUpdate(const String& price) {
  if (debugMode) {
    Serial.print("[API] Bitcoin Price: $");
    Serial.println(price);
  }
}

void debugPrintError(const String& error) {
  if (debugMode) {
    Serial.print("[ERROR] ");
    Serial.println(error);
  }
}

unsigned long lastUpdate = 0;
float lastPrice = 0;
bool firstUpdate = true;

// Communication state
String currentCommand = "";
String responseBuffer = "";

void showWelcomeScreen();
void showWiFiConnected();
void initializeWiFi();
void updateBitcoinPrice();
void showPriceDisplay(float price);
String formatPrice(float price);
void showErrorDisplay(String error);
void sendCommandToESP(String command);
void processESPResponse();
void handleWiFiCommunication();
void showRawDebug(String raw);
void showScrollingText(String text, int row);
void showFullResponse(String response);

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.println("UNO+WiFi R3 BTC Price Tracker");
  
  Wire.begin();
  lcd.init();
  lcd.backlight();
  
  // Create custom characters
  lcd.createChar(0, bitcoinChar);
  lcd.createChar(1, wifiChar);
  lcd.createChar(2, upArrow);
  lcd.createChar(3, downArrow);
  lcd.createChar(4, dollarChar);
  
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
  // Handle WiFi communication state machine
  handleWiFiCommunication();
  
  // Update price at intervals
  if (millis() - lastUpdate >= updateInterval) {
    updateBitcoinPrice();
    lastUpdate = millis();
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
    wifiConnected = true;
    lcdMsg(F("WiFi"), F("Ready"));
    delay(800);
  } else {
    wifiConnected = false;
    if (DEBUG_MODE) {
      Serial.print(F("[DEBUG] ESP not ready. Received: '"));
      Serial.print(receivedData);
      Serial.println(F("'"));
    }
    showErrorDisplay("ESP not ready");
    delay(2000);
  }
}

void sendCommandToESP(String command) {
  currentCommand = command;
  currentState = SENDING_COMMAND;
  debugPrintState("Sending command");
  
  // Send command to ESP8266
  debugPrintCommand(command);
  Serial.println(command);
  
  // Give ESP8266 time to process command
  delay(100);
  
  // Wait for response
  currentState = WAITING_RESPONSE;
  debugPrintState("Waiting for response");
  unsigned long startTime = millis();
  
  // Clear any previous response data
  responseBuffer = "";
  
  while (currentState == WAITING_RESPONSE && (millis() - startTime) < 10000) { // Increased timeout to 10 seconds
    if (Serial.available()) {
      processESPResponse();
    }
    delay(50); // Increased delay for better response handling
  }
  
  if (currentState == WAITING_RESPONSE) {
    debugPrintError("ESP8266 timeout - no response received");
    showErrorDisplay("ESP Timeout");
    currentState = IDLE;
    debugPrintState("Timeout - returning to IDLE");
  }
}

void processESPResponse() {
  if (Serial.available()) {
    String response = Serial.readString();
    responseBuffer += response;
    
    // Debug: Show raw response data
    if (debugMode) {
      Serial.print("[RAW] Length: ");
      Serial.print(response.length());
      Serial.print(" Data: ");
      for (int i = 0; i < response.length(); i++) {
        char c = response[i];
        if (c >= 32 && c <= 126) {
          Serial.print(c); // Printable character
        } else {
          Serial.print("["); // Non-printable character
          Serial.print((int)c, HEX);
          Serial.print("]");
        }
      }
      Serial.println();
    }
    
    debugPrintResponse(response);
    
    // Check for end of response indicators
    if (responseBuffer.indexOf("OK") != -1 || 
        responseBuffer.indexOf("ERROR") != -1 ||
        responseBuffer.indexOf("FAIL") != -1 ||
        responseBuffer.indexOf("ready") != -1) {
      
      currentState = PROCESSING_RESPONSE;
      debugPrintState("Processing response");
      
      // Process the response based on the command sent
      if (currentCommand.indexOf("AT+CWJAP") != -1) {
        if (responseBuffer.indexOf("OK") != -1) {
          debugPrint("WiFi connected successfully!");
          wifiConnected = true;
          debugPrintWiFiStatus();
          lcdMsg(F("WiFi"), F("Connected"));
          delay(400);
        } else if (responseBuffer.indexOf("FAIL") != -1) {
          debugPrintError("WiFi connection failed");
          wifiConnected = false;
          debugPrintWiFiStatus();
          showErrorDisplay("WiFi Failed");
        } else {
          debugPrintError("WiFi connection unknown status");
          wifiConnected = false;
          debugPrintWiFiStatus();
        }
      } else if (currentCommand.indexOf("AT+CIPSTART") != -1) {
        if (responseBuffer.indexOf("OK") != -1) {
          debugPrint("TCP connection established");
          lcdMsg(F("HTTP"), F("TCP OK"));
          delay(200);
        } else {
          debugPrintError("TCP connection failed");
          showErrorDisplay("TCP Failed");
        }
      } else if (currentCommand.indexOf("AT+CIPSEND") != -1) {
        if (responseBuffer.indexOf(">") != -1 || responseBuffer.indexOf("OK") != -1) {
          debugPrint("Ready to send data");
          lcdMsg(F("HTTP"), F("SEND..."));
          delay(150);
        } else {
          debugPrintError("Data send preparation failed");
          showErrorDisplay("SEND Failed");
        }
      } else if (currentCommand.indexOf("AT+CIPCLOSE") != -1) {
        if (responseBuffer.indexOf("OK") != -1) {
          debugPrint("Connection closed successfully");
        } else {
          debugPrintError("Connection close failed");
        }
      } else if (currentCommand.indexOf("AT+RST") != -1) {
        if (responseBuffer.indexOf("ready") != -1 || responseBuffer.indexOf("OK") != -1) {
          debugPrint("ESP8266 reset successful");
        } else {
          debugPrintError("ESP8266 reset failed");
        }
      } else if (currentCommand.indexOf("AT+CWMODE") != -1) {
        if (responseBuffer.indexOf("OK") != -1) {
          debugPrint("WiFi mode set successfully");
        } else {
          debugPrintError("WiFi mode setting failed");
        }
      } else if (currentCommand.indexOf("AT+CIPMUX") != -1) {
        if (responseBuffer.indexOf("OK") != -1) {
          debugPrint("Connection mode set successfully");
        } else {
          debugPrintError("Connection mode setting failed");
        }
      }
      
      // Clear buffers and return to idle
      responseBuffer = "";
      currentCommand = "";
      currentState = IDLE;
      debugPrintState("Response processed - returning to IDLE");
    }
  }
}

void handleWiFiCommunication() {
  if (currentState == WAITING_RESPONSE && Serial.available()) {
    processESPResponse();
  }
}

void updateBitcoinPrice() {
  // Indicate HTTP flow on LCD
  lcdMsg(F("HTTP"), F("GET..."));
  delay(1000);

  // Request price via custom ESP8266 firmware
  responseBuffer = "";
  Serial.println("GET");

  // Read response
  lcdMsg(F("Bitcoin Price"), F("Loading..."));
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
      
      // Look for "usd": and "usd_24h_change": patterns
      int usdStart = jsonData.indexOf("\"usd\":");
      int usd_24h_changeStart = jsonData.indexOf("\"usd_24h_change\":");
      
      if (usdStart != -1 && usd_24h_changeStart != -1) {
        // Extract current price
        int valueStart = usdStart + 6; // Skip "usd":
        int valueEnd = jsonData.indexOf(",", valueStart);
        if (valueEnd == -1) valueEnd = jsonData.indexOf("}", valueStart);
        
        // Extract 24h change
        int changeValueStart = usd_24h_changeStart + 18; // Skip "usd_24h_change":
        int changeValueEnd = jsonData.indexOf(",", changeValueStart);
        if (changeValueEnd == -1) changeValueEnd = jsonData.indexOf("}", changeValueStart);
        
        if (valueEnd != -1 && changeValueEnd != -1) {
          String priceStr = jsonData.substring(valueStart, valueEnd);
          String changeStr = jsonData.substring(changeValueStart, changeValueEnd);
          priceStr.trim();
          changeStr.trim();
          
          float btcPrice = priceStr.toFloat();
          float changePercent = changeStr.toFloat();
          
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
              lcd.print(abs(changePercent), 2);
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
    
    // If we get here, something went wrong
    showErrorDisplay("Price fetch failed");
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
