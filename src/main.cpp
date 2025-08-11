#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include "config.h"

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

unsigned long lastUpdate = 0;
float lastPrice = 0;
bool firstUpdate = true;

// Communication state
enum CommunicationState {
  IDLE,
  SENDING_COMMAND,
  WAITING_RESPONSE,
  PROCESSING_RESPONSE
};

CommunicationState commState = IDLE;
String currentCommand = "";
String responseBuffer = "";

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

void setup() {
  Serial.begin(SERIAL_BAUD);
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
  // Reset ESP8266
  sendCommandToESP("AT+RST");
  delay(2000);
  
  // Set WiFi mode to station
  sendCommandToESP("AT+CWMODE=1");
  delay(1000);
  
  // Connect to WiFi network
  String cmd = "AT+CWJAP=\"";
  cmd += ssid;
  cmd += "\",\"";
  cmd += password;
  cmd += "\"";
  sendCommandToESP(cmd);
  delay(5000);
  
  // Set single connection mode
  sendCommandToESP("AT+CIPMUX=0");
  delay(1000);
}

void sendCommandToESP(String command) {
  currentCommand = command;
  commState = SENDING_COMMAND;
  
  // Send command to ESP8266
  Serial.println(command);
  
  // Wait for response
  commState = WAITING_RESPONSE;
  unsigned long startTime = millis();
  
  while (commState == WAITING_RESPONSE && (millis() - startTime) < 5000) {
    if (Serial.available()) {
      processESPResponse();
    }
    delay(10);
  }
  
  if (commState == WAITING_RESPONSE) {
    showErrorDisplay("ESP Timeout");
    commState = IDLE;
  }
}

void processESPResponse() {
  if (Serial.available()) {
    String response = Serial.readString();
    responseBuffer += response;
    
    // Check for end of response indicators
    if (responseBuffer.indexOf("OK") != -1 || 
        responseBuffer.indexOf("ERROR") != -1 ||
        responseBuffer.indexOf("FAIL") != -1) {
      
      commState = PROCESSING_RESPONSE;
      
      // Process the response based on the command sent
      if (currentCommand.indexOf("AT+CWJAP") != -1) {
        if (responseBuffer.indexOf("OK") != -1) {
          Serial.println("WiFi connected successfully");
        } else {
          showErrorDisplay("WiFi Failed");
        }
      }
      
      // Clear buffers and return to idle
      responseBuffer = "";
      currentCommand = "";
      commState = IDLE;
    }
  }
}

void handleWiFiCommunication() {
  if (commState == WAITING_RESPONSE && Serial.available()) {
    processESPResponse();
  }
}

void updateBitcoinPrice() {
  // Connect to API server
  String cmd = "AT+CIPSTART=\"TCP\",\"api.coingecko.com\",80";
  sendCommandToESP(cmd);
  
  if (commState == IDLE) {
    // Prepare HTTP request
    String request = "GET /api/v3/simple/price?ids=bitcoin&vs_currencies=usd HTTP/1.1\r\n";
    request += "Host: api.coingecko.com\r\n";
    request += "Connection: close\r\n\r\n";
    
    // Send data length command
    String sendCmd = "AT+CIPSEND=";
    sendCmd += request.length();
    sendCommandToESP(sendCmd);
    
    if (commState == IDLE) {
      // Send the actual request
      Serial.print(request);
      delay(2000);
      
      // Read response
      responseBuffer = "";
      unsigned long startTime = millis();
      
      while ((millis() - startTime) < 10000) {
        if (Serial.available()) {
          String response = Serial.readString();
          responseBuffer += response;
          
          // Check if we have complete response
          if (responseBuffer.indexOf("\r\n\r\n") != -1) {
            break;
          }
        }
        delay(10);
      }
      
      // Parse JSON response
      int jsonStart = responseBuffer.indexOf("{");
      if (jsonStart != -1) {
        String jsonData = responseBuffer.substring(jsonStart);
        
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
      
      // Close connection
      sendCommandToESP("AT+CIPCLOSE");
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
