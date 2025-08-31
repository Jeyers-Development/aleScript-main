cmake_minimum_required(VERSION 3.10) # Specify a minimum CMake version
project(MyProject CXX) # Define the project name and language

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find all .cpp and .h files recursively in the current directory and subdirectories
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.h")

# Add an executable target using the found source files
add_executable(AleScript ${SOURCE_FILES})