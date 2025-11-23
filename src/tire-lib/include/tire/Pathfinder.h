#ifndef TIRE_PATHFINDER_H
#define TIRE_PATHFINDER_H

#include <vector>
#include <string>
#include "tire/NavigationGraph.h"

namespace tire {

    /**
     * @class Pathfinder
     * @brief Implements pathfinding algorithms for the navigation graph.
     */
    class Pathfinder {
    public:
        Pathfinder();

        /**
         * @brief Finds the optimal path between two nodes using the A* algorithm.
         * * @param graph The navigation graph to search (must be loaded first).
         * @param start_node_id The ID of the starting node (e.g., "RP_LOBBY").
         * @param target_node_id The ID of the destination node (e.g., "RP_ROOM_101").
         * @return A vector of strings containing the IDs of the nodes in the path 
         * (ordered from start to end). Returns an empty vector if no path is found.
         */
        std::vector<std::string> find_path(NavigationGraph& graph, 
                                           const std::string& start_node_id, 
                                           const std::string& target_node_id);
    };

} // namespace tire

#endif // TIRE_PATHFINDER_H