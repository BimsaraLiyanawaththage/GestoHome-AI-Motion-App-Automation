# GestoHome-AI-Motion-App-Automation
A complete IoT home automation project using AI-based gesture recognition, PIR motion detection, and mobile app MQTT control. Built with Python OpenCV + MediaPipe, ESP32, and EMQX MQTT broker.
# Smart Room Automation System
### Motion Detection + MQTT Control + Hand Gesture Control (OpenCV + Mediapipe)

This project combines **IoT automation**, **gesture recognition**, and **sensor-based control** to create an intelligent room system.  
It uses an **ESP32**, **PIR motion sensor**, **Relays**, **MQTT**, and a **Python-based hand gesture controller**.

---

## 🚀 Features

### 🔹 1. Motion-Based Automatic Lighting (ESP32)
- Detects human presence using PIR sensor
- Automatically turns ON the light when motion is detected
- After a selected timeout (ex: 10s), if **no motion**, sends:
  - `"NO_MOTION"` alert via MQTT  
  - Does NOT turn off the light immediately ; waits for user decision  
- Prevents discomfort by **avoiding unwanted auto-off**

### 🔹 2. Manual Override Controls (MQTT App)
Using IoT MQTT Panel:
- **Force Turn ON Light**
- **Force Turn OFF Light**
- **Turn ON/OFF Fan**
- **Reset System** (reenable motion detection)

### 🔹 3. Hand Gesture Control (Python + Mediapipe)
| Gesture | Action | MQTT Topic |
|--------|--------|------------|
| 5 fingers (Open Hand) | Light ON | `home/room1/light/on` |
| 0 fingers (Fist) | Light OFF | `home/room1/light/off` |
| 1 finger | Reset System | `home/room1/system/reset` |
| 2 fingers | Fan ON | `home/room1/fan/on` |
| 3 fingers | Fan OFF | `home/room1/fan/off` |

Uses:
- Webcam
- OpenCV
- Mediapipe Hand Landmarks
- MQTT client

### 🔹 4. Hybrid System (AI + IoT)
- Motion sensor gives **automatic** control  
- Hand gestures/mobile app give **manual overrides**  
- Physical wall switches still work (failsafe)  
- User can disable auto system & re-enable anytime  


## System Workflow

### **1. Normal Mode**
- Motion detected → Light ON  
- Motion stops → Waits 10 seconds  
- Sends Alert → User decides ON / OFF  

### **2. Manual Override**
- If user presses **Light ON/OFF** or uses gesture →  Motion system pauses temporarily (autoMode = false)

### **3. Reset Mode**
- User sends RESET (gesture/app) →  Auto system activates again


## 📡 Hardware Used
- ESP32 Devkit
- PIR Sensor (HW-416-B)
- Relay module
- LED bulbs
- WiFi connection


## 🛠 Software Used
- Arduino IDE  
- Python (OpenCV, Mediapipe, Paho MQTT)  
- IoT MQTT Panel mobile app  
- EMQX public broker


## Why This System Is Unique
- Avoids the common problem of motion sensors turning lights OFF while user is still in room  
- Combines **AI + IoT + Sensors**   
- Future-ready and scalable 
