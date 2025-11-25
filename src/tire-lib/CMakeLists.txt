# Define the library and list all source files (as per your README structure)
add_library(tire-lib
    private/EKF.cpp
    private/PDR.cpp
    private/BLEFingerprinting.cpp
    private/NavigationGraph.cpp
    private/Announcer.cpp
    private/interfaces/SimulatedHardware.cpp
    # private/interfaces/RaspberryPiHardware.cpp # Uncomment this when you add the file
	private/Pathfinder.cpp
)

# Allow other targets (like the app) to include headers from the 'include' folder
target_include_directories(tire-lib PUBLIC include)

# Link the third-party libraries we added in the root CMake file
target_link_libraries(tire-lib
    PUBLIC
    Eigen3::Eigen
    nlohmann_json::nlohmann_json
)