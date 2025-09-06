/*
 * WiFi Configuration for BantayGas IoT System
 * 
 * WiFi connectivity settings and network configuration
 */

#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

// ============================================================================
// WIFI ENABLE/DISABLE
// ============================================================================

#define ENABLE_WIFI           true   // Enable WiFi functionality
#define ENABLE_WIFI_AP_MODE   true   // Enable Access Point mode as fallback
#define ENABLE_OTA_UPDATES    true   // Enable Over-The-Air updates

// ============================================================================
// WIFI CREDENTIALS
// ============================================================================

// Primary WiFi Network
#define WIFI_SSID            "BantayGas_Network"
#define WIFI_PASSWORD        "BantayGas2024!"

// Backup WiFi Networks (up to 3)
#define WIFI_SSID_2          "BantayGas_Backup1"
#define WIFI_PASSWORD_2      "Backup2024!"

#define WIFI_SSID_3          "BantayGas_Backup2"
#define WIFI_PASSWORD_3      "Backup2024!"

// ============================================================================
// ACCESS POINT CONFIGURATION
// ============================================================================

// AP Mode settings (when WiFi connection fails)
#define AP_SSID              "BantayGas_Setup"
#define AP_PASSWORD          "setup123"
#define AP_CHANNEL           1
#define AP_MAX_CONNECTIONS   4

// ============================================================================
// SERVER CONFIGURATION
// ============================================================================

// Main server settings
#define SERVER_HOST          "192.168.1.100"  // Your server IP
#define SERVER_PORT          8080
#define SERVER_URL           "http://192.168.1.100:8080/api"
#define SERVER_SSL           false

// API endpoints
#define API_ENDPOINT_DATA    "/gas-data"
#define API_ENDPOINT_ALERT   "/alerts"
#define API_ENDPOINT_STATUS  "/status"
#define API_ENDPOINT_CONFIG  "/config"

// ============================================================================
// NETWORK TIMEOUTS
// ============================================================================

#define WIFI_CONNECT_TIMEOUT  30000  // WiFi connection timeout (30s)
#define WIFI_RECONNECT_DELAY  5000   // Reconnection delay (5s)
#define HTTP_TIMEOUT          10000  // HTTP request timeout (10s)
#define PING_TIMEOUT          5000   // Ping timeout (5s)

// ============================================================================
// DATA TRANSMISSION
// ============================================================================

#define TRANSMIT_INTERVAL     30000  // Data transmission interval (30s)
#define MAX_RETRY_ATTEMPTS    3      // Maximum retry attempts
#define RETRY_DELAY           2000   // Delay between retries (2s)

// ============================================================================
// SECURITY SETTINGS
// ============================================================================

#define ENABLE_WPA2           true   // Enable WPA2 security
#define ENABLE_WPS            false  // Enable WPS (not recommended)
#define ENABLE_MAC_FILTERING  false  // Enable MAC address filtering

// Device identification
#define DEVICE_ID             "BantayGas_001"
#define DEVICE_NAME           "BantayGas Gas Detector"
#define DEVICE_VERSION        "1.0.0"

// ============================================================================
// MQTT CONFIGURATION (Optional)
// ============================================================================

#define ENABLE_MQTT           false  // Enable MQTT support
#define MQTT_BROKER           "192.168.1.100"
#define MQTT_PORT             1883
#define MQTT_USERNAME         "bantaygas"
#define MQTT_PASSWORD         "mqtt2024"
#define MQTT_TOPIC_DATA       "bantaygas/data"
#define MQTT_TOPIC_ALERT      "bantaygas/alerts"
#define MQTT_TOPIC_STATUS     "bantaygas/status"

// ============================================================================
// NTP CONFIGURATION
// ============================================================================

#define ENABLE_NTP            true   // Enable NTP time sync
#define NTP_SERVER_1          "pool.ntp.org"
#define NTP_SERVER_2          "time.nist.gov"
#define NTP_SERVER_3          "time.google.com"
#define TIMEZONE_OFFSET       8      // UTC+8 (Philippines)

// ============================================================================
// DEBUGGING
// ============================================================================

#define ENABLE_WIFI_DEBUG     true   // Enable WiFi debug output
#define ENABLE_HTTP_DEBUG     true   // Enable HTTP debug output
#define ENABLE_MQTT_DEBUG     false  // Enable MQTT debug output

// ============================================================================
// VALIDATION
// ============================================================================

// Validate WiFi settings
#if ENABLE_WIFI && (strlen(WIFI_SSID) == 0)
  #error "WIFI_SSID cannot be empty when WiFi is enabled"
#endif

#if ENABLE_WIFI && (strlen(WIFI_PASSWORD) < 8)
  #warning "WIFI_PASSWORD should be at least 8 characters for security"
#endif

// Validate server settings
#if ENABLE_WIFI && (strlen(SERVER_HOST) == 0)
  #error "SERVER_HOST cannot be empty when WiFi is enabled"
#endif

#if SERVER_PORT < 1 || SERVER_PORT > 65535
  #error "SERVER_PORT must be between 1 and 65535"
#endif

// Validate timeouts
#if WIFI_CONNECT_TIMEOUT < 5000
  #error "WIFI_CONNECT_TIMEOUT should be at least 5000ms"
#endif

#if HTTP_TIMEOUT < 1000
  #error "HTTP_TIMEOUT should be at least 1000ms"
#endif

#endif // WIFI_CONFIG_H


