import json
import math
import os

class MapBuilder:
    def __init__(self):
        self.nodes = {} # Dictionary of node_id -> node_data
        self.fingerprints = [] # List of fingerprint objects

    def add_node(self, node_id, x, y, name="Unknown", audio_file=""):
        """Adds a navigation node (Reference Point)."""
        if node_id in self.nodes:
            print(f"Warning: Overwriting node {node_id}")
        
        self.nodes[node_id] = {
            "id": node_id,
            "x": float(x),
            "y": float(y),
            "name": name,
            "audio": audio_file,
            "neighbors": {} # Adjacency list: neighbor_id -> distance
        }

    def add_connection(self, id1, id2):
        """Creates a bi-directional link between two nodes and calculates distance."""
        if id1 not in self.nodes or id2 not in self.nodes:
            print(f"Error: Cannot connect {id1} and {id2}. One or both do not exist.")
            return

        n1 = self.nodes[id1]
        n2 = self.nodes[id2]

        # Calculate Euclidean distance
        dx = n1["x"] - n2["x"]
        dy = n1["y"] - n2["y"]
        distance = math.sqrt(dx*dx + dy*dy)

        # Add to neighbor lists
        self.nodes[id1]["neighbors"][id2] = distance
        self.nodes[id2]["neighbors"][id1] = distance
        print(f"Connected {id1} <--> {id2} (Distance: {distance:.2f}m)")

    def add_fingerprint(self, rp_id, signals):
        """
        Adds a BLE fingerprint for a specific RP.
        signals: dict of { "BEACON_ID": RSSI_INT }
        """
        if rp_id not in self.nodes:
            print(f"Warning: Adding fingerprint for non-existent node {rp_id}")
            # We allow this, as you might map radio data without nav nodes, but warn.
            x, y = 0.0, 0.0
        else:
            x = self.nodes[rp_id]["x"]
            y = self.nodes[rp_id]["y"]

        fingerprint = {
            "rp_id": rp_id,
            "x": x,
            "y": y,
            "signals": signals
        }
        self.fingerprints.append(fingerprint)

    def save_files(self, map_filename="campus_map.json", radio_filename="campus_radio_map.json"):
        # 1. Save Navigation Map
        nav_data = {"nodes": list(self.nodes.values())}
        
        # Ensure directory exists
        os.makedirs(os.path.dirname(map_filename), exist_ok=True)
        
        with open(map_filename, 'w') as f:
            json.dump(nav_data, f, indent=4)
        print(f"Successfully saved Navigation Map to: {map_filename}")

        # 2. Save Radio Map
        radio_data = {"fingerprints": self.fingerprints}
        
        with open(radio_filename, 'w') as f:
            json.dump(radio_data, f, indent=4)
        print(f"Successfully saved Radio Map to: {radio_filename}")

def generate_demo_map():
    print("Generating Demo Map (Simple Hallway)...")
    builder = MapBuilder()

    # --- 1. Define Nodes (Reference Points) ---
    # Matches the coordinates from your BLEFingerprinting.cpp placeholder
    builder.add_node("RP_HALLWAY_START", 0.0, 0.0, "Start of Hallway", "guidance_start.wav")
    builder.add_node("RP_HALLWAY_MIDDLE", 0.0, 5.0, "Middle of Hallway", "guidance_middle.wav")
    builder.add_node("RP_HALLWAY_END", 0.0, 10.0, "End of Hallway", "guidance_end.wav")

    # --- 2. Connect Nodes (Define walkable paths) ---
    builder.add_connection("RP_HALLWAY_START", "RP_HALLWAY_MIDDLE")
    builder.add_connection("RP_HALLWAY_MIDDLE", "RP_HALLWAY_END")
    # Note: We don't connect START directly to END, forcing pathfinding to go through MIDDLE

    # --- 3. Add Fingerprint Data (Simulated RSSI) ---
    
    # At Start: Beacon 1 is close (-50), others far
    builder.add_fingerprint("RP_HALLWAY_START", {
        "BEACON_ID_1": -50,
        "BEACON_ID_2": -80,
        "BEACON_ID_3": -90
    })

    # At Middle: Between Beacon 1 and 2
    builder.add_fingerprint("RP_HALLWAY_MIDDLE", {
        "BEACON_ID_1": -65,
        "BEACON_ID_2": -65,
        "BEACON_ID_3": -85
    })

    # At End: Beacon 2 is close (-50)
    builder.add_fingerprint("RP_HALLWAY_END", {
        "BEACON_ID_1": -90,
        "BEACON_ID_2": -50,
        "BEACON_ID_3": -80
    })

    # --- 4. Save Output ---
    # Saves to the path expected by main.cpp
    builder.save_files("data/maps/campus_map.json", "data/maps/campus_radio_map.json")

def main():
    print("TIRE Map Creator Tool")
    print("---------------------")
    print("1. Generate Demo Map (Hallway)")
    print("2. Create Custom Map (Interactive - Not Implemented)")
    
    choice = input("Select option (default 1): ").strip()
    
    if choice == "2":
        print("Interactive mode is not yet implemented. Please use the code to add nodes manually.")
    else:
        generate_demo_map()

if __name__ == "__main__":
    main()