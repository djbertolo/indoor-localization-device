#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>

// Include TIRE Library Headers
#include "tire/interfaces/SimulateHardware.h"
#include "tire/interfaces/RaspberryPiHardware.h" // Only compiles on Linux usually, but we include for logic
#include "tire/NavigationGraph.h"
#include "tire/PDR.h"
#include "tire/BLEFingerprinting.h"
#include "tire/EKF.h"
#include "tire/Pathfinder.h"
#include "tire/Announcer.h"

using namespace tire;

// Configuration Flags
// Set to false to use real hardware (requires running on RPi with wiringPi)
const bool USE_SIMULATION = true; 

int main() {
    std::cout << "=============================================" << std::endl;
    std::cout << "   TIRE: Turn-by-turn Indoor Routing Engine  " << std::endl;
    std::cout << "=============================================" << std::endl;

    // --- 1. Hardware Setup ---
    std::unique_ptr<interfaces::HardwareInterface> hw;

    if (USE_SIMULATION) {
        std::cout << "[Main] Mode: SIMULATION" << std::endl;
        hw = std::make_unique<interfaces::SimulatedHardware>();
    } else {
        std::cout << "[Main] Mode: RASPBERRY PI HARDWARE" << std::endl;
        // Note: If building on Windows/Mac, this header might fail if not guarded.
        // Ideally, use CMake to conditionally compile this file.
        hw = std::make_unique<interfaces::RaspberryPiHardware>();
    }

    if (!hw->initialize()) {
        std::cerr << "[Main] Critical Error: Hardware initialization failed." << std::endl;
        return -1;
    }

    // --- 2. Module Initialization ---
    
    // Load Map
    NavigationGraph graph;
    if (!graph.load_from_json("data/maps/campus_map.json")) {
        std::cerr << "[Main] Failed to load map. Exiting." << std::endl;
        return -1;
    }

    // Initialize Algorithms
    PDR pdr;
    pdr.initialize();

    BLEFingerpinting ble_fp(3); // k=3
    if (!ble_fp.load_map("data/maps/campus_radio_map.json")) {
         std::cerr << "[Main] Failed to load radio map." << std::endl;
    }

    EKF ekf;
    // Initialize EKF at a default start (e.g., Lobby: 0,0, North)
    // In a real system, we might use the first BLE scan to set this.
    ekf.initialize(0.0, 0.0, 0.0); 

    Pathfinder pathfinder;
    Announcer announcer;

    // --- 3. State Variables ---
    bool is_navigating = false;
    std::vector<std::string> current_path;
    std::string current_destination_id = "";

    // Loop Timing
    auto last_time = std::chrono::steady_clock::now();

    // --- 4. Main Loop ---
    std::cout << "[Main] System Ready. Waiting for input..." << std::endl;

    while (hw->is_power_switch_on()) {
        // Calculate Delta Time
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - last_time;
        double dt = elapsed.count();
        last_time = now;

        // A. Read Sensors
        auto imu_data = hw->read_IMU();
        auto key_press = hw->get_key_press();
        
        // B. Process User Input
        if (key_press != interfaces::KeyPress::KEY_NONE) {
            switch (key_press) {
                case interfaces::KeyPress::KEY_WHERE_AM_I:
                {
                    std::cout << "[Main] Input: Where Am I?" << std::endl;
                    // Force BLE scan to find closest RP
                    auto scan = hw->scan_BLE();
                    Position2D pos = ble_fp.find_closest_position(scan);
                    // Simple update to EKF to snap to this location
                    ekf.update(pos); 
                    hw->play_audio("location_update");
                    break;
                }
                case interfaces::KeyPress::KEY_START_NAVIGATION:
                    std::cout << "[Main] Input: Start Navigation" << std::endl;
                    // Hardcoded destination for prototype: "RP_HALLWAY_END"
                    current_destination_id = "RP_HALLWAY_END";
                    
                    // Find closest start node (naive: just iterate distances)
                    {
                        Eigen::Vector3d state = ekf.get_state();
                        std::string start_id = "RP_HALLWAY_START"; // Default fallback
                        double min_dist = 99999.0;
                        
                        for(const auto& pair : graph.get_all_nodes()) {
                            double dx = pair.second.position.x - state(0);
                            double dy = pair.second.position.y - state(1);
                            double d = std::sqrt(dx*dx + dy*dy);
                            if(d < min_dist) {
                                min_dist = d;
                                start_id = pair.first;
                            }
                        }

                        current_path = pathfinder.find_path(graph, start_id, current_destination_id);
                        if (!current_path.empty()) {
                            is_navigating = true;
                            announcer.reset();
                            hw->play_audio("navigation_started");
                        } else {
                            hw->play_audio("error_no_path");
                        }
                    }
                    break;
                default:
                    // Handle keycode entry (omitted for brevity)
                    break;
            }
        }

        // C. Positioning Pipeline
        
        // 1. PDR (High Frequency)
        pdr.process_IMU_data(imu_data, dt);
        PDRState pdr_update = pdr.get_pdr_update();
        
        // 2. EKF Prediction
        ekf.predict(pdr_update);

        // 3. BLE Correction (Low Frequency)
        // In real life, scan takes time, so we might do this async or every N seconds.
        // Here we simulate a check every 5 seconds.
        static double ble_timer = 0.0;
        ble_timer += dt;
        if (ble_timer > 5.0) {
            auto scan = hw->scan_BLE();
            if (!scan.empty()) {
                Position2D ble_pos = ble_fp.find_closest_position(scan);
                ekf.update(ble_pos);
                // std::cout << "[Main] BLE Correction Applied" << std::endl;
            }
            ble_timer = 0.0;
        }

        // D. Navigation & Guidance
        if (is_navigating) {
            Eigen::Vector3d current_state = ekf.get_state();
            int next_idx = announcer.update(current_state, current_path, graph, *hw);
            
            if (next_idx == -1 && current_path.size() > 0) {
                // Path finished or Announcer returned "arrived" state
                // If we want to auto-clear path:
                // is_navigating = false;
                // current_path.clear();
            }
        }

        // E. Sleep to maintain loop rate (e.g., ~50Hz)
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::cout << "[Main] Power Switch OFF. Shutting down." << std::endl;
    return 0;
}