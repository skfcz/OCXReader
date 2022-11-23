# Configure OCCT libraries to link with
macro (target_link_occt_libraries target)
  foreach (LIB ${OpenCASCADE_LIBRARIES})
    if (WIN32)
      if (EXISTS ${OpenCASCADE_LIBRARY_DIR}${DEBUG_SUFFIX}/${LIB}.lib)
        if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
          set(DEBUG_SUFFIX "d")
        endif ()
        set(occt_lib_dir ${OpenCASCADE_LIBRARY_DIR}${DEBUG_SUFFIX}/${LIB}.lib)
        message(STATUS "Adding ${occt_lib_dir}")

        target_link_libraries(${target} debug ${occt_lib_dir})
        target_link_libraries(${target} optimized ${occt_lib_dir})
      else ()
        message(FATAL_ERROR "Library ${LIB} not found in ${OpenCASCADE_LIBRARY_DIR}${DEBUG_SUFFIX}")
      endif ()
    elseif (APPLE)
      if (EXISTS ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.dylib)
        set(occt_lib_dir ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.dylib)
        message(STATUS "Adding ${occt_lib_dir}")

        target_link_libraries(${target} debug ${occt_lib_dir})
        target_link_libraries(${target} optimized ${occt_lib_dir})
      else ()
        message(FATAL_ERROR "Library lib${LIB} not found in ${OpenCASCADE_LIBRARY_DIR}")
      endif ()
    elseif (UNIX)
      if (EXISTS ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
        set(occt_lib_dir ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
        message(STATUS "Adding ${occt_lib_dir}")

        target_link_libraries(${target} debug ${occt_lib_dir})
        target_link_libraries(${target} optimized ${occt_lib_dir})
      else ()
        message(FATAL_ERROR "Library lib${LIB} not found in ${OpenCASCADE_LIBRARY_DIR}")
      endif ()
    endif ()
  endforeach ()
endmacro ()


# Copy target runtime dlls to the build directory by manually specifying the dlls to copy
macro (copy_runtime_dlls_manual target)
  message(STATUS "OpenCASCADE_BINARY_DIR: ${OpenCASCADE_BINARY_DIR}")
  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND
    $<$<CONFIG:DEBUG,RELEASE>:${CMAKE_COMMAND}> -E copy_directory
    $<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>
    $<TARGET_FILE_DIR:${target}>)
endmacro ()


# Dynamically resolve target runtime dlls with $<TARGET_RUNTIME_DLLS:tgt>
# available with CMake 3.21+
# https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html#genex:TARGET_RUNTIME_DLLS
#
# Not working currently: https://gitlab.kitware.com/cmake/cmake/-/issues/22845
macro (copy_runtime_dlls_dynamic target)
  # Print runtime dlls
  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Runtime Dlls:
   $<TARGET_RUNTIME_DLLS:${target}>")

  # Copy dlls to build directory dynamically resolving target runtime dlls
  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${target}>
    $<TARGET_FILE_DIR:${target}>
    COMMAND_EXPAND_LISTS)
endmacro ()
