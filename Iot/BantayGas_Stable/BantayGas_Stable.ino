#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin definitions
#define Buzzer 33
#define RGB_R  32
#define RGB_G  25
#define RGB_B  26
#define Sensor 34   // Gas sensor AO → GPIO34 (ADC)

// I2C pins for ESP32
#define I2C_SDA 21
#define I2C_SCL 22

// --- Thresholds ---
#define SAFE_MAX        40
#define WARNING_MIN     41
#define WARNING_MAX     60
#define HIGHWARN_MIN    61
#define HIGHWARN_MAX    80
#define DANGER_MIN      81

// Variables for buzzer timing
unsigned long previousMillis = 0;
bool buzzerState = false;

// Track system state
enum State { SAFE, WARNING, HIGH_WARNING, DANGER };
State currentState = SAFE;
State previousState = SAFE;

// --- Function to read stable gas value (averaging) ---
int readGasLevel() {
  long sum = 0;
  const int samples = 20;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(Sensor);
    delay(5);
  }
  int avg = sum / samples;
  return map(avg, 0, 4095, 0, 100);  // scale 0–4095 → 0–100
}

// --- Helper: RGB LED control ---
void setRGB(bool r, bool g, bool b) {
  digitalWrite(RGB_R, r);
  digitalWrite(RGB_G, g);
  digitalWrite(RGB_B, b);
}

// --- Helper: Safe entry chime ---
void playSafeChime() {
  digitalWrite(Buzzer, HIGH);
  delay(120);
  digitalWrite(Buzzer, LOW);
  delay(120);
  digitalWrite(Buzzer, HIGH);
  delay(200);
  digitalWrite(Buzzer, LOW);
}

// --- Helper: Startup melody ---
void playStartupChime() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(Buzzer, HIGH);
    delay(150);
    digitalWrite(Buzzer, LOW);
    delay(100);
  }
  delay(200);
  digitalWrite(Buzzer, HIGH);
  delay(300);
  digitalWrite(Buzzer, LOW);
}

// --- Update LCD display ---
void updateLCD(int gasLevel, State state) {
  lcd.setCursor(0, 0);
  lcd.print("Value : ");
  lcd.print(gasLevel);
  lcd.print("   ");  // clear residue

  lcd.setCursor(0, 1);
  if (state == SAFE) {
    lcd.print("Gas value: SAFE    ");
  } else if (state == WARNING) {
    lcd.print("Gas value: WARNING ");
  } else if (state == HIGH_WARNING) {
    lcd.print("Gas value: HI-WARN ");
  } else if (state == DANGER) {
    lcd.print("Gas value: DANGER  ");
  }
}

// --- Determine system state ---
State determineState(int gasLevel) {
  if (gasLevel <= SAFE_MAX) return SAFE;
  else if (gasLevel <= WARNING_MAX) return WARNING;
  else if (gasLevel <= HIGHWARN_MAX) return HIGH_WARNING;
  else return DANGER;
}

// --- Handle outputs (LEDs + buzzer) ---
void handleOutputs(State state) {
  // Reset buzzer
  digitalWrite(Buzzer, LOW);

  if (state == SAFE) {
    setRGB(LOW, HIGH, LOW);  // Green
    if (previousState != SAFE) playSafeChime();

  } else if (state == WARNING) {
    setRGB(HIGH, HIGH, LOW); // Yellow (Red+Green)
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 500) { // slow beep
      previousMillis = currentMillis;
      buzzerState = !buzzerState;
      digitalWrite(Buzzer, buzzerState);
    }

  } else if (state == HIGH_WARNING) {
    setRGB(HIGH, HIGH, LOW); // Yellow
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 200) { // fast beep
      previousMillis = currentMillis;
      buzzerState = !buzzerState;
      digitalWrite(Buzzer, buzzerState);
    }

  } else if (state == DANGER) {
    setRGB(HIGH, LOW, LOW);  // Red
    digitalWrite(Buzzer, HIGH); // Continuous buzzer
  }
}

void setup() {
  Serial.begin(9600);

  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();

  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Sensor, INPUT);

  // Welcome screen + chime
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Welcome to   ");   // centered line 1
  lcd.setCursor(0, 1);
  lcd.print("   BantayGas!   ");  // centered line 2
  playStartupChime();
  delay(3000);
  lcd.clear();

  Serial.println("✅ Gas Detector System Started");
}

void loop() {
  int gasLevel = readGasLevel();
  Serial.println(gasLevel);

  currentState = determineState(gasLevel);
  handleOutputs(currentState);
  updateLCD(gasLevel, currentState);

  previousState = currentState;
}
