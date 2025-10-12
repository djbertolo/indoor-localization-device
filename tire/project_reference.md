```
/tire-project/
├── data/
│   ├── jack_brown_hall.json         # Map file with RP coordinates and connections
│   └── audio/
│       ├── turn_left.mp3
│       └── arrival.mp3
│
├── tire/
│   ├── __main__.py                  # Main application entry point
│   │
│   ├── navigation/
│   │   ├── graph.py                 # Loads and manages the navigation graph from JSON
│   │   └── pathfinder.py            # Contains the A* search algorithm implementation
│   │
│   ├── positioning/
│   │   ├── pdr.py                   # Logic for Pedestrian Dead Reckoning (step counting, etc.)
│   │   ├── ble_fingerprinting.py    # Logic for k-NN matching of BLE signals
│   │   └── ekf.py                   # Extended Kalman Filter for data fusion
│   │
│   ├── guidance/
│   │   └── announcer.py             # Determines which audio cue to play based on user state
│   │
│   └── interfaces/
│       ├── hardware_interface.py    # Defines the abstract base class for all hardware
│       ├── simulated_hardware.py    # Fakes sensor data for testing on a PC
│       └── raspberry_pi_hardware.py # Real hardware implementation for the Pi
│
├── scripts/
│   └── map_creator.py               # Utility script to help create the JSON map file
│
└── tests/
    ├── test_pathfinder.py           # Unit tests for the A* algorithm
    ├── test_graph.py                # Unit tests for loading and querying the map
    └── test_ekf.py                  # Unit tests for the EKF logic with sample data
```