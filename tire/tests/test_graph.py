import json
from pathlib import Path

from src.navigation.graph import NavigationGraph

# Unit tests for loading and querying the map
def test1():
	# Create a dummy JSON file for testing purposes.
	dummy_map_data = {
		"reference_points": [
        {
          "id": "JB-3-HW-CENTER",
          "coordinates": { "x": 50, "y": 20 },
          "descriptor": "You are in the main hallway.",
          "ble_fingerprint": [
            { "mac_address": "C0:98:E5:54:00:01", "avg_rssi": -65 }
          ]
        },
        {
          "id": "JB-355",
          "coordinates": { "x": 50, "y": 35 },
          "descriptor": "You are outside room 355.",
          "ble_fingerprint": [
            { "mac_address": "C0:98:E5:54:00:01", "avg_rssi": -58 }
          ]
        }
		],
		"connections": [
			{ "from": "JB-3-HW-CENTER", "to": "JB-355", "distance": 15.0 },
			{ "from": "JB-355", "to": "JB-3-HW-CENTER", "distance": 15.0 }
		]
	}

	# Write the dummy data to a temporary file
	project_root = Path(__file__).resolve().parent
	dummy_filepath = str(project_root / "data" / "dummy_map.json")
	with open(dummy_filepath, 'w') as f:
		json.dump(dummy_map_data, f, indent=2)

	print("--- Testing NavigationGraph ---")
	
	# 1. Initialize the graph
	try:
		nav_graph = NavigationGraph(dummy_filepath)

		# 2. Test getting a specific RP
		rp = nav_graph.get_rp("JB-355")
		print(f"\nFetching RP 'JB-355':")
		if rp:
			print(f"	ID: {rp.id}")
			print(f"	Descriptor: {rp.descriptor}")
			print(f"	Coordinates: {rp.coordinates}")
		else:
			print(" RP not found.")

		# 3. Test getting neighbors
		neighbors = nav_graph.get_neighbors("JB-3-HW-CENTER")
		print(f"\nFetching neighbors for 'JB-3-HW-CENTER':")
		if neighbors:
			for neighbor_id, distance in neighbors:
				print(f" -> Neighbor: {neighbor_id}, Distance: {distance} ft")
		else:
			print(" No neighbors found.")
	
	except (FileNotFoundError, KeyError) as e:
		print(f"An error occurred during testing: {e}")