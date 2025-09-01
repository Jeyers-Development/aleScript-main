cmake_minimum_required(VERSION 3.10)
project(alescript CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.h")
file(GLOB_RECURSE SOURCEIDE_FILES "src-ide/*.cpp" "src-ide/*.h")

add_executable(alescript ${SOURCE_FILES})
add_executable(aleIDE ${SOURCEIDE_FILES})

if (WIN32)
	target_include_directories(aleIDE PRIVATE "C:/msys64/ucrt64/include/ncurses")
	target_link_directories(aleIDE PRIVATE "C:/msys64/ucrt64/lib")
	target_link_libraries(aleIDE PRIVATE ncursesw)
else()
	# Linux/macOS
    find_package(Curses REQUIRED)
    if(CURSES_FOUND)
        target_include_directories(aleIDE PRIVATE ${CURSES_INCLUDE_DIRS})
        target_link_libraries(aleIDE PRIVATE ${CURSES_LIBRARIES})
    else()
        message(FATAL_ERROR "ncurses not found on Linux/macOS.")
    endif()
endif()