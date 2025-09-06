# BantayGas IoT Circuit Diagram

## Overview
This document provides detailed circuit diagrams and connection instructions for the BantayGas IoT gas detection system using ESP32 and MQ-6 gas sensor.

## Components Required

### Main Components
- **ESP32 Dev Board** (any variant with WiFi)
- **MQ-6 Gas Sensor** (LPG/Butane/Propane detection)
- **LEDs**: Red, Green, Yellow (5mm, 3.3V compatible)
- **Buzzer** (3.3V compatible, active or passive)
- **Resistors**: 3x 220Ω (for LEDs), 1x 10kΩ (for MQ-6)

### Optional Components
- **OLED Display** (128x64, I2C)
- **Temperature Sensor** (DS18B20 or DHT22)
- **Humidity Sensor** (DHT22)
- **Push Button** (for calibration)
- **Relay Module** (for ventilation control)

## Circuit Connections

### ESP32 Pin Assignments
```
ESP32 GPIO Pins:
├── GPIO 21 → Green LED (Anode) + 220Ω resistor
├── GPIO 22 → Yellow LED (Anode) + 220Ω resistor  
├── GPIO 23 → Red LED (Anode) + 220Ω resistor
├── GPIO 19 → Buzzer (Positive terminal)
├── GPIO 34 → MQ-6 Analog Output (A0)
├── 3.3V    → MQ-6 VCC, LED power
└── GND     → MQ-6 GND, LED cathodes, Buzzer negative
```

### Detailed Connections

#### MQ-6 Gas Sensor
```
MQ-6 Pin → ESP32 Pin → Description
VCC      → 3.3V      → Power supply
GND      → GND       → Ground
A0       → GPIO 34   → Analog output (ADC1_CH6)
D0       → Not used  → Digital output (optional)
```

#### LED Indicators
```
Green LED (Safe):
├── Anode   → GPIO 21 (through 220Ω resistor)
└── Cathode → GND

Yellow LED (Warning):
├── Anode   → GPIO 22 (through 220Ω resistor)
└── Cathode → GND

Red LED (Danger):
├── Anode   → GPIO 23 (through 220Ω resistor)
└── Cathode → GND
```

#### Buzzer
```
Active Buzzer:
├── Positive → GPIO 19
└── Negative → GND

Passive Buzzer (with transistor):
├── Base     → GPIO 19 (through 1kΩ resistor)
├── Collector → Buzzer positive
├── Emitter  → GND
└── Buzzer negative → GND
```

### Optional Components

#### OLED Display (I2C)
```
OLED Pin → ESP32 Pin
VCC      → 3.3V
GND      → GND
SDA      → GPIO 21 (or GPIO 4)
SCL      → GPIO 22 (or GPIO 5)
```

#### Temperature/Humidity Sensor (DHT22)
```
DHT22 Pin → ESP32 Pin
VCC       → 3.3V
GND       → GND
Data      → GPIO 18 (with 4.7kΩ pull-up resistor)
```

#### Push Button (Calibration)
```
Button:
├── One terminal → GPIO 0
└── Other terminal → GND (with 10kΩ pull-up resistor)
```

#### Relay Module (Ventilation Control)
```
Relay Module:
├── VCC → 5V (external power)
├── GND → GND
├── IN  → GPIO 25
└── NO/NC → Ventilation fan/valve
```

## Circuit Diagram (ASCII Art)

```
                    ESP32 Dev Board
                    ┌─────────────────┐
                    │                 │
                    │  GPIO 21 ───────┼─── 220Ω ──── Green LED ──── GND
                    │                 │
                    │  GPIO 22 ───────┼─── 220Ω ──── Yellow LED ─── GND
                    │                 │
                    │  GPIO 23 ───────┼─── 220Ω ──── Red LED ────── GND
                    │                 │
                    │  GPIO 19 ───────┼─── Buzzer ──── GND
                    │                 │
                    │  GPIO 34 ───────┼─── MQ-6 A0
                    │                 │
                    │  3.3V ──────────┼─── MQ-6 VCC
                    │                 │
                    │  GND ───────────┼─── MQ-6 GND
                    │                 │
                    └─────────────────┘

MQ-6 Gas Sensor
┌─────────────┐
│ VCC    A0   │
│  │      │   │
│  │      │   │
│  │      │   │
│ GND     │   │
└─────────┼───┘
          │
          └─── To ESP32 GPIO 34
```

## Power Requirements

### ESP32 Power
- **Voltage**: 3.3V (internal regulator from 5V USB)
- **Current**: 80-240mA (depending on WiFi usage)
- **Power**: 0.26-0.79W

### MQ-6 Sensor Power
- **Voltage**: 3.3V (compatible with ESP32)
- **Current**: 150mA (heater current)
- **Power**: 0.495W

### LED Power
- **Voltage**: 3.3V
- **Current**: 20mA per LED (with 220Ω resistor)
- **Power**: 0.066W per LED

### Buzzer Power
- **Voltage**: 3.3V
- **Current**: 30mA (active buzzer)
- **Power**: 0.099W

### Total System Power
- **Minimum**: 0.5W (idle)
- **Maximum**: 1.5W (all components active)
- **Average**: 0.8W (normal operation)

## Safety Considerations

### Electrical Safety
1. **Use appropriate current limiting resistors** for LEDs
2. **Ensure proper grounding** for stable readings
3. **Use 3.3V compatible components** to avoid damage
4. **Check polarity** of LEDs and buzzer
5. **Use proper wire gauge** for power connections

### Gas Detection Safety
1. **Test in well-ventilated areas** only
2. **Allow proper sensor warm-up time** (30 seconds minimum)
3. **Calibrate regularly** for accurate readings
4. **Never ignore critical level alarms**
5. **Keep sensor away from direct air currents**
6. **Replace sensor if readings become erratic**

### Installation Guidelines
1. **Mount sensor at appropriate height** (1-2 meters from floor)
2. **Avoid mounting near ventilation outlets**
3. **Ensure sensor is accessible** for maintenance
4. **Use proper mounting hardware** for stability
5. **Protect from direct sunlight** and moisture

## Troubleshooting

### Common Connection Issues

**Sensor not responding:**
- Check 3.3V power supply
- Verify analog pin connection (GPIO 34)
- Ensure 30-second warm-up period
- Check for loose connections

**LEDs not working:**
- Check 220Ω resistors are connected
- Verify GPIO pin connections
- Test with multimeter for voltage
- Check LED polarity

**Buzzer not sounding:**
- Check buzzer polarity
- Verify 3.3V compatibility
- Test with direct power connection
- Check GPIO pin connection

**WiFi connection issues:**
- Verify SSID and password
- Check signal strength
- Ensure 2.4GHz network
- Check for interference

### Testing Procedures

**Hardware Test:**
1. Power on system
2. Check all LEDs light up during startup test
3. Verify buzzer sounds during test
4. Check sensor reading in serial monitor
5. Test WiFi connection

**Sensor Test:**
1. Place sensor in clean air
2. Monitor readings for 60 seconds
3. Check for stable baseline reading
4. Test with known gas source (carefully)
5. Verify alert levels trigger correctly

**System Integration Test:**
1. Connect to WiFi network
2. Verify data transmission
3. Test remote monitoring
4. Check alert notifications
5. Verify calibration procedure

## Advanced Configurations

### Multiple Sensor Setup
For monitoring multiple areas, connect additional MQ-6 sensors to different analog pins:
- Sensor 1: GPIO 34
- Sensor 2: GPIO 35
- Sensor 3: GPIO 36
- Sensor 4: GPIO 39

### Relay Control
Connect relay module to control ventilation:
- GPIO 25 → Relay IN
- Relay NO → Ventilation fan
- Relay NC → Gas valve (normally closed)

### Display Integration
Add OLED display for local monitoring:
- GPIO 21 → SDA
- GPIO 22 → SCL
- 3.3V → VCC
- GND → GND

## Maintenance Schedule

### Daily
- Check system status LED
- Verify WiFi connection
- Monitor gas readings

### Weekly
- Clean sensor surface
- Check all connections
- Test alert system
- Review error logs

### Monthly
- Perform calibration
- Check sensor performance
- Update firmware if available
- Clean system components

### Quarterly
- Replace sensor if needed
- Check power supply
- Update configuration
- Perform full system test

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

**Note**: This is an educational project. For commercial or safety-critical applications, use certified gas detection equipment and follow local safety regulations.


