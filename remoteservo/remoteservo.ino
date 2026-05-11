#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

// ─── OLED Setup ───────────────────────────────────────
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ─── Joystick 2 Pins ──────────────────────────────────
#define JOY2_X 33

// ─── Servo ────────────────────────────────────────────
#define SERVO_PIN 13
Servo myServo;

int currentAngle = 90;  // Track current position
int deadzone = 200;     // Joystick deadzone around center

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("ESPion - Servo Test");
  display.drawLine(0, 10, 128, 10, WHITE);
  display.display();

  myServo.attach(SERVO_PIN);
  myServo.write(currentAngle);

  Serial.println("Servo test ready!");
}

void loop() {
  int rawX = analogRead(JOY2_X);
  int center = 2048;

  // Only move if joystick is pushed past deadzone
  if (rawX < center - deadzone) {
    // Pushed left — decrease angle
    int speed = map(rawX, center - deadzone, 0, 0, 3);
    currentAngle -= speed;
  } else if (rawX > center + deadzone) {
    // Pushed right — increase angle
    int speed = map(rawX, center + deadzone, 4095, 0, 3);
    currentAngle += speed;
  }
  // If inside deadzone — do nothing, hold position

  currentAngle = constrain(currentAngle, 0, 180);
  myServo.write(currentAngle);

  // Update OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.println("ESPion - Servo Test");
  display.drawLine(0, 10, 128, 10, WHITE);

  display.setCursor(0, 16);
  display.print("Raw:   ");
  display.println(rawX);

  display.setCursor(0, 28);
  display.print("Angle: ");
  display.print(currentAngle);
  display.println(" deg");

  display.setCursor(0, 48);
  display.println("0       90      180");
  int barX = map(currentAngle, 0, 180, 0, 123);
  display.drawRect(0, 57, 128, 6, WHITE);
  display.fillRect(0, 57, barX, 6, WHITE);

  display.display();

  Serial.printf("Raw: %d | Angle: %d\n", rawX, currentAngle);

  delay(20);
}