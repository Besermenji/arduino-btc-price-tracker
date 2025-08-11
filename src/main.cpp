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
  // Check for ESP8266 data continuously
  if (Serial.available()) {
    String data = Serial.readString();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ESP Data:");
    lcd.setCursor(0, 1);
    lcd.print(data.substring(0, 16));
    delay(2000);
  }
  
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
  
  delay(1500);
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
  lcdMsg(F("Reading"), F("Response..."));
  unsigned long startTime = millis();
  responseBuffer = "";
  
  while ((millis() - startTime) < 15000) {
    if (Serial.available()) {
      String chunk = Serial.readString();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Chunk:");
      lcd.setCursor(0, 1);
      lcd.print(chunk.substring(0, 16));
      delay(500);
      
      responseBuffer += chunk;
      
      // Check if we have a complete JSON response
      if (responseBuffer.indexOf('{') != -1 && responseBuffer.indexOf('}') != -1) {
        lcdMsg(F("Complete"), F("JSON Found"));
        delay(1000);
        break;
      }
    }
    delay(10);
  }
  
  lcdMsg(F("Response"), F("Complete"));
  delay(1000);
  
  // Parse JSON response - extract price directly
  if (responseBuffer.length() > 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Response:");
    lcd.setCursor(0, 1);
    lcd.print(responseBuffer.substring(0, 16));
    delay(2000);
    
    int jsonStart = responseBuffer.indexOf('{');
    if (jsonStart != -1) {
      String jsonData = responseBuffer.substring(jsonStart);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("JSON:");
      lcd.setCursor(0, 1);
      lcd.print(jsonData.substring(0, 16));
      delay(2000);
      
      // Extract price directly from JSON string
      lcdMsg(F("Extracting"), F("Price..."));
      delay(1000);
      
      // Look for "usd": pattern in the JSON
      int usdStart = jsonData.indexOf("\"usd\":");
      if (usdStart != -1) {
        int valueStart = usdStart + 6; // Skip "usd":
        int valueEnd = jsonData.indexOf(",", valueStart);
        if (valueEnd == -1) valueEnd = jsonData.indexOf("}", valueStart);
        
        if (valueEnd != -1) {
          String priceStr = jsonData.substring(valueStart, valueEnd);
          priceStr.trim();
          float btcPrice = priceStr.toFloat();
          
          if (btcPrice > 0) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Price Found!");
            lcd.setCursor(0, 1);
            lcd.print("$");
            lcd.print(btcPrice, 2);
            delay(2000);
            showPriceDisplay(btcPrice);
            lastPrice = btcPrice;
            firstUpdate = false;
            return;
          } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Invalid Price");
            lcd.setCursor(0, 1);
            lcd.print("Value: ");
            lcd.print(priceStr);
            delay(2000);
          }
        } else {
          lcdMsg(F("No Price End"), F("Found"));
          delay(2000);
        }
      } else {
        lcdMsg(F("No USD Field"), F("Found"));
        delay(2000);
      }
      
      showFullResponse("Price extraction failed");
    } else {
      showFullResponse("No JSON: " + responseBuffer.substring(0, 32));
    }
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
