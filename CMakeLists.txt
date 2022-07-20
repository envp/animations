if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(simulations VERSION 0.0.1 LANGUAGES CXX)

cmake_minimum_required(VERSION 3.23)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

find_package(CCache REQUIRED)
include(ColorDiagnostics)

add_subdirectory(vendor/SFML)
add_subdirectory(src/Starfield)
