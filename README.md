# Weather Dashboard with ESP8266

## Overview

A simple IoT-based weather dashboard using an ESP8266. The dashboard displays real-time weather data fetched from OpenWeatherAPI, along with local temperature readings received via MQTT. Time synchronization is handled via NTP, and all the data is displayed on an OLED screen.

## Features

- Real-time weather information for a specific city
- Local temperature readings via MQTT
- NTP-based time synchronization
- OLED display for showing time and weather details

## Hardware Requirements

- ESP8266-based microcontroller
- OLED display (128x64)
- Some MQTT broker for retrieving local readings

## Software Requirements

- Arduino IDE with ESP8266 support
- Adafruit GFX Library
- Adafruit SSD1306 Library
- ArduinoJson Library
- PubSubClient Library
- NTPClient Library

## Setup Instructions

1. **Install Libraries**: Make sure to install all the required libraries through the Arduino Library Manager.
2. **WiFi and API Setup**: Update the `ssid`, `password`, `apiKey`, and `cityID` in the Arduino sketch.
3. **MQTT Broker**: If you have a local MQTT broker, update the `mqttServer`, `mqttPort`, `mqttUser`, and `mqttPassword` in the Arduino sketch.
4. **Compile and Upload**: After making the necessary changes, compile and upload the sketch to your ESP8266.
5. **Hardware Connections**: Connect the OLED display to your ESP8266 according to your setup.
6. **Run**: Power up your ESP8266 and view real-time weather and time details on the OLED display.

## Future Improvements

- Implement automatic city selection based on IP geolocation
- Add support for multiple MQTT topics
- Improve error handling for MQTT and API failures

## License

This project is open-sourced under GPL V3. See the LICENSE.md file for details.

---
