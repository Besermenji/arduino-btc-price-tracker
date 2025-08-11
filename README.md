# Arduino Bitcoin Price Tracker

A real-time Bitcoin price tracker built with **UNO+WiFi R3** and LCD1602 I2C display that fetches current BTC prices from a cryptocurrency API and displays them on a 16x2 character LCD screen.

## Hardware Requirements

- **UNO+WiFi R3** - ATmega328P + ESP8266 with built-in WiFi
  - ATmega328P microcontroller (Arduino UNO compatible)
  - ESP8266 WiFi module with 32MB flash
  - CH340G USB-TTL converter
  - 14 Digital I/O pins
  - 6 Analog Input pins
  - 32MB flash memory
  - Built-in WiFi 802.11 b/g/n 2.4 GHz
  - Micro USB connector
- **LCD1602 I2C Display** - 16x2 character LCD with PCF8574T I2C interface
- **USB Cable** - Micro USB for programming and power
- **Jumper Wires** - For connections

## Features

- Real-time Bitcoin price updates
- Clean 16x2 LCD display output
- Automatic price refresh every 30 seconds
- Error handling for network issues
- Easy-to-read price formatting
- **Built-in WiFi** - No external WiFi module needed!

## Pin Connections

- **VCC** → 5V
- **GND** → GND  
- **SDA** → A4 (I2C Data)
- **SCL** → A5 (I2C Clock)

## Libraries Required

- LiquidCrystal_I2C
- WiFi (built into ESP8266)
- HTTPClient (built into ESP8266)
- ArduinoJson

## Detailed Setup Instructions

### 1. Hardware Switch Configuration

Your UNO+WiFi R3 board has an 8-position switch that controls how the modules work together. For the Bitcoin price tracker, you need to configure it as follows:

#### **Mode 1: Upload Code to ATmega328P (Arduino)**
Switch positions: `OFF OFF ON ON OFF OFF OFF OFF`
- Position 3: ON
- Position 4: ON
- All others: OFF
- **Use this mode to upload the Arduino code**

#### **Mode 2: Run with WiFi Enabled**
Switch positions: `ON ON OFF OFF OFF OFF OFF OFF`
- Position 1: ON
- Position 2: ON
- All others: OFF
- **Use this mode to run the Bitcoin tracker**

### 2. Arduino IDE Setup

#### **Step 1: Install ESP8266 Board Support**
1. Open Arduino IDE
2. Go to **File > Preferences**
3. In "Additional Boards Manager URLs" add:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Click **OK**

#### **Step 2: Install ESP8266 Board Package**
1. Go to **Tools > Board > Boards Manager**
2. Search for "esp8266"
3. Find "ESP8266 by ESP8266 Community"
4. Install version **2.1.0** (latest stable)
5. Close the window

### 3. Board Configuration

#### **For Uploading Arduino Code:**
- **Board**: "Arduino Uno"
- **Processor**: "ATmega328P"
- **Port**: Select your USB port

#### **For ESP8266 Programming (if needed):**
- **Board**: "Generic ESP8266 Module"
- **Upload Speed**: 115200

### 4. Code Upload Process

#### **Step 1: Upload Arduino Code**
1. Set switch to **Mode 1** (OFF OFF ON ON OFF OFF OFF OFF)
2. Select "Arduino Uno" board
3. Upload `btc_price_tracker.ino`
4. Wait for upload to complete

#### **Step 2: Configure WiFi and Run**
1. **IMPORTANT**: Change switch to **Mode 2** (ON ON OFF OFF OFF OFF OFF OFF)
2. **IMPORTANT**: Press the "ESP Reboot" button on the board
3. Open Serial Monitor (Tools > Serial Monitor)
4. Set baud rate to 115200
5. The device will connect to WiFi and start tracking Bitcoin prices

### 5. WiFi Configuration

Edit the code to add your WiFi credentials:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

## Power Options

- **Micro USB**: 5V power via USB connection (500mA max)
- **VIN Pin**: 9-24V DC external power supply
- **Power Output**: Up to 800mA at 5V and 3.3V

## API

Uses CoinGecko API to fetch real-time Bitcoin prices in USD.

## Troubleshooting

### **Common Issues:**

1. **Code won't upload**: Make sure switch is in Mode 1 (OFF OFF ON ON OFF OFF OFF OFF)
2. **WiFi not working**: Make sure switch is in Mode 2 (ON ON OFF OFF OFF OFF OFF OFF) and press ESP Reboot button
3. **LCD not displaying**: Check I2C connections and address (default: 0x27)
4. **Serial communication issues**: Ensure baud rate is set to 115200

### **Switch Position Reference:**
```
Position: 1  2  3  4  5  6  7  8
Mode 1:  OFF OFF ON ON OFF OFF OFF OFF  (Upload Arduino code)
Mode 2:  ON ON OFF OFF OFF OFF OFF OFF  (Run with WiFi)
```

## Project Status

🚧 In Development

## Board Compatibility

This project is specifically designed for the **UNO+WiFi R3** board, which combines Arduino UNO compatibility with built-in ESP8266 WiFi capabilities. The dual-mode operation allows you to program the Arduino and then run it with WiFi enabled, making it perfect for IoT projects like this Bitcoin price tracker.
