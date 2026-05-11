#include <esp_now.h>
#include <WiFi.h>

#define ENA 14  // Changed from 14
#define ENB 32
#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33

typedef struct {
  int16_t x;
  int16_t y;
} struct_message;

struct_message incomingData;
unsigned long lastReceiveTime = 0;
bool dataReceived = false;

void setMotor(int pwmPin, int dirPin1, int dirPin2, int speed) {
  if (speed > 0) {
    digitalWrite(dirPin1, HIGH);
    digitalWrite(dirPin2, LOW);
    ledcWrite(pwmPin, speed);
  } else if (speed < 0) {
    digitalWrite(dirPin1, LOW);
    digitalWrite(dirPin2, HIGH);
    ledcWrite(pwmPin, -speed);
  } else {
    digitalWrite(dirPin1, LOW);
    digitalWrite(dirPin2, LOW);
    ledcWrite(pwmPin, 0);
  }
}

void driveMotors(int16_t x, int16_t y) {
  int deadzone = 100;
  if (abs(x) < deadzone) x = 0;
  if (abs(y) < deadzone) y = 0;

  int16_t motorA = y + x;
  int16_t motorB = y - x;

  motorA = constrain(motorA, -1000, 1000);
  motorB = constrain(motorB, -1000, 1000);

  motorA = map(motorA, -1000, 1000, -255, 255);
  motorB = map(motorB, -1000, 1000, -255, 255);

  // Add this debug line
  Serial.printf("motorA: %d | motorB: %d\n", motorA, motorB);

  setMotor(ENA, IN1, IN2, motorA);
  setMotor(ENB, IN3, IN4, motorB);
}

// Fixed callback signature
void onReceive(const esp_now_recv_info *info, const uint8_t *incomingDataRaw, int len) {
  memcpy(&incomingData, incomingDataRaw, sizeof(incomingData));
  lastReceiveTime = millis();
  dataReceived = true;
  Serial.printf("X: %d | Y: %d\n", incomingData.x, incomingData.y);
  driveMotors(incomingData.x, incomingData.y);
}

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Setup PWM
  ledcAttach(ENA, 1000, 8);
  ledcAttach(ENB, 1000, 8);

  // Stop motors on boot
  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (1);
  }

  esp_now_register_recv_cb(onReceive);
  Serial.println("CAR READY");
}

void loop() {
  // Failsafe
  if (dataReceived && millis() - lastReceiveTime > 500) {
    ledcWrite(ENA, 0);
    ledcWrite(ENB, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    Serial.println("No signal - stopped");
    dataReceived = false;
  }
}