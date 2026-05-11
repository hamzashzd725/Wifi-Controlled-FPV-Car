#include <NewPing.h>

// Pin Definitions
#define TRIGGER_PIN   5    // ESP32 pin connected to HC-SR04 Trig
#define ECHO_PIN      18   // ESP32 pin connected to HC-SR04 Echo
#define BUZZER_PIN    19   // ESP32 pin connected to YL44 buzzer

#define MAX_DISTANCE  200  // Maximum distance to ping (cm)
#define ALERT_DISTANCE 10  // Buzz when object is closer than this (cm)

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Make sure buzzer is off at start
}

void loop() {
  delay(1000); // Small delay between pings (NewPing recommends >= 29ms)

  unsigned int distance = sonar.ping_cm();

  // ping_cm() returns 0 if out of range
  if (distance == 0) {
    Serial.println("Out of range");
    digitalWrite(BUZZER_PIN, LOW);
  } 
  else if (distance < ALERT_DISTANCE) {
    Serial.print("ALERT! Distance: ");
    Serial.print(distance);
    Serial.println(" cm - BUZZING");
    digitalWrite(BUZZER_PIN, LOW); // Activate buzzer
  } 
  else {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    digitalWrite(BUZZER_PIN, HIGH); // Buzzer off
  }
}