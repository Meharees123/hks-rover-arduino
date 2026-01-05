# 🤖 HKS Rover - Arduino Firmware

Arduino firmware for the HKS Rover omnidirectional robot. This code runs on an **Arduino UNO R4 WiFi** and controls three motors plus a kicker mechanism via MQTT commands from the web interface.

> **🌐 Web Interface**: [HKS Rover Web Control](https://github.com/Script-hpp/hks-rover) - The browser-based control system for this rover.

## 📁 Project Files

- **`rover.ino`** - Main rover control firmware (Arduino UNO R4 WiFi)
- **`camera.ino`** - ESP32-CAM streaming firmware (ESP32-CAM module)
- **`arduino_secrets.h`** - WiFi credentials configuration

## 📋 Table of Contents

- [Hardware](#-hardware)
- [Features](#-features)
- [Pin Configuration](#-pin-configuration)
- [Installation](#-installation)
- [Configuration](#-configuration)
- [MQTT Commands](#-mqtt-commands)
- [Camera Setup (ESP32-CAM)](#-camera-setup-esp32-cam)
- [Troubleshooting](#-troubleshooting)
- [License](#-license)

## 🔧 Hardware

### Required Components
- **Arduino UNO R4 WiFi** (or compatible WiFi-enabled Arduino)
- **3x DC Motors** for omnidirectional movement
- **2x H-Bridge Motor Drivers** (e.g., L298N)
- **External Power Supply** (recommended: separate from Arduino power)

### Wiring Diagram

```
Arduino UNO R4 WiFi
├── H-Bridge 1 (L298N)
│   ├── Motor 1 (Front/Back)
│   │   ├── Pin 1  → IN1 (Forward)
│   │   ├── Pin 2  → IN2 (Backward)
│   │   └── Pin 3  → ENA (Speed - PWM)
│   └── Motor 2 (Front/Back)
│       ├── Pin 4  → IN3 (Forward)
│       ├── Pin 5  → IN4 (Backward)
│       └── Pin 6  → ENB (Speed - PWM)
└── H-Bridge 2 (L298N)
    └── Motor 3 (Left/Right)
        ├── Pin 7  → IN1 (Left)
        ├── Pin 8  → IN2 (Right)
        └── Pin 9  → ENA (Speed - PWM)
```

## ✨ Features

- **WiFi Connectivity** with automatic reconnection
- **MQTT Communication** for real-time control
- **Omnidirectional Movement** (forward, backward, left, right, rotate)
- **Dynamic Speed Control** based on command repetition
- **Boost Mode** for maximum speed
- **Heartbeat Messages** for connection monitoring
- **Comprehensive Debug Logging** via Serial Monitor

## 📌 Pin Configuration

| Pin | Function | Description |
|-----|----------|-------------|
| 1 | Motor 1 Forward | Front/back motor direction |
| 2 | Motor 1 Backward | Front/back motor direction |
| 3 | Motor 1 Speed (PWM) | Front/back motor speed control |
| 4 | Motor 2 Forward | Front/back motor direction |
| 5 | Motor 2 Backward | Front/back motor direction |
| 6 | Motor 2 Speed (PWM) | Front/back motor speed control |
| 7 | Motor 3 Left | Left/right motor direction |
| 8 | Motor 3 Right | Left/right motor direction |
| 9 | Motor 3 Speed (PWM) | Left/right motor speed control |
| LED_BUILTIN | Status LED | Connection status indicator |

## 🚀 Installation

### 1. Install Arduino IDE

Download and install the [Arduino IDE](https://www.arduino.cc/en/software) (version 2.0 or higher recommended).

### 2. Install Required Libraries

Open Arduino IDE and install the following libraries via **Library Manager** (Sketch → Include Library → Manage Libraries):

- **ArduinoMqttClient** by Arduino
- **WiFiS3** (included with Arduino UNO R4 WiFi board support)

### 3. Install Board Support

1. Go to **Tools → Board → Boards Manager**
2. Search for "Arduino UNO R4 WiFi"
3. Install the board package

### 4. Create Secrets File

Create a file named `arduino_secrets.h` in the same directory as `rover.ino`:

```cpp
#define SECRET_SSID "YourWiFiSSID"
#define SECRET_PASS "YourWiFiPassword"
```


### 5. Upload the Code

1. Connect your Arduino UNO R4 WiFi via USB
2. Select **Tools → Board → Arduino UNO R4 WiFi**
3. Select the correct **Port**
4. Click **Upload** (→)

## ⚙️ Configuration

### WiFi Settings

Edit `arduino_secrets.h`:
```cpp
#define SECRET_SSID "YourNetworkName"
#define SECRET_PASS "YourNetworkPassword"
```

### MQTT Broker Settings

Edit `rover.ino` (lines 47-49):
```cpp
const char broker[] = "your-broker-ip-or-domain";
int port = 1883;
const char topic[] = "rover/control";
```

### Speed Settings

Adjust motor speeds in `rover.ino` (lines 55-56):
```cpp
int baseSpeed = 160;  // Base speed (0-255)
int maxSpeed = 255;   // Maximum speed for boost mode
```

## 📡 MQTT Commands

The rover listens to the `rover/control` topic and responds to the following commands:

| Command | Action | Description |
|---------|--------|-------------|
| `forward` | Move forward | Both front motors forward |
| `backward` | Move backward | Both front motors backward |
| `left` | Strafe left | Diagonal movement using all motors |
| `right` | Strafe right | Diagonal movement using all motors |
| `rotate-left` | Rotate counterclockwise | Motors in opposite directions |
| `rotate-right` | Rotate clockwise | Motors in opposite directions |
| `stop` | Stop all motors | Emergency stop |
| `gas` | Boost mode | Maximum speed on all motors |

### MQTT Topics

- **Subscribe**: `rover/control` - Receives movement commands
- **Publish**: `rover/status` - Sends connection status on startup
- **Publish**: `rover/heartbeat` - Sends heartbeat every 30 seconds

## 📷 Camera Setup (ESP32-CAM)

The `camera.ino` file contains firmware for the **ESP32-CAM** module to stream video to the web interface.

> **⚠️ Note**: This code was written in a rush and does not follow best practices. It works but could be optimized for better performance and error handling.

### Hardware Requirements
- **ESP32-CAM** module (AI Thinker model)
- **FTDI Programmer** (for uploading code)
- **External 5V power supply** (ESP32-CAM draws too much current for USB)

### Installation

1. **Install ESP32 Board Support** in Arduino IDE:
   - Go to **File → Preferences**
   - Add to "Additional Board Manager URLs": `https://dl.espressif.com/dl/package_esp32_index.json`
   - Go to **Tools → Board → Boards Manager**
   - Search for "ESP32" and install

2. **Select Board**:
   - **Tools → Board → ESP32 Arduino → AI Thinker ESP32-CAM**

3. **Configure Settings**:
   - Edit `arduino_secrets.h` with your WiFi credentials
   - Update `serverURL` in `camera.ino` to point to your Node.js server

4. **Upload**:
   - Connect ESP32-CAM to FTDI programmer
   - Set **GPIO 0 to GND** (programming mode)
   - Upload the code
   - Remove GPIO 0 connection and reset

### How It Works

The ESP32-CAM:
1. Connects to WiFi
2. Captures JPEG frames at ~5 FPS
3. Sends frames to `/api/camera/upload` endpoint via HTTP POST
4. The web interface fetches frames from `/api/camera/stream`

### Known Issues

- **Memory limitations**: ESP32-CAM has limited SRAM (~4KB), so large images may cause crashes
- **No error recovery**: If upload fails, the camera doesn't retry
- **Hardcoded settings**: Frame size and quality are not configurable without code changes
- **Our hardware broke**: The ESP32-CAM stopped working after extended use due to hardware limitations

## 🔧 Troubleshooting

### WiFi Connection Issues

**Problem**: Arduino cannot connect to WiFi.

**Solution**:
- Verify SSID and password in `arduino_secrets.h`
- Ensure WiFi network is 2.4GHz (Arduino UNO R4 WiFi does not support 5GHz)
- Check WiFi signal strength near the rover

### MQTT Connection Fails

**Problem**: Arduino connects to WiFi but MQTT connection fails.

**Solution**:
- **Firewall Restrictions**: Most public and home networks block MQTT port 1883. The rover **only works over mobile hotspot** or requires **tunneling/reverse proxy** setup.
- Verify broker IP address and port in `rover.ino`
- Check that the MQTT broker is running and accessible
- Use Serial Monitor to view detailed error codes

### Motors Not Responding

**Problem**: MQTT connection is successful but motors don't move.

**Solution**:
- Check motor driver connections and power supply
- Verify pin assignments match your hardware setup
- Ensure external power supply is connected (USB power is insufficient)
- Use Serial Monitor to confirm commands are being received

### Serial Monitor Shows Connection Errors

**Problem**: `TCP Connection: FAILED!` or `MQTT Connection: FAILED`

**Solution**:
- **Network Restrictions**: The code includes connectivity tests. If TCP fails but Google DNS (8.8.8.8) succeeds, your network is blocking the MQTT broker.
- **Workaround**: Use a mobile hotspot or set up NGINX reverse proxy (see web interface README)
- Check broker IP and port configuration

## 📊 Serial Monitor Output

The code provides detailed debug information via Serial Monitor (9600 baud):

```
=== Arduino Network & MQTT Debug ===
Connecting to WiFi: YourNetwork
WiFi Connected!
IP Address: 192.168.1.100
MAC Address: AA:BB:CC:DD:EE:FF
Testing TCP connection to broker...
TCP Connection: SUCCESS!
MQTT Client ID: Arduino-12345
Connecting to MQTT broker 40.113.80.61:1883
MQTT Connection: SUCCESS!
=== Setup Complete ===
📩 Command received: forward
🔼 Moving forward
```

## 🎓 About This Project

This firmware was developed for our school Info-Day presentation. It demonstrates:
- Real-time IoT communication using MQTT
- WiFi connectivity on Arduino
- Motor control with PWM
- Network troubleshooting and diagnostics

**Related Repository**: [HKS Rover Web Interface](https://github.com/Script-hpp/hks-rover)

## 📄 License

This project is open source and available for educational purposes.

---

**Firmware developed with ❤️ by Onuralp Akca, Nam Feist**
