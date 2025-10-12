# Loads and manages the navigation graph from JSON

import json
from typing import Dict, List, Optional, Tuple
from pathlib import Path
from dataclasses import dataclass

@dataclass(frozen=True)
class Coordinates:
	"""Represents the X, Y coordinates of a Reference Point."""
	x: float
	y: float

@dataclass(frozen=True)
class BLEFingerprint:
	"""Represents a single entry in a BLE fingerprint scan."""
	mac_address: str
	avg_rssi: int

@dataclass(frozen=True)
class ReferencePoint:
	"""Represents a single node (RP) in our navigation graph."""
	id: str
	coordinates: Coordinates
	descriptor: str
	ble_fingerprint: List[BLEFingerprint]

class NavigationGraph:
	"""
	Loads and manages the indoor navigation map from a JSON file.

	This class serves as the primary interface for the pathfinding and positioning modules to access map data. It parses the JSON map file and constructs an in-memory graph representation, including an adjacency list for efficient neighbor lookups.
	"""

	def __init__(self, map_filepath: str) -> None:
		"""
		Initializes the NavigationGraph by loading the map from a JSON file.

		Args:
			map_filepath: The full path to the .json map file.
		"""

		self._reference_points: Dict[str, ReferencePoint] = {}

		# Adjacency list storing neighbors and the distance (weight) to them.
		# Format: {"RP-ID-1": [("RP-ID-2", 15.0), ("RP-ID-3", 10.5)], ...}
		self._adjacency_list: Dict[str, List[Tuple[str, float]]] = {}
		self._load_map(map_filepath)

	def _load_map(self, map_filepath: str) -> None:
		"""
		Parses the JSON map file and populates the graph's data structures.

		Raises:
			FileNotFoundError: If the map file cannot be found.
			KeyError: If the JSON is missing required keys.
		"""

		print(f"Attempting to load map from: {map_filepath}")

		try:
			with open(map_filepath, 'r') as f:
				map_data = json.load(f)
		
		except FileNotFoundError:
			print(f"ERROR: Map file not found at '{map_filepath}'")
			raise

		# 1. Load all Reference Points
		for rp_data in map_data["reference_points"]:
			coords = Coordinates(**rp_data["coordinates"])
			fingerprints = [BLEFingerprint(**fp) for fp in rp_data["ble_fingerprint"]]

			rp = ReferencePoint(
				id = rp_data["id"],
				coordinates = coords,
				descriptor = rp_data["descriptor"],
				ble_fingerprint = fingerprints
			)

			self._reference_points[rp.id] = rp

			# Initialize an empty list for each RP in the adjacency list
			self._adjacency_list[rp.id] = []

		# 2. Load all Connections to build the adjacency list
		for conn_data in map_data["connections"]:
			from_id = conn_data["from"]
			to_id = conn_data["to"]
			distance = float(conn_data["distance"])

			if from_id in self._adjacency_list:
				self._adjacency_list[from_id].append((to_id, distance))
			else:
				print(f"Warning: Connection 'from' ID '{from_id}' not found in reference points.")

		print(f"Successfully loaded {len(self._reference_points)} reference points and {len(map_data['connections'])} connections.")

	def get_rp(self, rp_id: str) -> Optional[ReferencePoint]:
		"""
		Retrieves a ReferencePoint object by its ID.

		Args:
			rp_id: The unique identifier of the Reference Point.

		Returns:
			The ReferencePoint object if found, otherwise None.
		"""
		return self._reference_points.get(rp_id)

	def get_all_rps(self) -> List[ReferencePoint]:
		"""Returns a list of all ReferencePoint objects in the graph."""
		return list(self._reference_points.values())

	def get_neighbors(self, rp_id: str) -> List[Tuple[str, float]]:
		"""
		Gets all neighbors for a given RP and the distance to them.

		Args:
			rp_id: The ID of the reference point to find neighbors for.

		Returns:
			A list of tuples, where each tuple contains a neighbor's ID and the distance (weight) to it. Returns an empty list if the RP has no neighbors or doesn't exit.
		"""
		return self._adjacency_list.get(rp_id, [])