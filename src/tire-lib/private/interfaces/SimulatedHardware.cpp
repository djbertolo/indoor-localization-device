#include "tire/interfaces/SimulatedHardware.h"
#include <iostream>     // For std::cout (simulating audio, initialization)
#include <chrono>       // For std::chrono (simulating time delays)
#include <thread>       // For std::this_thread::sleep_for (simulating delays)

namespace tire {
	namespace interfaces {

		// Constructor
		SimulatedHardware::SimulatedHardware() : simulated_gyroscope_angle(0.0) {
			// Initialize simulation-specific variables
			std::cout << "[SimulatedHardware] Simulation created." << std::endl;
		}

		// Destructor
		SimulatedHardware::~SimulatedHardware() {
			std::cout << "[SimulatedHardware] Simulation destroyed." << std::endl;
		}

		// initialize()
		bool SimulatedHardware::initialize() {
			std::cout << "[SimulatedHardware] Initializing fake hardware... OK." << std::endl;
			// In a real class, this would be where you set up GPIO, I2C, etc.
			return true;
		}

		// readIMU()
		IMUData SimulatedHardware::read_IMU() {
			// "Fake" the data. Let's pretend the user is walking straight.
			simulated_gyroscope_angle += 0.01; // Simulate slight rotational drift
			
			IMUData fakeData;
			fakeData.acceleration_x= 0.98; // Simulating gravity on one axis + some movement
			fakeData.acceleration_y = 0.05; // Simulating minor side-to-side movement
			fakeData.acceleration_z = 0.1;  // Simulating forward step acceleration
			
			fakeData.gyroscope_x = 0.0;
			fakeData.gyroscope_y = 0.0;
			fakeData.gyroscope_z = simulated_gyroscope_angle; // Use our changing variable

			// We don't need to print here, as this function will be called very rapidly.
			return fakeData;
		}

		// scan_BLE()
		std::vector<BLEBeaconData> SimulatedHardware::scan_BLE() {
			std::cout << "[SimulatedHardware] Simulating BLE scan (will take 1 sec)..." << std::endl;
			
			// Simulate the time it takes to perform a scan
			std::this_thread::sleep_for(std::chrono::seconds(1));

			// Create a fake, hard-coded list of beacons
			std::vector<BLEBeaconData> fakeBeacons;
			fakeBeacons.push_back(
				{
					"BEACON_ID_1",
					-55
				}
			); // Beacon 1 is close
			fakeBeacons.push_back(
				{
					"BEACON_ID_2",
					-78
				}
			); // Beacon 2 is further away
			fakeBeacons.push_back(
				{
					"BEACON_ID_3",
					-62
				}
			); // Beacon 3 is nearby

			std::cout << "[SimulatedHardware] Scan complete. Found " << fakeBeacons.size() << " beacons." << std::endl;
			return fakeBeacons;
		}

		// get_key_press()
		KeyPress SimulatedHardware::get_key_press() {
			// TODO: build a more complex simulation that pushes key presses into a queue from another thread.
			return KeyPress::KEY_NONE;
		}

		// play_audio()
		void SimulatedHardware::play_audio(const std::string& audio_cue_name) {
			// Simulate playing audio by printing to the console
			std::cout << "[SimulatedHardware] Playing audio cue: '" << audio_cue_name << ".wav'" << std::endl;
		}

		// is_power_switch_on()
		bool SimulatedHardware::is_power_switch_on() {
			// Simulate that the switch is always on
			return true;
		}

	} // namespace interfaces
} // namespace tire