# Bitcoin Price Tracker

A real-time Bitcoin price tracker built with **UNO+WiFi R3** and LCD1602 I2C display that fetches current BTC prices from a cryptocurrency API and displays them on a 16x2 character LCD screen.

## 🚀 Project Status

✅ **Converted to PlatformIO** - Professional embedded development workflow

## 🎯 What This Project Is

A **real-time Bitcoin price tracker** that displays current BTC prices on an LCD1602 I2C display. Built for the **UNO+WiFi R3** board with built-in WiFi capabilities.

## 🏗️ Project Structure

```
arduino-btc-price-tracker/
├── 📁 src/                    # Source code
│   ├── 📄 main.cpp           # Bitcoin price tracker (main project)
│   ├── 📄 lcd_test.cpp       # LCD test and demo
│   ├── 📄 config.h           # Configuration file (create from template)
│   └── 📄 config.h.template  # Template for configuration
├── ⚙️ platformio.ini         # PlatformIO configuration
├── 📖 README.md               # This comprehensive guide
└── 🚫 .gitignore             # Git ignore rules
```

## 🔐 Configuration Setup

### **Important Security Note:**
The `config.h` file contains your WiFi credentials and is **automatically gitignored** to prevent accidentally sharing sensitive information on GitHub.

### **Setup Steps:**

1. **Copy the template:**
   ```bash
   cp src/config.h.template src/config.h
   ```

2. **Edit `src/config.h` with your credentials:**
   ```cpp
   const char* ssid = "YOUR_ACTUAL_WIFI_NAME";
   const char* password = "YOUR_ACTUAL_WIFI_PASSWORD";
   ```

3. **Customize other settings if needed:**
   - LCD I2C address (default: 0x27)
   - Update interval (default: 30 seconds)
   - Serial baud rate (default: 115200)

### **Configuration Options:**

| Setting | Default | Description |
|---------|---------|-------------|
| `ssid` | `YOUR_WIFI_SSID` | Your WiFi network name |
| `password` | `YOUR_WIFI_PASSWORD` | Your WiFi password |
| `LCD_ADDRESS` | `0x27` | I2C address of your LCD |
| `updateInterval` | `30000` | Price update frequency (ms) |
| `SERIAL_BAUD` | `115200` | Serial communication speed |

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
- Clean 16x2 LCD display output with custom characters
- Automatic price refresh every 30 seconds
- Error handling for network issues
- Easy-to-read price formatting with trend indicators
- **Built-in WiFi** - No external WiFi module needed!
- **Secure configuration** - Credentials stored locally, not in Git

## Pin Connections

- **VCC** → 5V
- **GND** → GND  
- **SDA** → A4 (I2C Data)
- **SCL** → A5 (I2C Clock)

## 🔧 **Critical: Switch Configuration & ESP8266 Firmware**

**⚠️ IMPORTANT: Your board has 8 DIP switches that MUST be set correctly for WiFi to work!**

**🚨 CRITICAL DISCOVERY: The ESP8266 needs its own firmware to process AT commands!**

### **Why ESP8266 Firmware is Required:**
- **ATmega328 code** sends AT commands to ESP8266
- **ESP8266 chip** needs firmware to understand and respond to these commands
- **Without ESP8266 firmware**: You get garbled responses (`[` characters) instead of "OK"
- **With ESP8266 firmware**: Clean communication with proper "OK" responses

### **Required Switch Settings for Bitcoin Tracker:**
```
[1] [2] [3] [4] [5] [6] [7] [8]
 ↓   ↓   ↓   ↓   ↓   ↓   ↓   ↓
ON  ON  OFF OFF OFF OFF OFF OFF
```

### **What Each Mode Does (Based on Official Documentation):**
| Mode | Purpose | Switch Settings | Use Case |
|------|---------|-----------------|----------|
| **ATmega ↔ ESP8266** | **ATmega328 ↔ ESP8266** | **SW1, SW2 ON, others OFF** | **Bitcoin Tracker (WiFi)** |
| **USB → ATmega** | USB → ATmega328 (programming) | SW3, SW4 ON, others OFF | Upload Arduino code |
| **USB → ESP8266** | USB → ESP8266 (programming) | SW5, SW6, SW7 ON, others OFF | Upload ESP8266 code |
| **USB → ESP8266** | USB → ESP8266 (communication) | SW5, SW6 ON, others OFF | ESP8266 serial monitor |
| **All Independent** | No communication | All switches OFF | Factory reset |

### **Official Switch Configuration (from Arduino Forum):**
```
Switch: 1  2  3  4  5  6  7  8
ATmega ↔ ESP8266: 1  1  0  0  0  0  0  0  (Bitcoin Tracker)
USB → ATmega:     0  0  1  1  0  0  0  0  (Upload Arduino code)
USB → ESP8266:    0  0  0  0  1  1  1  0  (Upload ESP8266 code)
USB → ESP8266:    0  0  0  0  1  1  0  0  (ESP8266 serial monitor)
All Independent:  0  0  0  0  0  0  0  0  (Factory reset)
```

### **Why ATmega ↔ ESP8266 Mode Works:**
- **SW1 = ON**: Enables ATmega328 → ESP8266 communication
- **SW2 = ON**: Enables ESP8266 → ATmega328 communication  
- **SW3,4 = OFF**: Prevents USB programming conflicts
- **SW5,6,7,8 = OFF**: Disables ESP8266 programming mode

### **Complete Setup Process:**

#### **Phase 1: Upload ESP8266 Firmware**
1. **Power off** board completely (unplug USB)
2. **Set switches** to ESP8266 programming mode: `OFF OFF OFF OFF ON ON ON OFF`
3. **Power on** (plug USB back in)
4. **Upload ESP8266 firmware**:
   ```bash
   platformio run --environment esp8266_programming --target upload
   ```
5. **After upload, turn switch 7 OFF** to prevent factory reset on reboot

#### **Phase 2: Upload ATmega328 Code**
1. **Power off** board completely (unplug USB)
2. **Set switches** to ATmega programming mode: `OFF OFF ON ON OFF OFF OFF OFF`
3. **Power on** (plug USB back in)
4. **Upload Bitcoin tracker code**:
   ```bash
   platformio run --environment uno_wifi_r3 --target upload
   ```

#### **Phase 3: Run WiFi Mode**
1. **Power off** board completely (unplug USB)
2. **Set switches** to WiFi communication mode: `ON ON OFF OFF OFF OFF OFF OFF`
3. **Power on** (plug USB back in)
4. **Reset** board (press RESET button)
5. **Test WiFi communication** (see Serial Testing below)

### **Programming Notes (from Arduino Forum):**
- **For ATmega programming**: Set switches 3,4 ON, others OFF, then upload
- **For ESP8266 programming**: Set switches 5,6,7 ON, others OFF, then upload
- **After ESP8266 upload**: Turn switch 7 OFF to prevent factory reset on reboot
- **Timing matters**: Press reset button during upload if needed
- **If upload fails**: Try multiple times with different reset timing

## 🧪 **Serial Testing & Verification**

### **Step 1: Test ESP8266 Communication**
After completing all phases, open serial monitor:
```bash
platformio run --environment uno_wifi_r3 --target monitor
```

**Expected Output (Success):**
```
UNO+WiFi R3 BTC Price Tracker
[ESP8266] Sending: AT+RST
[STATE] Sending command -> State: SENDING_COMMAND
[STATE] Waiting for response -> State: WAITING_RESPONSE
[ESP8266] Response: OK
[STATE] Processing response -> State: PROCESSING_RESPONSE
[DEBUG] ESP8266 reset successful
[ESP8266] Sending: AT+CWMODE=1
[ESP8266] Response: OK
[DEBUG] WiFi mode set successfully
[ESP8266] Sending: AT+CWJAP="YOUR_WIFI","PASSWORD"
[ESP8266] Response: OK
[WiFi] Status: CONNECTED
[ESP8266] Sending: AT+CIPSTART="TCP","api.coingecko.com",80
[ESP8266] Response: OK
[DEBUG] TCP connection established
[API] Bitcoin Price: $XX,XXX.XX
```

### **Step 2: Troubleshooting Common Issues**

#### **Issue: No ESP8266 Response**
**Symptoms:** `[ERROR] ESP8266 timeout - no response received`
**Solutions:**
- Verify switches are in WiFi mode: `ON ON OFF OFF OFF OFF OFF OFF`
- Check ESP8266 LED is lit (indicates power)
- Try power cycling the board
- Ensure ESP8266 firmware was uploaded successfully

#### **Issue: Garbled Responses**
**Symptoms:** `[ESP8266] Response: [ [ [ [ [`
**Solutions:**
- ESP8266 firmware not uploaded - complete Phase 1
- Wrong switch configuration - use WiFi mode
- Communication timing issues - check baud rate (115200)

#### **Issue: WiFi Connection Fails**
**Symptoms:** `[ESP8266] Response: FAIL`
**Solutions:**
- Check WiFi credentials in ESP8266 firmware
- Verify WiFi network is 2.4GHz (ESP8266 doesn't support 5GHz)
- Check WiFi signal strength
- Ensure ESP8266 has proper firmware

### **Step 3: Verify LCD Display**
**Expected LCD Output:**
- **Welcome screen**: Bitcoin symbol + "BTC Tracker"
- **WiFi status**: WiFi symbol + "WiFi Connected!"
- **Price display**: Bitcoin symbol + "$XX,XXX.XX" + trend arrow

### **Step 4: Performance Monitoring**
**Good Performance Indicators:**
- WiFi connects within 5-10 seconds
- Price updates every 30 seconds
- Clean serial output with no timeouts
- LCD updates smoothly without flickering

## 🚀 PlatformIO Advantages

- **Better library management** - Automatic dependency resolution
- **Multiple environments** - Support for different board configurations
- **Integrated development** - Build, upload, and monitor in one place
- **Professional workflow** - Industry-standard embedded development

## Quick Start

### 1. Install PlatformIO
- **VS Code Extension** (Recommended): Install "PlatformIO IDE"
- **Command Line**: `pip install platformio`

### 2. Setup Configuration
```bash
# Copy configuration template
cp src/config.h.template src/config.h

# Edit with your WiFi credentials
# Use your preferred text editor
nano src/config.h
# or
code src/config.h
```

### 3. Open Project
```bash
cd arduino-btc-price-tracker
```

### 4. Debug Configuration
The project includes essential debug output to help you troubleshoot:
- **WiFi connection status** and AT command responses
- **ESP8266 communication** state machine transitions
- **API responses** from CoinGecko
- **Error conditions** and failures

**To enable/disable debug mode:**
1. Edit `src/config.h`
2. Set `DEBUG_MODE` to `true` (enabled) or `false` (disabled)
3. Rebuild and upload

**Example config changes:**
```cpp
// Enable debug output (default)
#define DEBUG_MODE true

// Disable debug output (cleaner serial monitor)
#define DEBUG_MODE false
```

**Debug output includes:**
- `[STATE]` - State machine transitions (IDLE → SENDING_COMMAND → WAITING_RESPONSE → PROCESSING_RESPONSE)
- `[ESP8266]` - AT commands sent and responses received
- `[WiFi]` - WiFi connection status updates
- `[API]` - Bitcoin price updates
- `[ERROR]` - Error conditions and failures

### 5. Build and Upload

#### **Bitcoin Price Tracker (Default):**
```bash
# Build project
platformio run --environment uno_wifi_r3

# Upload to board
platformio run --environment uno_wifi_r3 --target upload

# Monitor serial output
platformio run --environment uno_wifi_r3 --target monitor
```

**💡 Debug Output Example:**
```
[ESP8266] Sending: AT+RST
[STATE] Sending command -> State: SENDING_COMMAND
[STATE] Waiting for response -> State: WAITING_RESPONSE
[ESP8266] Response: OK
[STATE] Processing response -> State: PROCESSING_RESPONSE
[WiFi] Status: CONNECTED
[ESP8266] Sending: AT+CIPSTART="TCP","api.coingecko.com",80
[ESP8266] Response: OK
[API] Bitcoin Price: $43,250.12
```

#### **LCD Test and Demo:**
```bash
# Build project
platformio run --environment lcd_test

# Upload to board
platformio run --environment lcd_test --target upload

# Monitor serial output
platformio run --environment lcd_test --target monitor
```

#### **Quick Commands (Default Environment):**
```bash
# Build and upload Bitcoin tracker (default)
platformio run --target upload

# Monitor output
platformio run --target monitor

# Clean build
platformio run --target clean
```

## 🔧 Environment Configuration

### **uno_wifi_r3 (Default)**
- **Board**: Arduino Uno
- **Platform**: atmelavr
- **Framework**: arduino
- **Upload Speed**: 115200
- **Monitor Speed**: 115200

### **uno (Standard Arduino Uno)**
- **Board**: Arduino Uno
- **Platform**: atmelavr
- **Framework**: arduino

### **esp8266 (ESP8266 Module)**
- **Board**: ESP-01 1MB
- **Platform**: espressif8266
- **Framework**: arduino

## 📚 Library Dependencies

PlatformIO automatically installs these libraries:

- **LiquidCrystal_I2C** (v1.1.4) - LCD1602 I2C display
- **ArduinoJson** (v6.21.3) - JSON parsing for API responses

## 🎯 How to Use This Project

### **Option 1: Bitcoin Price Tracker (Default)**
- **Environment**: `uno_wifi_r3` (default)
- **File**: `src/main.cpp`
- **Purpose**: Real-time Bitcoin price tracking with WiFi
- **Features**: Price updates, trend indicators, error handling
- **Build Command**: `platformio run --environment uno_wifi_r3`

### **Option 2: LCD Test and Demo**
- **Environment**: `lcd_test`
- **File**: `src/lcd_test.cpp`
- **Purpose**: Test LCD functionality and see pretty display features
- **Features**: Custom characters, animations, scrolling text
- **Build Command**: `platformio run --environment lcd_test`

## 🔄 Switching Between Projects

### **Method 1: Use Different Environments (Recommended)**
```bash
# For Bitcoin Tracker
platformio run --environment uno_wifi_r3

# For LCD Test
platformio run --environment lcd_test
```

### **Method 2: Upload Specific Environment**
```bash
# Upload Bitcoin Tracker
platformio run --environment uno_wifi_r3 --target upload

# Upload LCD Test
platformio run --environment lcd_test --target upload
```

### **Method 3: Monitor Specific Environment**
```bash
# Monitor Bitcoin Tracker
platformio run --environment uno_wifi_r3 --target monitor

# Monitor LCD Test
platformio run --environment lcd_test --target monitor
```

## 🚨 Hardware Switch Configuration

Your UNO+WiFi R3 board has an 8-position switch that controls how the modules work together:

### **Mode 1: Upload Code to ATmega328P (Arduino)**
Switch positions: `OFF OFF ON ON OFF OFF OFF OFF`
- Position 3: ON
- Position 4: ON
- All others: OFF
- **Use this mode to upload the Arduino code**

### **Mode 2: Run with WiFi Enabled**
Switch positions: `ON ON OFF OFF OFF OFF OFF OFF`
- Position 1: ON
- Position 2: ON
- All others: OFF
- **Use this mode to run the Bitcoin tracker**

## 🔄 Code Upload Process

### **Step 1: Upload Arduino Code**
1. Set switch to **Mode 1** (OFF OFF ON ON OFF OFF OFF OFF)
2. Select "Arduino Uno" board in PlatformIO
3. Upload the code
4. Wait for upload to complete

### **Step 2: Configure WiFi and Run**
1. **IMPORTANT**: Change switch to **Mode 2** (ON ON OFF OFF OFF OFF OFF OFF)
2. **IMPORTANT**: Press the "ESP Reboot" button on the board
3. Open Serial Monitor (Tools > Serial Monitor)
4. Set baud rate to 115200
5. The device will connect to WiFi and start tracking Bitcoin prices

## 🔧 WiFi Configuration

**⚠️ Important:** WiFi credentials are now stored in `src/config.h` (not in the main code):

1. **Copy the template:**
   ```bash
   cp src/config.h.template src/config.h
   ```

2. **Edit `src/config.h`:**
   ```cpp
   const char* ssid = "YOUR_ACTUAL_WIFI_NAME";
   const char* password = "YOUR_ACTUAL_WIFI_PASSWORD";
   ```

3. **The file is automatically gitignored** - your credentials won't be shared on GitHub!

## 📚 What's Included

### **Main Bitcoin Tracker (`main.cpp`)**
- ✅ WiFi connection management
- ✅ CoinGecko API integration
- ✅ Real-time price updates
- ✅ Custom LCD characters (Bitcoin, WiFi, arrows)
- ✅ Price trend indicators
- ✅ Error handling and retry logic
- ✅ Professional display formatting
- ✅ **Secure configuration system**

### **LCD Test (`lcd_test.cpp`)**
- ✅ Custom character creation
- ✅ Welcome animations
- ✅ Scrolling text demos
- ✅ Price simulation with arrows
- ✅ WiFi status display
- ✅ Multiple display styles

### **Configuration (`config.h`)**
- ✅ WiFi credentials (secure, gitignored)
- ✅ LCD settings
- ✅ API configuration
- ✅ Update intervals
- ✅ Serial communication settings

## 🎨 Visual Features

### **Custom Characters**
- **₿ Bitcoin symbol** - Custom 8x8 pixel design
- **📶 WiFi symbol** - Connection status indicator
- **⬆️⬇️ Arrows** - Price movement indicators
- **💲 Dollar sign** - Professional currency display

### **Display Layouts**
- **Welcome screen** - Centered with symbols
- **Price display** - Bitcoin symbol + formatted price + trend arrows
- **Status screens** - WiFi connection, errors, etc.
- **Smart formatting** - Automatic comma insertion for thousands

## Power Options

- **Micro USB**: 5V power via USB connection (500mA max)
- **VIN Pin**: 9-24V DC external power supply
- **Power Output**: Up to 800mA at 5V and 3.3V

## API

Uses CoinGecko API to fetch real-time Bitcoin prices in USD.

## 🐛 Troubleshooting

### **Common PlatformIO Issues:**

1. **Library not found**: Run `platformio lib install <library_name>`
2. **Board not recognized**: Check `platformio.ini` board configuration
3. **Upload failed**: Verify board switch position and USB connection
4. **Build errors**: Run `platformio run --target clean` then rebuild

### **Common Hardware Issues:**

1. **Code won't upload**: Make sure switch is in Mode 1 (OFF OFF ON ON OFF OFF OFF OFF)
2. **WiFi not working**: Make sure switch is in Mode 4 (ON ON OFF OFF OFF OFF OFF OFF) for ATmega328 ↔ ESP8266 communication
3. **LCD not displaying**: Check I2C connections and address (default: 0x27)
4. **Serial communication issues**: Ensure baud rate is set to 115200

### **Configuration Issues:**

1. **WiFi not connecting**: Check that `config.h` exists and has correct credentials
2. **Build fails**: Ensure `config.h` is copied from `config.h.template`
3. **Wrong LCD address**: Update `LCD_ADDRESS` in `config.h`

### **Switch Position Reference (Official):**
```
Position: 1  2  3  4  5  6  7  8
ATmega ↔ ESP8266: 1  1  0  0  0  0  0  0  (Bitcoin Tracker - WiFi)
USB → ATmega:     0  0  1  1  0  0  0  0  (Upload Arduino code)
USB → ESP8266:    0  0  0  0  1  1  1  0  (Upload ESP8266 code)
USB → ESP8266:    0  0  0  0  1  1  0  0  (ESP8266 serial monitor)
All Independent:  0  0  0  0  0  0  0  0  (Factory reset)
```

### **Serial Monitor Issues:**
```bash
# Set correct baud rate
platformio device monitor --baud 115200
```

## 🔧 Advanced Configuration

### **Custom Build Flags:**
```ini
[env:uno_wifi_r3]
build_flags = 
    -D ARDUINO_AVR_UNO
    -D F_CPU=16000000L
    -D UNO_WIFI_R3
    -D DEBUG_MODE
```

### **Multiple Source Files:**
```ini
[env:uno_wifi_r3]
src_filter = +<main.cpp> +<utils.cpp>
```

## 📈 Performance Monitoring

### **Memory Usage:**
```bash
# Check memory usage
platformio run --target size
```

### **Upload Statistics:**
```bash
# Monitor upload progress
platformio run --target upload --verbose
```

## 📊 Build Information

### **Build Statistics:**
- **Flash Usage**: ~15-20KB (out of 32KB available)
- **SRAM Usage**: ~1.5-2KB (out of 2KB available)
- **EEPROM Usage**: Minimal

### **Optimization:**
- **Compiler**: avr-gcc
- **Optimization Level**: -Os (size optimization)
- **Debug Symbols**: Disabled for production

## Development

This project uses **PlatformIO** for professional embedded development:

- **Automatic library management** - No manual library installation needed
- **Multiple environments** - Support for different board configurations
- **Integrated workflow** - Build, upload, and monitor in one place
- **Professional tools** - Industry-standard development environment
- **Secure configuration** - Credentials stored locally, not in Git

## Board Compatibility

This project is specifically designed for the **UNO+WiFi R3** board, which combines Arduino UNO compatibility with built-in ESP8266 WiFi capabilities. The dual-mode operation allows you to program the Arduino and then run it with WiFi enabled, making it perfect for IoT projects like this Bitcoin price tracker.

## 🎯 **Complete Workflow Summary**

### **1. Initial Setup**
- Install PlatformIO and ESP8266 support
- Configure WiFi credentials in ESP8266 firmware
- Set up project structure

### **2. Upload Process (3 Phases)**
- **Phase 1**: Upload ESP8266 firmware (switches: `OFF OFF OFF OFF ON ON ON OFF`)
- **Phase 2**: Upload ATmega328 code (switches: `OFF OFF ON ON OFF OFF OFF OFF`)
- **Phase 3**: Run WiFi mode (switches: `ON ON OFF OFF OFF OFF OFF OFF`)

### **3. Testing & Verification**
- Monitor serial output for clean communication
- Verify WiFi connection and price updates
- Check LCD display functionality
- Monitor performance and troubleshoot issues

### **4. Expected Results**
- Clean "OK" responses from ESP8266
- Successful WiFi connection
- Real-time Bitcoin price updates
- Smooth LCD display updates

## 🚀 **Next Steps**

1. **Install PlatformIO IDE** in VS Code
2. **Setup configuration** by copying `config.h.template` to `config.h`
3. **Edit WiFi credentials** in `src/config.h` and `src/esp8266_firmware.cpp`
4. **Open the project folder**
5. **Follow the 3-phase upload process** (ESP8266 → ATmega328 → WiFi mode)
6. **Test and verify** WiFi communication
7. **Enjoy your Bitcoin price tracker!**

## 🔒 Security Features

- **WiFi credentials are gitignored** - Never shared on GitHub
- **Configuration template** - Easy setup without exposing secrets
- **Local configuration** - All sensitive data stays on your machine

## License

This project is open source and available under the MIT License.

---

**Happy coding! 🚀**
