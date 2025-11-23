#ifndef TIRE_EKF_H
#define TIRE_EKF_H

#include <Eigen/Dense>
#include "tire/BLEFingerprinting.h" // For Position2D
#include "tire/PDR.h"              // For PDRState

namespace tire {

    /**
     * @class EKF
     * @brief Extended Kalman Filter for fusing PDR and BLE data.
     * * State Vector (x): [pos_x, pos_y, theta]
     * Control Vector (u): [step_length, delta_heading]
     * Measurement Vector (z): [ble_x, ble_y]
     */
    class EKF {
    public:
        EKF();

        /**
         * @brief Initializes the filter state.
         * @param start_x Initial X position.
         * @param start_y Initial Y position.
         * @param start_theta Initial facing direction (radians).
         */
        void initialize(double start_x, double start_y, double start_theta);

        /**
         * @brief Prediction Step (Time Update).
         * Uses PDR data to estimate the new position.
         * @param pdr_state The step length and heading change from the PDR module.
         */
        void predict(const PDRState& pdr_state);

        /**
         * @brief Correction Step (Measurement Update).
         * Uses BLE Fingerprinting data to correct the position estimate.
         * @param ble_position The position estimated by the k-NN algorithm.
         */
        void update(const Position2D& ble_position);

        /**
         * @brief Returns the current estimated position and heading.
         * @return A vector [x, y, theta].
         */
        Eigen::Vector3d get_state() const;

    private:
        // State vector [x, y, theta]
        Eigen::Vector3d x;

        // State covariance matrix (Uncertainty)
        Eigen::Matrix3d P;

        // Process noise covariance (Uncertainty of PDR)
        Eigen::Matrix3d Q;

        // Measurement noise covariance (Uncertainty of BLE)
        Eigen::Matrix2d R;
    };

} // namespace tire

#endif // TIRE_EKF_H