# Copy target runtime dlls to the build directory

# ##### FIRST METHOD (Currently the only working option) #####

# Copy dlls by manually specifying the dlls to copy
# cmake-format: off
# message(STATUS "OpenCASCADE_BINARY_DIR: ${OpenCASCADE_BINARY_DIR}")
# add_custom_command(
#   TARGET OCXReader
#   POST_BUILD
#   COMMAND
#     $<$<CONFIG:Debug,Release>:${CMAKE_COMMAND}> -E copy_directory
#     $<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>
#     $<TARGET_FILE_DIR:OCXReader>)
# cmake-format: on

# #### SECOND METHOD (Not working) #####
# Dynamically resolve target runtime dlls with $<TARGET_RUNTIME_DLLS:tgt>
# available with CMake 3.21+
# https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html#genex:TARGET_RUNTIME_DLLS
#
# Issue: https://gitlab.kitware.com/cmake/cmake/-/issues/22845

# Print runtime dlls
# cmake-format: off
# add_custom_command(
#   TARGET OCXReader
#   POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E echo "Runtime Dlls:
# $<TARGET_RUNTIME_DLLS:OCXReader>")
# cmake-format: on

# Copy dlls to build directory by dynamically resolving target runtime dlls
# cmake-format: off
# add_custom_command(
#   TARGET OCXReader
#   POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:OCXReader>
#           $<TARGET_FILE_DIR:OCXReader>
#   COMMAND_EXPAND_LISTS)
# cmake-format: on
