cmake_minimum_required(VERSION 3.15)
project(TIRE VERSION 0.1.0 LANGUAGES CXX)

# Use C++17 (Required for some nlohmann/json features and modern C++ practices)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. External Dependencies ---
# These assume you have run the git submodule commands for eigen and json
add_subdirectory(external/eigen)
add_subdirectory(external/json)

# --- 2. Internal Source Code ---
# Process the library first so the app can link to it
add_subdirectory(src/tire-lib)
add_subdirectory(src/app)