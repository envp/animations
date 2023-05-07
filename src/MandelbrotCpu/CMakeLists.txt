add_executable(mandelbrot Mandelbrot.cpp)

set(MANDELBROT_PALETTE_NUM_LEVELS 16)
set(MANDELBROT_PALETTE_KIND gaussian)

target_compile_definitions(
  mandelbrot
  PRIVATE
  MANDELBROT_PALETTE_INC="${CMAKE_CURRENT_BINARY_DIR}/Palette.inc"
  MANDELBROT_PALETTE_NUM_LEVELS=${MANDELBROT_PALETTE_NUM_LEVELS}
)
target_link_libraries(mandelbrot raylib)

add_custom_target(
  run-mandelbrot
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/mandelbrot"
  COMMENT "Running mandelbrot simulation..."
)

add_custom_target(
  generate-palette
  "./color_palette.py"
    --num-levels "${MANDELBROT_PALETTE_NUM_LEVELS}"
    --palette ${MANDELBROT_PALETTE_KIND}
    --output "${CMAKE_CURRENT_BINARY_DIR}/Palette.inc"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/Palette.inc
  COMMENT "Generating palette..."
)

add_dependencies(run-mandelbrot mandelbrot)
add_dependencies(mandelbrot generate-palette)
