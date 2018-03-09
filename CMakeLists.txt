cmake_minimum_required(VERSION 3.6)

project(chip_8_emulator CXX)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(chip_8_emulator	srcs/main.cpp
				srcs/Chip8.cpp
				srcs/Screen.cpp)

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED network audio graphics window system)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(chip_8_emulator ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()
