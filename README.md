# Turn-by-turn Indoor Routing Engine (TIRE)

The # Turn-by-turn Indoor Routing Engine (TIRE) is a handheld navigation device used to provide real-time indoor guidance. It's primary focus is to serve students with visual impairments navigating new and complex environments such as a university campus. TIRE aims to promote user independence and navigational confidence by reducing a reliance on sighted guides or memorization of routes.

This project is being developed as a Senior Capstone for Computer Engineering Design at **California State University, San Bernardino**, under the guidance of Dr. Yunfei Hou.

---

## Project Overview
### 1. The Problem
One of the most common methods of user-positioning systems is a standard Global Positioning System (GPS). However, GPS becomes largely ineffective indoors due to the signal interference caused by the building's construction. The level of precision and accuracy for a user's location required for indoor navigation demands another solution. The common method of indoor navigation is the use of visual cues, signs indicating the range of room numbers in each hall, signs above each door to indicate location, and more. However, these visual cues are not useable by those with visual impairment. TIRE's solution is to provide a reliable indoor positioning and navigation system that does not require visual cues for operation.

### 2. The Solution
TIRE is a portable, battery-operated device that utilizes a hybrid approach to indoor localization. By implementing two user-positioning systems, Pedestrian Dead Reckoning (PDR) via an Inertial Measurement Unit (IMU) and Bluetooth Low Energy (BLE) Fingerprinting, and fusing their data, TIRE can achieve a high degree of accuracy and precision than what a more traditional singular implementation of either technology would be capable of. The user interacts with the device through a simple, tactile keypad and receives navigational instructions via a series of distinct audio cues. 

This project aligns with the United Nation's Sustainable Development Goal 10: Reduced Inequalities. With TIRE's core principle to reduce the accessibility gap that exists for students with visual impairment within their own campuses, it in doing so increases the inclusivity for students with disabilities. The design is specifically made to reduce the barriers users with disabilities might face, while providing a more equitable campus environment. TIRE's purpose to provide indoor navigation promotes the greater sense of independence in an area of life that is take for granted by many sighted students. Additionally, this project explicitly aligns with "AI for Social Good (AI4SG)" principles as it TIRE is a complex technological system to solve a distinctly human and social problem of inequality.

![This is the first prototype sketch for TIRE!](/documents/project-proposals/2025-10-7/user_interface_design.png)

---

## Key Features
- **Hybrid Indoor Positioning:** Fuses IMU-based PDR data with periodic Bluetooth Fingerprinting checks using an **Extended Kalman Filter (EKF)** to mitigate accumulated error and ensure high accuracy.

- **Tactile & Auditory Interface:** The device is designed to be completely operable without visual input. It's primary interface is a physical keypad for destination input along with a speaker module that will provide navigational guidance through a series of intuitive audio beeps and tones.

- **A\* Pathfinding:** Utilizes the A* search algorithm to efficiently calculate the optimal route to a destination through a pre-mapped navigation graph of Reference Points (RPs).

- **"Where Am I?" Function:** A dedicated button that allows the user to get an immediate audio announcement of their current location based on the nearest identifiable RP.

- **Portable & Self-Contained Design:** A handheld, battery-operated form factor designed for all-day use. TIRE Relies primarily on its own sensors and internalized map data to reduce dependence on external systems.

---

## Technology Stack & Architecture
### Hardware
- **Control Unit:** Raspberry Pi 4

- **Positioning:** Inertial Measurement Unit (IMU) & built-in Bluetooth module

- **User I/O:** 4x3 Keypad, Speaker Module, Slide Switch

- **Power:** Raspberry Pi Power Pack

### Software

- **Languages:** C++ (Primary), Python (for utility scripts)

- **Build System:** CMake (for cross-platform building and dependency management)

- **Core Algorithms:** Extended Kalman Filter (EKF), k-Nearest Neighbors (k-NN), A* Search Algorithm

- **Key Libraries:**

	- `nlohmann/json` for parsing JSON map files

    - `Eigen` for linear algebra and EKF matrix math

    - `wiringPi` for C++ GPIO hardware interface

    - `bluez` for interfacing with the Bluetooth module

- **Version Control:** Git & GitHub

### Software Architecture

The software is designed with a modular, object-oriented architecture in C++ to ensure a clear separation of concerns.

1. **Hardware Abstraction Layer (HAL):** An abstract base class, `HardwareInterface`, defines a common contract for all hardware interactions (e.g., `readIMU()`, `scanBLE()`, `playAudio()`). This allows the core logic to be identical whether it's running on a PC with `SimulatedHardware` or the Raspberry Pi with `RaspberryPiHardware`.

2. **Positioning Layer:** This layer contains the classes responsible for localization. It includes the PDR processor, the BLEFingerprinting module (which implements k-NN), and the EKF class that fuses their respective data into a single, reliable position estimate.

3. **Navigation Layer:** This layer manages map data and routing. The `NavigationGraph` class loads the JSON map file into memory, and the Pathfinder class uses the A* algorithm to find the optimal path between two Reference Points (RPs) on that graph.

4. **Guidance Layer:** The `Announcer` (or `GuidanceLogic`) class interprets the user's real-time position from the Positioning Layer and compares it against the planned route from the Navigation Layer to select and play the correct audio cues.

---

## :chart_with_upwards_trend: Project Status
### Current Status (As of November 6<sup>th</sup>, 2025): 🟢 Active Development

The project is currently in the planning and research phase including component sourcing, literature review, and algorithm design.

---

## Current Repository Map

```
/indoor-localization-device/      # Root directory for the entire TIRE project
│
├── .gitignore                    # Specifies files for Git to ignore (C++ build artifacts, logs, .vscode/, etc.)
├── CMakeLists.txt                # Root CMake file: defines the project and tells CMake to process the 'src/' directory
├── LICENSE                       # Contains the MIT License for the project
├── README.md                     # Main project page: describes the project, features, C++ tech stack, and build instructions
├── DEVELOPMENT-LOG.md            # Your weekly research journal for logging progress, challenges, and ideas
│
├── data/                         # Holds all non-code assets required by the application at runtimepathfinding algorithm
│   └── audio/                    # Directory for all audio guidance cues
│   └── maps/                     # Directory for all pre-mapped locations JSON files
│
├── documents/                    # Contains all formal research, planning, and project documentation
│   ├── project-proposals/        # Folder for project proposals
│   └── technical-briefs/         # Folder for detailed technical implementation documents
│
├── external/                     # Stores third-party libraries (e.g., as Git submodules)
│
├── src/                          # Contains all C++ source code for the project
│   │
│   ├── app/                      # Holds the main executable code
│   │   ├── CMakeLists.txt        # CMake file to build the 'tire' executable and link it against 'tire-lib'
│   │   └── main.cpp              # Main entry point: initializes hardware, loads the map, and starts the navigation loop
│   │
│   └── tire-lib/                 # The core TIRE logic, built as a reusable library
│       ├── CMakeLists.txt        # CMake file to define 'tire-lib' as a library and list its source files
│       │
│       ├── include/              # Public header files (.h, .hpp) that can be included by 'app'
│       │   └── tire/             # Namespace directory to prevent naming conflicts (e.g., #include "tire/Pathfinder.h")
│       │       ├── NavigationGraph.h     # Header for the class that loads and manages the map graph from JSON
│       │       ├── Pathfinder.h          # Header for the A* search algorithm implementation
│       │       ├── PDR.h                 # Header for Pedestrian Dead Reckoning (step counting, heading)
│       │       ├── BLEFingerprinting.h   # Header for k-NN logic to find the closest RP based on BLE signals
│       │       ├── EKF.h                 # Header for the Extended Kalman Filter to fuse PDR and BLE data
│       │       ├── Announcer.h           # Header for the module that selects which audio cue to play
│       │       │
│       │       └── interfaces/           # Sub-directory for hardware abstraction
│       │           ├── HardwareInterface.h   # Abstract base class defining all hardware functions (e.g., readIMU, playSound)
│       │           ├── SimulatedHardware.h   # Header for the PC-based simulation (fakes sensor data for testing)
│       │           └── RaspberryPiHardware.h # Header for the real Raspberry Pi implementation (interfaces with GPIO, I2C, etc.)
│       │
│       └── private/                  # Private source files (.cpp) containing the implementation details
│           ├── NavigationGraph.cpp   # Implementation for loading and managing the map graph
│           ├── Pathfinder.cpp        # Implementation of the A* algorithm
│           ├── PDR.cpp               # Implementation of the PDR step counting and heading logic
│           ├── BLEFingerprinting.cpp # Implementation of the k-NN matching algorithm
│           ├── EKF.cpp               # Implementation of the EKF data fusion math
│           ├── Announcer.cpp         # Implementation of the guidance logic
│           │
│           └── interfaces/           # Implementation of the hardware interfaces
│               ├── SimulatedHardware.cpp   # Implements the simulation class
│               └── RaspberryPiHardware.cpp # Implements the real Raspberry Pi hardware class
│
└── scripts/                        # Utility scripts (Python, Bash, etc.) to support the project
    └── map_creator.py              # Python script to help create new JSON map files
```

---

## Contributors (Group 5)
- Francisco Alvarez
- Alejandro Armenta
- Dominic Bertolo
- Carlos Gomez
- Josah Spann
- Isaac Spann

---

## Acknowledgements
This project is conducted as part of the CSUSB Computer Engineering program. Special thanks to our faculty advisor, **Dr. Yunfei Hou**, for his guidance and support.

---

## License

This project is licensed under the MIT License - see the [LICENSE](/LICENSE) file for more details.