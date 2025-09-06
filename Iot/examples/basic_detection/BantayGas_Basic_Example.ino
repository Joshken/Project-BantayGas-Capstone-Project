/*
 * BantayGas Basic Detection Example
 * 
 * This example demonstrates basic gas detection functionality using
 * the BantayGas IoT system with ESP32 and MQ-6 gas sensor.
 * 
 * Features:
 * - Basic gas detection and monitoring
 * - LED indicators for different gas levels
 * - Buzzer alerts for dangerous levels
 * - Serial output for debugging
 * - Simple calibration procedure
 * 
 * Hardware Required:
 * - ESP32 Dev Board
 * - MQ-6 Gas Sensor
 * - 3x LEDs (Red, Green, Yellow)
 * - 1x Buzzer
 * - Resistors: 3x 220Ω, 1x 10kΩ
 * 
 * Connections:
 * - MQ-6 VCC → 3.3V
 * - MQ-6 GND → GND
 * - MQ-6 A0 → GPIO 34
 * - Green LED → GPIO 21 (with 220Ω resistor)
 * - Yellow LED → GPIO 22 (with 220Ω resistor)
 * - Red LED → GPIO 23 (with 220Ω resistor)
 * - Buzzer → GPIO 19
 * 
 * Author: BantayGas Development Team
 * Version: 1.0.0
 */

// ============================================================================
// INCLUDES
// ============================================================================

#include <Arduino.h>
#include "hardware_config.h"
#include "gas_config.h"

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

// Gas detection variables
float gasPPM = 0.0;
float R0 = 0.0;
bool isCalibrated = false;

// Alert variables
int alertLevel = 0; // 0=safe, 1=warning, 2=danger, 3=critical
bool alertActive = false;
unsigned long alertStartTime = 0;

// Timing variables
unsigned long lastReading = 0;
unsigned long lastBlinkTime = 0;
unsigned long lastBeepTime = 0;

// LED and buzzer states
bool ledState = false;
bool buzzerState = false;

// ============================================================================
// SETUP FUNCTION
// ============================================================================

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== BantayGas Basic Detection Example ===");
  Serial.println("Initializing system...");
  
  // Initialize hardware
  initializeHardware();
  
  // Load calibration data
  loadCalibrationData();
  
  // Perform system tests
  performSystemTests();
  
  // Start calibration if needed
  if (!isCalibrated) {
    Serial.println("System requires calibration...");
    performCalibration();
  }
  
  Serial.println("System ready for gas detection!");
  Serial.println("Gas Type: " + String(GAS_TYPE_NAME));
  Serial.println("Safe Threshold: " + String(GAS_SAFE_THRESHOLD) + " ppm");
  Serial.println("Warning Threshold: " + String(GAS_WARNING_THRESHOLD) + " ppm");
  Serial.println("Danger Threshold: " + String(GAS_DANGER_THRESHOLD) + " ppm");
  Serial.println("Critical Threshold: " + String(GAS_CRITICAL_THRESHOLD) + " ppm");
  Serial.println("\nMonitoring gas levels...");
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  unsigned long currentTime = millis();
  
  // Read gas sensor every 500ms
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
    
    // Print sensor data
    printSensorData();
  }
  
  // Handle alerts and indicators
  if (alertLevel > 0) {
    handleAlerts();
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
// GAS DETECTION FUNCTIONS
// ============================================================================

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
      Serial.println("Status: SAFE - Gas levels normal");
      break;
      
    case 1: // Warning
      digitalWrite(LED_YELLOW, HIGH);
      alertActive = true;
      alertStartTime = millis();
      Serial.println("ALERT: WARNING - Gas levels above safe threshold");
      break;
      
    case 2: // Danger
      digitalWrite(LED_RED, HIGH);
      alertActive = true;
      alertStartTime = millis();
      Serial.println("ALERT: DANGER - Gas levels in danger zone");
      break;
      
    case 3: // Critical
      digitalWrite(LED_RED, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      alertActive = true;
      alertStartTime = millis();
      Serial.println("ALERT: CRITICAL - Gas levels extremely high - EVACUATE!");
      break;
  }
}

void handleAlerts() {
  unsigned long currentTime = millis();
  
  // Handle different alert patterns
  switch (alertLevel) {
    case 1: // Warning - slow blink
      if (currentTime - lastBlinkTime > WARNING_BLINK_INTERVAL) {
        ledState = !ledState;
        digitalWrite(LED_YELLOW, ledState);
        lastBlinkTime = currentTime;
      }
      break;
      
    case 2: // Danger - fast blink + beep
      if (currentTime - lastBlinkTime > DANGER_BLINK_INTERVAL) {
        ledState = !ledState;
        digitalWrite(LED_RED, ledState);
        lastBlinkTime = currentTime;
      }
      
      if (currentTime - lastBeepTime > BUZZER_BEEP_INTERVAL) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(BUZZER_BEEP_DURATION);
        digitalWrite(BUZZER_PIN, LOW);
        lastBeepTime = currentTime;
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
  Serial.println("\n=== CALIBRATION PROCEDURE ===");
  Serial.println("1. Ensure sensor is in clean air");
  Serial.println("2. Remove any gas sources");
  Serial.println("3. Wait for sensor to stabilize");
  Serial.println("4. Calibration will begin in 10 seconds...");
  
  // Countdown
  for (int i = 10; i > 0; i--) {
    Serial.println("Starting in " + String(i) + " seconds...");
    delay(1000);
  }
  
  Serial.println("Starting calibration...");
  
  float sum = 0;
  int validSamples = 0;
  
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    int sensorValue = analogRead(MQ6_PIN);
    float voltage = (sensorValue * REFERENCE_VOLTAGE) / ADC_RESOLUTION;
    float Rs = ((REFERENCE_VOLTAGE - voltage) * MQ6_LOAD_RESISTANCE) / voltage;
    
    if (Rs > 0 && Rs < 1000) {
      sum += Rs;
      validSamples++;
    }
    
    delay(CALIBRATION_INTERVAL);
    
    // Show progress
    if (i % 20 == 0) {
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
    Serial.println("R0 value: " + String(R0, 2) + " kΩ");
    Serial.println("Valid samples: " + String(validSamples) + "/" + String(CALIBRATION_SAMPLES));
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
      Serial.println("R0 value: " + String(R0, 2) + " kΩ");
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
// SYSTEM TESTING
// ============================================================================

void performSystemTests() {
  Serial.println("Performing system tests...");
  
  // Test LEDs
  testLEDs();
  
  // Test buzzer
  testBuzzer();
  
  // Test sensor
  testSensor();
  
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

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void printSensorData() {
  Serial.print("Gas Level: ");
  Serial.print(gasPPM, 2);
  Serial.print(" ppm | Alert Level: ");
  Serial.print(alertLevel);
  Serial.print(" | R0: ");
  Serial.print(R0, 2);
  Serial.print(" | Calibrated: ");
  Serial.println(isCalibrated ? "Yes" : "No");
}

// ============================================================================
// SERIAL COMMANDS
// ============================================================================

void handleSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "calibrate") {
      Serial.println("Starting calibration...");
      performCalibration();
    } else if (command == "status") {
      printSensorData();
    } else if (command == "test") {
      performSystemTests();
    } else if (command == "help") {
      Serial.println("Available commands:");
      Serial.println("calibrate - Start calibration procedure");
      Serial.println("status - Show current sensor status");
      Serial.println("test - Perform system tests");
      Serial.println("help - Show this help message");
    } else {
      Serial.println("Unknown command. Type 'help' for available commands.");
    }
  }
}

// ============================================================================
// ADDITIONAL FEATURES
// ============================================================================

// Add this to the main loop to enable serial commands
// handleSerialCommands();

// Example of adding a button for manual calibration
/*
#define CALIBRATION_BUTTON_PIN 0

void checkCalibrationButton() {
  if (digitalRead(CALIBRATION_BUTTON_PIN) == LOW) {
    delay(50); // Debounce
    if (digitalRead(CALIBRATION_BUTTON_PIN) == LOW) {
      Serial.println("Calibration button pressed!");
      performCalibration();
      while (digitalRead(CALIBRATION_BUTTON_PIN) == LOW) {
        delay(10); // Wait for button release
      }
    }
  }
}
*/

// Example of adding data logging
/*
void logData() {
  // Log data to serial or SD card
  Serial.print("LOG: ");
  Serial.print(millis());
  Serial.print(",");
  Serial.print(gasPPM, 2);
  Serial.print(",");
  Serial.println(alertLevel);
}
*/


