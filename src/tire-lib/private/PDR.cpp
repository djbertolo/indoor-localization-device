#include "tire/PDR.h"
#include <cmath>
#include <iostream>
#include <algorithm>

// Constants for PDR tuning
#define GRAVITY 9.81
#define DEG_TO_RAD (3.1415926535 / 180.0)
#define TWO_PI (2.0 * 3.1415926535)

// Tuning Parameters
// Minimum time (seconds) between steps to avoid double counting (approx 0.3s for running, 0.4s walking)
#define MIN_STEP_DELAY 0.35 
// K constant for Weinberg step length estimation (needs calibration, approx 0.4 - 0.5)
#define STEP_LENGTH_K 0.45 

namespace tire {

    PDR::PDR() : 
        previous_acceleration_magnitude(0.0),
        is_peak(false),
        step_threshold(1.1 * GRAVITY), // Threshold just above gravity (approx 10.8 m/s^2)
        current_heading(0.0),
        accumulated_delta_heading(0.0),
        new_step_detected(false),
        last_step_length(0.0) 
    {
    }

    void PDR::initialize() {
        std::cout << "[PDR] Initializing..." << std::endl;
        
        // Reset state
        previous_acceleration_magnitude = GRAVITY; // Start assuming we are standing still
        is_peak = false;
        current_heading = 0.0;
        accumulated_delta_heading = 0.0;
        new_step_detected = false;
        last_step_length = 0.0;
    }

    void PDR::process_IMU_data(const interfaces::IMUData& imu_data, double delta_time) {
        // 1. Update Heading
        update_heading(imu_data.gyroscope_z, delta_time);

        // 2. Detect Step
        // We need to keep track of time for debouncing. 
        // Since the header doesn't store total time, we use a static variable or assume valid inputs.
        // For this implementation, we rely on the mechanics of the peak detection.
        
        if (detect_step(imu_data)) {
            new_step_detected = true;
            
            // 3. Estimate Step Length (Dynamic)
            last_step_length = estimate_step_length(imu_data);
            
            // Debug output
            // std::cout << "[PDR] Step! Len: " << last_step_length << "m, Heading: " << current_heading << std::endl;
        }
    }

    PDRState PDR::get_pdr_update() {
        PDRState state;
        state.step_detected = new_step_detected;
        state.step_length = last_step_length;
        state.delta_heading = accumulated_delta_heading;

        // Reset accumulators for the next cycle
        new_step_detected = false;
        last_step_length = 0.0;
        accumulated_delta_heading = 0.0;

        return state;
    }

    // --- Private Helper Functions ---

    bool PDR::detect_step(const interfaces::IMUData& imu_data) {
        // Calculate total acceleration magnitude
        double accel_mag = std::sqrt(std::pow(imu_data.acceleration_x, 2) +
                                     std::pow(imu_data.acceleration_y, 2) +
                                     std::pow(imu_data.acceleration_z, 2));

        // Simple Low-Pass Filter (Alpha = 0.8) to smooth noise
        // Note: Using previous_acceleration_magnitude as the "filtered" history
        accel_mag = (0.8 * previous_acceleration_magnitude) + (0.2 * accel_mag);

        bool step_found = false;

        // Peak Detection Algorithm:
        // 1. We are looking for a rise above the threshold.
        // 2. Once above threshold, we look for the inflection point (peak) where values start falling.
        
        if (!is_peak) {
            // We are waiting for the signal to rise above threshold
            if (accel_mag > step_threshold) {
                is_peak = true; // We have crossed the threshold, now looking for the peak
            }
        } else {
            // We are currently above threshold, checking if we are falling (peak passed)
            // Ideally, we check if (current < previous) AND (previous > threshold)
            if (accel_mag < previous_acceleration_magnitude) {
                // We just passed the peak
                is_peak = false;
                step_found = true;
            }
        }

        previous_acceleration_magnitude = accel_mag;
        return step_found;
    }

    double PDR::estimate_step_length(const interfaces::IMUData& imu_data) {
        // Dynamic Step Length Estimation (Weinberg approach simplified)
        // Step Length ~= K * fourth_root(Max_Accel - Min_Accel)
        
        // In this simplified version (stateless), we use the peak acceleration 
        // we just detected (stored in previous_acceleration_magnitude) and assume
        // the "min" was gravity (standing still).
        
        double max_accel = previous_acceleration_magnitude; // The peak we just found
        double min_accel = GRAVITY; // Approximation

        // Sanity check
        if (max_accel < min_accel) max_accel = min_accel + 0.1;

        double diff = max_accel - min_accel;
        double estimated_length = STEP_LENGTH_K * std::pow(diff, 0.25);

        // Clamp limits for safety (human steps are usually 0.3m to 1.2m)
        if (estimated_length < 0.3) estimated_length = 0.3;
        if (estimated_length > 1.0) estimated_length = 1.0;

        return estimated_length;
    }

    void PDR::update_heading(double gyroscope_z, double delta_time) {
        // Simple integration: angle = angle + (rate * time)
        // Note: gyroscope_z is expected in radians/sec
        
        double delta_theta = gyroscope_z * delta_time;

        // Invert if necessary based on IMU mounting (CW vs CCW)
        // Assuming standard CCW positive here.
        
        accumulated_delta_heading += delta_theta;
        current_heading += delta_theta;

        // Normalize heading to 0 - 2PI range
        if (current_heading > TWO_PI) current_heading -= TWO_PI;
        else if (current_heading < 0.0) current_heading += TWO_PI;
    }

} // namespace tire