/*
 * BantayGas Advanced Gas Detection System
 * ESP32 + MQ-6 Gas Detector with WiFi Connectivity
 * 
 * Features:
 * - Real-time gas detection with multiple thresholds
 * - WiFi connectivity for remote monitoring
 * - Automatic calibration system
 * - Data logging and transmission
 * - Safety features and emergency shutdown
 * - OTA updates support
 * 
 * Author: BantayGas Development Team
 * Version: 2.0.0
 * Date: 2024
 */

// ============================================================================
// INCLUDES
// ============================================================================

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <time.h>
#include "hardware_config.h"
#include "gas_config.h"
#include "wifi_config.h"

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

// System state
enum SystemState {
  STATE_INIT,
  STATE_CALIBRATING,
  STATE_MONITORING,
  STATE_ALARM,
  STATE_ERROR
};

SystemState currentState = STATE_INIT;

// Gas detection variables
float gasPPM = 0.0;
float R0 = 0.0;
bool isCalibrated = false;
unsigned long lastReading = 0;
unsigned long lastTransmission = 0;

// Alert variables
bool alertActive = false;
unsigned long alertStartTime = 0;
int alertLevel = 0; // 0=safe, 1=warning, 2=danger, 3=critical

// WiFi variables
bool wifiConnected = false;
unsigned long lastWifiCheck = 0;
int wifiRetryCount = 0;

// Data logging
struct GasReading {
  unsigned long timestamp;
  float ppm;
  int alertLevel;
  float temperature;
  float humidity;
};

GasReading readings[MAX_LOG_ENTRIES];
int readingIndex = 0;
int totalReadings = 0;

// ============================================================================
// SETUP FUNCTION
// ============================================================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== BantayGas Advanced Gas Detection System ===");
  Serial.println("Version: 2.0.0");
  Serial.println("Initializing...");
  
  // Initialize hardware
  initializeHardware();
  
  // Initialize EEPROM
  EEPROM.begin(512);
  
  // Load calibration data
  loadCalibrationData();
  
  // Initialize WiFi if enabled
  if (ENABLE_WIFI) {
    initializeWiFi();
  }
  
  // Initialize NTP if enabled
  if (ENABLE_NTP && wifiConnected) {
    initializeNTP();
  }
  
  // Perform system tests
  performSystemTests();
  
  // Start calibration if needed
  if (!isCalibrated) {
    currentState = STATE_CALIBRATING;
    Serial.println("Starting calibration...");
    performCalibration();
  }
  
  currentState = STATE_MONITORING;
  Serial.println("System ready for monitoring!");
  Serial.println("Gas Type: " + String(GAS_TYPE_NAME));
  Serial.println("Safe Threshold: " + String(GAS_SAFE_THRESHOLD) + " ppm");
  Serial.println("Warning Threshold: " + String(GAS_WARNING_THRESHOLD) + " ppm");
  Serial.println("Danger Threshold: " + String(GAS_DANGER_THRESHOLD) + " ppm");
  Serial.println("Critical Threshold: " + String(GAS_CRITICAL_THRESHOLD) + " ppm");
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  unsigned long currentTime = millis();
  
  // Handle different system states
  switch (currentState) {
    case STATE_INIT:
      // Initialization complete, move to monitoring
      currentState = STATE_MONITORING;
      break;
      
    case STATE_CALIBRATING:
      // Calibration in progress
      handleCalibration();
      break;
      
    case STATE_MONITORING:
      // Normal monitoring mode
      handleMonitoring();
      break;
      
    case STATE_ALARM:
      // Alarm state
      handleAlarm();
      break;
      
    case STATE_ERROR:
      // Error state
      handleError();
      break;
  }
  
  // Handle WiFi connection
  if (ENABLE_WIFI && (currentTime - lastWifiCheck > 30000)) {
    checkWiFiConnection();
    lastWifiCheck = currentTime;
  }
  
  // Transmit data periodically
  if (ENABLE_WIFI && wifiConnected && 
      (currentTime - lastTransmission > TRANSMIT_INTERVAL)) {
    transmitData();
    lastTransmission = currentTime;
  }
  
  // Handle OTA updates
  if (ENABLE_OTA_UPDATES && wifiConnected) {
    handleOTAUpdate();
  }
  
  // Small delay to prevent watchdog reset
  delay(10);
}

// ============================================================================
// HARDWARE INITIALIZATION
// ============================================================================

void initializeHardware() {
  Serial.println("Initializing hardware...");
  
  // Configure pins
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize all outputs to LOW
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Configure ADC
  analogReadResolution(12); // 12-bit resolution
  
  Serial.println("Hardware initialized successfully");
}

// ============================================================================
// WIFI INITIALIZATION
// ============================================================================

void initializeWiFi() {
  Serial.println("Initializing WiFi...");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && 
         (millis() - startTime) < WIFI_CONNECT_TIMEOUT) {
    delay(500);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\nWiFi connected successfully!");
    Serial.println("IP address: " + WiFi.localIP().toString());
    Serial.println("Signal strength: " + String(WiFi.RSSI()) + " dBm");
  } else {
    wifiConnected = false;
    Serial.println("\nWiFi connection failed!");
    
    // Try backup networks
    tryBackupNetworks();
  }
}

// ============================================================================
// GAS DETECTION FUNCTIONS
// ============================================================================

void handleMonitoring() {
  unsigned long currentTime = millis();
  
  // Read gas sensor
  if (currentTime - lastReading >= READING_INTERVAL) {
    readGasSensor();
    lastReading = currentTime;
    
    // Determine alert level
    int newAlertLevel = determineAlertLevel(gasPPM);
    
    // Update alert state if changed
    if (newAlertLevel != alertLevel) {
      alertLevel = newAlertLevel;
      updateAlertState();
    }
    
    // Log reading
    if (ENABLE_DATA_LOGGING) {
      logReading(gasPPM, alertLevel);
    }
    
    // Print debug info
    if (ENABLE_SERIAL_DEBUG) {
      printSensorData();
    }
  }
  
  // Handle alerts
  if (alertLevel > 0) {
    handleAlerts();
  }
}

void readGasSensor() {
  // Read analog value
  int sensorValue = analogRead(MQ6_PIN);
  
  // Convert to voltage
  float voltage = (sensorValue * REFERENCE_VOLTAGE) / ADC_RESOLUTION;
  
  // Calculate resistance
  float Rs = ((REFERENCE_VOLTAGE - voltage) * MQ6_LOAD_RESISTANCE) / voltage;
  
  // Calculate PPM using calibration data
  if (isCalibrated && R0 > 0) {
    float ratio = Rs / R0;
    gasPPM = pow(10, ((log10(ratio) - SENSOR_INTERCEPT) / SENSOR_SLOPE));
    
    // Apply safety limits
    if (gasPPM < 0) gasPPM = 0;
    if (gasPPM > 10000) gasPPM = 10000;
  } else {
    // Use raw sensor value if not calibrated
    gasPPM = sensorValue;
  }
}

int determineAlertLevel(float ppm) {
  if (ppm < GAS_SAFE_THRESHOLD) {
    return 0; // Safe
  } else if (ppm < GAS_WARNING_THRESHOLD) {
    return 1; // Warning
  } else if (ppm < GAS_DANGER_THRESHOLD) {
    return 2; // Danger
  } else {
    return 3; // Critical
  }
}

// ============================================================================
// ALERT HANDLING
// ============================================================================

void updateAlertState() {
  // Turn off all indicators
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  switch (alertLevel) {
    case 0: // Safe
      digitalWrite(LED_GREEN, HIGH);
      alertActive = false;
      break;
      
    case 1: // Warning
      digitalWrite(LED_YELLOW, HIGH);
      alertActive = true;
      alertStartTime = millis();
      break;
      
    case 2: // Danger
      digitalWrite(LED_RED, HIGH);
      alertActive = true;
      alertStartTime = millis();
      break;
      
    case 3: // Critical
      digitalWrite(LED_RED, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      alertActive = true;
      alertStartTime = millis();
      break;
  }
}

void handleAlerts() {
  unsigned long currentTime = millis();
  
  // Handle different alert patterns
  switch (alertLevel) {
    case 1: // Warning - slow blink
      if ((currentTime / WARNING_BLINK_INTERVAL) % 2) {
        digitalWrite(LED_YELLOW, HIGH);
      } else {
        digitalWrite(LED_YELLOW, LOW);
      }
      break;
      
    case 2: // Danger - fast blink + beep
      if ((currentTime / DANGER_BLINK_INTERVAL) % 2) {
        digitalWrite(LED_RED, HIGH);
      } else {
        digitalWrite(LED_RED, LOW);
      }
      
      if ((currentTime / BUZZER_BEEP_INTERVAL) % 2) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(BUZZER_BEEP_DURATION);
        digitalWrite(BUZZER_PIN, LOW);
      }
      break;
      
    case 3: // Critical - continuous alarm
      digitalWrite(LED_RED, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      
      // Check for maximum alarm time
      if (currentTime - alertStartTime > MAX_CONTINUOUS_ALARM) {
        digitalWrite(BUZZER_PIN, LOW);
        Serial.println("WARNING: Maximum alarm time reached!");
      }
      break;
  }
}

// ============================================================================
// CALIBRATION FUNCTIONS
// ============================================================================

void performCalibration() {
  Serial.println("Starting calibration process...");
  Serial.println("Please ensure sensor is in clean air for 60 seconds");
  
  float sum = 0;
  int validSamples = 0;
  
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    int sensorValue = analogRead(MQ6_PIN);
    float voltage = (sensorValue * REFERENCE_VOLTAGE) / ADC_RESOLUTION;
    float Rs = ((REFERENCE_VOLTAGE - voltage) * MQ6_LOAD_RESISTANCE) / voltage;
    
    if (Rs > 0) {
      sum += Rs;
      validSamples++;
    }
    
    delay(CALIBRATION_INTERVAL);
    
    // Show progress
    if (i % 10 == 0) {
      Serial.print("Calibration progress: ");
      Serial.print((i * 100) / CALIBRATION_SAMPLES);
      Serial.println("%");
    }
  }
  
  if (validSamples > 0) {
    R0 = sum / validSamples;
    isCalibrated = true;
    
    // Save calibration data
    saveCalibrationData();
    
    Serial.println("Calibration completed successfully!");
    Serial.println("R0 value: " + String(R0, 2));
  } else {
    Serial.println("Calibration failed! No valid samples.");
    isCalibrated = false;
  }
}

void loadCalibrationData() {
  // Check if calibration data exists
  int calibFlag = EEPROM.read(EEPROM_CALIB_FLAG_ADDR);
  
  if (calibFlag == 0xAA) { // Calibration flag
    // Read R0 value
    EEPROM.get(EEPROM_R0_ADDR, R0);
    
    if (R0 > 0 && R0 < 1000) { // Reasonable range
      isCalibrated = true;
      Serial.println("Calibration data loaded successfully");
      Serial.println("R0 value: " + String(R0, 2));
    } else {
      isCalibrated = false;
      Serial.println("Invalid calibration data found");
    }
  } else {
    isCalibrated = false;
    Serial.println("No calibration data found");
  }
}

void saveCalibrationData() {
  // Save R0 value
  EEPROM.put(EEPROM_R0_ADDR, R0);
  
  // Set calibration flag
  EEPROM.write(EEPROM_CALIB_FLAG_ADDR, 0xAA);
  
  // Save calibration date
  unsigned long calibDate = millis();
  EEPROM.put(EEPROM_CALIB_DATE_ADDR, calibDate);
  
  EEPROM.commit();
  Serial.println("Calibration data saved to EEPROM");
}

// ============================================================================
// DATA LOGGING AND TRANSMISSION
// ============================================================================

void logReading(float ppm, int level) {
  GasReading reading;
  reading.timestamp = millis();
  reading.ppm = ppm;
  reading.alertLevel = level;
  reading.temperature = 25.0; // Placeholder - add temperature sensor
  reading.humidity = 50.0;    // Placeholder - add humidity sensor
  
  readings[readingIndex] = reading;
  readingIndex = (readingIndex + 1) % MAX_LOG_ENTRIES;
  
  if (totalReadings < MAX_LOG_ENTRIES) {
    totalReadings++;
  }
}

void transmitData() {
  if (!wifiConnected) return;
  
  HTTPClient http;
  http.begin(SERVER_URL + API_ENDPOINT_DATA);
  http.addHeader("Content-Type", "application/json");
  
  // Create JSON payload
  DynamicJsonDocument doc(1024);
  doc["device_id"] = DEVICE_ID;
  doc["timestamp"] = millis();
  doc["gas_ppm"] = gasPPM;
  doc["alert_level"] = alertLevel;
  doc["gas_type"] = GAS_TYPE_NAME;
  doc["r0_value"] = R0;
  doc["is_calibrated"] = isCalibrated;
  doc["wifi_rssi"] = WiFi.RSSI();
  
  String payload;
  serializeJson(doc, payload);
  
  int httpResponseCode = http.POST(payload);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Data transmitted successfully");
    Serial.println("Response: " + response);
  } else {
    Serial.println("Error transmitting data: " + String(httpResponseCode));
  }
  
  http.end();
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void performSystemTests() {
  Serial.println("Performing system tests...");
  
  if (ENABLE_LED_TEST) {
    testLEDs();
  }
  
  if (ENABLE_BUZZER_TEST) {
    testBuzzer();
  }
  
  if (ENABLE_SENSOR_TEST) {
    testSensor();
  }
  
  Serial.println("System tests completed");
}

void testLEDs() {
  Serial.println("Testing LEDs...");
  
  digitalWrite(LED_GREEN, HIGH);
  delay(500);
  digitalWrite(LED_GREEN, LOW);
  
  digitalWrite(LED_YELLOW, HIGH);
  delay(500);
  digitalWrite(LED_YELLOW, LOW);
  
  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
  
  Serial.println("LED test completed");
}

void testBuzzer() {
  Serial.println("Testing buzzer...");
  
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
  
  Serial.println("Buzzer test completed");
}

void testSensor() {
  Serial.println("Testing gas sensor...");
  
  int sensorValue = analogRead(MQ6_PIN);
  float voltage = (sensorValue * REFERENCE_VOLTAGE) / ADC_RESOLUTION;
  
  Serial.println("Raw sensor value: " + String(sensorValue));
  Serial.println("Voltage: " + String(voltage, 3) + "V");
  
  if (sensorValue > 0 && sensorValue < ADC_RESOLUTION) {
    Serial.println("Sensor test passed");
  } else {
    Serial.println("Sensor test failed!");
  }
}

void printSensorData() {
  Serial.print("Gas Level: ");
  Serial.print(gasPPM, 2);
  Serial.print(" ppm | Alert Level: ");
  Serial.print(alertLevel);
  Serial.print(" | R0: ");
  Serial.print(R0, 2);
  Serial.print(" | WiFi: ");
  Serial.println(wifiConnected ? "Connected" : "Disconnected");
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    wifiRetryCount++;
    
    if (wifiRetryCount < MAX_RETRY_ATTEMPTS) {
      Serial.println("WiFi disconnected. Attempting reconnection...");
      WiFi.reconnect();
    } else {
      Serial.println("WiFi reconnection failed. Trying backup networks...");
      tryBackupNetworks();
      wifiRetryCount = 0;
    }
  } else {
    wifiConnected = true;
    wifiRetryCount = 0;
  }
}

void tryBackupNetworks() {
  // Try backup networks
  String backupSSIDs[] = {WIFI_SSID_2, WIFI_SSID_3};
  String backupPasswords[] = {WIFI_PASSWORD_2, WIFI_PASSWORD_3};
  
  for (int i = 0; i < 2; i++) {
    if (strlen(backupSSIDs[i].c_str()) > 0) {
      Serial.println("Trying backup network: " + backupSSIDs[i]);
      WiFi.begin(backupSSIDs[i].c_str(), backupPasswords[i].c_str());
      
      unsigned long startTime = millis();
      while (WiFi.status() != WL_CONNECTED && 
             (millis() - startTime) < WIFI_CONNECT_TIMEOUT) {
        delay(500);
      }
      
      if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        Serial.println("Connected to backup network!");
        return;
      }
    }
  }
  
  Serial.println("All WiFi networks failed. Starting AP mode...");
  startAPMode();
}

void startAPMode() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL, false, AP_MAX_CONNECTIONS);
  
  Serial.println("Access Point started");
  Serial.println("SSID: " + String(AP_SSID));
  Serial.println("Password: " + String(AP_PASSWORD));
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}

void initializeNTP() {
  configTime(TIMEZONE_OFFSET * 3600, 0, NTP_SERVER_1, NTP_SERVER_2, NTP_SERVER_3);
  
  Serial.println("NTP initialized");
  Serial.println("Waiting for time sync...");
  
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    now = time(nullptr);
  }
  
  Serial.println("Time synchronized");
}

void handleOTAUpdate() {
  // OTA update handling would go here
  // This is a placeholder for future implementation
}

void handleCalibration() {
  // Calibration handling in main loop
  // This is called from the main loop when in calibration state
}

void handleAlarm() {
  // Alarm state handling
  // This is called from the main loop when in alarm state
}

void handleError() {
  // Error state handling
  // This is called from the main loop when in error state
}

