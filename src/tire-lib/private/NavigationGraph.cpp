#include "tire/NavigationGraph.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <nlohmann/json.hpp> // Requires the nlohmann_json library

using json = nlohmann::json;

namespace tire {

    NavigationGraph::NavigationGraph() {}

    bool NavigationGraph::load_from_json(const std::string& file_path) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "[NavigationGraph] Error: Could not open map file " << file_path << std::endl;
            return false;
        }

        try {
            json j;
            file >> j;

            nodes.clear();

            // Expected JSON structure: { "nodes": [ { "id": "...", "x": 0.0, "y": 0.0, ... } ] }
            if (j.contains("nodes") && j["nodes"].is_array()) {
                for (const auto& item : j["nodes"]) {
                    GraphNode node;
                    node.id = item.value("id", "");
                    node.name = item.value("name", "Unknown");
                    node.audio_file = item.value("audio", "");
                    node.position.x = item.value("x", 0.0);
                    node.position.y = item.value("y", 0.0);

                    // Parse Neighbors if available
                    if (item.contains("neighbors") && item["neighbors"].is_object()) {
                        for (auto& neighbor : item["neighbors"].items()) {
                            node.neighbors[neighbor.key()] = neighbor.value();
                        }
                    }

                    nodes[node.id] = node;
                }
            }
            
            std::cout << "[NavigationGraph] Loaded " << nodes.size() << " nodes from " << file_path << std::endl;
            return true;

        } catch (const json::parse_error& e) {
            std::cerr << "[NavigationGraph] JSON Parse Error: " << e.what() << std::endl;
            return false;
        }
    }

    GraphNode* NavigationGraph::get_node(const std::string& id) {
        auto it = nodes.find(id);
        if (it != nodes.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    const std::map<std::string, GraphNode>& NavigationGraph::get_all_nodes() const {
        return nodes;
    }

    double NavigationGraph::get_distance(const std::string& id_a, const std::string& id_b) {
        GraphNode* a = get_node(id_a);
        GraphNode* b = get_node(id_b);

        if (!a || !b) return -1.0;

        double dx = a->position.x - b->position.x;
        double dy = a->position.y - b->position.y;
        return std::sqrt(dx*dx + dy*dy);
    }

} // namespace tire