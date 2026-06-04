# System Walkthrough (Responsive Mobile UI)

This document outlines the complete codebase architecture, deployment instructions, and physical wiring guidelines for the **IoT Inauguration Curtain Control System** created for **Bhu Pu Sainik Rising Secondary English School**.

## Mobile Responsiveness & Tactile UI Updates

Based on the latest requirements, we enhanced the mobile viewport behavior and visual details to make it highly impressive and optimized for phone browsers:

- **Eliminated Tap Latency**: Integrated CSS `touch-action: manipulation;` on all tactile buttons to instantly trigger actions without waiting for the default mobile 300ms tap delay.
- **Large Touch targets**: Increased range slider tracks and button dimensions (comfortable `20px` vertical paddings and large `22px` slider thumbs) for convenient thumb adjustments.
- **Responsive Stacking Layout**: Under `410px` screen widths, the button grid automatically stacks vertically into full-width tap targets, and settings collapse into single-column layouts.
- **Live Progress & Timer**: Added a live progress bar that animates smoothly during curtain movement, accompanied by a dynamic readout of completion percentage and remaining seconds (e.g. `4.5s remaining`).
- **LED Pulse Indicator**: Added a visual status indicator dot that changes colors and patterns based on current hardware states:
  - **IDLE**: Solid deep green.
  - **OPENING**: Pulsing bright blue.
  - **CLOSING**: Pulsing warm orange.
  - **STOPPED**: Fast flashing safety red.
- **Ceremonial Countdown Box**: The fullscreen countdown delay now renders in a double-bordered gold frame, making it look highly formal and ceremonial.

---

## File Structure

1. **[parda.ino](file:///d:/parda/parda.ino)**:
   - Configures the ESP32 as a standalone WiFi Access Point (`SSID: INAUGURATION-BHU-PU`, `Password: 12345678`).
   - Runs a local DNS Server redirecting all DNS lookups to `192.168.4.1` (Captive Portal mode).
   - Manages a non-blocking motor state machine (`IDLE`, `OPENING`, `CLOSING`, `STOPPED`) using `millis()` to ensure responsiveness to `/stop` commands.
   - Saves movement duration parameters persistently in Flash memory using the `Preferences` library.
   - Standard WebServer serving status queries (`/status`), execution requests (`/open`, `/close`, `/stop`), and duration configs (`/set_duration`).

2. **[html.h](file:///d:/parda/html.h)**:
   - Contains the redesigned minimal web template string stored in ESP32 Program Memory (`PROGMEM`).

---

## Hardware Wiring Guide

To deploy the physical system, wire the ESP32 and L298N driver according to the following layout:

| ESP32 Pin | L298N Pin | Description |
| :--- | :--- | :--- |
| **GPIO 26** | **IN1** | Motor A Direction Control |
| **GPIO 27** | **IN2** | Motor A Direction Control |
| **GPIO 14** | **IN3** | Motor B Direction Control |
| **GPIO 12** | **IN4** | Motor B Direction Control |
| **GPIO 25** | **ENA** | Motor A Speed/Enable (Remove Jumper) |
| **GPIO 33** | **ENB** | Motor B Speed/Enable (Remove Jumper) |
| **GND** | **GND** | Common Ground |

> [!CAUTION]
> **Common Ground Requirement**
> Ensure the Ground (GND) pin of the ESP32 is tied directly to the L298N Ground (GND) terminal. Without a common ground, the digital control lines will carry floating voltages, causing unpredictable motor stutter or failure to trigger.

---

## Deployment & Usage Instructions

### 1. Compile & Flash
- Open Arduino IDE on your PC.
- Place `parda.ino` and `html.h` in the same directory named `parda`.
- Select **ESP32 Dev Module** as your target board.
- Click **Upload** to flash the code to the ESP32.

### 2. Connect to the Hotspot
- Connect your device to the **`INAUGURATION-BHU-PU`** WiFi network using password **`12345678`**.
- Open your browser and navigate to **`192.168.4.1`**.

### 3. Operation
- **Open Curtain**: Click **Open Curtain**. If "Ceremonial 3s Delay" is checked, a fullscreen countdown counts down from 3 to 1 before initiating the motor.
- **Timing Adjustment**: Drag the **Movement Duration** slider. Changes take effect instantly and survive board reboots.
- **Emergency Pause**: Click the **Emergency Stop** button to immediately cease motor control output.
