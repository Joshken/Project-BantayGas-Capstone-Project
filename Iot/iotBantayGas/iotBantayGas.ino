/*****************************************************
   ESP32 + MQ-6 Gas Detector
   Components:
   - MQ-6 Gas Sensor (AOUT → GPIO34)
   - Green LED (GPIO21)
   - Yellow LED (GPIO22)
   - Red LED (GPIO23)
   - Buzzer (GPIO19)
 *****************************************************/

#define MQ6_PIN     34   // Analog input (AOUT of MQ-6)

// LED pins
#define LED_GREEN   21
#define LED_YELLOW  22
#define LED_RED     23

// Buzzer pin
#define BUZZER      19

// Thresholds (adjust after calibration/testing)
int thresholdSafe    = 1200;  // below this = safe
int thresholdWarning = 2500;  // above this = danger

void setup() {
  Serial.begin(115200);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Start with everything OFF
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(BUZZER, LOW);

  Serial.println("Gas Detector Ready...");
}

void loop() {
  int gasValue = analogRead(MQ6_PIN);
  Serial.print("Gas Sensor Value: ");
  Serial.println(gasValue);

  if (gasValue < thresholdSafe) {
    // SAFE ZONE
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(BUZZER, LOW);
  }
  else if (gasValue >= thresholdSafe && gasValue < thresholdWarning) {
    // WARNING ZONE
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
    digitalWrite(BUZZER, LOW);
  }
  else {
    // DANGER ZONE
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, HIGH);

    // Buzzer ON (continuous alarm)
    digitalWrite(BUZZER, HIGH);
  }

  delay(500); // update every 0.5s
}
