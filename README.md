# Nyatank (ESP32 WiFi Controlled Car)
![License](https://img.shields.io/badge/License-Apache_2.0-yellow)
![Microcontroller](https://img.shields.io/badge/Microcontroller-ESP32-blue)
![Firmware](https://img.shields.io/badge/Firmware-C++-blue)
![IDE](https://img.shields.io/badge/IDE-Arduino-teal)
![GitHub stars](https://img.shields.io/github/stars/Ras-pe/Nyacar)
![GitHub forks](https://img.shields.io/github/forks/Ras-pe/Nyacar)
## 📖 Project Overview

This project consists of a **mobile robotic vehicle controlled remotely through a web interface**.
The system uses an **ESP32 microcontroller** that hosts a web server and communicates with a user’s device using **WiFi and HTTP requests**.

The main idea of the project is to control a mobile device **without using physical controllers**, relying only on a **web browser connected to the ESP32 network**.

The system demonstrates the integration of:

* Embedded systems
* Wireless communication
* Web interfaces
* Motor control
* Real-time hardware interaction

---

# 🎯 Objectives

## General Objective

Design and implement a **two-motor mobile vehicle controlled remotely through a web interface using HTTP communication**.

## Specific Objectives

* Implement stable **WiFi communication using ESP32**
* Design a **functional web interface** for movement commands
* Control **two DC motors using an H-bridge**
* Achieve **response times under 100 ms**
* Maintain **energy consumption under 500 mA during normal operation**

---

# ⚙️ System Specifications

## Functional Requirements

* Connect to a WiFi network or create an Access Point
* Host an internal **web server**
* Receive **HTTP movement commands**
* Control two DC motors
* Allow control from **any browser in the same network**

## Non-Functional Requirements

* Power supply between **7V – 12V**
* Communication through **HTTP protocol**
* Efficient use of **RAM and Flash memory**
* Compact design mounted on a **small robotic chassis**
* **Low implementation cost**

---

# 🧩 Hardware Design

## System Architecture

```
User (Web Browser)
        │
      HTTP
        │
      WiFi
        │
      ESP32
        │
      GPIO
        │
  H-Bridge (L298N)
      │      │
   Motor A  Motor B
      │      │
     Wheels
```

---

## Hardware Components

| Component          | Description                    |
| ------------------ | ------------------------------ |
| ESP32              | Main microcontroller with WiFi |
| L298N Motor Driver | Controls two DC motors         |
| DC Motors          | Provide movement               |
| Robotic Chassis    | Mechanical structure           |
| Battery Pack       | Power supply                   |
| Wheels             | Mobility                       |

---

## Motor Driver Connections

Typical connections between the **ESP32 and L298N**:

| L298N Pin | ESP32 Function    |
| --------- | ----------------- |
| IN1       | Motor A Direction |
| IN2       | Motor A Direction |
| IN3       | Motor B Direction |
| IN4       | Motor B Direction |
| ENA       | PWM speed control |
| ENB       | PWM speed control |

Additional requirements:

* Common **GND between ESP32 and L298N**
* External power supply for motors
* Capacitors to reduce electrical noise

---

# 💻 Software Design (Firmware)

The firmware runs directly on the **ESP32** and implements a simple **HTTP web server**.

## Main Operation Flow

```
Start System
     │
Initialize WiFi
     │
Create Web Server
     │
Wait for HTTP Request
     │
Parse Command
     │
Activate Motor Pins
     │
Repeat Loop
```

---

## Firmware Modules

### WiFi Module

Handles the network connection and creates the WiFi Access Point.

### HTTP Server

Receives commands from the web interface such as:

```
/forward
/backward
/left
/right
/stop
```

### Motor Control

Controls the motors using **GPIO pins and PWM signals**.

### Main Loop

Continuously monitors incoming HTTP requests.

---

# 🌐 Network Configuration

Example network created by the ESP32:

```
SSID: ESP32-CAR
Password: 12345678
IP Address: 192.168.4.1
```

To control the car:

1. Connect your phone or computer to the WiFi network
2. Open a browser
3. Navigate to:

```
http://192.168.4.1
```

---

# 🎮 Web Control Interface

The interface contains movement buttons such as:

```
[ Forward ]
[ Backward ]
[ Left ]
[ Right ]
[ Stop ]
```

Each button sends an **HTTP request** to the ESP32 which immediately activates the motors.

---

# 🧪 Testing and Results

## Test Cases

| HTTP Command | Expected Result | Status    |
| ------------ | --------------- | --------- |
| /forward     | Move forward    | ✅ Correct |
| /backward    | Move backward   | ✅ Correct |
| /left        | Turn left       | ✅ Correct |
| /right       | Turn right      | ✅ Correct |
| /stop        | Stop motors     | ✅ Correct |

---

## Performance Measurements

| Metric            | Result       |
| ----------------- | ------------ |
| Response Time     | ~80 ms       |
| Power Consumption | 400 – 600 mA |

The system maintained **stable communication and reliable motor control** within the local network.

---

# ⚠️ Issues Encountered

### Power Interference

Electrical noise caused unstable behavior.

Solution:

* Added **capacitors to stabilize voltage**

### Current Spikes

Motors caused power spikes that restarted the ESP32.

Solution:

* Used a **separate power supply for motors**

---

# 🚀 Possible Future Improvements

* Add **camera streaming using ESP32-CAM**
* Implement **WiFi authentication**
* Add **distance sensors**
* Add **autonomous navigation**
* Implement **mobile-friendly UI**
* Add **Bluetooth control**

---

# 💰 Bill of Materials (BOM)

| Component | Quantity | Approximate Cost |
| --------- | -------- | ---------------- |
| ESP32     | 1        | $150 MXN         |
| L298N     | 1        | $80 MXN          |
| DC Motors | 2        | $120 MXN         |
| Batteries | 1        | $100 MXN         |

---

# 📚 Learning Outcomes

This project demonstrates concepts in:

* Embedded systems
* Microcontroller programming
* Wireless communication
* Robotics
* Web servers on microcontrollers
* Internet of Things (IoT)


---

# 🔗 Repository

Project repository:

https://github.com/Ras-pe/Nyacar
