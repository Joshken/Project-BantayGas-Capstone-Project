# ESP32 Gas Detector System

A complete gas detection system using ESP32, MQ-6 gas sensor, LEDs, and buzzer for monitoring LPG, butane, and methane gas levels.

## 📁 Files Included

1. **`circuit_diagram_gas_detector.md`** - Complete circuit diagram and wiring guide
2. **`gas_detector_esp32.ino`** - Basic Arduino code for gas detection
3. **`gas_detector_improved.ino`** - Advanced version with configuration file
4. **`gas_detector_config.h`** - Configuration file for easy customization
5. **`README_Gas_Detector.md`** - This documentation file

## 🔧 Hardware Requirements

### Components
- **ESP32 Dev Board** (any variant)
- **MQ-6 Gas Sensor** (LPG/Butane detection)
- **LEDs**: Red, Green, Yellow (5mm)
- **Buzzer** (3.3V compatible)
- **Resistors**: 3x 220Ω, 1x 10kΩ
- **Breadboard and jumper wires**

### Pin Connections
```
ESP32 Pin → Component
GPIO 2    → Red LED (with 220Ω resistor)
GPIO 4    → Green LED (with 220Ω resistor)
GPIO 5    → Yellow LED (with 220Ω resistor)
GPIO 18   → Buzzer positive
GPIO 34   → MQ-6 Analog output
3.3V      → MQ-6 VCC
GND       → MQ-6 GND, LED cathodes, Buzzer negative
```

## 🚀 Quick Start

### 1. Hardware Setup
1. Follow the circuit diagram in `circuit_diagram_gas_detector.md`
2. Double-check all connections
3. Ensure proper power supply (3.3V)

### 2. Software Setup
1. Install Arduino IDE with ESP32 board support
2. Open `gas_detector_improved.ino`
3. Install the `gas_detector_config.h` file in the same folder
4. Select your ESP32 board and COM port
5. Upload the code

### 3. Calibration
1. Place the sensor in clean air for 60 seconds
2. The system will automatically calibrate
3. Monitor serial output for calibration values

## ⚙️ Configuration

### Gas Type Selection
Edit `gas_detector_config.h` and uncomment your gas type:
```cpp
#define GAS_TYPE_LPG        // For LPG detection
// #define GAS_TYPE_BUTANE   // For butane detection  
// #define GAS_TYPE_METHANE  // For methane detection
```

### Threshold Adjustment
Modify thresholds in `gas_detector_config.h`:
```cpp
#define LPG_SAFE_THRESHOLD 200      // Safe level (Green LED)
#define LPG_WARNING_THRESHOLD 500   // Warning level (Yellow LED)
#define LPG_DANGER_THRESHOLD 800    // Danger level (Red LED + Buzzer)
```

### Feature Toggles
Enable/disable features in the config file:
```cpp
#define ENABLE_SERIAL_DEBUG true    // Serial output
#define ENABLE_LED_TEST true        // LED test on startup
#define ENABLE_BUZZER_TEST true     // Buzzer test on startup
#define ENABLE_WIFI false           // WiFi functionality
```

## 📊 Operation Modes

### LED Indicators
- **🟢 Green LED**: Gas levels are safe (below threshold)
- **🟡 Yellow LED**: Warning level detected
- **🔴 Red LED**: Danger level detected
- **🔴 Red LED + Buzzer**: Critical level - evacuate immediately

### Buzzer Patterns
- **Silent**: Safe levels
- **Intermittent beeping**: Danger level (1-second intervals)
- **Continuous**: Critical level (with 5-minute safety limit)

## 🔧 Advanced Features

### Emergency Mode
- Automatically activates at critical gas levels
- Continuous alarm for 5 minutes maximum
- Prevents system damage from prolonged high gas exposure

### Calibration System
- Automatic calibration in clean air
- R0 value calculation for accurate readings
- EEPROM storage for calibration data

### Safety Features
- Maximum continuous buzzer time limit
- Emergency shutdown threshold
- System self-test on startup

## 📈 Gas Level Thresholds

### LPG (Liquefied Petroleum Gas)
- **Safe**: 0-200 ppm
- **Warning**: 200-500 ppm  
- **Danger**: 500-800 ppm
- **Critical**: >800 ppm

### Butane
- **Safe**: 0-150 ppm
- **Warning**: 150-400 ppm
- **Danger**: 400-700 ppm
- **Critical**: >700 ppm

### Methane
- **Safe**: 0-100 ppm
- **Warning**: 100-300 ppm
- **Danger**: 300-600 ppm
- **Critical**: >600 ppm

## 🛠️ Troubleshooting

### Common Issues

**Sensor not responding:**
- Check 3.3V power supply
- Verify analog pin connection (GPIO 34)
- Ensure 30-second warm-up period

**LEDs not working:**
- Check 220Ω resistors are connected
- Verify GPIO pin connections
- Test with LED test function

**Buzzer not sounding:**
- Check buzzer polarity
- Verify 3.3V compatibility
- Test with buzzer test function

**Inaccurate readings:**
- Recalibrate in clean air
- Check for air currents near sensor
- Verify gas type selection in config

### Serial Monitor Output
Enable `ENABLE_SERIAL_DEBUG` to see:
- ADC values and voltage readings
- Calculated PPM values
- System status and warnings
- Calibration data

## 🔒 Safety Considerations

1. **Always test in well-ventilated areas**
2. **Allow proper sensor warm-up time**
3. **Calibrate regularly for accurate readings**
4. **Never ignore critical level alarms**
5. **Keep sensor away from direct air currents**
6. **Replace sensor if readings become erratic**

## 📱 Future Enhancements

### Optional Additions
- **OLED Display**: Real-time PPM readings
- **WiFi Connectivity**: Remote monitoring
- **Data Logging**: Historical gas level data
- **Mobile App**: Smartphone notifications
- **Multiple Sensors**: Multi-gas detection
- **Relay Control**: Automatic ventilation control

### WiFi Integration
Uncomment WiFi code in config file and add:
```cpp
#define ENABLE_WIFI true
#define WIFI_SSID "YourNetwork"
#define WIFI_PASSWORD "YourPassword"
```

## 📄 License

This project is open source. Feel free to modify and distribute according to your needs.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for improvements.

## ⚠️ Disclaimer

This is an educational project. For commercial or safety-critical applications, use certified gas detection equipment and follow local safety regulations.

---

**Happy Gas Detection! 🔥💨**
