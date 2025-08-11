# Bitcoin Price Tracker for UNO+WiFi R3

A simple and efficient Bitcoin price tracker using an Arduino UNO+WiFi R3 board with built-in ESP8266 WiFi module and an LCD1602 I2C display.

## 🎯 **Current Status: WORKING! ✅**

- **Price Extraction**: Successfully extracts Bitcoin prices from CoinGecko API
- **Memory Optimized**: Uses direct string parsing instead of ArduinoJson library
- **Clean Display**: Simple, clean price display without debug clutter
- **Production Ready**: All test code removed, streamlined operation

## 🏗️ **Hardware Requirements**

- **Board**: UNO+WiFi R3 (ATmega328P + ESP8266 built-in)
- **Display**: LCD1602 I2C with PCF8574T interface
- **Power**: USB or external power supply (5V, 1A+ recommended)
- **Connections**: I2C (SDA: A4, SCL: A5)

## 🔧 **Key Features**

### **Working Features:**
- ✅ **WiFi Connection**: Automatic connection to your WiFi network
- ✅ **Price Fetching**: Gets real-time Bitcoin prices from CoinGecko API
- ✅ **Price Display**: Clean LCD display showing "Bitcoin: $43,250.25"
- ✅ **Memory Efficient**: Direct JSON parsing without memory issues
- ✅ **Auto-Update**: Refreshes price every 30 seconds
- ✅ **Error Handling**: Graceful fallback if price fetch fails

### **Technical Improvements:**
- **No ArduinoJson**: Eliminated memory overflow issues
- **Direct String Parsing**: Looks for `"usd":` pattern in JSON response
- **Chunked Response Handling**: Properly collects complete JSON responses
- **Clean Code**: Removed all debug/test functionality

## 📋 **Setup Instructions**

### **Phase 1: Upload ESP8266 Firmware**
```bash
platformio run -e esp8266_programming -t upload
```
- **DIP Switches**: 5+6 ON (USB → ESP8266 programming mode)
- **Expected**: Upload success message

### **Phase 2: Upload UNO Firmware**
```bash
platformio run -e uno_wifi_r3 -t upload
```
- **DIP Switches**: 3+4 ON (USB → ATmega programming mode)
- **Expected**: Upload success message

### **Phase 3: Run Production Mode**
- **DIP Switches**: 1+2 ON (ATmega ↔ ESP8266 communication mode)
- **Expected Flow**:
  1. Welcome screen with Bitcoin symbols
  2. WiFi connecting animation
  3. "WiFi Connected! Ready to track!"
  4. **Price Display**: "Bitcoin: $[PRICE]"
  5. **Auto-updates** every 30 seconds

## 🔌 **DIP Switch Configuration**

| Switches | Mode | Use Case |
|----------|------|----------|
| **1+2 ON** | ATmega ↔ ESP8266 | **Production Mode** - Normal operation |
| **3+4 ON** | USB → ATmega | Upload UNO firmware |
| **5+6 ON** | USB → ESP8266 | Upload ESP8266 firmware |
| **All OFF** | Independent | Not recommended |

## 📁 **Project Structure**

```
arduino-btc-price-tracker/
├── src/
│   ├── main.cpp              # UNO firmware (Bitcoin tracker)
│   ├── esp8266_firmware.cpp  # ESP8266 firmware (WiFi + HTTP)
│   ├── config.h              # WiFi credentials & settings
│   └── config.h.template     # Template for config
├── platformio.ini            # PlatformIO configuration
└── README.md                 # This file
```

## ⚙️ **Configuration**

### **WiFi Setup**
Edit `src/config.h`:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### **API Settings**
- **Endpoint**: CoinGecko API (free, no API key required)
- **URL**: `https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd`
- **Update Interval**: 30 seconds (configurable)

## 🚀 **How It Works**

1. **Startup**: UNO initializes LCD and waits for ESP8266
2. **WiFi Connection**: ESP8266 connects to WiFi automatically
3. **Price Request**: UNO sends "GET" command to ESP8266
4. **HTTP Fetch**: ESP8266 fetches price from CoinGecko API
5. **Price Extraction**: UNO parses JSON response for `"usd":` value
6. **Display**: Shows "Bitcoin: $[PRICE]" on LCD
7. **Repeat**: Updates every 30 seconds

## 🔍 **Troubleshooting**

### **Common Issues & Solutions:**

#### **"ESP not ready" Error**
- **Cause**: ESP8266 firmware not uploaded or wrong switch configuration
- **Solution**: Upload ESP8266 firmware (switches 5+6 ON)

#### **"Price fetch failed" Error**
- **Cause**: WiFi connection issues or API problems
- **Solution**: Check WiFi credentials and internet connection

#### **LCD Shows Garbage**
- **Cause**: Wrong I2C address or wiring issues
- **Solution**: Check wiring and verify I2C address in config.h

#### **Board Restarts**
- **Cause**: Power supply insufficient for ESP8266
- **Solution**: Use powered USB hub or external 5V/1A+ supply



## 📊 **Performance**

- **RAM Usage**: 75.2% (1541/2048 bytes)
- **Flash Usage**: 51.3% (16548/32256 bytes)
- **Update Frequency**: Every 30 seconds
- **Response Time**: ~2-5 seconds per update
- **Memory Efficiency**: Direct string parsing, no JSON library overhead

## 🔮 **Future Enhancements**

- **Price History**: Store and display price trends
- **Multiple Cryptocurrencies**: Add ETH, ADA, etc.
- **Price Alerts**: Notify when price reaches thresholds
- **Web Interface**: Remote monitoring via web browser
- **Data Logging**: Save prices to SD card or cloud

## 📝 **Code Highlights**

### **Efficient Price Extraction**
```cpp
// Look for "usd": pattern in JSON response
int usdStart = jsonData.indexOf("\"usd\":");
if (usdStart != -1) {
  int valueStart = usdStart + 6; // Skip "usd":
  int valueEnd = jsonData.indexOf(",", valueStart);
  if (valueEnd == -1) valueEnd = jsonData.indexOf("}", valueStart);
  
  if (valueEnd != -1) {
    String priceStr = jsonData.substring(valueStart, valueEnd);
    priceStr.trim();
    float btcPrice = priceStr.toFloat();
    // Display price...
  }
}
```

### **Clean LCD Display**
```cpp
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Bitcoin:");
lcd.setCursor(0, 1);
lcd.print("$");
lcd.print(btcPrice);
```

## 🤝 **Contributing**

This project is open source! Feel free to:
- Report bugs or issues
- Suggest improvements
- Submit pull requests
- Share your modifications

## 📄 **License**

This project is open source and available under the MIT License.

---

**Status**: ✅ **Production Ready** - Successfully tracks and displays Bitcoin prices!
**Last Updated**: January 2025
**Version**: 2.0 (Simplified & Optimized)
