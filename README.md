# Bitcoin Price Tracker & Clock for UNO+WiFi R3

A sophisticated Bitcoin price tracker and real-time clock using an Arduino UNO+WiFi R3 board with built-in ESP8266 WiFi module and an LCD1602 I2C display. Features alternating display between Bitcoin prices and Belgrade time (UTC+2).

## 🎯 **Current Status: FULLY FUNCTIONAL! ✅**

- **Bitcoin Price Tracking**: Real-time prices from CoinGecko API with 24h change percentage
- **Real-Time Clock**: Belgrade timezone (UTC+2) from NTP servers
- **Alternating Display**: 30 seconds Bitcoin price, 30 seconds current time
- **Enhanced Error Handling**: Detailed error messages for troubleshooting
- **Production Ready**: Optimized code with comprehensive error handling

## 🏗️ **Hardware Requirements**

- **Board**: UNO+WiFi R3 (ATmega328P + ESP8266 built-in)
- **Display**: LCD1602 I2C with PCF8574T interface
- **Power**: USB or external power supply (5V, 1A+ recommended)
- **Connections**: I2C (SDA: A4, SCL: A5)
- **Network**: WiFi connection for API calls and NTP time sync

## 🔧 **Key Features**

### **Bitcoin Price Tracking:**
- ✅ **Real-Time Prices**: Live Bitcoin prices from CoinGecko API
- ✅ **24h Change**: Shows percentage change with proper negative sign handling
- ✅ **Clean Display**: "BTC +0.45%" on top line, "$118,748" on bottom line
- ✅ **Auto-Update**: Refreshes every 30 seconds when in BTC mode
- ✅ **Robust Parsing**: Handles JSON responses with error checking

### **Real-Time Clock:**
- ✅ **NTP Synchronization**: Accurate time from pool.ntp.org
- ✅ **Belgrade Timezone**: UTC+2 (configurable)
- ✅ **HH:MM Format**: Clean time display without seconds
- ✅ **Auto-Sync**: Updates time when switching to clock mode

### **Smart Display System:**
- ✅ **Alternating Display**: 30 seconds Bitcoin, 30 seconds time
- ✅ **Seamless Switching**: Automatic transitions between modes
- ✅ **Fresh Data**: Always fetches latest information when switching

### **Error Handling & Reliability:**
- ✅ **WiFi Status**: Checks connection before API calls
- ✅ **HTTP Error Codes**: Handles various HTTP response codes
- ✅ **Network Failures**: Graceful handling of connection issues
- ✅ **Detailed Messages**: Specific error descriptions for troubleshooting
- ✅ **ESP8266 Communication**: Robust serial communication between boards

## 📋 **Setup Instructions**

### **Phase 1: Configuration**
1. **Copy config template**: `cp src/config.h.template src/config.h`
2. **Edit config.h**: Add your WiFi credentials
3. **Verify settings**: Check update intervals and display settings

### **Phase 2: Upload ESP8266 Firmware**
```bash
platformio run -e esp8266_programming -t upload
```
- **DIP Switches**: 5+6 ON (USB → ESP8266 programming mode)
- **Expected**: Upload success message

### **Phase 3: Upload UNO Firmware**
```bash
platformio run -e uno_wifi_r3 -t upload
```
- **DIP Switches**: 3+4 ON (USB → ATmega programming mode)
- **Expected**: Upload success message

### **Phase 4: Production Mode**
- **DIP Switches**: 1+2 ON (ATmega ↔ ESP8266 communication mode)
- **Expected Flow**:
  1. Welcome screen with Bitcoin theme
  2. WiFi initialization and connection
  3. **30 seconds**: Bitcoin price display (BTC +0.45% / $118,748)
  4. **30 seconds**: Current time display (Current Time / 14:30)
  5. **Continuous alternation** every 30 seconds

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
│   ├── main.cpp              # UNO firmware (Bitcoin tracker + clock)
│   ├── esp8266_firmware.cpp  # ESP8266 firmware (WiFi + HTTP + NTP)
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

### **Display Settings**
```cpp
#define LCD_ADDRESS 0x27        // I2C address of your LCD
#define LCD_COLUMNS 16          // LCD width
#define LCD_ROWS 2              // LCD height
#define DEBUG_MODE false        // Set to true for debugging
```

### **Timing Configuration**
```cpp
const unsigned long updateInterval = 60000;  // 60 seconds total cycle
// 30 seconds Bitcoin + 30 seconds Time
```

### **API & NTP Settings**
- **Bitcoin API**: CoinGecko API (free, no API key required)
- **NTP Server**: pool.ntp.org
- **Timezone**: Belgrade (UTC+2) - configurable in ESP8266 code

## 🚀 **How It Works**

### **System Architecture:**
1. **ESP8266**: Handles WiFi, HTTP requests, and NTP time sync
2. **Arduino UNO**: Manages LCD display and user interface
3. **Serial Communication**: Bidirectional communication between boards

### **Bitcoin Price Flow:**
1. UNO sends "GET" command to ESP8266
2. ESP8266 fetches price from CoinGecko API
3. UNO parses JSON response for `"usd"` and `"usd_24h_change"`
4. Displays formatted price with change percentage

### **Time Sync Flow:**
1. UNO sends "TIME" command to ESP8266
2. ESP8266 syncs with NTP server
3. Converts to Belgrade timezone (UTC+2)
4. Sends formatted time (HH:MM) to UNO

### **Display Alternation:**
- **Timer-based**: Every 30 seconds, switches between modes
- **Fresh Data**: Always fetches latest information when switching
- **Seamless**: Smooth transitions between Bitcoin and time displays

## 🔍 **Error Handling & Troubleshooting**

### **Enhanced Error Messages:**

#### **ESP8266 Errors (displayed as "ESP: [error]"):**
- `"WiFi not connected"` - WiFi connection lost
- `"HTTP begin failed"` - HTTP client initialization failed
- `"HTTP 404"` - API endpoint not found
- `"Network request failed"` - Network connectivity issues
- `"NTP not synchronized"` - Time server sync failed

#### **Arduino Parsing Errors:**
- `"Empty response"` - No data received from ESP8266
- `"No JSON: [data]"` - Invalid response format
- `"Missing fields: no USD"` - Incomplete API response
- `"Parse error: no price end"` - JSON parsing failed

### **Common Issues & Solutions:**

#### **"ESP not ready" Error**
- **Cause**: ESP8266 firmware not uploaded or wrong switch configuration
- **Solution**: Upload ESP8266 firmware (switches 5+6 ON)

#### **"WiFi not connected" Error**
- **Cause**: WiFi credentials incorrect or network unavailable
- **Solution**: Check WiFi credentials in config.h and network status

#### **"HTTP [code]" Error**
- **Cause**: API endpoint issues or network problems
- **Solution**: Check internet connection and API availability

#### **Time Not Updating**
- **Cause**: NTP sync issues or timezone problems
- **Solution**: Check internet connection and verify timezone settings

#### **LCD Shows Garbage**
- **Cause**: Wrong I2C address or wiring issues
- **Solution**: Check wiring and verify I2C address in config.h

## 📊 **Performance & Memory**

### **ESP8266:**
- **RAM Usage**: 35.4% (28,972/81,920 bytes)
- **Flash Usage**: 51.3% (390,651/761,840 bytes)
- **WiFi**: Automatic reconnection handling
- **NTP**: Syncs every time request

### **Arduino UNO:**
- **RAM Usage**: 45.7% (935/2,048 bytes)
- **Flash Usage**: 41.2% (13,296/32,256 bytes)
- **Update Frequency**: 30 seconds per mode
- **Response Time**: ~2-5 seconds per update

### **Memory Efficiency:**
- **Direct String Parsing**: No JSON library overhead
- **Optimized Display**: Minimal LCD operations
- **Efficient Communication**: Streamlined serial protocol

## 🔮 **Future Enhancements**

### **Planned Features:**
- **Price History**: Store and display price trends
- **Multiple Cryptocurrencies**: Add ETH, ADA, etc.
- **Price Alerts**: Notify when price reaches thresholds
- **Web Interface**: Remote monitoring via web browser
- **Data Logging**: Save prices to SD card or cloud

### **Potential Improvements:**
- **Customizable Timezones**: User-selectable timezone
- **Display Themes**: Different LCD layouts
- **Battery Backup**: RTC module for offline timekeeping
- **Mobile App**: Bluetooth/WiFi control interface

## 📝 **Code Highlights**

### **Robust Bitcoin Price Parsing:**
```cpp
// Look for "usd": and "usd_24h_change": patterns
int usdStart = jsonData.indexOf("\"usd\":");
int usd_24h_changeStart = jsonData.indexOf("\"usd_24h_change\":");

if (usdStart != -1 && usd_24h_changeStart != -1) {
  // Extract current price
  int valueStart = usdStart + 6; // Skip "usd":
  int valueEnd = jsonData.indexOf(",", valueStart);
  
  // Extract 24h change - robust approach
  int colonPos = jsonData.indexOf(":", usd_24h_changeStart);
  int changeValueStart = colonPos + 1; // Start right after the colon
  
  // Parse and display with proper negative sign handling
  float btcPrice = priceStr.toFloat();
  float changePercent = changeStr.toFloat();
  
  if (changePercent < 0) {
    lcd.print("-");
    lcd.print(fabs(changePercent), 2); // Use fabs() for floats
  }
}
```

### **Smart Display Alternation:**
```cpp
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
```

### **Enhanced Error Handling:**
```cpp
// Check if ESP8266 sent an error response
int errorStart = jsonData.indexOf("\"error\":");
if (errorStart != -1) {
  // Extract and display specific error message
  String errorStr = jsonData.substring(errorValueStart, errorValueEnd);
  showErrorDisplay("ESP: " + errorStr);
  return;
}
```

## 🤝 **Contributing**

This project is open source! Feel free to:
- Report bugs or issues
- Suggest improvements
- Submit pull requests
- Share your modifications
- Help with testing and validation

### **Development Guidelines:**
- Test thoroughly before submitting changes
- Maintain backward compatibility
- Follow existing code style
- Update documentation for new features

## 📄 **License**

This project is open source and available under the MIT License.

## 🙏 **Acknowledgments**

- **CoinGecko**: Free cryptocurrency API
- **NTP Pool**: Network time protocol servers
- **Arduino Community**: Open-source hardware platform
- **PlatformIO**: Modern embedded development platform

---

**Status**: ✅ **Production Ready** - Successfully tracks Bitcoin prices and displays real-time clock!
**Last Updated**: January 2025
**Version**: 3.0 (Bitcoin Tracker + Real-Time Clock)
**Features**: Bitcoin Price Tracking, Belgrade Time, Alternating Display, Enhanced Error Handling
