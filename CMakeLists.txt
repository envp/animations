if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif(POLICY CMP0048)

project(simulations VERSION 0.0.1 LANGUAGES CXX)

cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

find_package(CCache REQUIRED)
include(ColorDiagnostics)

set(RAYLIB_VERSION 4.2.0)
find_package(raylib ${RAYLIB_VERSION})
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()


add_subdirectory(src/DVDBounce)
add_subdirectory(src/Starfield)
