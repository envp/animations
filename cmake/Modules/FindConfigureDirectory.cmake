# Called as:
# LOCAL_ASSET_DIR names the
# ```
# target_configure_asset_dir(
#   target LOCAL_ASSET_DIR <path> DEFINE <variable>
#   CONFIGURE <file> [CONFIGURE <file>...]
# )
# ```
function(target_configure_asset_dir target)
  message(STATUS "Configuring assets for: ${target}")
  set(one_value_args ASSETS_RELPATH DEFINE_VAR)
  cmake_parse_arguments(PARSED_ARGS "" "${one_value_args}" "" ${ARGN})

  if(NOT PARSED_ARGS_ASSETS_RELPATH)
    message(FATAL_ERROR "Missing required arg: ASSETS_RELPATH <relpath>")
  endif()
  set(ASSET_DIRNAME "${PARSED_ARG_ASSETS}")

  if(NOT PARSED_ARGS_DEFINE_VAR)
    message(FATAL_ERROR "Missing required arg: DEFINE_VAR <output-var>")
  endif()
  set(OUTPUTVAR "${PARSED_ARG_DEFINE_VAR}")

  cmake_path(APPEND full_dest_path "${CMAKE_CURRENT_BINARY_DIR}" "${ASSET_DIRNAME}")
  cmake_path(APPEND full_src_path "${CMAKE_CURRENT_SOURCE_DIR}" "${ASSET_DIRNAME}")
  add_custom_target(
    "${target}-copy-assets"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${full_src_path}" "${full_dest_path}"
    COMMENT "Copying assets for: ${target}"
  )
  add_dependencies(${target} "${target}-copy-assets")
  set("${PARSED_ARGS_DEFINE_VAR}" "${full_asset_path}" PARENT_SCOPE)
endfunction()
