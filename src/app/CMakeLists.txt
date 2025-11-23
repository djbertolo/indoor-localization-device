# Define the executable using your main entry point
add_executable(tire main.cpp)

# Link your custom library (which automatically pulls in Eigen and JSON)
target_link_libraries(tire PRIVATE tire-lib)