# REMA IoT

IoT sensor monitoring system that publishes temperature, humidity, and light data to an MQTT broker over TLS. Supports multiple ESP platforms through a Hardware Abstraction Layer (HAL).

## Features

- Temperature and humidity monitoring (DHT11 sensor)
- Light level monitoring (LDR sensor)
- Secure MQTT communication over TLS
- Multi-platform support (ESP32 and ESP8266)
- Hardware Abstraction Layer for clean platform-agnostic code

## Supported Devices

| Device | Environment | Board |
|--------|-------------|-------|
| ESP32 DevKit v1 | `esp32dev` | `esp32dev` |
| ESP8266 NodeMCU v3 | `nodemcuv3` | `nodemcuv2` |

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

## Directory Structure

```
rema-iot/
├── src/
│   └── main.cpp              # Application entry point
├── include/
│   └── secrets.h             # Credentials config (uses .env values)
├── lib/
│   └── HAL/
│       └── src/
│           ├── HAL.h             # Hardware abstraction interface
│           ├── HAL_ESP32.cpp     # ESP32 implementation
│           └── HAL_ESP8266.cpp   # ESP8266 implementation
├── .env                      # Your credentials (gitignored)
├── .env.example              # Template for .env
├── load_env.py               # Loads .env at build time
├── platformio.ini            # PlatformIO configuration
└── README.md
```

## Technologies

| Technology | Purpose |
|------------|---------|
| [PlatformIO](https://platformio.org/) | Build system and dependency management |
| [Arduino Framework](https://www.arduino.cc/) | Hardware abstraction for microcontrollers |
| [PubSubClient](https://github.com/knolleary/pubsubclient) | MQTT client library |
| [Adafruit DHT](https://github.com/adafruit/DHT-sensor-library) | DHT sensor library |
| WiFiClientSecure | TLS/SSL connections |
| BearSSL (ESP8266) | Cryptographic library for ESP8266 |

## Setup

### 1. Install PlatformIO

Install the [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode) for VSCode, or install the CLI:

```bash
pip install platformio
```

### 2. Clone the Repository

```bash
git clone <repository-url>
cd rema-iot
```

### 3. Configure Credentials

Copy the example environment file and fill in your credentials:

```bash
cp .env.example .env
```

Edit `.env` with your values:

```env
WIFI_SSID=your_wifi_ssid
WIFI_PASSWORD=your_wifi_password
HOSTNAME=your_device_hostname
MQTT_HOST=your.mqtt.broker.com
MQTT_PORT=8883
MQTT_USER=your_mqtt_username
MQTT_PASS=your_mqtt_password
```

The `.env` file is automatically loaded at build time via `load_env.py` and injected as compiler flags. The `.env` file is gitignored to keep your credentials safe.

### 4. Build and Upload

**For ESP32:**
```bash
pio run -e esp32dev -t upload
```

**For ESP8266:**
```bash
pio run -e nodemcuv3 -t upload
```

### 5. Monitor Serial Output

```bash
pio device monitor
```

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

1. Create `lib/HAL/src/HAL_<PLATFORM>.cpp`
2. Implement the required constants and functions
3. Add a new environment in `platformio.ini`

## TLS Security

The project supports three TLS verification methods (configure in `secrets.h`):

| Method | Define | Description |
|--------|--------|-------------|
| CA Root Certificate | `CHECK_CA_ROOT` | Recommended - validates server certificate |
| Fingerprint | `CHECK_FINGERPRINT` | SHA1 fingerprint verification |
| Insecure | (none) | No verification - development only |

## License

MIT
