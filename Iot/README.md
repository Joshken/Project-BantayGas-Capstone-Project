# BantayGas IoT System

A comprehensive gas detection system using ESP32 microcontroller with MQ-6 gas sensor for monitoring LPG, butane, and methane gas levels in real-time.

## 📁 Project Structure

```
Iot/
├── README.md                    # This documentation
├── src/                        # Source code
│   ├── main/                   # Main Arduino sketches
│   │   ├── BantayGas_Basic.ino     # Basic gas detection
│   │   ├── BantayGas_Advanced.ino  # Advanced with WiFi
│   │   └── BantayGas_Production.ino # Production-ready version
│   ├── libraries/              # Custom libraries
│   │   ├── GasSensor/          # Gas sensor management
│   │   ├── WiFiManager/        # WiFi connectivity
│   │   └── DataLogger/         # Data logging utilities
│   └── config/                 # Configuration files
│       ├── gas_config.h        # Gas detection settings
│       ├── wifi_config.h       # WiFi settings
│       └── hardware_config.h   # Hardware pin definitions
├── docs/                       # Documentation
│   ├── circuit_diagram.md      # Circuit connections
│   ├── calibration_guide.md    # Sensor calibration
│   ├── troubleshooting.md      # Common issues
│   └── api_reference.md        # API documentation
├── examples/                   # Example implementations
│   ├── basic_detection/        # Basic gas detection
│   ├── wifi_monitoring/        # WiFi-enabled monitoring
│   └── multi_sensor/           # Multiple sensor setup
└── tools/                      # Development tools
    ├── calibration_tool.py     # Python calibration utility
    └── data_analyzer.py        # Data analysis script
```

## 🚀 Quick Start

### 1. Hardware Setup
- Follow the circuit diagram in `docs/circuit_diagram.md`
- Connect ESP32, MQ-6 sensor, LEDs, and buzzer as specified
- Ensure proper power supply (3.3V)

### 2. Software Setup
1. Install Arduino IDE with ESP32 board support
2. Install required libraries (see `docs/installation.md`)
3. Open the appropriate sketch from `src/main/`
4. Configure settings in `src/config/`
5. Upload to your ESP32

### 3. Calibration
- Run the calibration procedure (see `docs/calibration_guide.md`)
- Adjust thresholds for your specific environment
- Test with known gas concentrations

## 🔧 Features

### Core Features
- **Real-time Gas Detection**: Continuous monitoring of gas levels
- **Multi-level Alerts**: Safe, Warning, Danger, and Critical levels
- **Visual Indicators**: RGB LED status system
- **Audio Alerts**: Buzzer with different patterns
- **Automatic Calibration**: Self-calibrating sensor system

### Advanced Features
- **WiFi Connectivity**: Remote monitoring and data transmission
- **Data Logging**: Historical data storage and analysis
- **Multiple Gas Support**: LPG, Butane, Methane detection
- **Safety Features**: Emergency shutdown and alarm limits
- **OTA Updates**: Over-the-air firmware updates

## 📊 Gas Level Thresholds

| Gas Type | Safe (ppm) | Warning (ppm) | Danger (ppm) | Critical (ppm) |
|----------|------------|---------------|--------------|----------------|
| LPG      | 0-200      | 200-500       | 500-800      | >800           |
| Butane   | 0-150      | 150-400       | 400-700      | >700           |
| Methane  | 0-100      | 100-300       | 300-600      | >600           |

## 🛠️ Configuration

### Gas Type Selection
Edit `src/config/gas_config.h`:
```cpp
#define GAS_TYPE_LPG        // For LPG detection
// #define GAS_TYPE_BUTANE   // For butane detection  
// #define GAS_TYPE_METHANE  // For methane detection
```

### WiFi Configuration
Edit `src/config/wifi_config.h`:
```cpp
#define WIFI_SSID "YourNetwork"
#define WIFI_PASSWORD "YourPassword"
#define SERVER_URL "http://your-server.com/api"
```

## 📱 Integration

### Web Dashboard
The system integrates with the BantayGas web dashboard for:
- Real-time monitoring
- Historical data analysis
- Alert management
- User notifications

### Mobile App
Compatible with the BantayGas mobile application for:
- Remote monitoring
- Push notifications
- Emergency alerts
- System status

## 🔒 Safety Features

- **Automatic Calibration**: Ensures accurate readings
- **Emergency Shutdown**: Prevents system damage
- **Alarm Limits**: Maximum continuous alarm time
- **Self-Test**: System health monitoring
- **Fail-Safe Operation**: Safe mode on errors

## 📈 Performance

- **Response Time**: < 1 second for gas detection
- **Accuracy**: ±5% with proper calibration
- **Range**: 0-10000 ppm (MQ-6 sensor)
- **Power Consumption**: < 200mA average
- **Operating Temperature**: -10°C to +50°C

## 🛠️ Troubleshooting

See `docs/troubleshooting.md` for common issues and solutions.

## 📄 License

This project is part of the BantayGas Capstone Project. See the main project license for details.

## 🤝 Contributing

Contributions are welcome! Please follow the coding standards and submit pull requests.

## ⚠️ Safety Disclaimer

This is an educational project. For commercial or safety-critical applications, use certified gas detection equipment and follow local safety regulations.

---

**BantayGas IoT System - Keeping You Safe! 🔥💨**


