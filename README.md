# REMA IoT

IoT sensor monitoring system that publishes temperature, humidity, and light data to an MQTT broker over TLS. Supports multiple ESP platforms through a Hardware Abstraction Layer (HAL).

## Features

- Temperature and humidity monitoring (DHT11 sensor)
- Light level monitoring (LDR sensor)
- Secure MQTT communication over TLS
- Multi-platform support (ESP32 and ESP8266)
- Hardware Abstraction Layer for clean platform-agnostic code

## Supported Devices

| Device | Arduino IDE Board |
|--------|-------------------|
| ESP32 DevKit v1 | ESP32 Dev Module |
| ESP8266 NodeMCU v3 | NodeMCU 1.0 (ESP-12E Module) |

## Hardware Requirements

- ESP32 or ESP8266 development board
- DHT11 temperature/humidity sensor
- LDR (Light Dependent Resistor) with voltage divider
- Jumper wires

### Wiring

| Component | ESP32 | ESP8266 (NodeMCU) |
|-----------|-------|-------------------|
| DHT11 Data | GPIO4 | GPIO4 (D2) |
| LDR | GPIO34 | A0 |
| LED | GPIO2 | GPIO2 |

## Project Structure

```
rema-iot/
├── main/
│   ├── main.ino              # Main sketch
│   ├── sketch.yaml           # Arduino CLI profiles (boards & libraries)
│   ├── config.h              # Your configuration (gitignored)
│   ├── config.h.example      # Configuration template
│   ├── HAL.h                 # Hardware abstraction interface
│   ├── HAL_ESP32.cpp         # ESP32 implementation
│   └── HAL_ESP8266.cpp       # ESP8266 implementation
└── README.md
```

## Setup

### Option A: Arduino CLI (recommended)

Use the included `sketch.yaml` to automatically install boards and libraries:

```bash
# Install Arduino CLI (macOS)
brew install arduino-cli

# Configure credentials
cp main/config.h.example main/config.h
# Edit main/config.h with your values

# Compile for ESP32
arduino-cli compile --profile esp32 main/

# Compile for ESP8266
arduino-cli compile --profile esp8266 main/

# Upload (replace /dev/cu.usbserial-XXX with your port)
arduino-cli upload -p /dev/cu.usbserial-XXX --profile esp32 main/
```

### Option B: Arduino IDE

#### 1. Install Arduino IDE

Download and install [Arduino IDE](https://www.arduino.cc/en/software) (version 2.x recommended).

#### 2. Install Board Packages

1. Open Arduino IDE
2. Go to **File > Preferences**
3. In "Additional Boards Manager URLs", add these URLs (comma-separated):
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json,https://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Go to **Tools > Board > Boards Manager**
5. Search and install:
   - **esp32** by Espressif Systems
   - **esp8266** by ESP8266 Community

#### 3. Install Required Libraries

Go to **Sketch > Include Library > Manage Libraries** and install:

| Library | Author | Version |
|---------|--------|---------|
| DHT sensor library | Adafruit | 1.4.6+ |
| Adafruit Unified Sensor | Adafruit | 1.1.14+ |
| PubSubClient | Nick O'Leary | 2.8+ |

#### 4. Configure Credentials

Copy the template and fill in your credentials:

```bash
cp main/config.h.example main/config.h
```

Edit `main/config.h` with your values:

```cpp
#define WIFI_SSID       "your_wifi_ssid"
#define WIFI_PASSWORD   "your_wifi_password"
#define HOSTNAME        "your_device_hostname"
#define MQTT_HOST       "your.mqtt.broker.com"
#define MQTT_PORT       8883
#define MQTT_USER       "your_mqtt_username"
#define MQTT_PASS       "your_mqtt_password"
```

The `main/config.h` file is gitignored to keep your credentials safe.

#### 5. Select Board and Upload

1. Open `main/main.ino` in Arduino IDE
2. Select your board:
   - **ESP32:** Tools > Board > ESP32 Arduino > ESP32 Dev Module
   - **ESP8266:** Tools > Board > ESP8266 Boards > NodeMCU 1.0 (ESP-12E Module)
3. Select the correct port: **Tools > Port**
4. Click **Upload** (arrow button)

#### 6. Monitor Serial Output

Open **Tools > Serial Monitor** and set baud rate to **115200**.

## MQTT Topics

The device publishes sensor data to the following topics:

| Topic | Payload |
|-------|---------|
| `humedad/panama/<HOSTNAME>` | `{"value": <humidity>}` |
| `temperatura/panama/<HOSTNAME>` | `{"value": <temperature>}` |
| `luminosidad/panama/<HOSTNAME>` | `{"value": <light_percent>}` |

## HAL (Hardware Abstraction Layer)

The HAL pattern abstracts platform-specific differences, allowing the main application code to remain platform-agnostic.

### Platform Differences Handled

| Feature | ESP32 | ESP8266 |
|---------|-------|---------|
| WiFiClientSecure | `setCACert()` | BearSSL `setTrustAnchors()` |
| ADC Resolution | 12-bit (0-4095) | 10-bit (0-1023) |
| LDR Pin | GPIO34 | A0 |
| WiFi Hostname | `WiFi.setHostname()` | `WiFi.hostname()` |

### Adding a New Platform

1. Create `HAL_<PLATFORM>.cpp` in the `main/` folder
2. Wrap the implementation with `#ifdef <PLATFORM>` / `#endif`
3. Implement the required constants and functions

## TLS Security

The project supports three TLS verification methods (configure in `main/config.h`):

| Method | Define | Description |
|--------|--------|-------------|
| CA Root Certificate | `CHECK_CA_ROOT` | Recommended - validates server certificate |
| Fingerprint | `CHECK_FINGERPRINT` | SHA1 fingerprint verification |
| Insecure | (none) | No verification - development only |

## License

MIT
