#include "tire/EKF.h"
#include <cmath>
#include <iostream>

namespace tire {

    EKF::EKF() {
        // Initialize matrices with default/guess values
        x.setZero();
        P.setIdentity(); 
        
        // Tunable Parameter: Trust in PDR (Low values = high trust)
        Q.setIdentity();
        Q(0,0) = 0.1; // Variance in X
        Q(1,1) = 0.1; // Variance in Y
        Q(2,2) = 0.05; // Variance in Theta

        // Tunable Parameter: Trust in BLE (High values = low trust/noisy)
        R.setIdentity();
        R(0,0) = 2.0; // BLE X variance (meters)
        R(1,1) = 2.0; // BLE Y variance (meters)
    }

    void EKF::initialize(double start_x, double start_y, double start_theta) {
        x << start_x, start_y, start_theta;
        // Reset covariance to high uncertainty if needed, or keeping tight
        P.setIdentity(); 
        std::cout << "[EKF] Initialized at: " << x.transpose() << std::endl;
    }

    void EKF::predict(const PDRState& pdr_state) {
        if (!pdr_state.step_detected) {
            // If no step, we assume no movement, but maybe heading changed?
            // For simplicity, we only update on steps or significant gyro movement.
            // If delta_heading is non-zero but step is false, we just update theta.
            if (std::abs(pdr_state.delta_heading) > 0.001) {
                 x(2) += pdr_state.delta_heading;
                 // Normalize theta
                 x(2) = std::atan2(std::sin(x(2)), std::cos(x(2)));
            }
            return;
        }

        double step_len = pdr_state.step_length;
        double d_theta = pdr_state.delta_heading;
        double theta = x(2);

        // --- 1. State Prediction (Non-linear) ---
        // x_new = x_old + L * cos(theta + d_theta/2)
        // y_new = y_old + L * sin(theta + d_theta/2)
        // theta_new = theta + d_theta
        
        double mid_theta = theta + (d_theta / 2.0);
        
        x(0) = x(0) + step_len * std::cos(mid_theta);
        x(1) = x(1) + step_len * std::sin(mid_theta);
        x(2) = theta + d_theta;

        // Normalize Theta
        x(2) = std::atan2(std::sin(x(2)), std::cos(x(2)));

        // --- 2. Jacobian Calculation (Linearization) ---
        // Jacobian F (partial derivatives of the motion model w.r.t state x, y, theta)
        Eigen::Matrix3d F;
        F.setIdentity();
        
        // derivative of x_new w.r.t theta
        F(0, 2) = -step_len * std::sin(mid_theta); 
        // derivative of y_new w.r.t theta
        F(1, 2) =  step_len * std::cos(mid_theta);

        // --- 3. Covariance Prediction ---
        // P = F * P * F^T + Q
        P = F * P * F.transpose() + Q;
        
        // std::cout << "[EKF] Predict PDR: " << x.transpose() << std::endl;
    }

    void EKF::update(const Position2D& ble_position) {
        // Measurement Vector z
        Eigen::Vector2d z;
        z << ble_position.x, ble_position.y;

        // Measurement Matrix H (We observe X and Y directly, but not Theta)
        Eigen::Matrix<double, 2, 3> H;
        H.setZero();
        H(0, 0) = 1.0; // Observe x
        H(1, 1) = 1.0; // Observe y

        // Innovation (Residual) y = z - Hx
        Eigen::Vector2d y = z - H * x;

        // Innovation Covariance S = H * P * H^T + R
        Eigen::Matrix2d S = H * P * H.transpose() + R;

        // Optimal Kalman Gain K = P * H^T * S^-1
        Eigen::Matrix<double, 3, 2> K = P * H.transpose() * S.inverse();

        // Update State x = x + Ky
        x = x + K * y;

        // Update Covariance P = (I - KH) * P
        Eigen::Matrix3d I = Eigen::Matrix3d::Identity();
        P = (I - K * H) * P;

        // std::cout << "[EKF] Update BLE Correction applied." << std::endl;
    }

    Eigen::Vector3d EKF::get_state() const {
        return x;
    }

} // namespace tire