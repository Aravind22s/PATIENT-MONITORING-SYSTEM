// Blynk Configuration
#define BLYNK_TEMPLATE_ID "Your_Template_ID"
#define BLYNK_TEMPLATE_NAME "Your_Template_Name"
#define BLYNK_AUTH_TOKEN "Your_Auth_Token"

#include <Wire.h>
#include <MAX30100_PulseOximeter.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Pin Definitions
#define LM35_PIN A0


char ssid[] = "Your_WiFi_SSID";
char pass[] = "Your_WiFi_Password";

// MAX30100 Configuration
PulseOximeter pox;

// Reporting Timers
#define REPORTING_PERIOD_MS 5000
unsigned long tsLastReport = 0;

// Variables
float bodyTemp = 0.0;
int BPM = 0;

void onBeatDetected() {
  Serial.println("Heartbeat detected!");
}

void setup() {
  // Serial Monitor
  Serial.begin(115200);

  // WiFi Connection
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  // Blynk Initialization
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // MAX30100 Initialization
  if (!pox.begin()) {
    Serial.println("MAX30100 Sensor initialization failed!");
    while (1);
  }
  pox.setOnBeatDetectedCallback(onBeatDetected);
  Serial.println("MAX30100 Sensor initialized successfully!");
}

void loop() {
  Blynk.run();
  pox.update();

  // Read body temperature
  bodyTemp = readTemperature();

  // Read BPM
  BPM = pox.getHeartRate();

  // Send data to Blynk every 5 seconds
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    Blynk.virtualWrite(V0, BPM);       // Heart Rate
    Blynk.virtualWrite(V1, bodyTemp); // Body Temperature

    Serial.print("BPM: ");
    Serial.print(BPM);
    Serial.print(" | Temp: ");
    Serial.print(bodyTemp);
    Serial.println(" C");

    tsLastReport = millis();
  }
  delay(500);
}

float readTemperature() {
  int analogValue = analogRead(LM35_PIN);
  float millivolts = (analogValue / 1024.0) * 3300; // Convert to millivolts
  return millivolts / 10.0; // Convert millivolts to Celsius
}