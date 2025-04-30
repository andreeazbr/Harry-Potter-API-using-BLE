# ESP32 BLE-WiFi Integration Project – “ProiectIa”

## Overview

This project demonstrates an ESP32-based system that uses **Bluetooth Low Energy (BLE)** to interact with a mobile application and **WiFi** to access external web resources. The BLE connection enables a client (e.g., mobile app) to send commands to the ESP32, which then responds with WiFi scan results, connects to networks, or retrieves data from a remote API.

## Features

- BLE server with a custom service and characteristic.
- Handles BLE requests:
  - **getNetworks**: Scans available WiFi networks and returns their details.
  - **connect**: Connects to a specified WiFi network.
  - **getData**: Fetches character data from a Harry Potter-themed API.
  - **getDetails**: Retrieves and formats detailed information for a selected character.
- Uses **HTTPClient** to fetch data from a public REST API.
- Sends all responses back via BLE notifications.

## Technologies Used

- **ESP32 (Arduino framework)**
- **Bluetooth Low Energy (BLE)**
- **WiFi (Station mode)**
- **ArduinoJson** for JSON handling
- **HTTPClient** for HTTP requests
- **Serial Monitor** for debug output

## Setup Instructions

1. **Hardware Requirements:**
   - ESP32 Development Board
   - Serial Monitor (Arduino IDE)

2. **Libraries Required:**
   - `ArduinoJson`
   - `BLEDevice` (part of ESP32 Arduino core)
   - `WiFi`, `HTTPClient`, `WiFiClientSecure`

3. **Usage:**
   - Flash the code to the ESP32.
   - Use a BLE scanner (e.g., *nRF Connect*) to connect to the ESP32 BLE server named `ESP32_BLE_A40`.
   - Send JSON-formatted commands to the BLE characteristic.

### Example BLE JSON Commands

#### Scan Networks
```json
{
  "action": "getNetworks",
  "teamId": "Team123"
}
```

#### Connect to WiFi
```json
{
  "action": "connect",
  "ssid": "YourWiFi",
  "password": "YourPassword"
}
```

#### Fetch Character List
```json
{
  "action": "getData"
}
```

#### Get Character Details
```json
{
  "action": "getDetails",
  "id": "123"
}
```

## Notes

- Make sure only one BLE client is connected at a time.
- The ESP32 resumes BLE advertising if disconnected.
- Ensure the WiFi credentials are correct; otherwise, the connection will fail silently.

## License

This project is intended for educational use only.
