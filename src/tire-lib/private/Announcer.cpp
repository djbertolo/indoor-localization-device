#include "tire/Announcer.h"
#include <cmath>
#include <iostream>

// Helper for normalizing angles to -PI to +PI
double normalize_angle(double angle) {
    while (angle > M_PI) angle -= 2.0 * M_PI;
    while (angle < -M_PI) angle += 2.0 * M_PI;
    return angle;
}

namespace tire {

    Announcer::Announcer() : 
        next_node_index(1), // Start aiming for the second node (index 0 is start)
        destination_reached(false),
        last_announcement_time(std::chrono::steady_clock::now()) 
    {}

    void Announcer::reset() {
        next_node_index = 1;
        destination_reached = false;
    }

    int Announcer::update(const Eigen::Vector3d& current_pose, 
                          const std::vector<std::string>& current_path, 
                          NavigationGraph& graph,
                          interfaces::HardwareInterface& hw) {

        // 1. Check if navigation is active
        if (current_path.empty() || destination_reached) {
            return -1;
        }

        // If we ran out of nodes, we arrived
        if (next_node_index >= current_path.size()) {
            if (!destination_reached) {
                hw.play_audio("destination_reached");
                destination_reached = true;
            }
            return -1;
        }

        // 2. Get Data
        double user_x = current_pose(0);
        double user_y = current_pose(1);
        double user_heading = current_pose(2);

        std::string target_id = current_path[next_node_index];
        GraphNode* target_node = graph.get_node(target_id);

        if (!target_node) return -1; // Safety check

        // 3. Calculate Distance to Target
        double dx = target_node->position.x - user_x;
        double dy = target_node->position.y - user_y;
        double distance = std::sqrt(dx*dx + dy*dy);

        // 4. Check if Reached
        if (distance < WAYPOINT_REACHED_RADIUS) {
            // We arrived at this waypoint. Advance to next.
            std::cout << "[Announcer] Reached waypoint: " << target_id << std::endl;
            
            // Play the specific audio for this landmark if it exists
            if (!target_node->audio_file.empty()) {
                hw.play_audio(target_node->audio_file);
            } else {
                // Generic confirmation beep
                hw.play_audio("beep_checkpoint");
            }

            next_node_index++;
            // Force an immediate guidance update for the new target
            last_announcement_time = std::chrono::steady_clock::time_point(); 
            return next_node_index;
        }

        // 5. Guidance Logic (Turn Instructions)
        
        // Check cooldown
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - last_announcement_time;
        if (elapsed.count() < ANNOUNCEMENT_COOLDOWN) {
            return next_node_index; // Wait before talking again
        }

        // Calculate Angle to Target
        double angle_to_target = std::atan2(dy, dx);
        
        // Calculate Error (Difference between where we are looking and where we should look)
        double error = normalize_angle(angle_to_target - user_heading);

        // Thresholds for audio cues (in radians)
        // 0.35 rad is approx 20 degrees
        const double TURN_THRESHOLD = 0.35; 

        if (error > TURN_THRESHOLD) {
            hw.play_audio("turn_left");
            last_announcement_time = now;
        } else if (error < -TURN_THRESHOLD) {
            hw.play_audio("turn_right");
            last_announcement_time = now;
        } else {
            // If vaguely on track and distance is large, maybe encourage?
            // For now, silence implies "keep going straight".
        }

        return next_node_index;
    }

} // namespace tire