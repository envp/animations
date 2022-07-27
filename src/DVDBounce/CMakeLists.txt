add_executable(dvdbounce DVDBounce.cpp)

target_link_libraries(dvdbounce sfml-graphics)
target_include_directories(dvdbounce PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

find_package(ConfigureDirectory)

target_configure_asset_dir(
  dvdbounce
  ASSETS_RELPATH "./assets"
  DEFINE_VAR DVDBOUNCE_ASSET_ROOT
)

add_custom_target(
  run-dvdbounce
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/dvdbounce"
  COMMENT "Running DVD Bounce..."
)

add_dependencies(run-dvdbounce dvdbounce)
