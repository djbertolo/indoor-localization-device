#include "tire/Pathfinder.h"
#include <queue>
#include <map>
#include <algorithm> // For std::reverse
#include <iostream>
#include <limits>    // For infinity

namespace tire {

    Pathfinder::Pathfinder() {}

    // Helper struct for the priority queue (min-heap)
    struct NodeScore {
        std::string id;
        double f_score; // f = g (cost so far) + h (heuristic estimate)

        // Overload > operator because std::priority_queue is a max-heap by default,
        // so we use > to act as a min-heap (smallest f_score on top).
        bool operator>(const NodeScore& other) const {
            return f_score > other.f_score;
        }
    };

    std::vector<std::string> Pathfinder::find_path(NavigationGraph& graph, 
                                                   const std::string& start_node_id, 
                                                   const std::string& target_node_id) {
        
        // 1. Validate inputs
        if (graph.get_node(start_node_id) == nullptr) {
            std::cerr << "[Pathfinder] Error: Start node '" << start_node_id << "' not found." << std::endl;
            return {};
        }
        if (graph.get_node(target_node_id) == nullptr) {
            std::cerr << "[Pathfinder] Error: Target node '" << target_node_id << "' not found." << std::endl;
            return {};
        }

        // 2. Initialize Data Structures
        
        // Open Set: Nodes to be evaluated
        std::priority_queue<NodeScore, std::vector<NodeScore>, std::greater<NodeScore>> open_set;
        
        // Came From: Tracks the path (CurrentID -> PreviousID)
        std::map<std::string, std::string> came_from;

        // G Score: Exact cost from start to node. Default is infinity.
        std::map<std::string, double> g_score;
        // Initialize all g_scores to infinity
        for (const auto& pair : graph.get_all_nodes()) {
            g_score[pair.first] = std::numeric_limits<double>::infinity();
        }
        g_score[start_node_id] = 0.0;

        // Add start node to open set
        // Heuristic: Euclidean distance from start to target
        double h_start = graph.get_distance(start_node_id, target_node_id);
        open_set.push({start_node_id, h_start});

        // 3. Main A* Loop
        while (!open_set.empty()) {
            // Get the node with the lowest f_score
            NodeScore current = open_set.top();
            open_set.pop();
            std::string current_id = current.id;

            // Check if we reached the target
            if (current_id == target_node_id) {
                // Reconstruct path
                std::vector<std::string> path;
                while (came_from.find(current_id) != came_from.end()) {
                    path.push_back(current_id);
                    current_id = came_from[current_id];
                }
                path.push_back(start_node_id);
                std::reverse(path.begin(), path.end()); // Reverse to get Start -> End
                return path;
            }

            // Current cost to get here
            double current_g = g_score[current_id];

            // Explore neighbors
            GraphNode* node_ptr = graph.get_node(current_id);
            
            // Iterate over the adjacency list (neighbor_id -> distance)
            for (auto const& [neighbor_id, distance_to_neighbor] : node_ptr->neighbors) {
                
                // tentative_g_score = cost to current + distance to neighbor
                double tentative_g = current_g + distance_to_neighbor;

                // If this path to neighbor is shorter than any previous one recorded
                if (tentative_g < g_score[neighbor_id]) {
                    came_from[neighbor_id] = current_id;
                    g_score[neighbor_id] = tentative_g;
                    
                    // f_score = g_score + heuristic (distance to target)
                    double f = tentative_g + graph.get_distance(neighbor_id, target_node_id);
                    
                    // Add to queue for exploration
                    open_set.push({neighbor_id, f});
                }
            }
        }

        // 4. No path found
        std::cerr << "[Pathfinder] Failure: No path found from " << start_node_id << " to " << target_node_id << std::endl;
        return {};
    }

} // namespace tire