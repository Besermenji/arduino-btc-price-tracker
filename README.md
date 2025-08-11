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

## 🚀 PlatformIO Advantages

- **Better library management** - Automatic dependency resolution
- **Multiple environments** - Support for different board configurations
- **Integrated development** - Build, upload, and monitor in one place
- **Professional workflow** - Industry-standard embedded development

## 🛠️ Setup Instructions

### 1. Install PlatformIO

#### **Option A: VS Code Extension (Recommended)**
1. Install Visual Studio Code
2. Install "PlatformIO IDE" extension
3. Open the project folder in VS Code

#### **Option B: Command Line**
```bash
pip install platformio
```

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
platformio run
```

### 4. Build and Upload

#### **Build the project:**
```bash
platformio run
```

#### **Upload to board:**
```bash
platformio run --target upload
```

#### **Monitor serial output:**
```bash
platformio run --target monitor
```

#### **Clean build:**
```bash
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
- **File**: `src/main.cpp`
- **Purpose**: Real-time Bitcoin price tracking with WiFi
- **Features**: Price updates, trend indicators, error handling

### **Option 2: LCD Test and Demo**
- **File**: `src/lcd_test.cpp`
- **Purpose**: Test LCD functionality and see pretty display features
- **Features**: Custom characters, animations, scrolling text

## 🔄 Switching Between Projects

### **Method 1: Rename Files (Recommended)**
```bash
# For Bitcoin Tracker
mv src/lcd_test.cpp src/lcd_test.cpp.bak
mv src/main.cpp.bak src/main.cpp

# For LCD Test
mv src/main.cpp src/main.cpp.bak
mv src/lcd_test.cpp.bak src/main.cpp
```

### **Method 2: Edit platformio.ini**
```ini
[env:uno_wifi_r3]
# Change source filter
src_filter = +<main.cpp>      # For Bitcoin tracker
# OR
src_filter = +<lcd_test.cpp>  # For LCD test
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
2. **WiFi not working**: Make sure switch is in Mode 2 (ON ON OFF OFF OFF OFF OFF OFF) and press ESP Reboot button
3. **LCD not displaying**: Check I2C connections and address (default: 0x27)
4. **Serial communication issues**: Ensure baud rate is set to 115200

### **Configuration Issues:**

1. **WiFi not connecting**: Check that `config.h` exists and has correct credentials
2. **Build fails**: Ensure `config.h` is copied from `config.h.template`
3. **Wrong LCD address**: Update `LCD_ADDRESS` in `config.h`

### **Switch Position Reference:**
```
Position: 1  2  3  4  5  6  7  8
Mode 1:  OFF OFF ON ON OFF OFF OFF OFF  (Upload Arduino code)
Mode 2:  ON ON OFF OFF OFF OFF OFF OFF  (Run with WiFi)
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

## 🎯 Next Steps

1. **Install PlatformIO IDE** in VS Code
2. **Setup configuration** by copying `config.h.template` to `config.h`
3. **Edit WiFi credentials** in `src/config.h`
4. **Open the project folder**
5. **Build the project** to install dependencies
6. **Upload to your board**
7. **Enjoy your Bitcoin price tracker!**

## 🔒 Security Features

- **WiFi credentials are gitignored** - Never shared on GitHub
- **Configuration template** - Easy setup without exposing secrets
- **Local configuration** - All sensitive data stays on your machine

## License

This project is open source and available under the MIT License.

---

**Happy coding! 🚀**
