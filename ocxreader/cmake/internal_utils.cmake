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

########################################################################
#
# Helper functions for creating build targets.

# cxx_executable_with_flags(name cxx_flags libs srcs...)
#
# creates a named C++ executable that depends on the given libraries and
# is built from the given source files with the given compiler flags.
function (cxx_executable_with_flags name cxx_flags libs)
  add_executable(${name} ${ARGN})
  if (cxx_flags)
    set_target_properties(${name}
                          PROPERTIES
                          COMPILE_FLAGS "${cxx_flags}")
  endif ()
  # To support mixing linking in static and dynamic libraries, link each
  # library in with an extra call to target_link_libraries.
  foreach (lib ${libs})
    target_link_libraries(${name} ${lib})
  endforeach ()
endfunction ()

# cxx_executable(name dir lib srcs...)
#
# creates a named target that depends on the given libs and is built
# from the given source files.  dir/name.cc is implicitly included in
# the source file list.
function (cxx_executable name dir libs)
  cxx_executable_with_flags(
    ${name} "${cxx_default}" "${libs}" "${dir}/${name}.cc" ${ARGN})
endfunction ()
