cmake_minimum_required(VERSION 3.10)
project(alescript CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.h")
file(GLOB_RECURSE SOURCEIDE_FILES "src-ide/*.cpp" "src-ide/*.h")

# Main program (always built)
add_executable(alescript ${SOURCE_FILES})

# IDE program (Linux/macOS only)
if (NOT WIN32)
    add_executable(aleIDE ${SOURCEIDE_FILES})

    find_package(Curses REQUIRED)
    if (CURSES_FOUND)
        target_include_directories(aleIDE PRIVATE ${CURSES_INCLUDE_DIRS})
        target_link_libraries(aleIDE PRIVATE ${CURSES_LIBRARIES})
    else()
        message(FATAL_ERROR "ncurses not found on Linux/macOS.")
    endif()
endif()
