#ifndef TIRE_NAVIGATION_GRAPH_H
#define TIRE_NAVIGATION_GRAPH_H

#include <string>
#include <vector>
#include <map>
#include "tire/BLEFingerprinting.h" // For Position2D

namespace tire {

    struct GraphNode {
        std::string id;         // "RP_HALLWAY_1"
        Position2D position;    // {x, y}
        std::string name;       // "Main Hallway North"
        std::string audio_file; // "guidance_hallway_north.wav"
        
        // Adjacency list: Neighbor Node ID -> Distance (Weight)
        std::map<std::string, double> neighbors; 
    };

    class NavigationGraph {
    public:
        NavigationGraph();

        /**
         * @brief Loads map data from a JSON file.
         * @param file_path Path to the .json map file.
         * @return true if successful.
         */
        bool load_from_json(const std::string& file_path);

        /**
         * @brief Retrieves a node by its ID.
         */
        GraphNode* get_node(const std::string& id);

        /**
         * @brief Returns all nodes (useful for finding the closest start node).
         */
        const std::map<std::string, GraphNode>& get_all_nodes() const;

        /**
         * @brief Calculates Euclidean distance between two nodes.
         */
        double get_distance(const std::string& id_a, const std::string& id_b);

    private:
        // Map of NodeID -> Node Object
        std::map<std::string, GraphNode> nodes;
    };

} // namespace tire

#endif // TIRE_NAVIGATION_GRAPH_H