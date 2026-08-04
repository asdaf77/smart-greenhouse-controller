#include <LiquidCrystal.h>
#include <Servo.h>

// ========== PIN DEFINITIONS ==========
const int tempPin = A0;
const int lightPin = A1;
const int moisturePin = A2;
const int fanPin = 9;
const int servoPin = 10;
const int growLightPin = 8;
const int redPin = 6;
const int greenPin = 7;
const int bluePin = 13;
const int buzzerPin = A3;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo waterValve;

// ========== THRESHOLDS ==========
const float TEMP_HIGH = 28.0;
const float TEMP_CRITICAL = 35.0;
const int LIGHT_DARK = 300;
const int MOISTURE_DRY = 400;

// ========== DATA LOGGING ==========
float maxTemp = -100.0;
float minTemp = 1000.0;
float maxMoisture = 0;
float minMoisture = 1023;
unsigned long startTime = 0;
int logCounter = 0;

// ========== AVERAGING ==========
const int NUM_SAMPLES = 5;

float readAverageTemp() {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(tempPin);
    delay(5);
  }
  float avgReading = sum / (float)NUM_SAMPLES;
  float voltage = avgReading * (5.0 / 1023.0);
  return (voltage - 0.5) * 100.0;
}

int readAverageLight() {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(lightPin);
    delay(5);
  }
  return sum / NUM_SAMPLES;
}

int readAverageMoisture() {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(moisturePin);
    delay(5);
  }
  return sum / NUM_SAMPLES;
}

void setup() {
  Serial.begin(9600);
  
  pinMode(fanPin, OUTPUT);
  pinMode(growLightPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  waterValve.attach(servoPin);
  waterValve.write(0);
  
  lcd.begin(16, 2);
  lcd.print("Greenhouse Ready");
  delay(2000);
  lcd.clear();
  
  startTime = millis();
  
  Serial.println("========================================");
  Serial.println("   SMART GREENHOUSE CONTROLLER v2.1");
  Serial.println("========================================");
  Serial.println("Temp(C) | Light | Moisture | FanSpeed | Grow | Water | Status");
  Serial.println("-------------------------------------------------------------");
}

void loop() {
  // --- READ SENSORS ---
  float temperature = readAverageTemp();
  int lightLevel = readAverageLight();
  int moistureLevel = readAverageMoisture();

  // --- UPDATE MIN/MAX ---
  if (temperature > maxTemp) maxTemp = temperature;
  if (temperature < minTemp) minTemp = temperature;
  if (moistureLevel > maxMoisture) maxMoisture = moistureLevel;
  if (moistureLevel < minMoisture) minMoisture = moistureLevel;

  // --- SMOOTH FAN CONTROL (starts at 28C, stronger PWM range) ---
  int fanSpeed = 0;
  bool fanOn = false;
  if (temperature > TEMP_HIGH) {
    fanSpeed = map(constrain(temperature, 28, 40), 28, 40, 100, 255);
    analogWrite(fanPin, fanSpeed);
    fanOn = true;
  } else {
    analogWrite(fanPin, 0);
  }

  // --- GROW LIGHT ---
  bool lightOn = false;
  if (lightLevel < LIGHT_DARK) {
    digitalWrite(growLightPin, HIGH);
    lightOn = true;
  } else {
    digitalWrite(growLightPin, LOW);
  }

  // --- WATER VALVE (daytime only) ---
  bool waterOn = false;
  bool isDaytime = (lightLevel > LIGHT_DARK);
  
  if (moistureLevel < MOISTURE_DRY && isDaytime) {
    waterValve.write(90);
    waterOn = true;
  } else if (moistureLevel < MOISTURE_DRY && !isDaytime) {
    // Night + dry = blink to show "waiting for morning"
    int blinkPos = (millis() / 500) % 2 == 0 ? 15 : 0;
    waterValve.write(blinkPos);
  } else {
    waterValve.write(0);
  }

  // --- RGB + BUZZER ---
  bool critical = false;
  if (temperature > TEMP_CRITICAL) {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);
    tone(buzzerPin, 1000, 500);
    critical = true;
  } else if (temperature > TEMP_HIGH || moistureLevel < MOISTURE_DRY) {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, LOW);
    noTone(buzzerPin);
  } else {
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, LOW);
    noTone(buzzerPin);
  }

  // --- LCD DISPLAY (FIXED PRIORITY) ---
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print(" L:");
  lcd.print(lightLevel);

  lcd.setCursor(0, 1);
  lcd.print("M:");
  lcd.print(moistureLevel);
  lcd.print(" ");

  // PRIORITY ORDER (highest to lowest):
  if (critical) {
    lcd.print("ALERT!");
  } 
  else if (fanOn && fanSpeed > 220) {
    lcd.print("HOT!");
  } 
  else if (fanOn) {
    lcd.print("COOLING");
  } 
  else if (waterOn) {
    lcd.print("WATERING");
  } 
  else if (!isDaytime && moistureLevel < MOISTURE_DRY) {
    // FIXED: Now checked BEFORE GROWING
    lcd.print("WAIT 4 MORN");
  } 
  else if (lightOn) {
    lcd.print("GROWING");
  } 
  else {
    lcd.print("OPTIMAL");
  }

  // --- SERIAL MONITOR ---
  Serial.print(temperature, 1);
  Serial.print("    | ");
  Serial.print(lightLevel);
  Serial.print("   | ");
  Serial.print(moistureLevel);
  Serial.print("      | ");
  Serial.print(fanSpeed);
  Serial.print("      | ");
  Serial.print(lightOn ? "ON " : "OFF");
  Serial.print(" | ");
  Serial.print(waterOn ? "ON " : (moistureLevel < MOISTURE_DRY ? "WAIT" : "OFF"));
  Serial.print(" | ");
  
  if (critical) Serial.println("CRITICAL");
  else if (!isDaytime && moistureLevel < MOISTURE_DRY) Serial.println("NIGHT-WAIT");
  else if (fanOn || waterOn || lightOn) Serial.println("ACTIVE");
  else Serial.println("OK");

  // --- DATA LOG (every 10 seconds) ---
  logCounter++;
  if (logCounter >= 10) {
    logCounter = 0;
    unsigned long elapsed = (millis() - startTime) / 1000;
    
    Serial.println();
    Serial.println("----- DATA LOG (" + String(elapsed) + "s) -----");
    Serial.print("Temp:  Min="); Serial.print(minTemp, 1);
    Serial.print("  Max="); Serial.println(maxTemp, 1);
    Serial.print("Moist: Min="); Serial.print(minMoisture);
    Serial.print("  Max="); Serial.println(maxMoisture);
    Serial.println("--------------------------------");
    Serial.println();
  }

  delay(1000);
}