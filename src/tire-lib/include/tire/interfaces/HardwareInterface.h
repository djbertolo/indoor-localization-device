#ifndef TIRE_INTERFACES_HARDWARE_INTERFACE_H
#define TIRE_INTERFACES_HARDWARE_INTERFACE_H

#include <vector>
#include <string>
#include <cstdint> // For fixed-width integer types like uint8_t

namespace tire {
	namespace interfaces {

		/**
		 * @struct IMUData
		 * @brief Holds raw data from a single Inertial Measurement Unit reading.
		 * Data is from the accelerometer and gyroscope.
		 */
		struct IMUData {
			// Accelerometer data (x, y, z axes)
			double acceleration_x, acceleration_y, acceleration_z;
			// Gyroscope data (x, y, z axes)
			double gyroscope_x, gyroscope_y, gyroscope_z;
		};

		/**
		 * @struct BLEBeaconData
		 * @brief Holds data from a single detected Bluetooth Low Energy (BLE) beacon.
		 * Contains the beacon's unique ID and its signal strength (RSSI).
		 */
		struct BLEBeaconData {
			std::string id;  // The unique identifier (e.g., MAC address)
			int rssi;        // Received Signal Strength Indicator
		};

		/**
		 * @enum KeyPress
		 * @brief Represents all possible button presses from the user interface.
		 */
		enum class KeyPress {
			// Keycode selection buttons
			KEYCODE_COLUMN_1_UP,
			KEYCODE_COLUMN_1_DOWN,
			KEYCODE_COLUMN_2_UP,
			KEYCODE_COLUMN_2_DOWN,
			KEYCODE_COLUMN_3_UP,
			KEYCODE_COLUMN_3_DOWN,
			KEYCODE_COLUMN_4_UP,
			KEYCODE_COLUMN_4_DOWN,

			// Function buttons
			KEY_START_NAVIGATION,                 // "Start Navigation" button
			KEY_WHERE_AM_I,         // "Where Am I?" button
			KEY_CURRENT_SELECTION,  // "Current Selection" button

			KEY_NONE                // No key pressed
		};

		/**
		 * @class HardwareInterface
		 * @brief Abstract base class defining the contract for all hardware interactions.
		 *
		 * This class provides a pure virtual interface for the core TIRE library.
		 * Concrete implementations (like RaspberryPiHardware or SimulatedHardware)
		 * will provide the actual logic to interact with real or faked hardware.
		 */
		class HardwareInterface {
		public:
			// Virtual destructor is required for base classes
			virtual ~HardwareInterface() = default;

			/**
			 * @brief Initializes the hardware components.
			 * @return true if initialization is successful, false otherwise.
			 */
			virtual bool initialize() = 0;

			/**
			 * @brief Reads the latest data packet from the IMU.
			 * @return An IMUData struct containing accelerometer and gyroscope readings.
			 */
			virtual IMUData read_IMU() = 0;

			/**
			 * @brief Performs a scan for nearby BLE beacons.
			 * @return A vector of BLEBeaconData structs, one for each beacon found.
			 */
			virtual std::vector<BLEBeaconData> scan_BLE() = 0;

			/**
			 * @brief Checks for and returns the latest key pressed on the keypad.
			 * This should be non-blocking.
			 * @return A KeyPress enum value. Returns KEY_NONE if no key was pressed.
			 */
			virtual KeyPress get_key_press() = 0;

			/**
			 * @brief Plays an audio cue through the speaker.
			 * @param audio_cue_name The identifier for the audio file to be played 
			 * (e.g., "turn_left", "arrived").
			 */
			virtual void play_audio(const std::string& audio_cue_name) = 0;

			/**
			 * @brief Checks the state of the main power switch.
			 * @return true if the device is on, false if off.
			 */
			virtual bool is_power_switch_on() = 0;
		};

	} // namespace interfaces
} // namespace tire

#endif // TIRE_INTERFACES_HARDWARE_INTERFACE_H