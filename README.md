# Turn-by-turn Indoor Routing Engine (TIRE)

The # Turn-by-turn Indoor Routing Engine (TIRE) is a handheld navigation device used to provide real-time indoor guidance. It's primary focus is to serve students with visual impairments navigating new and complex environments such as a university campus. TIRE aims to promote user independence and navigational confidence by reducing a reliance on sighted guides or memorization of routes.

This project is being developed as a Senior Capstone for Computer Engineering Design at **California State University, San Bernardino**, under the guidance of Dr. Yunfei Hou.

---

## 🎯 Project Overview
### 1. The Problem
One of the most common methods of user-positioning systems is a standard Global Positioning System (GPS). However, GPS becomes largely ineffective indoors due to the signal interference caused by the building's construction. The level of precision and accuracy for a user's location required for indoor navigation demands another solution. The common method of indoor navigation is the use of visual cues, signs indicating the range of room numbers in each hall, signs above each door to indicate location, and more. However, these visual cues are not useable by those with visual impairment. TIRE's solution is to provide a reliable indoor positioning and navigation system that does not require visual cues for operation.

### 2. The Solution
TIRE is a portable, battery-operated device that utilizes a hybrid approach to indoor localization. By implementing two user-positioning systems, Pedestrian Dead Reckoning (PDR) via an Inertial Measurement Unit (IMU) and Bluetooth Low Energy (BLE) Fingerprinting, and fusing their data, TIRE can achieve a high degree of accuracy and precision than what a more traditional singular implementation of either technology would be capable of. The user interacts with the device through a simple, tactile keypad and receives navigational instructions via a series of distinct audio cues. 

This project aligns with the United Nation's Sustainable Development Goal 10: Reduced Inequalities. With TIRE's core principle to reduce the accessibility gap that exists for students with visual impairment within their own campuses, it in doing so increases the inclusivity for students with disabilities. The design is specifically made to reduce the barriers users with disabilities might face, while providing a more equitable campus environment. TIRE's purpose to provide indoor navigation promotes the greater sense of independence in an area of life that is take for granted by many sighted students. Additionally, this project explicitly aligns with "AI for Social Good (AI4SG)" principles as it TIRE is a complex technological system to solve a distinctly human and social problem of inequality.

![This is the first prototype sketch for TIRE!](/documents/project-proposal/user_interface_design.png)

---

## ✨ Key Features
- **Hybrid Indoor Positioning:** Fuses IMU-based PDR data with periodic Bluetooth Fingerprinting checks using an **Extended Kalman Filter (EKF)** to mitigate accumulated error and ensure high accuracy.

- **Tactile & Auditory Interface:** The device is designed to be completely operable without visual input. It's primary interface is a physical keypad for destination input along with a speaker module that will provide navigational guidance through a series of intuitive audio beeps and tones.

- **A\* Pathfinding:** Utilizes the A* search algorithm to efficiently calculate the optimal route to a destination through a pre-mapped navigation graph of Reference Points (RPs).

- **"Where Am I?" Function:** A dedicated button that allows the user to get an immediate audio announcement of their current location based on the nearest identifiable RP.

- **Portable & Self-Contained Design:** A handheld, battery-operated form factor designed for all-day use. TIRE Relies primarily on its own sensors and internalized map data to reduce dependence on external systems.

---

## :hammer_and_wrench: Technology Stack & Architecture
### Hardware
- **Control Unit:** Raspberry Pi 4

- **Positioning:** Inertial Measurement Unit (IMU) & built-in Bluetooth module

- **User I/O:** 4x3 Keypad, Speaker Module, Slide Switch

- **Power:** Raspberry Pi Power Pack

### Software
- **Languages:** Python (Primary), C/C++ (for performance-critical sensor processing)

- **Core Algorithms:** Extended Kalman Filter (EKF), k-Nearest Neighbors (k-NN), A* Search Algorithm

- **Key Libraries:** `NumPy` (for EKF/k-NN math), `RPi.GPIO` (for hardware interface), `BluePy` (for BLE detection)

- **Version Control:** Git & GitHub

### Software Architecture
The software is designed with a modular, multi-layered architecture:
1. **Data Acquisition Layer:** Collects raw data from the IMU and Bluetooth scanner in a non-blocking manner.

2. **Positioning Layer:** Contains the PDR Processor, Bluetooth Fingerprinting Processor, and the EKF Fuser to calculate the user's real-time position and heading.

3. **Navigation Layer:** Implements the A* Pathfinding Engine and Guidance Logic to generate and deliver audio cues to the user.

---

## :chart_with_upwards_trend: Project Status
### Current Status (As of October 8<sup>th</sup>, 2025): 🟢 Active Development

The project is currently in the planning and research phase including component sourcing, literature review, and algorithm design.

---

## 👥 Contributors (Group 5)
- Francisco Alvarez
- Alejandro Armenta
- Dominic Bertolo
- Carlos Gomez
- Josah Spann
- Isaac Spann

---

## 🙏 Acknowledgements
This project is conducted as part of the CSUSB Computer Engineering program. Special thanks to our faculty advisor, **Dr. Yunfei Hou**, for his guidance and support.

---

📄 License

This project is licensed under the MIT License - see the [LICENSE](/LICENSE) file for more details. 