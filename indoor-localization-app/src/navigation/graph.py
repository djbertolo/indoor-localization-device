# Loads and manages the navigation graph from JSON

import json
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass

@dataclass(frozen=True)
class Coordinates:
	"""Represents the X, Y coordinates of a Reference Point."""
	x: float
	y: float

@dataclass(frozen=True)
class BLEFingerprint:
	"""Represents a single entry in a BLE fingerprint scan."""
	mac_addr: str
	avg_rssi: int

@dataclass(frozen=True)
class ReferencePoint:
	"""Represents a single node (RP) in our navigation graph."""
	id: str
	coordinates: Coordinates
	descriptor: str
	ble_fingerprint: List[BLEFingerprint]

