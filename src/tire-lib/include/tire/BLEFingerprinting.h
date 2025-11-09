#ifndef TIRE_BLE_FINGERPRINTING_H
#define TIRE_BLE_FINGERPRINTING_H

#include <string>
#include <vector>
#include <map>
#include "tire/interfaces/HardwareInterface.h" // For BleBeaconData struct

namespace tire {

	/**
	 * @struct Position2D
	 * @brief A simple struct to hold a 2D coordinate.
	 */
	struct Position2D {
		double x;
		double y;
	};

	/**
	 * @struct RPFingerprint
	 * @brief Stores the pre-mapped BLE fingerprint for a single Reference Point (RP).
	 * It contains the RP's known position and a map of {beacon_id, rssi} pairs.
	 */
	struct RPFingerprint {
		std::string rp_id;          // The unique name of the Reference Point (e.g., "HALLWAY_1")
		Position2D position;        // The known (x, y) coordinates of the RP
		
		// A map where:
		// Key   = Beacon ID (e.g., "BEACON_ID_1")
		// Value = Average RSSI at this location (e.g., -65)
		std::map<std::string, int> signal_strengths;
	};

	/**
	 * @class BleFingerprinting
	 * @brief Manages BLE fingerprint data and performs location matching.
	 *
	 * This class loads a "radio map" of pre-recorded BLE fingerprints from a file
	 * (e.g., a JSON file). It then uses a k-Nearest Neighbors (k-NN) algorithm
	 * to compare a live BLE scan against the map to find the most probable
	 * Reference Point (RP) where the user is located.
	 */
	class BLEFingerpinting {
	public:
		/**
		 * @brief Constructor for the BLEFingerprinting module.
		 * @param k The number of neighbors to use for the k-NN algorithm.
		 */
		BLEFingerpinting(int k = 3);

		/**
		 * @brief Loads the fingerprint "radio map" from a specified file.
		 * This will parse a file (e.g., JSON) and populate the internal
		 * fingerprint_map_ vector.
		 *
		 * @param map_file_path The path to the file containing the map data.
		 * @return true if the map was loaded successfully, false otherwise.
		 */
		bool load_map(const std::string& map_file_path);

		/**
		 * @brief Finds the closest Reference Point to the user's current location.
		 * This implements the k-NN algorithm. It compares the live scan data
		 * against all known fingerprints in the map.
		 *
		 * @param current_scan A vector of beacon data from a fresh hardware scan.
		 * @return A Position2D struct representing the estimated (x, y) coordinates
		 * of the user.
		 */
		Position2D find_closest_position(
			const std::vector<interfaces::BLEBeaconData>& current_scan
		);

	private:
		/**
		 * @brief Calculates the "distance" (dissimilarity) between two fingerprints.
		 *
		 * @param scan_a A map representing one fingerprint's {beacon_id, rssi} pairs.
		 * @param scan_b A map representing the other fingerprint's {beacon_id, rssi} pairs.
		 * @return A double representing the calculated distance.
		 */
		double calculate_fingerprint_distance(
			const std::map<std::string, int>& scan_a,
			const std::map<std::string, int>& scan_b
		);

		// The 'k' value for the k-Nearest Neighbors algorithm.
		int k;

		// The in-memory "radio map". A list of all known fingerprints.
		std::vector<RPFingerprint> fingerprint_map;
	};

} // namespace tire

#endif // TIRE_BLE_FINGERPRINTING_H