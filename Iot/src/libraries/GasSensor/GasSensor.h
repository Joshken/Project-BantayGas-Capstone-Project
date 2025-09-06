/*
 * GasSensor Library for BantayGas IoT System
 * 
 * Provides high-level interface for gas sensor operations including:
 * - Sensor reading and calibration
 * - Gas concentration calculations
 * - Sensor health monitoring
 * - Data validation and filtering
 * 
 * Author: BantayGas Development Team
 * Version: 1.0.0
 */

#ifndef GAS_SENSOR_H
#define GAS_SENSOR_H

#include <Arduino.h>
#include "hardware_config.h"
#include "gas_config.h"

// ============================================================================
// DATA STRUCTURES
// ============================================================================

struct SensorReading {
  float rawValue;
  float voltage;
  float resistance;
  float gasPPM;
  float temperature;
  float humidity;
  unsigned long timestamp;
  bool isValid;
  int quality; // 0-100, quality score
};

struct CalibrationData {
  float R0;
  float R0_clean_air;
  float temperature_coefficient;
  float humidity_coefficient;
  unsigned long calibration_date;
  bool is_valid;
  float confidence; // 0-100, calibration confidence
};

struct SensorHealth {
  bool isHealthy;
  int failureCount;
  float averageReading;
  float standardDeviation;
  unsigned long lastMaintenance;
  int totalReadings;
};

// ============================================================================
// GAS SENSOR CLASS
// ============================================================================

class GasSensor {
private:
  // Hardware configuration
  int sensorPin;
  float loadResistance;
  float referenceVoltage;
  int adcResolution;
  
  // Calibration data
  CalibrationData calibration;
  
  // Health monitoring
  SensorHealth health;
  
  // Reading history for filtering
  float readingHistory[10];
  int historyIndex;
  int historyCount;
  
  // Private methods
  float calculateResistance(float voltage);
  float calculatePPM(float resistance);
  float applyTemperatureCorrection(float ppm, float temperature);
  float applyHumidityCorrection(float ppm, float humidity);
  bool validateReading(float ppm);
  void updateHealthMetrics(float ppm);
  float calculateMovingAverage();
  float calculateStandardDeviation();

public:
  // Constructor
  GasSensor(int pin = MQ6_PIN, float loadR = MQ6_LOAD_RESISTANCE);
  
  // Initialization
  bool begin();
  void reset();
  
  // Reading operations
  SensorReading readSensor();
  SensorReading readSensorFiltered();
  float getLastReading();
  
  // Calibration
  bool calibrate(int samples = CALIBRATION_SAMPLES);
  bool loadCalibration();
  bool saveCalibration();
  bool isCalibrated();
  CalibrationData getCalibrationData();
  
  // Health monitoring
  SensorHealth getHealthStatus();
  bool isHealthy();
  void performHealthCheck();
  void resetHealth();
  
  // Configuration
  void setLoadResistance(float resistance);
  void setReferenceVoltage(float voltage);
  void setADCResolution(int resolution);
  
  // Utility functions
  float rawToVoltage(int rawValue);
  float voltageToResistance(float voltage);
  float resistanceToPPM(float resistance);
  String getGasType();
  int getAlertLevel(float ppm);
  
  // Debugging
  void printCalibrationData();
  void printHealthStatus();
  void printLastReading();
};

// ============================================================================
// GAS DETECTOR MANAGER CLASS
// ============================================================================

class GasDetectorManager {
private:
  GasSensor* sensors;
  int sensorCount;
  int activeSensor;
  
  // Multi-sensor data
  float averagePPM;
  float maxPPM;
  int overallAlertLevel;
  
  // Calibration management
  bool allSensorsCalibrated;
  unsigned long lastCalibrationCheck;
  
public:
  // Constructor
  GasDetectorManager(int count = 1);
  
  // Initialization
  bool begin();
  void addSensor(GasSensor* sensor);
  
  // Reading operations
  SensorReading readAllSensors();
  SensorReading readActiveSensor();
  void switchActiveSensor(int index);
  
  // Calibration
  bool calibrateAllSensors();
  bool calibrateActiveSensor();
  bool allCalibrated();
  
  // Health monitoring
  bool allHealthy();
  void performHealthCheck();
  
  // Data processing
  float getAveragePPM();
  float getMaxPPM();
  int getOverallAlertLevel();
  
  // Configuration
  void setActiveSensor(int index);
  int getActiveSensor();
  int getSensorCount();
  
  // Utility
  void printSystemStatus();
  void printAllReadings();
};

// ============================================================================
// GAS ALERT MANAGER CLASS
// ============================================================================

class GasAlertManager {
private:
  // Alert configuration
  float safeThreshold;
  float warningThreshold;
  float dangerThreshold;
  float criticalThreshold;
  
  // Alert state
  int currentLevel;
  int previousLevel;
  unsigned long alertStartTime;
  bool alertActive;
  
  // Alert history
  struct AlertRecord {
    unsigned long timestamp;
    int level;
    float ppm;
    String description;
    bool acknowledged;
  };
  
  AlertRecord alertHistory[50];
  int alertIndex;
  int alertCount;
  
  // Alert patterns
  unsigned long lastBlinkTime;
  unsigned long lastBeepTime;
  bool ledState;
  bool buzzerState;
  
public:
  // Constructor
  GasAlertManager();
  
  // Configuration
  void setThresholds(float safe, float warning, float danger, float critical);
  void setSafeThreshold(float threshold);
  void setWarningThreshold(float threshold);
  void setDangerThreshold(float threshold);
  void setCriticalThreshold(float threshold);
  
  // Alert processing
  int processReading(float ppm);
  void updateAlertState(int level);
  void clearAlerts();
  
  // Alert handling
  void handleAlerts();
  void updateVisualIndicators();
  void updateAudioIndicators();
  
  // Alert history
  void logAlert(int level, float ppm, String description);
  AlertRecord* getAlertHistory();
  int getAlertCount();
  void acknowledgeAlert(int index);
  
  // Status
  bool isAlertActive();
  int getCurrentLevel();
  int getPreviousLevel();
  unsigned long getAlertDuration();
  
  // Utility
  String getAlertDescription(int level);
  void printAlertStatus();
  void printAlertHistory();
};

// ============================================================================
// GLOBAL INSTANCES
// ============================================================================

extern GasSensor gasSensor;
extern GasDetectorManager detectorManager;
extern GasAlertManager alertManager;

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// Gas type detection
String detectGasType(float ppm, float temperature, float humidity);

// PPM conversion utilities
float rawToPPM(int rawValue, float R0, float temperature = 25.0, float humidity = 50.0);
float voltageToPPM(float voltage, float R0, float temperature = 25.0, float humidity = 50.0);
float resistanceToPPM(float resistance, float R0, float temperature = 25.0, float humidity = 50.0);

// Sensor validation
bool validateSensorReading(float ppm, float temperature, float humidity);
bool isReasonableReading(float ppm);

// Calibration utilities
float calculateR0(float* readings, int count);
float calculateCalibrationConfidence(float* readings, int count);
bool isCalibrationValid(float R0, float confidence);

// Alert level calculation
int calculateAlertLevel(float ppm, float safe, float warning, float danger, float critical);
String getAlertLevelName(int level);

// Data filtering
float applyLowPassFilter(float newValue, float oldValue, float alpha = 0.1);
float applyMedianFilter(float* values, int count);
float applyMovingAverage(float* values, int count);

// Temperature and humidity corrections
float applyTemperatureCorrection(float ppm, float temperature, float referenceTemp = 25.0);
float applyHumidityCorrection(float ppm, float humidity, float referenceHumidity = 50.0);

// Safety functions
bool isSafeLevel(float ppm);
bool isWarningLevel(float ppm);
bool isDangerLevel(float ppm);
bool isCriticalLevel(float ppm);

// Debugging functions
void printSensorDiagnostics();
void printCalibrationReport();
void printAlertReport();

#endif // GAS_SENSOR_H


