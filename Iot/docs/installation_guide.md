# BantayGas IoT Installation Guide

## Overview
This guide provides step-by-step instructions for installing and setting up the BantayGas IoT gas detection system.

## Prerequisites

### Hardware Requirements
- **ESP32 Dev Board** (any variant with WiFi)
- **MQ-6 Gas Sensor** (LPG/Butane/Propane detection)
- **LEDs**: Red, Green, Yellow (5mm, 3.3V compatible)
- **Buzzer** (3.3V compatible)
- **Resistors**: 3x 220Ω, 1x 10kΩ
- **Breadboard and jumper wires**
- **USB cable** for programming

### Software Requirements
- **Arduino IDE** (version 2.0 or later)
- **ESP32 Board Package** (version 2.0.0 or later)
- **Required Libraries** (see Library Installation section)

### Optional Components
- **OLED Display** (128x64, I2C)
- **Temperature/Humidity Sensor** (DHT22)
- **Push Button** (for calibration)
- **Relay Module** (for ventilation control)

## Installation Steps

### Step 1: Arduino IDE Setup

1. **Download Arduino IDE**
   - Visit [arduino.cc](https://www.arduino.cc/en/software)
   - Download and install Arduino IDE 2.0 or later

2. **Install ESP32 Board Package**
   - Open Arduino IDE
   - Go to File → Preferences
   - Add this URL to Additional Board Manager URLs:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to Tools → Board → Boards Manager
   - Search for "ESP32" and install "ESP32 by Espressif Systems"

3. **Select Board**
   - Go to Tools → Board → ESP32 Arduino
   - Select "ESP32 Dev Module" or your specific board

### Step 2: Library Installation

Install the following libraries through Arduino IDE Library Manager:

1. **ArduinoJson** (version 6.19.0 or later)
   - Go to Tools → Manage Libraries
   - Search for "ArduinoJson" and install

2. **WiFi** (included with ESP32 package)
   - Already included with ESP32 board package

3. **HTTPClient** (included with ESP32 package)
   - Already included with ESP32 board package

4. **EEPROM** (included with ESP32 package)
   - Already included with ESP32 board package

5. **SPIFFS** (included with ESP32 package)
   - Already included with ESP32 board package

### Step 3: Hardware Assembly

1. **Prepare Components**
   - Gather all required components
   - Check component specifications
   - Prepare breadboard and wires

2. **Connect MQ-6 Gas Sensor**
   ```
   MQ-6 VCC → ESP32 3.3V
   MQ-6 GND → ESP32 GND
   MQ-6 A0  → ESP32 GPIO 34
   ```

3. **Connect LEDs**
   ```
   Green LED  → ESP32 GPIO 21 (with 220Ω resistor)
   Yellow LED → ESP32 GPIO 22 (with 220Ω resistor)
   Red LED    → ESP32 GPIO 23 (with 220Ω resistor)
   ```

4. **Connect Buzzer**
   ```
   Buzzer Positive → ESP32 GPIO 19
   Buzzer Negative → ESP32 GND
   ```

5. **Verify Connections**
   - Double-check all connections
   - Ensure proper polarity
   - Check for loose connections

### Step 4: Software Installation

1. **Download Source Code**
   - Clone or download the BantayGas IoT repository
   - Extract to your Arduino projects folder

2. **Open Project**
   - Open Arduino IDE
   - Go to File → Open
   - Navigate to the project folder
   - Open the appropriate sketch:
     - `BantayGas_Basic.ino` for basic functionality
     - `BantayGas_Advanced.ino` for WiFi features
     - `BantayGas_Production.ino` for production use

3. **Configure Settings**
   - Edit configuration files in `src/config/`:
     - `hardware_config.h` for pin assignments
     - `gas_config.h` for gas type and thresholds
     - `wifi_config.h` for WiFi settings (if using WiFi)

4. **Upload Code**
   - Connect ESP32 to computer via USB
   - Select correct COM port in Tools → Port
   - Click Upload button or press Ctrl+U
   - Wait for upload to complete

### Step 5: Initial Setup

1. **Power On System**
   - Connect ESP32 to power
   - Wait for system initialization
   - Check serial monitor (115200 baud)

2. **Perform System Tests**
   - System will automatically run startup tests
   - Verify all LEDs light up during test
   - Check buzzer sounds during test
   - Confirm sensor readings appear

3. **Calibration**
   - System will prompt for calibration if needed
   - Follow calibration procedure
   - Ensure sensor is in clean air
   - Wait for calibration to complete

### Step 6: Configuration

1. **Gas Type Selection**
   - Edit `gas_config.h`
   - Uncomment your gas type:
     ```cpp
     #define GAS_TYPE_LPG        // For LPG
     // #define GAS_TYPE_BUTANE   // For butane
     // #define GAS_TYPE_METHANE  // For methane
     ```

2. **Threshold Adjustment**
   - Modify thresholds in `gas_config.h`:
     ```cpp
     #define GAS_SAFE_THRESHOLD     200    // Safe level
     #define GAS_WARNING_THRESHOLD  500    // Warning level
     #define GAS_DANGER_THRESHOLD   800    // Danger level
     #define GAS_CRITICAL_THRESHOLD 1000   // Critical level
     ```

3. **WiFi Configuration** (if using WiFi)
   - Edit `wifi_config.h`:
     ```cpp
     #define WIFI_SSID     "YourNetworkName"
     #define WIFI_PASSWORD "YourPassword"
     #define SERVER_URL    "http://your-server.com/api"
     ```

### Step 7: Testing and Verification

1. **Basic Functionality Test**
   - Check sensor readings in serial monitor
   - Verify LED indicators work correctly
   - Test buzzer functionality
   - Confirm alert levels trigger properly

2. **Calibration Verification**
   - Place sensor in clean air
   - Verify readings are reasonable
   - Check calibration quality
   - Test with known gas source (carefully)

3. **WiFi Test** (if using WiFi)
   - Check WiFi connection status
   - Verify data transmission
   - Test remote monitoring
   - Check alert notifications

## Troubleshooting

### Common Installation Issues

**Upload Failed**
```
Error: Failed to connect to ESP32
Solution:
- Check USB cable connection
- Press and hold BOOT button during upload
- Try different USB port
- Check COM port selection
```

**Libraries Not Found**
```
Error: Library not found
Solution:
- Install required libraries through Library Manager
- Check library versions
- Restart Arduino IDE
- Check library paths
```

**Hardware Not Responding**
```
Error: Sensor not responding
Solution:
- Check power connections
- Verify pin assignments
- Check component specifications
- Test with multimeter
```

**WiFi Connection Failed**
```
Error: WiFi connection failed
Solution:
- Check SSID and password
- Verify 2.4GHz network
- Check signal strength
- Try different network
```

### Diagnostic Commands

Use these commands in the serial monitor:

```
calibrate  - Start calibration procedure
status     - Show current sensor status
test       - Perform system tests
help       - Show available commands
```

### Hardware Diagnostics

1. **Check Power Supply**
   - Measure 3.3V on ESP32
   - Check current consumption
   - Verify stable voltage

2. **Test Connections**
   - Use multimeter to check continuity
   - Verify pin assignments
   - Check for short circuits

3. **Sensor Testing**
   - Measure sensor voltage
   - Check resistance values
   - Verify analog readings

## Advanced Configuration

### Multiple Sensor Setup
For monitoring multiple areas:
1. Connect additional MQ-6 sensors to different analog pins
2. Modify `hardware_config.h` for additional pins
3. Update code to handle multiple sensors
4. Configure individual calibration for each sensor

### Display Integration
To add OLED display:
1. Connect display to I2C pins (GPIO 21, 22)
2. Install required display library
3. Modify code to include display functions
4. Configure display settings

### Relay Control
To add ventilation control:
1. Connect relay module to GPIO 25
2. Modify code to include relay control
3. Configure relay settings
4. Test relay functionality

## Maintenance

### Regular Maintenance
- **Daily**: Check system status and readings
- **Weekly**: Clean sensor surface and check connections
- **Monthly**: Perform calibration and system tests
- **Quarterly**: Check power supply and update firmware

### Calibration Schedule
- **Initial**: After installation
- **Periodic**: Every 30 days
- **After Changes**: When moving or modifying system
- **When Needed**: If readings become inaccurate

### Firmware Updates
1. Download latest firmware
2. Backup current configuration
3. Upload new firmware
4. Restore configuration
5. Test system functionality

## Safety Considerations

### Installation Safety
- **Power Off**: Always power off before making connections
- **Proper Wiring**: Use appropriate wire gauge and connectors
- **Secure Mounting**: Ensure stable mounting of components
- **Protection**: Use appropriate enclosures for outdoor use

### Operation Safety
- **Ventilation**: Ensure adequate ventilation during testing
- **Gas Sources**: Remove all gas sources during calibration
- **Monitoring**: Always monitor system during operation
- **Emergency**: Know emergency procedures

### Maintenance Safety
- **Power Off**: Always power off before maintenance
- **Clean Environment**: Work in clean, well-ventilated area
- **Proper Tools**: Use appropriate tools and equipment
- **Documentation**: Keep maintenance records

## Support and Resources

### Documentation
- **Circuit Diagram**: `docs/circuit_diagram.md`
- **Calibration Guide**: `docs/calibration_guide.md`
- **Troubleshooting**: `docs/troubleshooting.md`
- **API Reference**: `docs/api_reference.md`

### Tools
- **Calibration Tool**: `tools/calibration_tool.py`
- **Data Analyzer**: `tools/data_analyzer.py`
- **Configuration Tool**: Web-based configuration interface

### Community
- **GitHub Repository**: Project source code and issues
- **Documentation**: Comprehensive guides and examples
- **Support**: Community support and discussions

## Compliance and Standards

### Safety Standards
- **UL 1484**: Residential gas detectors
- **EN 50194**: Gas detection systems
- **IEC 61508**: Functional safety

### Environmental Standards
- **Operating Temperature**: -10°C to +50°C
- **Operating Humidity**: 10% to 90% RH
- **Storage Temperature**: -20°C to +70°C

### Electromagnetic Compatibility
- **FCC Part 15**: Class B digital device
- **CE Marking**: European conformity
- **WiFi Certification**: IEEE 802.11 b/g/n

---

**Important**: This is an educational project. For commercial or safety-critical applications, use certified gas detection equipment and follow local safety regulations.


