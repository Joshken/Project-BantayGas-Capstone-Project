# BantayGas IoT Calibration Guide

## Overview
This guide provides comprehensive instructions for calibrating the BantayGas IoT gas detection system. Proper calibration is essential for accurate gas detection and reliable safety monitoring.

## Understanding Calibration

### What is Calibration?
Calibration is the process of establishing the relationship between the sensor's electrical output and the actual gas concentration in the environment. For the MQ-6 sensor, this involves determining the R0 value (sensor resistance in clean air).

### Why is Calibration Important?
- **Accuracy**: Ensures readings correspond to actual gas concentrations
- **Reliability**: Provides consistent and repeatable measurements
- **Safety**: Critical for proper alert threshold activation
- **Compliance**: Required for safety-critical applications

### Calibration Types
1. **Initial Calibration**: First-time setup in clean air
2. **Periodic Calibration**: Regular recalibration (monthly)
3. **Drift Correction**: Adjustment for sensor aging
4. **Multi-point Calibration**: Using known gas concentrations

## Pre-Calibration Requirements

### Environmental Conditions
- **Temperature**: 20-25°C (68-77°F)
- **Humidity**: 40-60% RH
- **Air Quality**: Clean, well-ventilated area
- **Gas Sources**: No gas sources within 10 meters
- **Air Movement**: Minimal air currents

### Equipment Needed
- **Clean Air Source**: Outdoor air or certified clean air
- **Stopwatch**: For timing calibration process
- **Multimeter**: For voltage measurements (optional)
- **Known Gas Source**: For verification (optional)

### Safety Precautions
- **Ventilation**: Ensure adequate ventilation
- **No Smoking**: No smoking or open flames
- **Gas Sources**: Remove all potential gas sources
- **Monitoring**: Have someone monitor the process
- **Emergency**: Know emergency procedures

## Calibration Procedure

### Step 1: System Preparation
1. **Power On**: Turn on the BantayGas system
2. **Wait for Warm-up**: Allow 30 seconds for sensor stabilization
3. **Check Connections**: Verify all connections are secure
4. **Serial Monitor**: Open serial monitor (115200 baud)

### Step 2: Environmental Setup
1. **Clean Air**: Ensure sensor is in clean air environment
2. **Stable Conditions**: Wait for stable temperature and humidity
3. **Remove Sources**: Remove all potential gas sources
4. **Ventilation**: Ensure good air circulation

### Step 3: Start Calibration
1. **Access Menu**: Use web interface or serial commands
2. **Select Calibration**: Choose "Start Calibration"
3. **Confirm Environment**: Confirm clean air conditions
4. **Begin Process**: Start calibration procedure

### Step 4: Calibration Process
The system will automatically:
1. **Take Samples**: Collect 100 sensor readings
2. **Calculate R0**: Determine clean air resistance
3. **Validate Data**: Check for consistent readings
4. **Save Results**: Store calibration data

### Step 5: Verification
1. **Check Results**: Review calibration data
2. **Test Reading**: Verify sensor reading in clean air
3. **Confirm Accuracy**: Check against expected values
4. **Save Configuration**: Store calibration data

## Calibration Parameters

### MQ-6 Sensor Characteristics
```
Parameter          Value
Load Resistance    10 kΩ
Clean Air Ratio    10.0
Heater Current     150 mA
Warm-up Time       30 seconds
Calibration Time   60 seconds
Sample Count       100 readings
```

### Gas-Specific Parameters
```
Gas Type    Slope    Intercept    R0 Range
LPG         -0.318   1.133        5-15 kΩ
Butane      -0.318   1.133        5-15 kΩ
Propane     -0.318   1.133        5-15 kΩ
Methane     -0.318   1.133        5-15 kΩ
```

### Calibration Quality Indicators
- **Confidence Level**: >90% for good calibration
- **Standard Deviation**: <5% of mean value
- **R0 Range**: 5-15 kΩ for MQ-6 sensor
- **Consistency**: <2% variation between samples

## Calibration Methods

### Method 1: Automatic Calibration (Recommended)
```cpp
// Start automatic calibration
bool success = gasSensor.calibrate(100);

if (success) {
    Serial.println("Calibration successful!");
    CalibrationData calib = gasSensor.getCalibrationData();
    Serial.println("R0: " + String(calib.R0, 2));
    Serial.println("Confidence: " + String(calib.confidence, 1) + "%");
} else {
    Serial.println("Calibration failed!");
}
```

### Method 2: Manual Calibration
```cpp
// Manual calibration with custom parameters
float sum = 0;
int validSamples = 0;

for (int i = 0; i < 100; i++) {
    int rawValue = analogRead(MQ6_PIN);
    float voltage = (rawValue * 3.3) / 4095;
    float resistance = ((3.3 - voltage) * 10.0) / voltage;
    
    if (resistance > 0 && resistance < 1000) {
        sum += resistance;
        validSamples++;
    }
    
    delay(100);
}

if (validSamples > 80) {
    float R0 = sum / validSamples;
    // Save R0 value
    saveCalibrationData(R0);
}
```

### Method 3: Multi-Point Calibration
```cpp
// Calibration with known gas concentrations
struct CalibrationPoint {
    float concentration; // PPM
    float resistance;    // kΩ
};

CalibrationPoint points[] = {
    {0, R0_clean_air},      // Clean air
    {200, R0_warning},      // Warning level
    {500, R0_danger},       // Danger level
    {1000, R0_critical}     // Critical level
};

// Perform multi-point calibration
bool success = performMultiPointCalibration(points, 4);
```

## Calibration Data Management

### Data Storage
Calibration data is stored in EEPROM:
```
Address    Data Type    Description
0-3        float        R0 value
4-7        uint32_t     Calibration timestamp
8          uint8_t      Calibration flag (0xAA)
9-12       float        R0 clean air reference
13-16      float        Temperature coefficient
17-20      float        Humidity coefficient
```

### Data Validation
```cpp
bool validateCalibrationData() {
    // Check calibration flag
    if (EEPROM.read(8) != 0xAA) return false;
    
    // Read R0 value
    float R0;
    EEPROM.get(0, R0);
    
    // Validate R0 range
    if (R0 < 5.0 || R0 > 15.0) return false;
    
    // Check timestamp
    uint32_t timestamp;
    EEPROM.get(4, timestamp);
    if (timestamp == 0) return false;
    
    return true;
}
```

### Data Backup and Restore
```cpp
// Backup calibration data
void backupCalibrationData() {
    CalibrationData calib = gasSensor.getCalibrationData();
    
    // Save to SPIFFS
    File file = SPIFFS.open("/calibration.bak", "w");
    if (file) {
        file.write((uint8_t*)&calib, sizeof(calib));
        file.close();
    }
}

// Restore calibration data
void restoreCalibrationData() {
    File file = SPIFFS.open("/calibration.bak", "r");
    if (file) {
        CalibrationData calib;
        file.read((uint8_t*)&calib, sizeof(calib));
        file.close();
        
        // Restore calibration
        gasSensor.setCalibrationData(calib);
    }
}
```

## Troubleshooting Calibration

### Common Issues

**Calibration Fails - Insufficient Samples**
```
Error: "Insufficient valid samples for calibration"
Cause: Sensor not in clean air or hardware issues
Solution: 
- Ensure clean air environment
- Check sensor connections
- Verify power supply
- Wait for sensor warm-up
```

**Calibration Fails - High Variation**
```
Error: "High variation in calibration data"
Cause: Unstable environment or sensor issues
Solution:
- Check for air currents
- Ensure stable temperature
- Verify sensor is clean
- Check for gas sources
```

**Calibration Fails - Invalid R0**
```
Error: "R0 value out of range"
Cause: Sensor malfunction or wrong sensor type
Solution:
- Check sensor type (MQ-6)
- Verify connections
- Test with multimeter
- Replace sensor if needed
```

**Calibration Succeeds but Readings Wrong**
```
Issue: Calibration completes but readings are inaccurate
Cause: Wrong gas type or environmental factors
Solution:
- Check gas type configuration
- Verify environmental conditions
- Recalibrate with proper conditions
- Check for sensor drift
```

### Diagnostic Commands
```cpp
// Check sensor raw values
void checkSensorRawValues() {
    for (int i = 0; i < 10; i++) {
        int raw = analogRead(MQ6_PIN);
        float voltage = (raw * 3.3) / 4095;
        float resistance = ((3.3 - voltage) * 10.0) / voltage;
        
        Serial.print("Raw: ");
        Serial.print(raw);
        Serial.print(", Voltage: ");
        Serial.print(voltage, 3);
        Serial.print("V, Resistance: ");
        Serial.print(resistance, 2);
        Serial.println("kΩ");
        
        delay(1000);
    }
}

// Check calibration quality
void checkCalibrationQuality() {
    CalibrationData calib = gasSensor.getCalibrationData();
    
    Serial.println("=== Calibration Quality Check ===");
    Serial.println("R0: " + String(calib.R0, 2) + " kΩ");
    Serial.println("Confidence: " + String(calib.confidence, 1) + "%");
    Serial.println("Date: " + String(calib.calibration_date));
    Serial.println("Valid: " + String(calib.is_valid ? "Yes" : "No"));
    
    // Check R0 range
    if (calib.R0 < 5.0 || calib.R0 > 15.0) {
        Serial.println("WARNING: R0 out of normal range!");
    }
    
    // Check confidence
    if (calib.confidence < 90.0) {
        Serial.println("WARNING: Low calibration confidence!");
    }
}
```

## Calibration Schedule

### Initial Calibration
- **When**: First installation
- **Frequency**: One-time
- **Duration**: 60 seconds
- **Requirements**: Clean air, stable conditions

### Periodic Calibration
- **When**: Monthly
- **Frequency**: Every 30 days
- **Duration**: 60 seconds
- **Requirements**: Clean air, stable conditions

### Drift Correction
- **When**: When readings drift >20%
- **Frequency**: As needed
- **Duration**: 60 seconds
- **Requirements**: Clean air, stable conditions

### Emergency Calibration
- **When**: After sensor replacement
- **Frequency**: As needed
- **Duration**: 60 seconds
- **Requirements**: Clean air, stable conditions

## Calibration Records

### Record Keeping
Maintain calibration records including:
- **Date and Time**: When calibration was performed
- **R0 Value**: Calibrated resistance value
- **Confidence Level**: Calibration quality score
- **Environmental Conditions**: Temperature, humidity
- **Performed By**: Who performed the calibration
- **Results**: Pass/fail status
- **Notes**: Any observations or issues

### Sample Calibration Record
```
Calibration Record
==================
Date: 2024-01-15
Time: 14:30:00
R0 Value: 12.45 kΩ
Confidence: 95.2%
Temperature: 22.5°C
Humidity: 45% RH
Performed By: John Doe
Status: PASS
Notes: Clean air conditions, stable readings
```

## Advanced Calibration

### Temperature Compensation
```cpp
// Apply temperature correction
float applyTemperatureCorrection(float ppm, float temperature) {
    float referenceTemp = 25.0; // Reference temperature
    float tempCoeff = 0.02;     // 2% per degree C
    
    return ppm * (1.0 + tempCoeff * (temperature - referenceTemp));
}
```

### Humidity Compensation
```cpp
// Apply humidity correction
float applyHumidityCorrection(float ppm, float humidity) {
    float referenceHumidity = 50.0; // Reference humidity
    float humidityCoeff = 0.01;     // 1% per %RH
    
    return ppm * (1.0 + humidityCoeff * (humidity - referenceHumidity));
}
```

### Multi-Gas Calibration
```cpp
// Calibrate for multiple gas types
struct MultiGasCalibration {
    float lpg_R0;
    float butane_R0;
    float propane_R0;
    float methane_R0;
};

MultiGasCalibration multiGasCalib;

// Perform multi-gas calibration
bool calibrateMultiGas() {
    // Calibrate for each gas type
    // This requires known gas concentrations
    // or specialized calibration equipment
    return true;
}
```

## Calibration Verification

### Verification Methods
1. **Clean Air Test**: Verify readings in clean air
2. **Known Gas Test**: Test with known gas concentration
3. **Cross-Reference**: Compare with certified detector
4. **Statistical Analysis**: Check reading consistency

### Verification Procedure
```cpp
// Verify calibration accuracy
bool verifyCalibration() {
    // Test in clean air
    SensorReading reading = gasSensor.readSensor();
    
    // Check if reading is reasonable for clean air
    if (reading.gasPPM > 50) {
        Serial.println("WARNING: High reading in clean air!");
        return false;
    }
    
    // Check reading consistency
    float readings[10];
    for (int i = 0; i < 10; i++) {
        readings[i] = gasSensor.readSensor().gasPPM;
        delay(1000);
    }
    
    // Calculate standard deviation
    float mean = 0;
    for (int i = 0; i < 10; i++) {
        mean += readings[i];
    }
    mean /= 10;
    
    float variance = 0;
    for (int i = 0; i < 10; i++) {
        variance += pow(readings[i] - mean, 2);
    }
    variance /= 10;
    float stdDev = sqrt(variance);
    
    // Check consistency
    if (stdDev > mean * 0.1) { // 10% variation
        Serial.println("WARNING: High reading variation!");
        return false;
    }
    
    Serial.println("Calibration verification PASSED");
    return true;
}
```

## Best Practices

### Calibration Environment
- **Clean Air**: Use outdoor air or certified clean air
- **Stable Conditions**: Avoid temperature and humidity changes
- **No Interference**: Remove all gas sources
- **Proper Ventilation**: Ensure good air circulation

### Calibration Process
- **Follow Procedure**: Use established calibration procedure
- **Record Data**: Keep detailed calibration records
- **Verify Results**: Always verify calibration accuracy
- **Document Issues**: Record any problems or observations

### Maintenance
- **Regular Schedule**: Follow recommended calibration schedule
- **Quality Control**: Monitor calibration quality
- **Sensor Care**: Keep sensor clean and protected
- **Data Management**: Backup and manage calibration data

### Safety
- **Safe Environment**: Ensure safe calibration environment
- **Proper Equipment**: Use appropriate calibration equipment
- **Emergency Procedures**: Know emergency procedures
- **Documentation**: Maintain proper documentation

---

**Important**: Always follow safety procedures when working with gas detection systems. This is an educational project - for commercial applications, use certified equipment and follow local regulations.


