cmake_minimum_required (VERSION 3.0.0 FATAL_ERROR)

# Project name
project (OCXReader CXX)

# OpenCascade
find_package(OpenCASCADE)

# Configure C++ compiler's includes dir
include_directories ( includes SYSTEM ${OpenCASCADE_INCLUDE_DIR} )

set( SOURCES src/main.cpp src/OCXCAFControl_Reader.cpp src/OCXCAFControl_Reader.h
        src/OCXHelper.cpp src/OCXHelper.h   src/OCXPanel.cpp src/OCXPanel.h     )

# Add executable
add_executable (OCXReader ${SOURCES})


#message(STATUS ${OpenCASCADE_LIBRARIES})

# Add linker options
set(OpenCASCADE_LIBRARY_DIR /usr/local/lib)

foreach (LIB ${OpenCASCADE_LIBRARIES})
  if (WIN32)
    target_link_libraries(OCXReader debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
    target_link_libraries(OCXReader optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
  else()
    target_link_libraries(OCXReader debug ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
    target_link_libraries(OCXReader optimized ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
  endif()
endforeach()

# Adjust runtime environment
set_property(TARGET OCXReader PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%")
