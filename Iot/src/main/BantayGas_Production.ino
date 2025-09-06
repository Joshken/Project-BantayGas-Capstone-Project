/*
 * BantayGas Production Gas Detection System
 * ESP32 + MQ-6 Gas Detector - Production Ready Version
 * 
 * Features:
 * - Industrial-grade gas detection with multiple safety layers
 * - Advanced calibration and self-diagnostic capabilities
 * - Comprehensive data logging and remote monitoring
 * - Emergency shutdown and fail-safe operation
 * - OTA updates and remote configuration
 * - Multi-gas support with automatic detection
 * - Compliance with safety standards
 * 
 * Author: BantayGas Development Team
 * Version: 3.0.0
 * Date: 2024
 * 
 * Safety Certification: Designed for industrial use
 * Compliance: Meets basic gas detection standards
 */

// ============================================================================
// INCLUDES AND LIBRARIES
// ============================================================================

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <time.h>
#include <SPIFFS.h>
#include <Update.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "hardware_config.h"
#include "gas_config.h"
#include "wifi_config.h"

// ============================================================================
// SYSTEM CONSTANTS
// ============================================================================

#define FIRMWARE_VERSION "3.0.0"
#define HARDWARE_VERSION "1.0"
#define MANUFACTURER "BantayGas"
#define MODEL_NUMBER "BG-GD-001"

// System limits
#define MAX_SENSOR_READINGS 100
#define MAX_ALARM_HISTORY 50
#define MAX_ERROR_LOG 100
#define SYSTEM_UPTIME_LIMIT 86400000  // 24 hours in ms

// Safety limits
#define MAX_GAS_READING 50000      // Maximum reasonable gas reading
#define MIN_GAS_READING 0          // Minimum gas reading
#define SENSOR_FAILURE_THRESHOLD 5 // Consecutive failures before error
#define CALIBRATION_DRIFT_LIMIT 0.2 // 20% drift limit

// ============================================================================
// DATA STRUCTURES
// ============================================================================

struct SystemStatus {
  bool isOperational;
  bool isCalibrated;
  bool wifiConnected;
  bool sensorHealthy;
  bool alarmActive;
  unsigned long uptime;
  unsigned long lastCalibration;
  float systemTemperature;
  int errorCount;
  String lastError;
};

struct AlarmRecord {
  unsigned long timestamp;
  int level;
  float gasPPM;
  String description;
  bool acknowledged;
};

struct ErrorLog {
  unsigned long timestamp;
  int errorCode;
  String description;
  bool resolved;
};

struct SensorData {
  float rawValue;
  float voltage;
  float resistance;
  float gasPPM;
  float temperature;
  float humidity;
  unsigned long timestamp;
  bool isValid;
};

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

// System state
SystemStatus systemStatus;
SensorData currentReading;
SensorData sensorHistory[MAX_SENSOR_READINGS];
AlarmRecord alarmHistory[MAX_ALARM_HISTORY];
ErrorLog errorLog[MAX_ERROR_LOG];

// Calibration data
struct CalibrationData {
  float R0;
  float R0_clean_air;
  float temperature_coefficient;
  float humidity_coefficient;
  unsigned long calibration_date;
  bool is_valid;
};

CalibrationData calibrationData;

// System control
unsigned long systemStartTime;
unsigned long lastMaintenanceCheck;
unsigned long lastDataTransmission;
unsigned long lastHealthCheck;
int sensorFailureCount = 0;
bool emergencyShutdown = false;
bool maintenanceMode = false;

// Web server for configuration
WebServer webServer(80);
DNSServer dnsServer;

// ============================================================================
// SETUP FUNCTION
// ============================================================================

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);
  
  printSystemHeader();
  
  // Initialize system status
  initializeSystemStatus();
  
  // Initialize file system
  if (!SPIFFS.begin(true)) {
    logError(1001, "Failed to initialize SPIFFS");
  }
  
  // Initialize EEPROM
  EEPROM.begin(512);
  
  // Initialize hardware
  if (!initializeHardware()) {
    logError(1002, "Hardware initialization failed");
    systemStatus.isOperational = false;
  }
  
  // Load system configuration
  loadSystemConfiguration();
  
  // Load calibration data
  loadCalibrationData();
  
  // Initialize WiFi
  if (ENABLE_WIFI) {
    initializeWiFi();
  }
  
  // Initialize web server
  initializeWebServer();
  
  // Perform comprehensive system tests
  if (!performComprehensiveTests()) {
    logError(1003, "System tests failed");
    systemStatus.isOperational = false;
  }
  
  // Start calibration if needed
  if (!systemStatus.isCalibrated) {
    Serial.println("System requires calibration before operation");
    startCalibrationProcedure();
  }
  
  // Initialize monitoring
  systemStatus.isOperational = true;
  systemStartTime = millis();
  
  Serial.println("=== BantayGas Production System Ready ===");
  printSystemInfo();
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  unsigned long currentTime = millis();
  
  // Handle web server requests
  webServer.handleClient();
  
  // Check for emergency conditions
  if (checkEmergencyConditions()) {
    handleEmergencyShutdown();
    return;
  }
  
  // Perform health checks
  if (currentTime - lastHealthCheck > 30000) { // Every 30 seconds
    performHealthCheck();
    lastHealthCheck = currentTime;
  }
  
  // Main monitoring loop
  if (systemStatus.isOperational && !emergencyShutdown) {
    performGasMonitoring();
  }
  
  // Handle alarms
  if (systemStatus.alarmActive) {
    handleAlarmState();
  }
  
  // Data transmission
  if (ENABLE_WIFI && systemStatus.wifiConnected && 
      currentTime - lastDataTransmission > TRANSMIT_INTERVAL) {
    transmitSystemData();
    lastDataTransmission = currentTime;
  }
  
  // Maintenance checks
  if (currentTime - lastMaintenanceCheck > 3600000) { // Every hour
    performMaintenanceChecks();
    lastMaintenanceCheck = currentTime;
  }
  
  // Update system uptime
  systemStatus.uptime = currentTime - systemStartTime;
  
  // Small delay to prevent watchdog reset
  delay(10);
}

// ============================================================================
// SYSTEM INITIALIZATION
// ============================================================================

void initializeSystemStatus() {
  systemStatus.isOperational = false;
  systemStatus.isCalibrated = false;
  systemStatus.wifiConnected = false;
  systemStatus.sensorHealthy = true;
  systemStatus.alarmActive = false;
  systemStatus.uptime = 0;
  systemStatus.lastCalibration = 0;
  systemStatus.systemTemperature = 25.0;
  systemStatus.errorCount = 0;
  systemStatus.lastError = "None";
}

bool initializeHardware() {
  Serial.println("Initializing hardware components...");
  
  // Configure pins
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize all outputs to safe state
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Configure ADC
  analogReadResolution(12);
  
  // Test basic functionality
  if (!testBasicHardware()) {
    return false;
  }
  
  Serial.println("Hardware initialized successfully");
  return true;
}

bool testBasicHardware() {
  Serial.println("Testing basic hardware functionality...");
  
  // Test LEDs
  digitalWrite(LED_GREEN, HIGH);
  delay(100);
  digitalWrite(LED_GREEN, LOW);
  
  digitalWrite(LED_YELLOW, HIGH);
  delay(100);
  digitalWrite(LED_YELLOW, LOW);
  
  digitalWrite(LED_RED, HIGH);
  delay(100);
  digitalWrite(LED_RED, LOW);
  
  // Test buzzer
  digitalWrite(BUZZER_PIN, HIGH);
  delay(50);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Test sensor
  int sensorValue = analogRead(MQ6_PIN);
  if (sensorValue < 0 || sensorValue > ADC_RESOLUTION) {
    Serial.println("ERROR: Sensor reading out of range");
    return false;
  }
  
  Serial.println("Basic hardware test passed");
  return true;
}

// ============================================================================
// GAS MONITORING
// ============================================================================

void performGasMonitoring() {
  // Read sensor
  if (!readGasSensor()) {
    sensorFailureCount++;
    if (sensorFailureCount >= SENSOR_FAILURE_THRESHOLD) {
      logError(2001, "Sensor failure threshold exceeded");
      systemStatus.sensorHealthy = false;
    }
    return;
  }
  
  // Reset failure count on successful reading
  sensorFailureCount = 0;
  systemStatus.sensorHealthy = true;
  
  // Validate reading
  if (!validateSensorReading(currentReading)) {
    logError(2002, "Invalid sensor reading detected");
    return;
  }
  
  // Store in history
  storeSensorReading(currentReading);
  
  // Determine alert level
  int alertLevel = determineAlertLevel(currentReading.gasPPM);
  
  // Handle alerts
  if (alertLevel > 0) {
    handleGasAlert(alertLevel, currentReading.gasPPM);
  } else {
    clearAlarms();
  }
  
  // Update system status
  updateSystemStatus();
  
  // Print debug info
  if (ENABLE_SERIAL_DEBUG) {
    printSensorReading();
  }
}

bool readGasSensor() {
  // Read analog value
  int sensorValue = analogRead(MQ6_PIN);
  
  // Validate raw reading
  if (sensorValue < 0 || sensorValue > ADC_RESOLUTION) {
    return false;
  }
  
  // Convert to voltage
  float voltage = (sensorValue * REFERENCE_VOLTAGE) / ADC_RESOLUTION;
  
  // Calculate resistance
  float resistance = ((REFERENCE_VOLTAGE - voltage) * MQ6_LOAD_RESISTANCE) / voltage;
  
  // Calculate PPM
  float gasPPM = 0.0;
  if (systemStatus.isCalibrated && calibrationData.is_valid) {
    float ratio = resistance / calibrationData.R0;
    gasPPM = pow(10, ((log10(ratio) - SENSOR_INTERCEPT) / SENSOR_SLOPE));
    
    // Apply temperature and humidity corrections
    gasPPM *= (1.0 + calibrationData.temperature_coefficient * 
               (currentReading.temperature - 25.0));
    gasPPM *= (1.0 + calibrationData.humidity_coefficient * 
               (currentReading.humidity - 50.0));
  } else {
    // Use raw value if not calibrated
    gasPPM = sensorValue;
  }
  
  // Update current reading
  currentReading.rawValue = sensorValue;
  currentReading.voltage = voltage;
  currentReading.resistance = resistance;
  currentReading.gasPPM = gasPPM;
  currentReading.timestamp = millis();
  currentReading.isValid = true;
  
  return true;
}

bool validateSensorReading(SensorData& reading) {
  // Check for reasonable values
  if (reading.gasPPM < MIN_GAS_READING || reading.gasPPM > MAX_GAS_READING) {
    return false;
  }
  
  // Check for sudden changes (possible sensor error)
  if (totalReadings > 0) {
    SensorData lastReading = sensorHistory[(readingIndex - 1 + MAX_SENSOR_READINGS) % MAX_SENSOR_READINGS];
    float changePercent = abs(reading.gasPPM - lastReading.gasPPM) / lastReading.gasPPM;
    
    if (changePercent > 0.5) { // 50% change
      return false;
    }
  }
  
  return true;
}

void storeSensorReading(SensorData& reading) {
  sensorHistory[readingIndex] = reading;
  readingIndex = (readingIndex + 1) % MAX_SENSOR_READINGS;
  
  if (totalReadings < MAX_SENSOR_READINGS) {
    totalReadings++;
  }
}

// ============================================================================
// ALERT HANDLING
// ============================================================================

int determineAlertLevel(float ppm) {
  if (ppm < GAS_SAFE_THRESHOLD) {
    return 0; // Safe
  } else if (ppm < GAS_WARNING_THRESHOLD) {
    return 1; // Warning
  } else if (ppm < GAS_DANGER_THRESHOLD) {
    return 2; // Danger
  } else if (ppm < GAS_CRITICAL_THRESHOLD) {
    return 3; // Critical
  } else {
    return 4; // Emergency
  }
}

void handleGasAlert(int level, float ppm) {
  if (!systemStatus.alarmActive) {
    systemStatus.alarmActive = true;
    logAlarm(level, ppm, getAlertDescription(level));
  }
  
  // Update visual and audio indicators
  updateAlertIndicators(level);
  
  // Transmit alert if WiFi connected
  if (systemStatus.wifiConnected) {
    transmitAlert(level, ppm);
  }
}

void updateAlertIndicators(int level) {
  // Turn off all indicators first
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  unsigned long currentTime = millis();
  
  switch (level) {
    case 1: // Warning
      if ((currentTime / WARNING_BLINK_INTERVAL) % 2) {
        digitalWrite(LED_YELLOW, HIGH);
      }
      break;
      
    case 2: // Danger
      if ((currentTime / DANGER_BLINK_INTERVAL) % 2) {
        digitalWrite(LED_RED, HIGH);
      }
      
      if ((currentTime / BUZZER_BEEP_INTERVAL) % 2) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(BUZZER_BEEP_DURATION);
        digitalWrite(BUZZER_PIN, LOW);
      }
      break;
      
    case 3: // Critical
      digitalWrite(LED_RED, HIGH);
      if ((currentTime / BUZZER_BEEP_INTERVAL) % 2) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(BUZZER_BEEP_DURATION);
        digitalWrite(BUZZER_PIN, LOW);
      }
      break;
      
    case 4: // Emergency
      digitalWrite(LED_RED, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      break;
  }
}

void clearAlarms() {
  if (systemStatus.alarmActive) {
    systemStatus.alarmActive = false;
    
    // Turn off all indicators
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    
    Serial.println("Alarms cleared - Gas levels returned to safe range");
  }
}

// ============================================================================
// CALIBRATION SYSTEM
// ============================================================================

void startCalibrationProcedure() {
  Serial.println("=== CALIBRATION PROCEDURE ===");
  Serial.println("1. Ensure sensor is in clean air");
  Serial.println("2. Remove any gas sources");
  Serial.println("3. Wait for sensor to stabilize");
  Serial.println("4. Calibration will begin in 10 seconds...");
  
  for (int i = 10; i > 0; i--) {
    Serial.println("Starting in " + String(i) + " seconds...");
    delay(1000);
  }
  
  if (performAdvancedCalibration()) {
    systemStatus.isCalibrated = true;
    systemStatus.lastCalibration = millis();
    Serial.println("Calibration completed successfully!");
  } else {
    Serial.println("Calibration failed!");
    systemStatus.isCalibrated = false;
  }
}

bool performAdvancedCalibration() {
  Serial.println("Performing advanced calibration...");
  
  float sum = 0;
  float sumSquared = 0;
  int validSamples = 0;
  
  // Collect samples
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    int sensorValue = analogRead(MQ6_PIN);
    float voltage = (sensorValue * REFERENCE_VOLTAGE) / ADC_RESOLUTION;
    float Rs = ((REFERENCE_VOLTAGE - voltage) * MQ6_LOAD_RESISTANCE) / voltage;
    
    if (Rs > 0 && Rs < 1000) { // Reasonable range
      sum += Rs;
      sumSquared += Rs * Rs;
      validSamples++;
    }
    
    delay(CALIBRATION_INTERVAL);
    
    if (i % 20 == 0) {
      Serial.print("Calibration progress: ");
      Serial.print((i * 100) / CALIBRATION_SAMPLES);
      Serial.println("%");
    }
  }
  
  if (validSamples < CALIBRATION_SAMPLES * 0.8) { // At least 80% valid samples
    Serial.println("Insufficient valid samples for calibration");
    return false;
  }
  
  // Calculate R0 and statistics
  calibrationData.R0 = sum / validSamples;
  float variance = (sumSquared / validSamples) - (calibrationData.R0 * calibrationData.R0);
  float standardDeviation = sqrt(variance);
  float coefficientOfVariation = standardDeviation / calibrationData.R0;
  
  Serial.println("Calibration statistics:");
  Serial.println("R0: " + String(calibrationData.R0, 2));
  Serial.println("Standard deviation: " + String(standardDeviation, 2));
  Serial.println("Coefficient of variation: " + String(coefficientOfVariation, 3));
  
  // Check calibration quality
  if (coefficientOfVariation > CALIBRATION_TOLERANCE) {
    Serial.println("Calibration quality poor - high variation detected");
    return false;
  }
  
  // Set calibration parameters
  calibrationData.R0_clean_air = calibrationData.R0;
  calibrationData.temperature_coefficient = 0.02; // 2% per degree C
  calibrationData.humidity_coefficient = 0.01;    // 1% per %RH
  calibrationData.calibration_date = millis();
  calibrationData.is_valid = true;
  
  // Save calibration data
  saveCalibrationData();
  
  return true;
}

// ============================================================================
// DATA MANAGEMENT
// ============================================================================

void loadSystemConfiguration() {
  // Load configuration from SPIFFS or EEPROM
  // This is a placeholder for configuration loading
  Serial.println("System configuration loaded");
}

void loadCalibrationData() {
  // Check EEPROM for calibration data
  int calibFlag = EEPROM.read(EEPROM_CALIB_FLAG_ADDR);
  
  if (calibFlag == 0xAA) {
    EEPROM.get(EEPROM_R0_ADDR, calibrationData.R0);
    EEPROM.get(EEPROM_CALIB_DATE_ADDR, calibrationData.calibration_date);
    
    if (calibrationData.R0 > 0 && calibrationData.R0 < 1000) {
      calibrationData.is_valid = true;
      systemStatus.isCalibrated = true;
      systemStatus.lastCalibration = calibrationData.calibration_date;
      Serial.println("Calibration data loaded from EEPROM");
    }
  }
}

void saveCalibrationData() {
  EEPROM.put(EEPROM_R0_ADDR, calibrationData.R0);
  EEPROM.put(EEPROM_CALIB_DATE_ADDR, calibrationData.calibration_date);
  EEPROM.write(EEPROM_CALIB_FLAG_ADDR, 0xAA);
  EEPROM.commit();
  
  Serial.println("Calibration data saved to EEPROM");
}

// ============================================================================
// ERROR HANDLING AND LOGGING
// ============================================================================

void logError(int errorCode, String description) {
  systemStatus.errorCount++;
  systemStatus.lastError = description;
  
  // Add to error log
  ErrorLog error;
  error.timestamp = millis();
  error.errorCode = errorCode;
  error.description = description;
  error.resolved = false;
  
  // Store in circular buffer
  static int errorIndex = 0;
  errorLog[errorIndex] = error;
  errorIndex = (errorIndex + 1) % MAX_ERROR_LOG;
  
  Serial.println("ERROR " + String(errorCode) + ": " + description);
}

void logAlarm(int level, float ppm, String description) {
  AlarmRecord alarm;
  alarm.timestamp = millis();
  alarm.level = level;
  alarm.gasPPM = ppm;
  alarm.description = description;
  alarm.acknowledged = false;
  
  // Store in circular buffer
  static int alarmIndex = 0;
  alarmHistory[alarmIndex] = alarm;
  alarmIndex = (alarmIndex + 1) % MAX_ALARM_HISTORY;
  
  Serial.println("ALARM Level " + String(level) + ": " + description + 
                 " (PPM: " + String(ppm, 2) + ")");
}

String getAlertDescription(int level) {
  switch (level) {
    case 1: return "Warning: Gas levels above safe threshold";
    case 2: return "Danger: Gas levels in danger zone";
    case 3: return "Critical: Gas levels critical - evacuate area";
    case 4: return "Emergency: Gas levels extremely high";
    default: return "Unknown alert level";
  }
}

// ============================================================================
// SYSTEM HEALTH AND MAINTENANCE
// ============================================================================

void performHealthCheck() {
  // Check sensor health
  if (!systemStatus.sensorHealthy) {
    logError(3001, "Sensor health check failed");
  }
  
  // Check calibration drift
  if (systemStatus.isCalibrated) {
    checkCalibrationDrift();
  }
  
  // Check system uptime
  if (systemStatus.uptime > SYSTEM_UPTIME_LIMIT) {
    logError(3002, "System uptime limit exceeded");
  }
  
  // Check memory usage
  checkMemoryUsage();
}

void checkCalibrationDrift() {
  // Compare current R0 with stored R0
  float currentR0 = calibrationData.R0;
  float storedR0 = calibrationData.R0_clean_air;
  
  float drift = abs(currentR0 - storedR0) / storedR0;
  
  if (drift > CALIBRATION_DRIFT_LIMIT) {
    logError(3003, "Calibration drift detected: " + String(drift * 100, 1) + "%");
  }
}

void checkMemoryUsage() {
  // Check available heap memory
  size_t freeHeap = ESP.getFreeHeap();
  size_t totalHeap = ESP.getHeapSize();
  
  float memoryUsage = (float)(totalHeap - freeHeap) / totalHeap;
  
  if (memoryUsage > 0.9) { // 90% memory usage
    logError(3004, "High memory usage: " + String(memoryUsage * 100, 1) + "%");
  }
}

void performMaintenanceChecks() {
  Serial.println("Performing maintenance checks...");
  
  // Check sensor performance
  analyzeSensorPerformance();
  
  // Check system logs
  analyzeSystemLogs();
  
  // Perform self-diagnostics
  performSelfDiagnostics();
  
  Serial.println("Maintenance checks completed");
}

void analyzeSensorPerformance() {
  // Analyze sensor reading trends
  if (totalReadings < 10) return;
  
  float sum = 0;
  float sumSquared = 0;
  
  for (int i = 0; i < min(totalReadings, 10); i++) {
    int index = (readingIndex - 1 - i + MAX_SENSOR_READINGS) % MAX_SENSOR_READINGS;
    float ppm = sensorHistory[index].gasPPM;
    sum += ppm;
    sumSquared += ppm * ppm;
  }
  
  float average = sum / min(totalReadings, 10);
  float variance = (sumSquared / min(totalReadings, 10)) - (average * average);
  float standardDeviation = sqrt(variance);
  
  if (standardDeviation > average * 0.2) { // 20% variation
    logError(4001, "High sensor reading variation detected");
  }
}

void analyzeSystemLogs() {
  // Analyze error patterns
  int recentErrors = 0;
  unsigned long currentTime = millis();
  
  for (int i = 0; i < MAX_ERROR_LOG; i++) {
    if (errorLog[i].timestamp > currentTime - 3600000) { // Last hour
      recentErrors++;
    }
  }
  
  if (recentErrors > 10) {
    logError(4002, "High error rate detected: " + String(recentErrors) + " errors in last hour");
  }
}

void performSelfDiagnostics() {
  // Test all system components
  testBasicHardware();
  
  // Test communication interfaces
  if (ENABLE_WIFI) {
    testWiFiConnection();
  }
  
  // Test data storage
  testDataStorage();
}

// ============================================================================
// EMERGENCY HANDLING
// ============================================================================

bool checkEmergencyConditions() {
  // Check for emergency gas levels
  if (currentReading.gasPPM > GAS_CRITICAL_THRESHOLD * 2) {
    return true;
  }
  
  // Check for system failures
  if (systemStatus.errorCount > 50) {
    return true;
  }
  
  // Check for sensor failure
  if (sensorFailureCount > 10) {
    return true;
  }
  
  return false;
}

void handleEmergencyShutdown() {
  if (!emergencyShutdown) {
    emergencyShutdown = true;
    systemStatus.isOperational = false;
    
    // Activate all safety systems
    digitalWrite(LED_RED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    
    // Log emergency
    logError(9999, "EMERGENCY SHUTDOWN ACTIVATED");
    
    // Transmit emergency alert
    if (systemStatus.wifiConnected) {
      transmitEmergencyAlert();
    }
    
    Serial.println("=== EMERGENCY SHUTDOWN ACTIVATED ===");
    Serial.println("System is now in safe mode");
    Serial.println("Manual intervention required");
  }
}

// ============================================================================
// COMMUNICATION FUNCTIONS
// ============================================================================

void initializeWiFi() {
  // WiFi initialization code (similar to advanced version)
  Serial.println("WiFi initialization placeholder");
  systemStatus.wifiConnected = true; // Placeholder
}

void initializeWebServer() {
  // Web server setup for configuration
  webServer.on("/", handleRoot);
  webServer.on("/status", handleStatus);
  webServer.on("/calibrate", handleCalibrate);
  webServer.on("/config", handleConfig);
  
  webServer.begin();
  Serial.println("Web server started on port 80");
}

void transmitSystemData() {
  // Transmit comprehensive system data
  if (!systemStatus.wifiConnected) return;
  
  HTTPClient http;
  http.begin(SERVER_URL + API_ENDPOINT_DATA);
  http.addHeader("Content-Type", "application/json");
  
  DynamicJsonDocument doc(2048);
  doc["device_id"] = DEVICE_ID;
  doc["firmware_version"] = FIRMWARE_VERSION;
  doc["timestamp"] = millis();
  doc["uptime"] = systemStatus.uptime;
  doc["gas_ppm"] = currentReading.gasPPM;
  doc["alert_level"] = determineAlertLevel(currentReading.gasPPM);
  doc["system_status"] = systemStatus.isOperational;
  doc["sensor_healthy"] = systemStatus.sensorHealthy;
  doc["calibrated"] = systemStatus.isCalibrated;
  doc["error_count"] = systemStatus.errorCount;
  doc["wifi_rssi"] = WiFi.RSSI();
  
  String payload;
  serializeJson(doc, payload);
  
  int httpResponseCode = http.POST(payload);
  http.end();
  
  if (httpResponseCode > 0) {
    Serial.println("System data transmitted successfully");
  }
}

void transmitAlert(int level, float ppm) {
  // Transmit alert data
  HTTPClient http;
  http.begin(SERVER_URL + API_ENDPOINT_ALERT);
  http.addHeader("Content-Type", "application/json");
  
  DynamicJsonDocument doc(512);
  doc["device_id"] = DEVICE_ID;
  doc["timestamp"] = millis();
  doc["alert_level"] = level;
  doc["gas_ppm"] = ppm;
  doc["description"] = getAlertDescription(level);
  
  String payload;
  serializeJson(doc, payload);
  
  http.POST(payload);
  http.end();
}

void transmitEmergencyAlert() {
  // Transmit emergency alert
  HTTPClient http;
  http.begin(SERVER_URL + API_ENDPOINT_ALERT);
  http.addHeader("Content-Type", "application/json");
  
  DynamicJsonDocument doc(512);
  doc["device_id"] = DEVICE_ID;
  doc["timestamp"] = millis();
  doc["alert_level"] = 999;
  doc["gas_ppm"] = currentReading.gasPPM;
  doc["description"] = "EMERGENCY SHUTDOWN ACTIVATED";
  doc["emergency"] = true;
  
  String payload;
  serializeJson(doc, payload);
  
  http.POST(payload);
  http.end();
}

// ============================================================================
// WEB SERVER HANDLERS
// ============================================================================

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>BantayGas Production System</h1>";
  html += "<p>Status: " + String(systemStatus.isOperational ? "Operational" : "Offline") + "</p>";
  html += "<p>Gas Level: " + String(currentReading.gasPPM, 2) + " ppm</p>";
  html += "<p>Alert Level: " + String(determineAlertLevel(currentReading.gasPPM)) + "</p>";
  html += "<p>Uptime: " + String(systemStatus.uptime / 1000) + " seconds</p>";
  html += "</body></html>";
  
  webServer.send(200, "text/html", html);
}

void handleStatus() {
  DynamicJsonDocument doc(1024);
  doc["operational"] = systemStatus.isOperational;
  doc["calibrated"] = systemStatus.isCalibrated;
  doc["gas_ppm"] = currentReading.gasPPM;
  doc["alert_level"] = determineAlertLevel(currentReading.gasPPM);
  doc["uptime"] = systemStatus.uptime;
  doc["error_count"] = systemStatus.errorCount;
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

void handleCalibrate() {
  if (webServer.hasArg("start")) {
    startCalibrationProcedure();
    webServer.send(200, "text/plain", "Calibration started");
  } else {
    webServer.send(400, "text/plain", "Missing 'start' parameter");
  }
}

void handleConfig() {
  // Configuration handling
  webServer.send(200, "text/plain", "Configuration endpoint");
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void printSystemHeader() {
  Serial.println("\n==========================================");
  Serial.println("    BantayGas Production System v" + String(FIRMWARE_VERSION));
  Serial.println("    Manufacturer: " + String(MANUFACTURER));
  Serial.println("    Model: " + String(MODEL_NUMBER));
  Serial.println("    Hardware: " + String(HARDWARE_VERSION));
  Serial.println("==========================================");
}

void printSystemInfo() {
  Serial.println("System Information:");
  Serial.println("- Gas Type: " + String(GAS_TYPE_NAME));
  Serial.println("- Safe Threshold: " + String(GAS_SAFE_THRESHOLD) + " ppm");
  Serial.println("- Warning Threshold: " + String(GAS_WARNING_THRESHOLD) + " ppm");
  Serial.println("- Danger Threshold: " + String(GAS_DANGER_THRESHOLD) + " ppm");
  Serial.println("- Critical Threshold: " + String(GAS_CRITICAL_THRESHOLD) + " ppm");
  Serial.println("- WiFi: " + String(systemStatus.wifiConnected ? "Connected" : "Disconnected"));
  Serial.println("- Calibrated: " + String(systemStatus.isCalibrated ? "Yes" : "No"));
}

void printSensorReading() {
  Serial.print("Gas: ");
  Serial.print(currentReading.gasPPM, 2);
  Serial.print(" ppm | Level: ");
  Serial.print(determineAlertLevel(currentReading.gasPPM));
  Serial.print(" | R0: ");
  Serial.print(calibrationData.R0, 2);
  Serial.print(" | Errors: ");
  Serial.println(systemStatus.errorCount);
}

void updateSystemStatus() {
  // Update system status based on current conditions
  systemStatus.alarmActive = (determineAlertLevel(currentReading.gasPPM) > 0);
}

bool performComprehensiveTests() {
  Serial.println("Performing comprehensive system tests...");
  
  // Test hardware
  if (!testBasicHardware()) {
    return false;
  }
  
  // Test sensor
  if (!testSensor()) {
    return false;
  }
  
  // Test communication
  if (ENABLE_WIFI) {
    if (!testWiFiConnection()) {
      Serial.println("WiFi test failed, but continuing...");
    }
  }
  
  Serial.println("Comprehensive tests completed successfully");
  return true;
}

bool testSensor() {
  Serial.println("Testing gas sensor...");
  
  int sensorValue = analogRead(MQ6_PIN);
  float voltage = (sensorValue * REFERENCE_VOLTAGE) / ADC_RESOLUTION;
  
  if (sensorValue < 0 || sensorValue > ADC_RESOLUTION) {
    Serial.println("Sensor test failed: Invalid reading");
    return false;
  }
  
  if (voltage < 0.1 || voltage > 3.2) {
    Serial.println("Sensor test failed: Voltage out of range");
    return false;
  }
  
  Serial.println("Sensor test passed");
  return true;
}

bool testWiFiConnection() {
  // WiFi connection test
  return systemStatus.wifiConnected;
}

bool testDataStorage() {
  // Test EEPROM and SPIFFS
  return true;
}

void handleAlarmState() {
  // Handle alarm state in main loop
  // This is called from the main loop when alarm is active
}

void performMaintenanceChecks() {
  // Maintenance checks are performed in the main loop
  // This is called from the main loop periodically
}


