#ifndef TIRE_PDR_H
#define TIRE_PDR_H

// We need the definition of the IMUData struct
#include "tire/interfaces/HardwareInterface.h"

namespace tire {

	/**
	 * @struct PDRState
	 * @brief Holds the output of the PDR calculation for a single update.
	 * This represents the estimated change in the user's state.
	 */
	struct PDRState {
		double step_length;    // Estimated length of the last step taken (in meters)
		double delta_heading;  // Change in heading (in radians) since the last update
		bool step_detected;    // Flag indicating if a step was detected in this update
	};

	/**
	 * @class PDR
	 * @brief Processes raw IMU data for Pedestrian Dead Reckoning.
	 *
	 * This class implements algorithms for step detection, step length estimation,
	 * and heading calculation by integrating gyroscope data. It filters raw
	 * accelerometer data to find peaks indicating steps and uses gyroscope
	 * data to track orientation changes.
	 */
	class PDR {
	public:
		/**
		 * @brief Constructor for the PDR processor.
		 */
		PDR();

		/**
		 * @brief Initializes the PDR system.
		 * Call this once before processing data, e.g., to calibrate sensors.
		 */
		void initialize();

		/**
		 * @brief Processes a new packet of raw IMU data.
		 * This function should be called rapidly (e.g., on every new IMU reading).
		 * It will update its internal state for step detection and heading.
		 *
		 * @param imuData The raw data from the IMU.
		 * @param deltaTime The time elapsed (in seconds) since the last IMU reading.
		 */
		void process_IMU_data(const interfaces::IMUData& imu_data, double delta_time);

		/**
		 * @brief Checks if a new step has been detected and returns the PDR state.
		 * This function should be called at a regular, slower interval
		 * (e.g., 10 times per second) by the main positioning loop.
		 *
		 * @return A PDRState struct. If a step was detected, stepDetected will
		 * be true, and stepLength will be populated. deltaHeading will
		 * contain the accumulated heading change since the last call.
		 */
		PDRState get_pdr_update();

	private:
		// --- Private Member Variables ---

		// For Step Detection
		double previous_acceleration_magnitude; // Previous accelerometer magnitude
		bool is_peak;               // Flag to track if we're at a potential peak
		double step_threshold;      // Calibrated threshold to confirm a step

		// For Heading Calculation
		double current_heading;     // The user's current heading in radians
		
		// For storing accumulated changes until get_pdr_update() is called
		double accumulated_delta_heading;
		bool new_step_detected;
		double last_step_length;

		// --- Private Helper Functions ---

		/**
		 * @brief Detects steps from raw accelerometer data.
		 * Implements peak detection with thresholding.
		 * @param imu_data The raw IMU data.
		 * @return true if a step is detected, false otherwise.
		 */
		bool detect_step(const interfaces::IMUData& imu_data);

		/**
		 * @brief Estimates the length of the detected step.
		 * Based on min/max acceleration during the step cycle.
		 * @param imu_data The raw IMU data (or filtered data around the step).
		 * @return The estimated step length in meters.
		 */
		double estimate_step_length(const interfaces::IMUData& imu_data);

		/**
		 * @brief Updates the heading by integrating gyroscope data.
		 * Tracks cumulative drift.
		 * @param gyroscope_z The Z-axis (yaw) angular velocity from the gyroscope.
		 * @param delta_time The time interval for this integration step.
		 */
		void update_heading(double gyroscope_z, double delta_time);
	};

} // namespace tire

#endif // TIRE_PDR_H