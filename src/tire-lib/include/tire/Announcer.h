#ifndef TIRE_ANNOUNCER_H
#define TIRE_ANNOUNCER_H

#include <string>
#include <vector>
#include <chrono>
#include <Eigen/Dense>
#include "tire/interfaces/HardwareInterface.h"
#include "tire/NavigationGraph.h"

namespace tire {

    /**
     * @class Announcer
     * @brief Manages audio guidance logic.
     * * Calculates relative bearing to the next target to issue "Turn Left/Right" commands
     * and tracks progress along the path to issue "Arrived" commands.
     */
    class Announcer {
    public:
        Announcer();

        /**
         * @brief Updates the guidance logic. Should be called in the main loop.
         * * @param current_pose The user's current state vector [x, y, theta].
         * @param current_path The list of Node IDs representing the active route.
         * @param graph The map data to look up node coordinates.
         * @param hw Reference to hardware to play audio cues.
         * @return The index of the next target node in the path vector (for debug/UI).
         */
        int update(const Eigen::Vector3d& current_pose, 
                   const std::vector<std::string>& current_path, 
                   NavigationGraph& graph,
                   interfaces::HardwareInterface& hw);

        /**
         * @brief Resets the navigation state (e.g., when a new destination is set).
         */
        void reset();

    private:
        // State tracking
        int next_node_index;      // Index in the path vector we are moving towards
        bool destination_reached; // Flag to stop repeated announcements
        
        // Timing to prevent spamming audio
        std::chrono::steady_clock::time_point last_announcement_time;
        
        // Constants
        const double WAYPOINT_REACHED_RADIUS = 1.5; // Meters
        const double ANNOUNCEMENT_COOLDOWN = 3.0;   // Seconds
    };

} // namespace tire

#endif // TIRE_ANNOUNCER_H