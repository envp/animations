add_executable(starfield Starfield.cpp)

target_link_libraries(starfield sfml-graphics)

add_custom_target(
  run-starfield
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/starfield"
  COMMENT "Running starfield..."
)

add_dependencies(run-starfield starfield)
