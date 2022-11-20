# Copy target runtime dlls to the build directory by manually specifying the dlls to copy
macro (copy_runtime_dlls_manual TARGET_NAME)
  message(STATUS "OpenCASCADE_BINARY_DIR: ${OpenCASCADE_BINARY_DIR}")
  add_custom_command(
    TARGET ${TARGET_NAME}
    POST_BUILD
    COMMAND
    $<$<CONFIG:DEBUG,RELEASE>:${CMAKE_COMMAND}> -E copy_directory
    $<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>
    $<TARGET_FILE_DIR:${TARGET_NAME}>)
endmacro ()


# Dynamically resolve target runtime dlls with $<TARGET_RUNTIME_DLLS:tgt>
# available with CMake 3.21+
# https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html#genex:TARGET_RUNTIME_DLLS
#
# Not working currently: https://gitlab.kitware.com/cmake/cmake/-/issues/22845
macro (copy_runtime_dlls_dynamic TARGET_NAME)
  # Print runtime dlls
  add_custom_command(
    TARGET ${TARGET_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Runtime Dlls:
   $<TARGET_RUNTIME_DLLS:${TARGET_NAME}>")

  # Copy dlls to build directory dynamically resolving target runtime dlls
  add_custom_command(
    TARGET ${TARGET_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${TARGET_NAME}>
    $<TARGET_FILE_DIR:${TARGET_NAME}>
    COMMAND_EXPAND_LISTS)
endmacro ()
