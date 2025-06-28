# 🌱 Smart Irrigation System using Arduino & NodeMCU

This is a smart irrigation solution that automates and optimizes water usage by using sensor data, weather forecasts, and AI-based decision-making. Built with Arduino Uno, NodeMCU ESP8266, and a Node.js backend, the system supports both automatic and manual irrigation control.

---

## 🚀 Features

- **Real-Time Data Collection**  
  Collects data from:
  - Soil moisture sensor
  - Water level sensor
  - Temperature & humidity sensors

- **Wireless Data Transmission**  
  - Arduino processes sensor data
  - Sends data to NodeMCU via serial
  - NodeMCU communicates with a Node.js server using WebSocket

- **Smart Decision Making**  
  - Fetches weather forecasts using Weather API
  - AI automation mode decides pump operation based on:
    - Soil moisture
    - Weather forecast
  - Manual override available via web app

- **Irrigation Activation**  
  - Relay-controlled water pump
  - Pump starts only if soil is dry and no rain is expected

- **Web Dashboard & Logging**  
  - Live monitoring of sensor values and pump status
  - All data and actions are logged for performance tracking

---

## 🛠️ Technologies Used

- **Microcontrollers:** Arduino Uno, NodeMCU ESP8266  
- **Server:** Node.js with WebSocket  
- **Sensors:** Soil moisture, water level, temperature, humidity  
- **Relay module** for pump control  
- **Weather API** for forecast data  
- **Web Interface** for user interaction  
