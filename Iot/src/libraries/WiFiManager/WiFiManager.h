/*
 * WiFiManager Library for BantayGas IoT System
 * 
 * Provides comprehensive WiFi connectivity management including:
 * - Multiple network support with automatic failover
 * - Access Point mode for configuration
 * - Data transmission and remote monitoring
 * - OTA updates and remote configuration
 * - Connection health monitoring
 * 
 * Author: BantayGas Development Team
 * Version: 1.0.0
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <time.h>
#include "wifi_config.h"

// ============================================================================
// DATA STRUCTURES
// ============================================================================

struct WiFiNetwork {
  String ssid;
  String password;
  int rssi;
  bool isConnected;
  unsigned long lastConnected;
  int connectionAttempts;
  bool isAvailable;
};

struct ConnectionStatus {
  bool isConnected;
  String currentSSID;
  IPAddress localIP;
  IPAddress gateway;
  IPAddress subnet;
  int rssi;
  unsigned long connectionTime;
  unsigned long lastDataTransmission;
  int transmissionFailures;
  bool isStable;
};

struct TransmissionData {
  String deviceId;
  unsigned long timestamp;
  float gasPPM;
  int alertLevel;
  String gasType;
  float temperature;
  float humidity;
  bool sensorHealthy;
  bool isCalibrated;
  int errorCount;
  unsigned long uptime;
};

// ============================================================================
// WIFI MANAGER CLASS
// ============================================================================

class WiFiManager {
private:
  // Network configuration
  WiFiNetwork networks[3];
  int networkCount;
  int currentNetwork;
  
  // Connection status
  ConnectionStatus status;
  
  // Web server for configuration
  WebServer* webServer;
  DNSServer* dnsServer;
  
  // Transmission settings
  String serverURL;
  String deviceId;
  unsigned long lastTransmission;
  unsigned long transmissionInterval;
  int maxRetryAttempts;
  
  // Health monitoring
  unsigned long lastHealthCheck;
  unsigned long lastReconnectAttempt;
  int consecutiveFailures;
  bool autoReconnect;
  
  // Private methods
  bool connectToNetwork(int networkIndex);
  void scanNetworks();
  void updateConnectionStatus();
  bool isNetworkAvailable(String ssid);
  void handleConnectionFailure();
  void startAPMode();
  void stopAPMode();
  bool performHealthCheck();
  void logConnectionEvent(String event);

public:
  // Constructor
  WiFiManager();
  
  // Initialization
  bool begin();
  void end();
  
  // Network management
  bool addNetwork(String ssid, String password);
  bool removeNetwork(String ssid);
  void setPrimaryNetwork(String ssid, String password);
  void setBackupNetworks(String ssid2, String password2, String ssid3, String password3);
  
  // Connection control
  bool connect();
  bool connect(String ssid, String password);
  void disconnect();
  bool reconnect();
  void enableAutoReconnect(bool enable);
  
  // Status and information
  ConnectionStatus getStatus();
  bool isConnected();
  String getCurrentSSID();
  IPAddress getLocalIP();
  int getRSSI();
  unsigned long getConnectionTime();
  bool isStable();
  
  // Data transmission
  bool transmitData(TransmissionData data);
  bool transmitAlert(int level, float ppm, String description);
  bool transmitSystemStatus();
  void setTransmissionInterval(unsigned long interval);
  void setServerURL(String url);
  void setDeviceId(String id);
  
  // Web server management
  void startWebServer();
  void stopWebServer();
  void handleWebRequests();
  void setWebServerHandlers();
  
  // OTA updates
  bool checkForUpdates();
  bool performOTAUpdate(String updateURL);
  void enableOTA(bool enable);
  
  // Configuration
  void loadConfiguration();
  void saveConfiguration();
  void resetConfiguration();
  
  // Health monitoring
  void performHealthCheck();
  bool isHealthy();
  int getConsecutiveFailures();
  void resetFailureCount();
  
  // Utility functions
  void printStatus();
  void printNetworks();
  void printConnectionHistory();
  void scanAndPrintNetworks();
  
  // Event handlers
  void onConnect(void (*callback)());
  void onDisconnect(void (*callback)());
  void onTransmissionSuccess(void (*callback)());
  void onTransmissionFailure(void (*callback)());
  
private:
  // Event callbacks
  void (*onConnectCallback)();
  void (*onDisconnectCallback)();
  void (*onTransmissionSuccessCallback)();
  void (*onTransmissionFailureCallback)();
};

// ============================================================================
// DATA TRANSMITTER CLASS
// ============================================================================

class DataTransmitter {
private:
  WiFiManager* wifiManager;
  String serverURL;
  String deviceId;
  unsigned long lastTransmission;
  unsigned long transmissionInterval;
  int maxRetryAttempts;
  int consecutiveFailures;
  
  // Data buffering
  struct DataBuffer {
    TransmissionData data;
    unsigned long timestamp;
    bool transmitted;
    int retryCount;
  };
  
  DataBuffer dataBuffer[50];
  int bufferIndex;
  int bufferCount;
  
  // Private methods
  bool sendHTTPRequest(String endpoint, String payload);
  void addToBuffer(TransmissionData data);
  void processBuffer();
  bool isBufferFull();
  void clearBuffer();

public:
  // Constructor
  DataTransmitter(WiFiManager* wifi);
  
  // Configuration
  void setServerURL(String url);
  void setDeviceId(String id);
  void setTransmissionInterval(unsigned long interval);
  void setMaxRetryAttempts(int attempts);
  
  // Data transmission
  bool transmitData(TransmissionData data);
  bool transmitAlert(int level, float ppm, String description);
  bool transmitSystemStatus(bool operational, bool calibrated, int errorCount);
  bool transmitCalibrationData(float R0, float confidence);
  
  // Buffer management
  void processPendingTransmissions();
  int getPendingCount();
  void clearPendingTransmissions();
  
  // Status
  bool isTransmitting();
  int getConsecutiveFailures();
  void resetFailureCount();
  
  // Utility
  void printTransmissionStatus();
  void printBufferStatus();
};

// ============================================================================
// REMOTE CONFIGURATION CLASS
// ============================================================================

class RemoteConfiguration {
private:
  WiFiManager* wifiManager;
  String configURL;
  String deviceId;
  
  // Configuration data
  struct ConfigData {
    float safeThreshold;
    float warningThreshold;
    float dangerThreshold;
    float criticalThreshold;
    unsigned long readingInterval;
    unsigned long transmissionInterval;
    bool enableAlerts;
    bool enableDataLogging;
    String gasType;
    bool autoCalibration;
  };
  
  ConfigData currentConfig;
  ConfigData defaultConfig;
  unsigned long lastConfigUpdate;
  bool configLoaded;
  
  // Private methods
  bool downloadConfiguration();
  bool parseConfiguration(String jsonData);
  void applyConfiguration();
  void saveConfigurationToEEPROM();
  void loadConfigurationFromEEPROM();

public:
  // Constructor
  RemoteConfiguration(WiFiManager* wifi);
  
  // Configuration management
  bool loadConfiguration();
  bool updateConfiguration();
  void setConfigURL(String url);
  void setDeviceId(String id);
  
  // Configuration access
  ConfigData getConfiguration();
  float getSafeThreshold();
  float getWarningThreshold();
  float getDangerThreshold();
  float getCriticalThreshold();
  unsigned long getReadingInterval();
  unsigned long getTransmissionInterval();
  bool isAlertsEnabled();
  bool isDataLoggingEnabled();
  String getGasType();
  bool isAutoCalibrationEnabled();
  
  // Configuration updates
  void setThresholds(float safe, float warning, float danger, float critical);
  void setReadingInterval(unsigned long interval);
  void setTransmissionInterval(unsigned long interval);
  void setAlertsEnabled(bool enabled);
  void setDataLoggingEnabled(bool enabled);
  void setGasType(String type);
  void setAutoCalibrationEnabled(bool enabled);
  
  // Status
  bool isConfigLoaded();
  unsigned long getLastConfigUpdate();
  void printConfiguration();
};

// ============================================================================
// GLOBAL INSTANCES
// ============================================================================

extern WiFiManager wifiManager;
extern DataTransmitter dataTransmitter;
extern RemoteConfiguration remoteConfig;

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// Network utilities
String getNetworkSecurityType(int securityType);
int getSignalStrength(int rssi);
String formatIPAddress(IPAddress ip);
String formatMACAddress(uint8_t* mac);

// Data formatting
String createJSONPayload(TransmissionData data);
String createAlertJSON(int level, float ppm, String description);
String createStatusJSON(bool operational, bool calibrated, int errorCount);

// HTTP utilities
bool sendHTTPPost(String url, String payload, String contentType = "application/json");
bool sendHTTPGet(String url);
String getHTTPResponse();
int getHTTPResponseCode();

// Time utilities
String getCurrentTimestamp();
String formatUptime(unsigned long uptime);
bool isTimeValid();

// Configuration utilities
bool validateConfiguration(String jsonData);
String createConfigurationJSON(RemoteConfiguration::ConfigData config);
RemoteConfiguration::ConfigData parseConfigurationJSON(String jsonData);

// Debugging utilities
void printWiFiDiagnostics();
void printNetworkScanResults();
void printTransmissionLog();
void printConfigurationStatus();

#endif // WIFI_MANAGER_H


