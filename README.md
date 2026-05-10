# WiFi Controlled FPV Car

A wireless RC car with live first-person video streaming, and a physical joystick remote — built entirely on ESP32 hardware using ESP-NOW for low-latency communication.

---

## Overview

This project consists of three independent embedded systems that work together:

- **ESP32 Car** — receives joystick commands, drives motors, reads ultrasonic sensors and sends distance data back to the remote
- **ESP32 Remote** — reads two joystick inputs and transmits control data to the car via ESP-NOW, displays sensor readings on an OLED screen
- **ESP32-CAM** — independently streams a live MJPEG video feed over WiFi accessible from any browser

---

## Features

- Real-time WiFi FPV camera stream viewable in any browser
- Physical joystick remote with two axes for car control
- ESP-NOW communication with ~1-2ms latency
- Automatic motor cutoff (failsafe) when signal is lost
- Tank-style drive mixing for smooth turning
- No router required for car control — ESP-NOW is direct peer-to-peer

---

## System Architecture

```
                        ┌─────────────────────┐
                        │   ESP32 Remote       │
                        │                      │
                        │  Joystick 1 (Drive)  │
                        └────────┬─────────────┘
                                 │
                            ESP-NOW (bidirectional)
                            Control data →
                                 │
                        ┌────────▼─────────────┐
                        │   ESP32 Car          │
                        │                      │
                        │  L298N Motor Driver  │
                        │  2x DC Gear Motors   │
                        │  HC-SR04P (Front)    │
                        │  HC-SR04P (Rear)     │
                        └──────────────────────┘

                        ┌─────────────────────┐
                        │   ESP32-CAM          │
                        │                      │
                        │  MJPEG Stream →      │──── Browser
                        │  Powered via USB     │
                        └─────────────────────┘
```

---

## Hardware

| Component | Quantity | Purpose |
|---|---|---|
| ESP32 DOIT DevKit V1 | 2 | Car controller + Remote |
| ESP32-CAM (AI Thinker) | 1 | FPV video stream |
| ESP32-CAM Programmer Board | 1 | Flashing + powering ESP32-CAM |
| L298N Motor Driver | 1 | Controls DC motors |
| TT DC Gear Motors | 2 | Car movement |
| Joystick Module | 1 | Car control |
| 18650 Batteries | 2 | Car power |
| 2-cell 18650 Holder | 1 | Battery housing |
| 4WD Robot Chassis | 1 | Car base |

---

## Wiring

### ESP32 Car → L298N
```
GPIO 14  →  ENA
GPIO 27  →  IN1
GPIO 26  →  IN2
GPIO 25  →  IN3
GPIO 33  →  IN4
GPIO 32  →  ENB
GND      →  L298N GND (common ground)
```

### Joysticks → ESP32 Remote
```
Joystick 1: VRX → GPIO 35 | VRY → GPIO 34
VCC → 3.3V | GND → GND
```
### Power (Car)
```
18650 x2 (7.4V) → L298N VIN
XL6009 IN → Battery positive
XL6009 OUT (5V) → ESP32 Car VIN
ESP32-CAM → Powered separately via USB power bank
```

---

## Software

### Prerequisites

- Arduino IDE or PlatformIO
- ESP32 Arduino Core v3.x
- Libraries:
  - `esp_now.h` (built-in)
  - `WiFi.h` (built-in)

### Project Structure

```
Wifi-Controlled-FPV-Car/
├── Reciever/
│   └── rReciever.ino         ← Motor control ESP-NOW receiver
├── Transmitter/
│   └── Transmitter.ino          ← Joystick reading, ESP-NOW sender
├── esp32-cam/
│   └── esp32CamAP          ← MJPEG camera stream (Wifi Acesspoint stream)
└── docs/
    ├── proposal.docx
```

### Setup

**1. Get MAC addresses**

Flash this to each ESP32 and note the MAC from Serial Monitor:
```cpp
#include <WiFi.h>
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
}
void loop() {}
```

**2. Update MAC addresses in code**

In `esp32-remote/main.cpp`:
```cpp
uint8_t carMAC[] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};
```

**3. Flash ESP32-CAM**

- Set `#define CAMERA_MODEL_AI_THINKER`
- Set Acess point Name and Password
- Access stream at `http://<cam-ip>/stream`

---

## How It Works

### ESP-NOW Communication

Remote ESP32 act as transmitter — acts as master:

```
Remote sends every 20ms:  { x, y }  →  Car moves motors
```

### Motor Control

Tank-style drive mixing maps joystick axes to individual motor speeds:
```
motorA = y + x
motorB = y - x
```
This allows smooth forward, backward, turning, and spinning on the spot.

### Failsafe

If the car receives no signal for 500ms (remote turned off, out of range) all motors stop automatically.

---

## Known Limitations

- ESP-NOW range is limited by WiFi router range when using camera stream simultaneously (~20-30m indoors)
- Camera stream and car control are on separate networks — browser must be on same WiFi as ESP32-CAM

---

## License

MIT License — free to use, modify and distribute.