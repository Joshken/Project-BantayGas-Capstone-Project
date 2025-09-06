/*
 * Hardware Configuration for BantayGas IoT System
 * ESP32 + MQ-6 Gas Detector
 * 
 * Pin assignments and hardware-specific settings
 */

#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

// ============================================================================
// PIN CONFIGURATION
// ============================================================================

// Gas Sensor Pin
#define MQ6_PIN             34    // Analog input (AOUT of MQ-6)

// LED Pins
#define LED_GREEN           21    // Green LED for safe levels
#define LED_YELLOW          22    // Yellow LED for warning levels  
#define LED_RED             23    // Red LED for danger levels

// Buzzer Pin
#define BUZZER_PIN          19    // Buzzer for audio alerts

// Optional Components (uncomment if used)
// #define OLED_SDA_PIN       21    // OLED Display SDA
// #define OLED_SCL_PIN       22    // OLED Display SCL
// #define BUTTON_PIN         0     // Calibration button
// #define RELAY_PIN          25    // Ventilation relay

// ============================================================================
// HARDWARE SPECIFICATIONS
// ============================================================================

// ADC Configuration
#define ADC_RESOLUTION      4095  // ESP32 ADC resolution (12-bit)
#define REFERENCE_VOLTAGE   3.3   // ESP32 reference voltage (V)
#define ADC_MAX_VOLTAGE     3.3   // Maximum ADC input voltage

// MQ-6 Sensor Specifications
#define MQ6_LOAD_RESISTANCE 10.0  // Load resistance in kOhm
#define MQ6_CLEAN_AIR_RATIO 10.0  // Rs/R0 ratio in clean air
#define MQ6_HEATER_CURRENT  0.15  // Heater current in Amps
#define MQ6_WARMUP_TIME     30000 // Warm-up time in milliseconds (30s)

// LED Specifications
#define LED_BRIGHTNESS      255   // LED brightness (0-255)
#define LED_BLINK_INTERVAL  500   // LED blink interval in ms

// Buzzer Specifications
#define BUZZER_FREQUENCY    2000  // Buzzer frequency in Hz
#define BUZZER_DUTY_CYCLE   50    // Buzzer duty cycle (%)

// ============================================================================
// TIMING CONFIGURATION
// ============================================================================

#define READING_INTERVAL    500   // Gas reading interval in ms
#define CALIBRATION_TIME    60000 // Calibration time in ms (60s)
#define ALARM_DURATION      5000  // Alarm duration in ms (5s)
#define BLINK_INTERVAL      200   // LED blink interval in ms

// ============================================================================
// SAFETY LIMITS
// ============================================================================

#define MAX_CONTINUOUS_ALARM 300000  // Max continuous alarm time (5 min)
#define EMERGENCY_SHUTDOWN   2000    // Emergency shutdown threshold
#define SENSOR_TIMEOUT       10000   // Sensor timeout in ms

// ============================================================================
// DEBUGGING AND TESTING
// ============================================================================

#define ENABLE_LED_TEST      true   // Enable LED test on startup
#define ENABLE_BUZZER_TEST   true   // Enable buzzer test on startup
#define ENABLE_SENSOR_TEST   true   // Enable sensor test on startup
#define ENABLE_SERIAL_DEBUG  true   // Enable serial debug output

// ============================================================================
// PIN VALIDATION
// ============================================================================

// Validate pin assignments
#if MQ6_PIN < 0 || MQ6_PIN > 39
  #error "Invalid MQ6_PIN. Must be 0-39 for ESP32"
#endif

#if LED_GREEN < 0 || LED_GREEN > 39
  #error "Invalid LED_GREEN pin. Must be 0-39 for ESP32"
#endif

#if LED_YELLOW < 0 || LED_YELLOW > 39
  #error "Invalid LED_YELLOW pin. Must be 0-39 for ESP32"
#endif

#if LED_RED < 0 || LED_RED > 39
  #error "Invalid LED_RED pin. Must be 0-39 for ESP32"
#endif

#if BUZZER_PIN < 0 || BUZZER_PIN > 39
  #error "Invalid BUZZER_PIN. Must be 0-39 for ESP32"
#endif

// Check for pin conflicts
#if (LED_GREEN == LED_YELLOW) || (LED_GREEN == LED_RED) || (LED_YELLOW == LED_RED)
  #error "LED pins must be different"
#endif

#if (MQ6_PIN == LED_GREEN) || (MQ6_PIN == LED_YELLOW) || (MQ6_PIN == LED_RED) || (MQ6_PIN == BUZZER_PIN)
  #error "MQ6_PIN conflicts with other pins"
#endif

#endif // HARDWARE_CONFIG_H


