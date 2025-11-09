#include "tire/PDR.h"
#include <cmath> // For std::sqrt, std::pow, and std::fmod
#include <iostream> // For any debug output

namespace tire {

	// Constructor: Initialize all member variables to default states
	PDR::PDR() : 
		previous_acceleration_magnitude(0.0),
		is_peak(false),
		step_threshold(1.2), // PLACEHOLDER: This value MUST be calibrated (see brief line 125)
		current_heading(0.0),
		accumulated_delta_heading(0.0),
		new_step_detected(false),
		last_step_length(0.0) 
	{
		// We assume the starting acceleration magnitude is 0,
		// and the user starts facing "north" (0.0 radians).
	}

	// initialize()
	void PDR::initialize() {
		// In a real implementation, this might run a calibration sequence
		// to find the gyroscope's resting bias or the accelerometer's
		// orientation relative to gravity.
		std::cout << "[PDR] Initializing Pedestrian Dead Reckoning..." << std::endl;
		
		// Reset all state variables
		previous_acceleration_magnitude = 0.0;
		is_peak = false;
		current_heading = 0.0;
		accumulated_delta_heading = 0.0;
		new_step_detected = false;
		last_step_length = 0.0;
		
		// TODO: Add gyroscope calibration here to find and subtract resting drift.
	}

	// process_IMU_data()
	// This is the "fast" loop, called on every new IMU reading.
	void PDR::process_IMU_data(const interfaces::IMUData& imu_data, double delta_time) {
		// 1. Update the heading by integrating gyroscope data
		// We use gyroscope_z for yaw (rotation around the vertical axis)
		update_heading(imu_data.gyroscope_z, delta_time);

		// 2. Try to detect a step
		if (detect_step(imu_data)) {
			// 3. If a step was found, estimate its length
			// We set the flags for the "slow" loop (get_pdr_update) to consume
			new_step_detected = true;
			last_step_length = estimate_step_length(imu_data);
			
			std::cout << "[PDR] Step detected! Length: " << last_step_length << std::endl;
		}
	}

	// get_PDR_update()
	// This is the "slow" loop, called by the EKF to get the latest prediction.
	PDRState PDR::get_pdr_update() {
		// Create the state update object
		PDRState update;
		update.step_detected = new_step_detected;
		update.step_length = last_step_length;
		update.delta_heading = accumulated_delta_heading;

		// CRITICAL: Reset the accumulated values for the next update cycle
		new_step_detected = false;
		last_step_length = 0.0;
		accumulated_delta_heading = 0.0;

		// Return the bundled update
		return update;
	}


	// --- Private Helper Functions ---

	// detect_step()
	bool PDR::detect_step(const interfaces::IMUData& imu_data) {
		// This is a simple placeholder algorithm.
		// As noted in the brief (line 117-122), filtering is needed.
		// A real implementation would first apply a low-pass or band-pass
		// filter to the accelerometer data.

		// 1. Calculate the magnitude of the acceleration
		// (Ignoring gravity for this simple example)
		// A better way is to filter gravity or look at variance.
		double acceleration_magnitude = std::sqrt(std::pow(imu_data.acceleration_x, 2) + 
									std::pow(imu_data.acceleration_y, 2) + 
									std::pow(imu_data.acceleration_z, 2));

		bool step_found = false;

		// 2. Implement simple peak detection (as per brief line 123)
		// We look for the signal to cross *above* the threshold, then *below* it.
		
		if (acceleration_magnitude > step_threshold && !is_peak) {
			// We just crossed the threshold upwards, this is the *start* of a peak
			is_peak = true;
		} else if (acceleration_magnitude < step_threshold && is_peak) {
			// We just crossed the threshold downwards, this is the *end* of a peak
			// This confirms one full step cycle.
			is_peak = false;
			step_found = true;
		}

		// 3. Store the current magnitude for the next comparison
		previous_acceleration_magnitude = acceleration_magnitude;

		return step_found;
	}

	// estimate_step_length()
	double PDR::estimate_step_length(const interfaces::IMUData& imu_data) {
		// This is a placeholder.
		// As noted in the brief (line 130), a real implementation would
		// be "predicted based off of the minimum and maximum acceleration
		// detected during a single step cycle."
		
		// That complex logic would be implemented here.
		// For now, we return a calibrated constant (e.g., 0.7 meters).
		// This is a common simplification for PDR.
		
		// (void)data; // Supress "unused parameter" warning

		return 0.7; // Return a fixed step length in meters
	}

	// update_heading()
	void PDR::update_heading(double gyroscope_z, double delta_time) {
		// 1. Calculate the change in angle (delta theta)
		// This is a simple Euler integration: angular_velocity * time
		double deltaHeading = gyroscope_z * delta_time;

		// 2. Add this change to our running totals
		current_heading += deltaHeading;
		accumulated_delta_heading += deltaHeading;

		// 3. Keep the heading wrapped between 0 and 2*PI (360 degrees)
		// fmod is the floating-point version of the modulo operator (%)
		// We add 2*PI before the mod to handle negative results
		const double TWO_PI = 2.0 * 3.1415926535;
		current_heading = std::fmod(current_heading + TWO_PI, TWO_PI);
	}

} // namespace tire