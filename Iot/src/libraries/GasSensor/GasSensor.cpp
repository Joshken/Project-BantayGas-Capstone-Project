/*
 * GasSensor Library Implementation for BantayGas IoT System
 * 
 * Implementation of gas sensor operations including reading, calibration,
 * health monitoring, and data processing.
 * 
 * Author: BantayGas Development Team
 * Version: 1.0.0
 */

#include "GasSensor.h"
#include <EEPROM.h>
#include <math.h>

// ============================================================================
// GLOBAL INSTANCES
// ============================================================================

GasSensor gasSensor;
GasDetectorManager detectorManager(1);
GasAlertManager alertManager;

// ============================================================================
// GAS SENSOR CLASS IMPLEMENTATION
// ============================================================================

GasSensor::GasSensor(int pin, float loadR) {
  sensorPin = pin;
  loadResistance = loadR;
  referenceVoltage = REFERENCE_VOLTAGE;
  adcResolution = ADC_RESOLUTION;
  
  // Initialize calibration data
  calibration.R0 = 0.0;
  calibration.R0_clean_air = 0.0;
  calibration.temperature_coefficient = 0.02;
  calibration.humidity_coefficient = 0.01;
  calibration.calibration_date = 0;
  calibration.is_valid = false;
  calibration.confidence = 0.0;
  
  // Initialize health data
  health.isHealthy = true;
  health.failureCount = 0;
  health.averageReading = 0.0;
  health.standardDeviation = 0.0;
  health.lastMaintenance = 0;
  health.totalReadings = 0;
  
  // Initialize reading history
  historyIndex = 0;
  historyCount = 0;
  for (int i = 0; i < 10; i++) {
    readingHistory[i] = 0.0;
  }
}

bool GasSensor::begin() {
  // Configure ADC
  analogReadResolution(12);
  
  // Load calibration data
  loadCalibration();
  
  // Perform initial health check
  performHealthCheck();
  
  return true;
}

void GasSensor::reset() {
  // Reset calibration data
  calibration.is_valid = false;
  calibration.R0 = 0.0;
  calibration.confidence = 0.0;
  
  // Reset health data
  health.isHealthy = true;
  health.failureCount = 0;
  health.totalReadings = 0;
  
  // Clear reading history
  historyIndex = 0;
  historyCount = 0;
  for (int i = 0; i < 10; i++) {
    readingHistory[i] = 0.0;
  }
}

SensorReading GasSensor::readSensor() {
  SensorReading reading;
  reading.timestamp = millis();
  
  // Read raw sensor value
  int rawValue = analogRead(sensorPin);
  reading.rawValue = rawValue;
  
  // Convert to voltage
  reading.voltage = rawToVoltage(rawValue);
  
  // Calculate resistance
  reading.resistance = calculateResistance(reading.voltage);
  
  // Calculate PPM
  reading.gasPPM = calculatePPM(reading.resistance);
  
  // Apply corrections (placeholder values)
  reading.temperature = 25.0; // TODO: Add temperature sensor
  reading.humidity = 50.0;    // TODO: Add humidity sensor
  
  reading.gasPPM = applyTemperatureCorrection(reading.gasPPM, reading.temperature);
  reading.gasPPM = applyHumidityCorrection(reading.gasPPM, reading.humidity);
  
  // Validate reading
  reading.isValid = validateReading(reading.gasPPM);
  reading.quality = reading.isValid ? 100 : 0;
  
  // Update health metrics
  if (reading.isValid) {
    updateHealthMetrics(reading.gasPPM);
  }
  
  return reading;
}

SensorReading GasSensor::readSensorFiltered() {
  SensorReading reading = readSensor();
  
  if (reading.isValid) {
    // Add to history
    readingHistory[historyIndex] = reading.gasPPM;
    historyIndex = (historyIndex + 1) % 10;
    if (historyCount < 10) historyCount++;
    
    // Apply moving average filter
    reading.gasPPM = calculateMovingAverage();
    
    // Recalculate quality based on consistency
    float stdDev = calculateStandardDeviation();
    reading.quality = max(0, 100 - (int)(stdDev * 10));
  }
  
  return reading;
}

float GasSensor::getLastReading() {
  if (historyCount > 0) {
    int lastIndex = (historyIndex - 1 + 10) % 10;
    return readingHistory[lastIndex];
  }
  return 0.0;
}

bool GasSensor::calibrate(int samples) {
  Serial.println("Starting sensor calibration...");
  Serial.println("Please ensure sensor is in clean air");
  
  float sum = 0;
  float sumSquared = 0;
  int validSamples = 0;
  
  for (int i = 0; i < samples; i++) {
    int rawValue = analogRead(sensorPin);
    float voltage = rawToVoltage(rawValue);
    float resistance = calculateResistance(voltage);
    
    if (resistance > 0 && resistance < 1000) {
      sum += resistance;
      sumSquared += resistance * resistance;
      validSamples++;
    }
    
    delay(CALIBRATION_INTERVAL);
    
    if (i % 20 == 0) {
      Serial.print("Calibration progress: ");
      Serial.print((i * 100) / samples);
      Serial.println("%");
    }
  }
  
  if (validSamples < samples * 0.8) {
    Serial.println("Calibration failed: Insufficient valid samples");
    return false;
  }
  
  // Calculate R0 and statistics
  calibration.R0 = sum / validSamples;
  float variance = (sumSquared / validSamples) - (calibration.R0 * calibration.R0);
  float standardDeviation = sqrt(variance);
  calibration.confidence = max(0, 100 - (standardDeviation / calibration.R0) * 100);
  
  // Set calibration parameters
  calibration.R0_clean_air = calibration.R0;
  calibration.calibration_date = millis();
  calibration.is_valid = true;
  
  // Save calibration data
  saveCalibration();
  
  Serial.println("Calibration completed successfully");
  Serial.println("R0: " + String(calibration.R0, 2));
  Serial.println("Confidence: " + String(calibration.confidence, 1) + "%");
  
  return true;
}

bool GasSensor::loadCalibration() {
  int calibFlag = EEPROM.read(EEPROM_CALIB_FLAG_ADDR);
  
  if (calibFlag == 0xAA) {
    EEPROM.get(EEPROM_R0_ADDR, calibration.R0);
    EEPROM.get(EEPROM_CALIB_DATE_ADDR, calibration.calibration_date);
    
    if (calibration.R0 > 0 && calibration.R0 < 1000) {
      calibration.is_valid = true;
      calibration.R0_clean_air = calibration.R0;
      calibration.confidence = 95.0; // Assume good confidence for loaded data
      return true;
    }
  }
  
  return false;
}

bool GasSensor::saveCalibration() {
  EEPROM.put(EEPROM_R0_ADDR, calibration.R0);
  EEPROM.put(EEPROM_CALIB_DATE_ADDR, calibration.calibration_date);
  EEPROM.write(EEPROM_CALIB_FLAG_ADDR, 0xAA);
  EEPROM.commit();
  
  return true;
}

bool GasSensor::isCalibrated() {
  return calibration.is_valid;
}

CalibrationData GasSensor::getCalibrationData() {
  return calibration;
}

SensorHealth GasSensor::getHealthStatus() {
  return health;
}

bool GasSensor::isHealthy() {
  return health.isHealthy;
}

void GasSensor::performHealthCheck() {
  // Check sensor response
  SensorReading reading = readSensor();
  
  if (!reading.isValid) {
    health.failureCount++;
    if (health.failureCount > 5) {
      health.isHealthy = false;
    }
  } else {
    health.failureCount = 0;
    health.isHealthy = true;
  }
  
  // Update statistics
  if (reading.isValid) {
    health.totalReadings++;
    health.averageReading = (health.averageReading * (health.totalReadings - 1) + reading.gasPPM) / health.totalReadings;
  }
  
  health.lastMaintenance = millis();
}

void GasSensor::resetHealth() {
  health.isHealthy = true;
  health.failureCount = 0;
  health.totalReadings = 0;
  health.averageReading = 0.0;
  health.standardDeviation = 0.0;
}

void GasSensor::setLoadResistance(float resistance) {
  loadResistance = resistance;
}

void GasSensor::setReferenceVoltage(float voltage) {
  referenceVoltage = voltage;
}

void GasSensor::setADCResolution(int resolution) {
  adcResolution = resolution;
}

float GasSensor::rawToVoltage(int rawValue) {
  return (rawValue * referenceVoltage) / adcResolution;
}

float GasSensor::voltageToResistance(float voltage) {
  return ((referenceVoltage - voltage) * loadResistance) / voltage;
}

float GasSensor::resistanceToPPM(float resistance) {
  if (!calibration.is_valid || calibration.R0 <= 0) {
    return 0.0;
  }
  
  float ratio = resistance / calibration.R0;
  return pow(10, ((log10(ratio) - SENSOR_INTERCEPT) / SENSOR_SLOPE));
}

String GasSensor::getGasType() {
  return String(GAS_TYPE_NAME);
}

int GasSensor::getAlertLevel(float ppm) {
  if (ppm < GAS_SAFE_THRESHOLD) return 0;
  if (ppm < GAS_WARNING_THRESHOLD) return 1;
  if (ppm < GAS_DANGER_THRESHOLD) return 2;
  if (ppm < GAS_CRITICAL_THRESHOLD) return 3;
  return 4;
}

void GasSensor::printCalibrationData() {
  Serial.println("=== Calibration Data ===");
  Serial.println("R0: " + String(calibration.R0, 2));
  Serial.println("R0 Clean Air: " + String(calibration.R0_clean_air, 2));
  Serial.println("Calibration Date: " + String(calibration.calibration_date));
  Serial.println("Is Valid: " + String(calibration.is_valid ? "Yes" : "No"));
  Serial.println("Confidence: " + String(calibration.confidence, 1) + "%");
}

void GasSensor::printHealthStatus() {
  Serial.println("=== Sensor Health ===");
  Serial.println("Is Healthy: " + String(health.isHealthy ? "Yes" : "No"));
  Serial.println("Failure Count: " + String(health.failureCount));
  Serial.println("Total Readings: " + String(health.totalReadings));
  Serial.println("Average Reading: " + String(health.averageReading, 2));
  Serial.println("Standard Deviation: " + String(health.standardDeviation, 2));
}

void GasSensor::printLastReading() {
  SensorReading reading = readSensor();
  Serial.println("=== Last Reading ===");
  Serial.println("Raw Value: " + String(reading.rawValue));
  Serial.println("Voltage: " + String(reading.voltage, 3) + "V");
  Serial.println("Resistance: " + String(reading.resistance, 2) + "kΩ");
  Serial.println("Gas PPM: " + String(reading.gasPPM, 2));
  Serial.println("Is Valid: " + String(reading.isValid ? "Yes" : "No"));
  Serial.println("Quality: " + String(reading.quality) + "%");
}

// Private methods
float GasSensor::calculateResistance(float voltage) {
  return ((referenceVoltage - voltage) * loadResistance) / voltage;
}

float GasSensor::calculatePPM(float resistance) {
  if (!calibration.is_valid || calibration.R0 <= 0) {
    return 0.0;
  }
  
  float ratio = resistance / calibration.R0;
  return pow(10, ((log10(ratio) - SENSOR_INTERCEPT) / SENSOR_SLOPE));
}

float GasSensor::applyTemperatureCorrection(float ppm, float temperature) {
  return ppm * (1.0 + calibration.temperature_coefficient * (temperature - 25.0));
}

float GasSensor::applyHumidityCorrection(float ppm, float humidity) {
  return ppm * (1.0 + calibration.humidity_coefficient * (humidity - 50.0));
}

bool GasSensor::validateReading(float ppm) {
  return (ppm >= 0 && ppm <= 10000);
}

void GasSensor::updateHealthMetrics(float ppm) {
  // Update moving average
  if (historyCount > 0) {
    health.averageReading = (health.averageReading * (historyCount - 1) + ppm) / historyCount;
  } else {
    health.averageReading = ppm;
  }
  
  // Update standard deviation
  health.standardDeviation = calculateStandardDeviation();
}

float GasSensor::calculateMovingAverage() {
  if (historyCount == 0) return 0.0;
  
  float sum = 0;
  for (int i = 0; i < historyCount; i++) {
    sum += readingHistory[i];
  }
  return sum / historyCount;
}

float GasSensor::calculateStandardDeviation() {
  if (historyCount < 2) return 0.0;
  
  float sum = 0;
  float sumSquared = 0;
  
  for (int i = 0; i < historyCount; i++) {
    sum += readingHistory[i];
    sumSquared += readingHistory[i] * readingHistory[i];
  }
  
  float mean = sum / historyCount;
  float variance = (sumSquared / historyCount) - (mean * mean);
  return sqrt(variance);
}

// ============================================================================
// GAS DETECTOR MANAGER CLASS IMPLEMENTATION
// ============================================================================

GasDetectorManager::GasDetectorManager(int count) {
  sensorCount = count;
  activeSensor = 0;
  sensors = new GasSensor[count];
  averagePPM = 0.0;
  maxPPM = 0.0;
  overallAlertLevel = 0;
  allSensorsCalibrated = false;
  lastCalibrationCheck = 0;
}

bool GasDetectorManager::begin() {
  for (int i = 0; i < sensorCount; i++) {
    if (!sensors[i].begin()) {
      return false;
    }
  }
  return true;
}

void GasDetectorManager::addSensor(GasSensor* sensor) {
  // Implementation for adding sensors dynamically
  // This is a simplified version
}

SensorReading GasDetectorManager::readAllSensors() {
  SensorReading combinedReading;
  combinedReading.timestamp = millis();
  combinedReading.gasPPM = 0.0;
  combinedReading.isValid = false;
  
  float sum = 0;
  int validReadings = 0;
  float maxPPM = 0;
  
  for (int i = 0; i < sensorCount; i++) {
    SensorReading reading = sensors[i].readSensor();
    if (reading.isValid) {
      sum += reading.gasPPM;
      validReadings++;
      if (reading.gasPPM > maxPPM) {
        maxPPM = reading.gasPPM;
      }
    }
  }
  
  if (validReadings > 0) {
    combinedReading.gasPPM = sum / validReadings;
    combinedReading.isValid = true;
    this->averagePPM = combinedReading.gasPPM;
    this->maxPPM = maxPPM;
    this->overallAlertLevel = alertManager.processReading(combinedReading.gasPPM);
  }
  
  return combinedReading;
}

SensorReading GasDetectorManager::readActiveSensor() {
  return sensors[activeSensor].readSensor();
}

void GasDetectorManager::switchActiveSensor(int index) {
  if (index >= 0 && index < sensorCount) {
    activeSensor = index;
  }
}

bool GasDetectorManager::calibrateAllSensors() {
  bool allSuccess = true;
  for (int i = 0; i < sensorCount; i++) {
    if (!sensors[i].calibrate()) {
      allSuccess = false;
    }
  }
  allSensorsCalibrated = allSuccess;
  return allSuccess;
}

bool GasDetectorManager::calibrateActiveSensor() {
  bool success = sensors[activeSensor].calibrate();
  if (success) {
    // Check if all sensors are now calibrated
    allSensorsCalibrated = true;
    for (int i = 0; i < sensorCount; i++) {
      if (!sensors[i].isCalibrated()) {
        allSensorsCalibrated = false;
        break;
      }
    }
  }
  return success;
}

bool GasDetectorManager::allCalibrated() {
  return allSensorsCalibrated;
}

bool GasDetectorManager::allHealthy() {
  for (int i = 0; i < sensorCount; i++) {
    if (!sensors[i].isHealthy()) {
      return false;
    }
  }
  return true;
}

void GasDetectorManager::performHealthCheck() {
  for (int i = 0; i < sensorCount; i++) {
    sensors[i].performHealthCheck();
  }
}

float GasDetectorManager::getAveragePPM() {
  return averagePPM;
}

float GasDetectorManager::getMaxPPM() {
  return maxPPM;
}

int GasDetectorManager::getOverallAlertLevel() {
  return overallAlertLevel;
}

void GasDetectorManager::setActiveSensor(int index) {
  switchActiveSensor(index);
}

int GasDetectorManager::getActiveSensor() {
  return activeSensor;
}

int GasDetectorManager::getSensorCount() {
  return sensorCount;
}

void GasDetectorManager::printSystemStatus() {
  Serial.println("=== Gas Detector System Status ===");
  Serial.println("Sensor Count: " + String(sensorCount));
  Serial.println("Active Sensor: " + String(activeSensor));
  Serial.println("All Calibrated: " + String(allSensorsCalibrated ? "Yes" : "No"));
  Serial.println("All Healthy: " + String(allHealthy() ? "Yes" : "No"));
  Serial.println("Average PPM: " + String(averagePPM, 2));
  Serial.println("Max PPM: " + String(maxPPM, 2));
  Serial.println("Overall Alert Level: " + String(overallAlertLevel));
}

void GasDetectorManager::printAllReadings() {
  for (int i = 0; i < sensorCount; i++) {
    Serial.println("=== Sensor " + String(i) + " ===");
    sensors[i].printLastReading();
  }
}

// ============================================================================
// GAS ALERT MANAGER CLASS IMPLEMENTATION
// ============================================================================

GasAlertManager::GasAlertManager() {
  safeThreshold = GAS_SAFE_THRESHOLD;
  warningThreshold = GAS_WARNING_THRESHOLD;
  dangerThreshold = GAS_DANGER_THRESHOLD;
  criticalThreshold = GAS_CRITICAL_THRESHOLD;
  
  currentLevel = 0;
  previousLevel = 0;
  alertStartTime = 0;
  alertActive = false;
  
  alertIndex = 0;
  alertCount = 0;
  
  lastBlinkTime = 0;
  lastBeepTime = 0;
  ledState = false;
  buzzerState = false;
}

void GasAlertManager::setThresholds(float safe, float warning, float danger, float critical) {
  safeThreshold = safe;
  warningThreshold = warning;
  dangerThreshold = danger;
  criticalThreshold = critical;
}

void GasAlertManager::setSafeThreshold(float threshold) {
  safeThreshold = threshold;
}

void GasAlertManager::setWarningThreshold(float threshold) {
  warningThreshold = threshold;
}

void GasAlertManager::setDangerThreshold(float threshold) {
  dangerThreshold = threshold;
}

void GasAlertManager::setCriticalThreshold(float threshold) {
  criticalThreshold = threshold;
}

int GasAlertManager::processReading(float ppm) {
  int newLevel = calculateAlertLevel(ppm, safeThreshold, warningThreshold, dangerThreshold, criticalThreshold);
  
  if (newLevel != currentLevel) {
    previousLevel = currentLevel;
    currentLevel = newLevel;
    updateAlertState(newLevel);
  }
  
  return currentLevel;
}

void GasAlertManager::updateAlertState(int level) {
  if (level > 0) {
    if (!alertActive) {
      alertActive = true;
      alertStartTime = millis();
    }
    logAlert(level, 0.0, getAlertDescription(level));
  } else {
    alertActive = false;
    alertStartTime = 0;
  }
}

void GasAlertManager::clearAlerts() {
  if (alertActive) {
    alertActive = false;
    previousLevel = currentLevel;
    currentLevel = 0;
    alertStartTime = 0;
    
    // Turn off all indicators
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void GasAlertManager::handleAlerts() {
  if (!alertActive) return;
  
  updateVisualIndicators();
  updateAudioIndicators();
}

void GasAlertManager::updateVisualIndicators() {
  unsigned long currentTime = millis();
  
  switch (currentLevel) {
    case 1: // Warning
      if (currentTime - lastBlinkTime > WARNING_BLINK_INTERVAL) {
        ledState = !ledState;
        digitalWrite(LED_YELLOW, ledState);
        lastBlinkTime = currentTime;
      }
      break;
      
    case 2: // Danger
      if (currentTime - lastBlinkTime > DANGER_BLINK_INTERVAL) {
        ledState = !ledState;
        digitalWrite(LED_RED, ledState);
        lastBlinkTime = currentTime;
      }
      break;
      
    case 3: // Critical
      if (currentTime - lastBlinkTime > CRITICAL_BLINK_INTERVAL) {
        ledState = !ledState;
        digitalWrite(LED_RED, ledState);
        lastBlinkTime = currentTime;
      }
      break;
      
    case 4: // Emergency
      digitalWrite(LED_RED, HIGH);
      break;
  }
}

void GasAlertManager::updateAudioIndicators() {
  unsigned long currentTime = millis();
  
  switch (currentLevel) {
    case 2: // Danger
      if (currentTime - lastBeepTime > BUZZER_BEEP_INTERVAL) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(BUZZER_BEEP_DURATION);
        digitalWrite(BUZZER_PIN, LOW);
        lastBeepTime = currentTime;
      }
      break;
      
    case 3: // Critical
      if (currentTime - lastBeepTime > BUZZER_BEEP_INTERVAL) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(BUZZER_BEEP_DURATION);
        digitalWrite(BUZZER_PIN, LOW);
        lastBeepTime = currentTime;
      }
      break;
      
    case 4: // Emergency
      digitalWrite(BUZZER_PIN, HIGH);
      break;
  }
}

void GasAlertManager::logAlert(int level, float ppm, String description) {
  AlertRecord alert;
  alert.timestamp = millis();
  alert.level = level;
  alert.ppm = ppm;
  alert.description = description;
  alert.acknowledged = false;
  
  alertHistory[alertIndex] = alert;
  alertIndex = (alertIndex + 1) % 50;
  if (alertCount < 50) alertCount++;
}

AlertRecord* GasAlertManager::getAlertHistory() {
  return alertHistory;
}

int GasAlertManager::getAlertCount() {
  return alertCount;
}

void GasAlertManager::acknowledgeAlert(int index) {
  if (index >= 0 && index < alertCount) {
    alertHistory[index].acknowledged = true;
  }
}

bool GasAlertManager::isAlertActive() {
  return alertActive;
}

int GasAlertManager::getCurrentLevel() {
  return currentLevel;
}

int GasAlertManager::getPreviousLevel() {
  return previousLevel;
}

unsigned long GasAlertManager::getAlertDuration() {
  if (alertActive) {
    return millis() - alertStartTime;
  }
  return 0;
}

String GasAlertManager::getAlertDescription(int level) {
  switch (level) {
    case 0: return "Safe";
    case 1: return "Warning";
    case 2: return "Danger";
    case 3: return "Critical";
    case 4: return "Emergency";
    default: return "Unknown";
  }
}

void GasAlertManager::printAlertStatus() {
  Serial.println("=== Alert Status ===");
  Serial.println("Active: " + String(alertActive ? "Yes" : "No"));
  Serial.println("Current Level: " + String(currentLevel));
  Serial.println("Previous Level: " + String(previousLevel));
  Serial.println("Duration: " + String(getAlertDuration()) + "ms");
}

void GasAlertManager::printAlertHistory() {
  Serial.println("=== Alert History ===");
  for (int i = 0; i < min(alertCount, 10); i++) {
    int index = (alertIndex - 1 - i + 50) % 50;
    AlertRecord alert = alertHistory[index];
    Serial.println("Level " + String(alert.level) + ": " + alert.description + 
                   " (" + String(alert.timestamp) + ")");
  }
}

// ============================================================================
// UTILITY FUNCTIONS IMPLEMENTATION
// ============================================================================

String detectGasType(float ppm, float temperature, float humidity) {
  // Simple gas type detection based on PPM levels and environmental conditions
  // This is a placeholder implementation
  return String(GAS_TYPE_NAME);
}

float rawToPPM(int rawValue, float R0, float temperature, float humidity) {
  float voltage = (rawValue * REFERENCE_VOLTAGE) / ADC_RESOLUTION;
  float resistance = ((REFERENCE_VOLTAGE - voltage) * MQ6_LOAD_RESISTANCE) / voltage;
  
  if (R0 <= 0) return 0.0;
  
  float ratio = resistance / R0;
  float ppm = pow(10, ((log10(ratio) - SENSOR_INTERCEPT) / SENSOR_SLOPE));
  
  // Apply corrections
  ppm = applyTemperatureCorrection(ppm, temperature, 25.0);
  ppm = applyHumidityCorrection(ppm, humidity, 50.0);
  
  return ppm;
}

float voltageToPPM(float voltage, float R0, float temperature, float humidity) {
  float resistance = ((REFERENCE_VOLTAGE - voltage) * MQ6_LOAD_RESISTANCE) / voltage;
  return resistanceToPPM(resistance, R0, temperature, humidity);
}

float resistanceToPPM(float resistance, float R0, float temperature, float humidity) {
  if (R0 <= 0) return 0.0;
  
  float ratio = resistance / R0;
  float ppm = pow(10, ((log10(ratio) - SENSOR_INTERCEPT) / SENSOR_SLOPE));
  
  // Apply corrections
  ppm = applyTemperatureCorrection(ppm, temperature, 25.0);
  ppm = applyHumidityCorrection(ppm, humidity, 50.0);
  
  return ppm;
}

bool validateSensorReading(float ppm, float temperature, float humidity) {
  return (ppm >= 0 && ppm <= 10000 && 
          temperature >= -10 && temperature <= 60 &&
          humidity >= 0 && humidity <= 100);
}

bool isReasonableReading(float ppm) {
  return (ppm >= 0 && ppm <= 10000);
}

float calculateR0(float* readings, int count) {
  if (count <= 0) return 0.0;
  
  float sum = 0;
  for (int i = 0; i < count; i++) {
    sum += readings[i];
  }
  return sum / count;
}

float calculateCalibrationConfidence(float* readings, int count) {
  if (count < 2) return 0.0;
  
  float sum = 0;
  float sumSquared = 0;
  
  for (int i = 0; i < count; i++) {
    sum += readings[i];
    sumSquared += readings[i] * readings[i];
  }
  
  float mean = sum / count;
  float variance = (sumSquared / count) - (mean * mean);
  float standardDeviation = sqrt(variance);
  float coefficientOfVariation = standardDeviation / mean;
  
  return max(0, 100 - coefficientOfVariation * 100);
}

bool isCalibrationValid(float R0, float confidence) {
  return (R0 > 0 && R0 < 1000 && confidence > 80);
}

int calculateAlertLevel(float ppm, float safe, float warning, float danger, float critical) {
  if (ppm < safe) return 0;
  if (ppm < warning) return 1;
  if (ppm < danger) return 2;
  if (ppm < critical) return 3;
  return 4;
}

String getAlertLevelName(int level) {
  switch (level) {
    case 0: return "Safe";
    case 1: return "Warning";
    case 2: return "Danger";
    case 3: return "Critical";
    case 4: return "Emergency";
    default: return "Unknown";
  }
}

float applyLowPassFilter(float newValue, float oldValue, float alpha) {
  return alpha * newValue + (1 - alpha) * oldValue;
}

float applyMedianFilter(float* values, int count) {
  if (count <= 0) return 0.0;
  
  // Simple bubble sort for median
  for (int i = 0; i < count - 1; i++) {
    for (int j = 0; j < count - i - 1; j++) {
      if (values[j] > values[j + 1]) {
        float temp = values[j];
        values[j] = values[j + 1];
        values[j + 1] = temp;
      }
    }
  }
  
  if (count % 2 == 0) {
    return (values[count / 2 - 1] + values[count / 2]) / 2;
  } else {
    return values[count / 2];
  }
}

float applyMovingAverage(float* values, int count) {
  if (count <= 0) return 0.0;
  
  float sum = 0;
  for (int i = 0; i < count; i++) {
    sum += values[i];
  }
  return sum / count;
}

float applyTemperatureCorrection(float ppm, float temperature, float referenceTemp) {
  return ppm * (1.0 + 0.02 * (temperature - referenceTemp));
}

float applyHumidityCorrection(float ppm, float humidity, float referenceHumidity) {
  return ppm * (1.0 + 0.01 * (humidity - referenceHumidity));
}

bool isSafeLevel(float ppm) {
  return ppm < GAS_SAFE_THRESHOLD;
}

bool isWarningLevel(float ppm) {
  return ppm >= GAS_SAFE_THRESHOLD && ppm < GAS_WARNING_THRESHOLD;
}

bool isDangerLevel(float ppm) {
  return ppm >= GAS_WARNING_THRESHOLD && ppm < GAS_DANGER_THRESHOLD;
}

bool isCriticalLevel(float ppm) {
  return ppm >= GAS_DANGER_THRESHOLD;
}

void printSensorDiagnostics() {
  gasSensor.printLastReading();
  gasSensor.printCalibrationData();
  gasSensor.printHealthStatus();
}

void printCalibrationReport() {
  Serial.println("=== Calibration Report ===");
  CalibrationData calib = gasSensor.getCalibrationData();
  Serial.println("R0: " + String(calib.R0, 2));
  Serial.println("Confidence: " + String(calib.confidence, 1) + "%");
  Serial.println("Date: " + String(calib.calibration_date));
  Serial.println("Valid: " + String(calib.is_valid ? "Yes" : "No"));
}

void printAlertReport() {
  alertManager.printAlertStatus();
  alertManager.printAlertHistory();
}


