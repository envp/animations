add_executable(dvdbounce DVDBounce.cpp)

target_link_libraries(dvdbounce sfml-graphics)
target_include_directories(dvdbounce PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

cmake_path(SET DVDBOUNCE_ASSET_ROOT "${CMAKE_CURRENT_BINARY_DIR}/assets")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Assets.h.in" "${CMAKE_CURRENT_BINARY_DIR}/Assets.h" @ONLY)

add_custom_target(
  copy-assets
  COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets" "${CMAKE_CURRENT_BINARY_DIR}/assets"
  COMMENT "Copying assets..."
)

add_dependencies(dvdbounce copy-assets)

add_custom_target(
  run-dvdbounce
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/dvdbounce"
  COMMENT "Running DVD Bounce..."
)

add_dependencies(run-dvdbounce dvdbounce)
