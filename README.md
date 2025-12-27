🏠 ESP32 DALI MQTT Controller (Home Assistant Ready)

A professional-grade DALI Master Controller based on ESP32-S3, designed for robust integration with Home Assistant via MQTT.

This project bridges the gap between industrial DALI lighting and smart home systems, featuring Bi-Directional Communication, Auto-Discovery, and a unique software fix for Open Collector DALI interfaces.


🚀 Key Features
🔌 Bi-Directional Feedback: Reads the actual status of the lamp. If a lamp fails or is switched locally, HA is updated.

🧠 Smart Dimming Logic: Solves the common "100% Flash" issue. It intelligently ignores redundant "ON" commands sent by HA sliders immediately after dimming.

🔎 HA Auto-Discovery: Automatically appears in Home Assistant as a Light Device. No YAML configuration needed in HA.

🔧 Hardware RX Fix: Implements Software Internal Pull-Up to stabilize floating RX signals from the Waveshare DALI module without external resistors.

🏷️ Custom Branding: Reports custom Manufacturer & Model info to Home Assistant.


🛒 Hardware Bill of Materials (BOM)
Tested in a production environment with the following components:

1. Microcontroller
Model: ESP32-S3 (N16R8)

Specs: 16MB Flash / 8MB PSRAM (High stability variant).

2. DALI Interface
Model: Waveshare Pico-DALI (Adapted for ESP32).

Type: Opto-isolated TTL-to-DALI level shifter.

3. Power Supply
Brand: Mean Well

Model: DLP-04R

Specs: 16V DC / 240mA (DIN Rail).

Role: Powers the DALI Bus communication.

4. Verified LED Drivers
5. Brand Model Type Status
  QLTPBX 150D Dimmable ✅ Verified
  QLTPBX 200D Dimmable ✅ Verified

**Note:** You also need a 16V DALI Bus Power Supply connected to the DALI bus wires.

💡 The "Floating RX" Solution

The Waveshare Pico-DALI module has an Open Collector RX output. When connected to a standard ESP32 input, the signal "floats," causing garbage data or no response.

The Fix: Instead of soldering external resistors, this project enables the Internal Pull-Up Resistor of the ESP32-S3 via software code.

C++

// In setup():

pinMode(DALI_RX_PIN, INPUT_PULLUP); 

// This pulls the line to 3.3V internally, allowing clean reading of the DALI response.

⚙️ Configuration (src/main.cpp)
WiFi & MQTT:

const char* ssid = "ssid";

const char* password = "pass";

const char* mqtt_server = "192.168.xxx.xxx";

const char* mqtt_user = "mqtt"; 

const char* mqtt_pass = "test";


Device Identity (Home Assistant):

const char* DEVICE_NAME  = "My DALI Controller"; 

const char* DEVICE_ID    = "esp32_dali_master_v3"; 

const char* MANUFACTURER = "My Smart Home";

const char* MODEL        = "ESP32-S3 Pro Controller";

Topic Prefix:

const char* MQTT_PREFIX  = "dali/v3"; 

// Resulting topics: dali/v3/light/0/set, dali/v3/light/0/bri


## 🛒 Hardware Bill of Materials (BOM)

This project has been tested and verified with the following specific hardware components.

### 1. Microcontroller
* **Model:** ESP32-S3 (N16R8 Variant)
* **Specs:** Dual-core 240MHz, 16MB Flash, 8MB PSRAM.
* **Note:** The N16R8 version is recommended for stability, though standard ESP32 boards work.

### 2. DALI Interface Module
* **Model:** **Waveshare Pico-DALI2** (Adapted for ESP32 Pico)
* **Function:** Handles TTL (3.3V) to DALI (16V) logic conversion.
* **Connection:** Wired via jumper cables from the Pico header to the ESP32.

| Waveshare Pin | ESP32-S3 Pin | Description |
| :--- | :--- | :--- |
| **GPIO6 (Pin 9)** | **GPIO 21** | **TX** (Transmit DALI) |
| **VSYS (Pin 39)** | **5V** | Power Input |
| **GND (Pin 38)** | **GND** | Ground |
| **DALI Bus** | **DALI +/-** | To LED Drivers |

![Pico-DALI2-details-inter](https://github.com/user-attachments/assets/e71efed2-2cba-43d2-8d57-1e034cbc3b97)

### 3. DALI Bus Power Supply
* **Brand:** **Mean Well**
* **Model:** **DLP-04R**
* **Type:** DIN Rail Mount DALI Power Supply
* **Specs:** Output 16V DC / 240mA
* **Note:** Essential for powering the DALI bus communication line.

![S20dfc8ec6d304815b8aef36b51e602c3S](https://github.com/user-attachments/assets/11147d48-c4a1-4275-a6de-34e429af9bc8)



### 4. ✅ Verified LED Drivers
The code has been field-tested with the following drivers:

| Brand | Model | Type | Status |
| :--- | :--- | :--- | :--- |
| **QLT** | **PBX 150D** | Dimmable LED Driver | **VERIFIED** |
| **QLT** | **PBX 200D** | Dimmable LED Driver | **VERIFIED** |

> **Verification:** Drivers confirmed to accept Hard Reset (`0x20`) and Force Address commands correctly.

![PBOX200D2B-1024x736](https://github.com/user-attachments/assets/2de5d523-89f2-40d6-bc22-a11eec26e606)
