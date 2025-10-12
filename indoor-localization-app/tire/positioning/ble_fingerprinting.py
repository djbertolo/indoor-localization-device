# Logic for k-NN matching of BLE signals
import math
import json
from pathlib import Path
from typing import Dict, List, Optional

# Import the NavigationGraph class from the other module
from tire.navigation.graph import NavigationGraph, BLEFingerprint

# Type alias for clarity: A dictionary mapping a MAC address to its RSSI value.
LiveBLEScan = Dict[str, int]

def _calculate_distance(fingerprint1: List[BLEFingerprint], scan2: LiveBLEScan) -> float:
    """
    Calculates the Euclidean distance between a stored fingerprint and a live scan.

    This function treats the RSSI values of common beacons as dimensions in a
    multi-dimensional space. If a beacon is not present in both scans, it is ignored.
    """
    sum_of_squares = 0
    common_beacons = 0

    # Create a dictionary from the list for faster lookups
    fp1_dict = {fp.mac_address: fp.avg_rssi for fp in fingerprint1}

    for mac, rssi2 in scan2.items():
        if mac in fp1_dict:
            rssi1 = fp1_dict[mac]
            sum_of_squares += (rssi1 - rssi2) ** 2
            common_beacons += 1
    
    # If there are no common beacons, the distance is infinite (worst possible match)
    if common_beacons == 0:
        return float('inf')

    return math.sqrt(sum_of_squares)


def find_closest_rp(graph: NavigationGraph, live_scan: LiveBLEScan) -> Optional[str]:
    """
    Finds the closest Reference Point to the user based on a live BLE scan.

    This function implements a simple k-NN approach (with k=1) by calculating
    the signal distance between the live scan and every pre-recorded RP
    fingerprint in the graph.

    Args:
        graph: An initialized NavigationGraph object.
        live_scan: A dictionary representing the current BLE scan, mapping
                   MAC addresses to their live RSSI values.

    Returns:
        The ID of the closest matching Reference Point, or None if no RPs exist.
    """
    if not live_scan:
        return None

    closest_rp_id: Optional[str] = None
    smallest_distance = float('inf')

    all_rps = graph.get_all_rps()
    if not all_rps:
        return None

    for rp in all_rps:
        distance = _calculate_distance(rp.ble_fingerprint, live_scan)
        
        if distance < smallest_distance:
            smallest_distance = distance
            closest_rp_id = rp.id
            
    return closest_rp_id