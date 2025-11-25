#ifndef TIRE_INTERFACES_SIMULATED_HARDWARE_H
#define TIRE_INTERFACES_SIMULATED_HARDWARE_H

// Include the abstract base class we are implementing
#include "tire-lib/include/tire/interfaces/HardwareInterface.h"
#include <vector>   // For std::vector
#include <string>   // For std::string

namespace tire {
	namespace interfaces {

		/**
		 * @class SimulatedHardware
		 * @brief A concrete implementation of HardwareInterface for PC-based simulation.
		 *
		 * This class fakes hardware interactions, such as IMU data, BLE scans,
		 * and keypad presses. It is used for developing and testing the core TIRE
		 * library on a host machine without requiring the actual Raspberry Pi hardware.
		 */
		class SimulatedHardware : public HardwareInterface {
		public:
			/**
			 * @brief Constructor for SimulatedHardware.
			 */
			SimulatedHardware();

			/**
			 * @brief Virtual destructor.
			 */
			virtual ~SimulatedHardware() override;

			// --- Overridden HardwareInterface Functions ---

			/**
			 * @brief Simulates initializing hardware. Prints to the console.
			 * @return Always returns true.
			 */
			virtual bool initialize() override;

			/**
			 * @brief Simulates reading from the IMU.
			 * @return A dummy IMUData struct (e.g., all zeros or a fixed pattern).
			 */
			virtual IMUData read_IMU() override;

			/**
			 * @brief Simulates scanning for BLE beacons.
			 * @return A hard-coded vector of fake BLEBeaconData.
			 */
			virtual std::vector<BLEBeaconData> scan_BLE() override;

			/**
			 * @brief Simulates a keypad press by reading from the console.
			 * (Note: This is a simple simulation. A real app might use a GUI).
			 * For now, we'll return KEY_NONE.
			 * @return A KeyPress enum value.
			 */
			virtual KeyPress get_key_press() override;

			/**
			 * @brief Simulates playing an audio cue by printing to the console.
			 * @param audio_cue_name The identifier (e.g., "turn_left") to be printed.
			 */
			virtual void play_audio(const std::string& audio_cue_name) override;

			/**
			 * @brief Simulates the power switch state.
			 * @return Always returns true (simulating that the device is on).
			 */
			virtual bool is_power_switch_on() override;

		private:
			// Add any private helper functions or member variables needed
			// for simulation here. For example, a counter for faking IMU data.
			double simulated_gyroscope_angle;
		};

	} // namespace interfaces
} // namespace tire

#endif // TIRE_INTERFACES_SIMULATED_HARDWARE_H