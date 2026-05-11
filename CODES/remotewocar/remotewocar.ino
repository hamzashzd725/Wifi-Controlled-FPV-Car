#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ─── OLED Setup ───────────────────────────────────────
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ─── Car ESP32 MAC ────────────────────────────────────
uint8_t carMacAddress[] = {0xE8, 0x6B, 0xEA, 0xDF, 0x9E, 0xCC};

// ─── Joystick Pins ────────────────────────────────────
#define JOY1_X 34
#define JOY1_Y 35
#define JOY1_SW 32
#define JOY2_X 33
#define JOY2_Y 25
#define JOY2_SW 26

// ─── Servo angle state ────────────────────────────────
int currentServoAngle = 90;
int deadzone = 200;

// ─── Data Structs ─────────────────────────────────────
typedef struct {
  int carX;
  int carY;
  int servoAngle;
} RemoteToCarData;

typedef struct {
  int frontDist;
  int rearDist;
} CarToRemoteData;

RemoteToCarData joystickData;
CarToRemoteData sensorData;

// ─── Receive Callback ─────────────────────────────────
void onDataReceived(const esp_now_recv_info *info, const uint8_t *data, int len) {
  memcpy(&sensorData, data, sizeof(sensorData));
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("ESPion Starting...");
  display.display();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, carMacAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  esp_now_register_recv_cb(onDataReceived);

  Serial.println("Remote ready!");
  Serial.print("My MAC: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // ─── Joystick 1 (car control) ───────────────────────
  joystickData.carX = analogRead(JOY1_X);
  joystickData.carY = analogRead(JOY1_Y);
// look at subhans face
  // ─── Joystick 2 (servo — hold position) ─────────────
  analogRead(JOY2_X);
  delay(1);
  int rawX = analogRead(JOY2_X);
  int center = 2048;

  if (rawX < center - deadzone) {
    int speed = map(rawX, center - deadzone, 0, 0, 3);
    currentServoAngle -= speed;
  } else if (rawX > center + deadzone) {
    int speed = map(rawX, center + deadzone, 4095, 0, 3);
    currentServoAngle += speed;
  }

  currentServoAngle = constrain(currentServoAngle, 0, 180);
  joystickData.servoAngle = currentServoAngle;

  // ─── Send to car ────────────────────────────────────
  esp_now_send(carMacAddress, (uint8_t *)&joystickData, sizeof(joystickData));

  // ─── OLED: always update ─────────────────────────────
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("ESPion Remote");
  display.drawLine(0, 10, 128, 10, WHITE);

  display.setCursor(0, 16);
  display.print("Servo: ");
  display.print(currentServoAngle);
  display.println(" deg");

  display.setCursor(0, 28);
  display.print("Front: ");
  display.print(sensorData.frontDist);
  display.println(" cm");

  display.setCursor(0, 40);
  display.print("Rear:  ");
  display.print(sensorData.rearDist);
  display.println(" cm");

  display.setCursor(0, 52);
  if (sensorData.frontDist < 15 && sensorData.frontDist > 0) {
    display.println("!! FRONT OBSTACLE !!");
  } else if (sensorData.rearDist < 15 && sensorData.rearDist > 0) {
    display.println("!! REAR OBSTACLE  !!");
  } else {
    display.println("Waiting for car...");
  }

  display.display();

  Serial.print("Servo: "); Serial.print(currentServoAngle);
  Serial.print(" | Front: "); Serial.print(sensorData.frontDist);
  Serial.print(" | Rear: "); Serial.println(sensorData.rearDist);

  delay(20);
}