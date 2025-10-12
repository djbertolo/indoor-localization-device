# Contains the A* search algorithm implementation

import heapq
import math
import json
from pathlib import Path
from typing import List, Optional, Tuple

# Import the NavigationGraph class from the sibling file.
from graph import NavigationGraph, Coordinates

def _calculate_heuristic(p1: Coordinates, p2: Coordinates) -> float:
	"""
	Calculates the Euclidean (straight-line) distance between two points.
	This serves as the heuristic 'h' in the A* algorithm.
	"""
	return math.sqrt((p1.x - p2.x)**2 + (p1.y - p2.y)**2)

def _reconstruct_path(came_from: dict, current: str) -> List[str]:
	"""
	Rebuilds the path from the end node back to the start node.
	"""
	total_path = [current]
	while current in came_from:
		current = came_from[current]
		total_path.insert(0, current)
	return total_path

def find_path(graph: NavigationGraph, start_rp_id: str, end_rp_id: str) -> Optional[List[str]]:
	"""
	Finds the shortest path between two Reference Points using the A* algorithm.

	Args:
		graph: An initialized NavigationGraph object.
		start_rp_id: The ID of the starting Reference Point.
		end_rp_id: The ID of the destination Reference Point.

	Returns:
		An ordered list of RP IDs representing the shortest path, or None if no path is found.
	"""
	start_rp = graph.get_rp(start_rp_id)
	end_rp = graph.get_rp(end_rp_id)

	if not start_rp or not end_rp:
		print("Error: Start or end RP ID not found in the graph.")
		return None

	# A priority queue to store (f_cost, rp_id) tuples.
	# heapq is a min-heap, so it will always give us the node with the lowest f_cost.
	open_set: List[Tuple[float, str]] = [(0, start_rp_id)]

	# came_from[n] is the node immdiately preceding n on the cheapest path from start.
	came_from: dict[str, str] = {}

	# g_score[n] is the cost of the cheapest path from start to n currently known.
	g_score: dict[str, float] = {rp.id: float('inf') for rp in graph.get_all_rps()}
	g_score[start_rp_id] = 0

	# f_score[n] = g_score[n] + h(n).
	f_score: dict[str, float] = {rp.id: float('inf') for rp in graph.get_all_rps()}
	f_score[start_rp_id] = _calculate_heuristic(start_rp.coordinates, end_rp.coordinates)

	while open_set:
		# Get the node in open_set having the lowest f_score value.
		_, current_id = heapq.heappop(open_set)

		if current_id == end_rp_id:
			# We have reached the destination, reconstruct and return the path.
			return _reconstruct_path(came_from, current_id)
		
		for neighbor_id, distance in graph.get_neighbors(current_id):
			# tentative_g_score is the distance from start to the neighbor through current.
			tentative_g_score = g_score[current_id] + distance

			if tentative_g_score < g_score[neighbor_id]:
				# This path to the neighbor is better than any previously recorded. Record it!
				came_from[neighbor_id] = current_id
				g_score[neighbor_id] = tentative_g_score

				neighbor_rp = graph.get_rp(neighbor_id)
				if neighbor_rp:
					h_cost = _calculate_heuristic(neighbor_rp.coordinates, end_rp.coordinates)
					f_score[neighbor_id] = tentative_g_score + h_cost

					# If neighbor is not in the priority queue, add it.
					if not any(item[1] == neighbor_id for item in open_set):
						heapq.heappush(open_set, (f_score[neighbor_id], neighbor_id))

	# If the loop completes, it means there is no path.
	print(f"No path could be found from {start_rp_id} to {end_rp_id}")
	return None

# --- Example Usage ---
if __name__ == '__main__':
	dummy_map_data = {
		"reference_points": [
			{"id": "A", "coordinates": {"x": 0, "y": 0}, "descriptor": "Start", "ble_fingerprint": []},
			{"id": "B", "coordinates": {"x": 10, "y": 20}, "descriptor": "Midpoint 1", "ble_fingerprint": []},
			{"id": "C", "coordinates": {"x": 30, "y": 10}, "descriptor": "Midpoint 2", "ble_fingerprint": []},
			{"id": "D", "coordinates": {"x": 40, "y": 0}, "descriptor": "End", "ble_fingerprint": []}
		],
		"connections": [
			{"from": "A", "to": "B", "distance": 22.3}, # Correct distance is sqrt(10^2+20^2) = 22.36
			{"from": "B", "to": "A", "distance": 22.3},
			{"from": "A", "to": "C", "distance": 31.6}, # Correct distance is sqrt(30^2+10^2) = 31.62
			{"from": "C", "to": "A", "distance": 31.6},
			{"from": "B", "to": "D", "distance": 36.0}, # sqrt(30^2+20^2) = 36.05
			{"from": "D", "to": "B", "distance": 36.0},
			{"from": "C", "to": "D", "distance": 14.1}, # sqrt(10^2+10^2) = 14.14
			{"from": "D", "to": "C", "distance": 14.1}
		]
	}

	project_root = Path(__file__).resolve().parent.parent.parent

	dummy_filepath = str(project_root / "data" / "dummy_map.json")
	with open(dummy_filepath, 'w') as f:
		json.dump(dummy_map_data, f, indent=2)

	print("--- Testing Pathfinder ---")
	try:
		test_graph = NavigationGraph(dummy_filepath)
		start_node = "A"
		end_node = "D"

		path = find_path(test_graph, start_node, end_node)
		
		print(f"\nFinding path from '{start_node}' to '{end_node}':")
		if path:
			# The A* should choose A -> C -> D because 31.6 + 14.1 = 45.7
			# which is shorter than A -> B -> D (22.3 + 36.0 = 58.3)
			print(f"  -> Path found: {' -> '.join(path)}")
			assert path == ["A", "C", "D"]
			print("  -> Path is correct!")
		else:
			print("  -> No path found.")

	except (FileNotFoundError, KeyError) as e:
		print(f"An error occurred during testing: {e}")
