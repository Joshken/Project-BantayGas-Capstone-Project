/*
 * Gas Detection Configuration for BantayGas IoT System
 * 
 * Gas type selection, thresholds, and calibration settings
 */

#ifndef GAS_CONFIG_H
#define GAS_CONFIG_H

#include "hardware_config.h"

// ============================================================================
// GAS TYPE SELECTION
// ============================================================================

// Uncomment the gas type you want to detect
#define GAS_TYPE_LPG
// #define GAS_TYPE_BUTANE
// #define GAS_TYPE_METHANE
// #define GAS_TYPE_PROPANE
// #define GAS_TYPE_HYDROGEN

// ============================================================================
// GAS DETECTION THRESHOLDS (in PPM - Parts Per Million)
// ============================================================================

#ifdef GAS_TYPE_LPG
  // LPG (Liquefied Petroleum Gas) thresholds
  #define GAS_SAFE_THRESHOLD     200    // Below this: Safe (Green LED)
  #define GAS_WARNING_THRESHOLD  500    // 200-500: Warning (Yellow LED)
  #define GAS_DANGER_THRESHOLD   800    // 500-800: Danger (Red LED + Buzzer)
  #define GAS_CRITICAL_THRESHOLD 1000   // Above 800: Critical (Red LED + Continuous Buzzer)
  #define GAS_TYPE_NAME "LPG"
  #define GAS_MOLECULAR_WEIGHT 44.1
  #define GAS_DENSITY 2.01  // kg/m³ at STP
#endif

#ifdef GAS_TYPE_BUTANE
  // Butane thresholds
  #define GAS_SAFE_THRESHOLD     150
  #define GAS_WARNING_THRESHOLD  400
  #define GAS_DANGER_THRESHOLD   700
  #define GAS_CRITICAL_THRESHOLD 900
  #define GAS_TYPE_NAME "Butane"
  #define GAS_MOLECULAR_WEIGHT 58.12
  #define GAS_DENSITY 2.48  // kg/m³ at STP
#endif

#ifdef GAS_TYPE_METHANE
  // Methane thresholds
  #define GAS_SAFE_THRESHOLD     100
  #define GAS_WARNING_THRESHOLD  300
  #define GAS_DANGER_THRESHOLD   600
  #define GAS_CRITICAL_THRESHOLD 800
  #define GAS_TYPE_NAME "Methane"
  #define GAS_MOLECULAR_WEIGHT 16.04
  #define GAS_DENSITY 0.717  // kg/m³ at STP
#endif

#ifdef GAS_TYPE_PROPANE
  // Propane thresholds
  #define GAS_SAFE_THRESHOLD     180
  #define GAS_WARNING_THRESHOLD  450
  #define GAS_DANGER_THRESHOLD   750
  #define GAS_CRITICAL_THRESHOLD 950
  #define GAS_TYPE_NAME "Propane"
  #define GAS_MOLECULAR_WEIGHT 44.1
  #define GAS_DENSITY 1.88  // kg/m³ at STP
#endif

#ifdef GAS_TYPE_HYDROGEN
  // Hydrogen thresholds
  #define GAS_SAFE_THRESHOLD     50
  #define GAS_WARNING_THRESHOLD  200
  #define GAS_DANGER_THRESHOLD   400
  #define GAS_CRITICAL_THRESHOLD 600
  #define GAS_TYPE_NAME "Hydrogen"
  #define GAS_MOLECULAR_WEIGHT 2.016
  #define GAS_DENSITY 0.0899  // kg/m³ at STP
#endif

// ============================================================================
// SENSOR CALIBRATION
// ============================================================================

// Calibration settings
#define CALIBRATION_SAMPLES    100    // Number of samples for calibration
#define CALIBRATION_INTERVAL   100    // Interval between calibration samples (ms)
#define R0_CLEAN_AIR_RATIO     10.0   // Rs/R0 ratio in clean air
#define CALIBRATION_TOLERANCE  0.1    // Calibration tolerance (10%)

// EEPROM addresses for storing calibration data
#define EEPROM_R0_ADDR         0      // R0 value storage
#define EEPROM_CALIB_FLAG_ADDR 4      // Calibration flag
#define EEPROM_CALIB_DATE_ADDR 8      // Calibration date

// ============================================================================
// SENSOR CHARACTERISTICS
// ============================================================================

// MQ-6 Sensor characteristics for different gases
#ifdef GAS_TYPE_LPG
  #define SENSOR_SLOPE         -0.318
  #define SENSOR_INTERCEPT     1.133
  #define SENSOR_RS_RO_RATIO   10.0
#endif

#ifdef GAS_TYPE_BUTANE
  #define SENSOR_SLOPE         -0.318
  #define SENSOR_INTERCEPT     1.133
  #define SENSOR_RS_RO_RATIO   10.0
#endif

#ifdef GAS_TYPE_METHANE
  #define SENSOR_SLOPE         -0.318
  #define SENSOR_INTERCEPT     1.133
  #define SENSOR_RS_RO_RATIO   10.0
#endif

// ============================================================================
// ALERT CONFIGURATION
// ============================================================================

// Alert behavior
#define ENABLE_AUDIO_ALERTS    true   // Enable buzzer alerts
#define ENABLE_VISUAL_ALERTS   true   // Enable LED alerts
#define ENABLE_SERIAL_ALERTS   true   // Enable serial alerts

// Alert patterns
#define ALERT_PATTERN_SAFE     0      // No alert
#define ALERT_PATTERN_WARNING  1      // Slow blink
#define ALERT_PATTERN_DANGER   2      // Fast blink + beep
#define ALERT_PATTERN_CRITICAL 3      // Continuous + alarm

// Alert timing
#define WARNING_BLINK_INTERVAL 1000   // Warning blink interval (ms)
#define DANGER_BLINK_INTERVAL  500    // Danger blink interval (ms)
#define CRITICAL_BLINK_INTERVAL 200   // Critical blink interval (ms)
#define BUZZER_BEEP_DURATION   200    // Buzzer beep duration (ms)
#define BUZZER_BEEP_INTERVAL   1000   // Buzzer beep interval (ms)

// ============================================================================
// DATA LOGGING
// ============================================================================

#define ENABLE_DATA_LOGGING    true   // Enable data logging
#define LOG_INTERVAL           1000   // Log interval in ms
#define MAX_LOG_ENTRIES        1000   // Maximum log entries
#define LOG_RETENTION_DAYS     7      // Log retention period

// ============================================================================
// VALIDATION
// ============================================================================

// Validate threshold values
#if GAS_SAFE_THRESHOLD >= GAS_WARNING_THRESHOLD
  #error "GAS_SAFE_THRESHOLD must be less than GAS_WARNING_THRESHOLD"
#endif

#if GAS_WARNING_THRESHOLD >= GAS_DANGER_THRESHOLD
  #error "GAS_WARNING_THRESHOLD must be less than GAS_DANGER_THRESHOLD"
#endif

#if GAS_DANGER_THRESHOLD >= GAS_CRITICAL_THRESHOLD
  #error "GAS_DANGER_THRESHOLD must be less than GAS_CRITICAL_THRESHOLD"
#endif

// Validate calibration settings
#if CALIBRATION_SAMPLES < 10
  #error "CALIBRATION_SAMPLES must be at least 10"
#endif

#if CALIBRATION_TOLERANCE <= 0 || CALIBRATION_TOLERANCE > 1
  #error "CALIBRATION_TOLERANCE must be between 0 and 1"
#endif

#endif // GAS_CONFIG_H


