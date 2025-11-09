#include "tire-lib/include/tire/BLEFingerprinting.h"
#include <cmath>	// For std::sqrt and std::pow
#include <iostream> // For std::cout (debug/log messages)
#include <vector>
#include <map>
#include <algorithm> // For std::sort and std::for_each
#include <set>		 // For creating a union of beacon IDs
#include <limits>	 // For std::numeric_limits

namespace tire
{

	// A helper struct for sorting neighbors by distance
	struct Neighbor
	{
		double distance;
		Position2D position;

		// Overload the '<' operator so std::sort works
		bool operator<(const Neighbor &other) const
		{
			return distance < other.distance;
		}
	};

	// Constructor: Initializes the 'k' value
	BLEFingerpinting::BLEFingerpinting(int k) : k(k)
	{
		if (k < 1)
		{
			k = 1; // k-NN must have at least 1 neighbor
		}
		std::cout << "[BLEFingerpinting] Initialized with k=" << k << std::endl;
	}

	// load_map()
	bool BLEFingerpinting::load_map(const std::string &map_file_path)
	{
		// This is a PLACEHOLDER implementation.
		// In the real version, you would use the 'nlohmann/json' library
		// to parse the file at 'map_file_path' and populate 'fingerprint_map'.

		std::cout << "[BLEFingerpinting] WARNING: Using hard-coded placeholder map data." << std::endl;
		std::cout << "[BLEFingerpinting] Real implementation will parse: " << map_file_path << std::endl;

		// Clear any existing map data
		fingerprint_map.clear();

		// --- Create Fake Map Data ---
		// Simulating a hallway with 3 reference points

		// Reference Point 1 (Start of hallway)
		RPFingerprint rp1;
		rp1.rp_id = "RP_HALLWAY_START";
		rp1.position = {0.0, 0.0};				   // (x=0, y=0)
		rp1.signal_strengths["BEACON_ID_1"] = -50; // Close to Beacon 1
		rp1.signal_strengths["BEACON_ID_2"] = -80;
		rp1.signal_strengths["BEACON_ID_3"] = -90;
		fingerprint_map.push_back(rp1);

		// Reference Point 2 (Middle of hallway)
		RPFingerprint rp2;
		rp2.rp_id = "RP_HALLWAY_MIDDLE";
		rp2.position = {0.0, 5.0}; // (x=0, y=5)
		rp2.signal_strengths["BEACON_ID_1"] = -65;
		rp2.signal_strengths["BEACON_ID_2"] = -65; // Equidistant to 1 and 2
		rp2.signal_strengths["BEACON_ID_3"] = -85;
		fingerprint_map.push_back(rp2);

		// Reference Point 3 (End of hallway)
		RPFingerprint rp3;
		rp3.rp_id = "RP_HALLWAY_END";
		rp3.position = {0.0, 10.0}; // (x=0, y=10)
		rp3.signal_strengths["BEACON_ID_1"] = -90;
		rp3.signal_strengths["BEACON_ID_2"] = -50; // Close to Beacon 2
		rp3.signal_strengths["BEACON_ID_3"] = -80;
		fingerprint_map.push_back(rp3);

		// --- End of Fake Data ---

		return true;
	}

	// find_closest_position()
	Position2D BLEFingerpinting::find_closest_position(const std::vector<interfaces::BLEBeaconData> &current_scan)
	{
		if (fingerprint_map.empty())
		{
			std::cerr << "[BLEFingerpinting] ERROR: Fingerprint map is empty. "
					  << "Was load_map() called?" << std::endl;
			return {0.0, 0.0}; // Return origin
		}

		// 1. Convert the current scan vector into a map for easier lookups
		std::map<std::string, int> current_scan_map;
		for (const auto &beacon : current_scan)
		{
			current_scan_map[beacon.id] = beacon.rssi;
		}

		// 2. Calculate the distance to every known RP in the map
		std::vector<Neighbor> neighbors;
		for (const auto &known_rp : fingerprint_map)
		{
			double distance = calculate_fingerprint_distance(
				current_scan_map,
				known_rp.signal_strengths);
			neighbors.push_back({distance, known_rp.position});
		}

		// 3. Sort the neighbors by distance (closest first)
		std::sort(neighbors.begin(), neighbors.end());

		// 4. Take the top 'k' neighbors and average their positions
		double sum_x = 0.0;
		double sum_y = 0.0;

		// Ensure we don't try to access more neighbors than we have
		int neighbors_to_average = std::min(static_cast<int>(neighbors.size()), k);
		if (neighbors_to_average == 0)
		{
			std::cerr << "[BLEFingerpinting] ERROR: No neighbors found." << std::endl;
			return {0.0, 0.0};
		}

		for (int i = 0; i < neighbors_to_average; ++i)
		{
			sum_x += neighbors[i].position.x;
			sum_y += neighbors[i].position.y;
		}

		// 5. Return the averaged position
		return {sum_x / neighbors_to_average, sum_y / neighbors_to_average};
	}

	// calculate_fingerprint_distance()
	double BLEFingerpinting::calculate_fingerprint_distance(const std::map<std::string, int> &scan_a, const std::map<std::string, int> &scan_b)
	{
		// This implements a Euclidean distance formula for the RSSI values.

		double sum_of_squares = 0.0;

		// A high penalty for a beacon that's in one scan but not the other.
		// (Assumes a very weak signal of -100 dBm if not detected)
		const int RSSI_PENALTY = -100;

		// 1. Create a set of all unique beacon IDs from *both* scans
		std::set<std::string> all_beacon_ids;
		for (const auto &pair : scan_a)
		{
			all_beacon_ids.insert(pair.first);
		}
		for (const auto &pair : scan_b)
		{
			all_beacon_ids.insert(pair.first);
		}

		// 2. Iterate through all unique beacons and sum the squared differences
		for (const auto &id : all_beacon_ids)
		{
			// Find the RSSI in scan 'a', or use the penalty if not found
			auto iter_a = scan_a.find(id);
			int rssi_a = (iter_a != scan_a.end()) ? iter_a->second : RSSI_PENALTY;

			// Find the RSSI in scan 'b', or use the penalty if not found
			auto iter_b = scan_b.find(id);
			int rssi_b = (iter_b != scan_b.end()) ? iter_b->second : RSSI_PENALTY;

			// 3. Add the squared difference to the sum
			double diff = static_cast<double>(rssi_a - rssi_b);
			sum_of_squares += std::pow(diff, 2);
		}

		// 4. The final distance is the square root of the sum
		return std::sqrt(sum_of_squares);
	}

} // namespace tire