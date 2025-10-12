# Project TIRE - Development Log

This log serves as the official research journal for the project. Its purpose is to document progress, track challenges, and record key insights and ideas throughout the development lifecycle. Each entry should provide a snapshot of the project's status on that date.

---

## Log Entry: October 12<sup>th</sup> - October 18<sup>th</sup>, 2025

**Authors(s): Dominic Bertolo**

### ✅ Progress & Accomplishments
- United Nations' Sustainable Development Goal (SDG) 10, "Reduced Inequalities", was chosen as the SDG that TIRE most directly aligns with. Justification for this alignment was added to the [README.md](/README.md#2-the-solution).
- Added NavigationGraph class
- Added A* algorithm pathfinding
- Added BLE fingerprinting system

### 🤔 Insights & Discoveries

### 💡 Ideas & Next Steps
- **Idea:** While the United Nations' SDG 10, "Reduced Inequalities", is explained in the [README.md](/README.md#2-the-solution), a more formal defense for TIRE's intellectual merit might be beneficial in a revised version of a report about the project.

### ⚠️ Challenges & Blockers
- **Challenge:** The justification for TIRE's intellectual merit will be difficult as the world of assistive technology is not lacking in activity and neither is the subject of indoor-localization methods. Additionally, the need to defend both its intellectual merit as a technical project on cyber-physical tools and its intellectual merit as a UN SDG and social good requires an equally deep dive into both sections to understand where exactly is TIRE's niche.

## Log Entry: October 5<sup>th</sup> - October 11<sup>th</sup>, 2025

**Author(s): Dominic Bertolo**

### ✅ Progress & Accomplishments
- Created a GitHub Repository to host all project-related files, including software and documentation.
- Finalized and submitted the [Project Proposal](/documents/project-proposal/Project_Proposal.pdf) that details the planned development of TIRE, including design deliverables.
- Drafted a first revision of the [Technical Implementation Brief](/documents/technical-brief/Technical_Brief.pdf).
- Received and processed feedback provided by Dr. Hou during a team meeting where we discussed the project proposition and technical details.
- Made the decision to change from a WiFi fingerprinting system to a **BLE beacon-based** fingerprinting system for the wireless positioning subsystem, as suggested.
- Brainstormed new project names to better reflect the primary goal of the project, resulting in Turn-by-turn Indoor Routing Engine (TIRE).

### 🤔 Insights & Discoveries
- Dr. Hou's feedback confirmed that the technical depth of the project is strong, however the technical implementation remains impractically ambitious to tackle all at once. The suggestion to use BLE beacons as a fingerprinting method will be more reliable to work with and allow focus to be directed towards the other technically complex aspects of the TIRE's implementation. Additionally, Dr. Hou advised that if the desire to pursue WiFi Fingerprinting remained, successful implementation of the BLE Beacon fingerprinting version of TIRE will provide a strong base to attempt a WiFi fingerprinting alternative.
- Switching to BLE beacons gives us more control over the infrastructure, allowing us to reduce risk on the project by removing the dependency on the consistency and reliability of the university's WiFi network. This is a major advantage for a capstone project on a relatively tight timeline.
- Dr. Hou's suggestion of forcing a starting position and starting orientation for initial testing will eliminate the room for error when it comes to the initialization of the Navigation Handling for TIRE. Upon success in other functionality, dynamic starting positions and orientations may be explored.

### 💡 Ideas & Next Steps
- **Idea:** Create different versions of maps for the Jack Brown Hall 3<sup>rd</sup> floor (the intended testing area).
- **Next Step:** Reiterate through the United Nations' 17 Goals for Sustainable Development. Choose the goal that TIRE most aligns with and create a justification defending TIRE's intellectual merit with this goal needed to be strongly represented.
- **Next Step:** Create a revised version of the Technical Implementation Brief to include the updated BLE Beacon fingerprinting system. Additionally, this revised version should contain references to articles found and utilized during Literature Review phase.
- **Next Step:** Deep literature review into IMU-based Pedestrian Dead Reckoning systems and understanding how data from the accelerometer and gyroscope is properly filtered and cleaned before interpretation/analysis.

### ⚠️ Challenges & Blockers
- **Challenge:** The project's initial estimated expense of $63-$95 will need to be revised to account for the purchase of 10-15 BLE beacons and an RTC module.