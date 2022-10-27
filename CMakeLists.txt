cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# macro: include patched file if it exists
macro(OCXR_INCLUDE_CMAKE_FILE BEING_INCLUDED_FILE)
  include(${CMAKE_SOURCE_DIR}/${BEING_INCLUDED_FILE}.cmake)
endmacro()

# Project name
project(OCXReader CXX)

# Set OCCT cmake directory on windows
if (WIN32)
  set(OpenCASCADE_DIR "C:/Program Files/OCCT/cmake")
endif ()

find_package(OpenCASCADE REQUIRED)

# Configure C++ compiler's includes dir
include_directories(includes SYSTEM ${OpenCASCADE_INCLUDE_DIR})
# message("OpenCASCADE_INCLUDE_DIR: ${OpenCASCADE_INCLUDE_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/main.cpp
    src/OCXContext.h
    src/OCXContext.cpp
    src/OCXCAFControl_Reader.cpp
    src/OCXCAFControl_Reader.h
    src/OCXHelper.cpp
    src/OCXHelper.h
    src/OCXCoordinateSystemReader.cpp
    src/OCXCoordinateSystemReader.h
    src/OCXReferenceSurfacesReader.cpp
    src/OCXReferenceSurfacesReader.h
    src/OCXCurveReader.cpp
    src/OCXCurveReader.h
    src/OCXSurfaceReader.cpp
    src/OCXSurfaceReader.h
    src/OCXPanelReader.cpp
    src/OCXPanelReader.h)

# Add executable
add_executable(OCXReader ${SOURCES})

# message(STATUS ${OpenCASCADE_LIBRARIES})
foreach (LIB ${OpenCASCADE_LIBRARIES})
  if (WIN32)
    if (EXISTS ${OpenCASCADE_LIBRARY_DIR}${DEBUG_SUFFIX}/${LIB}.lib)
      if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(DEBUG_SUFFIX "d")
      endif ()
      message(STATUS "Adding ${OpenCASCADE_LIBRARY_DIR}${DEBUG_SUFFIX}/${LIB}.lib")
      target_link_libraries(OCXReader debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
      target_link_libraries(OCXReader optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
    else ()
      message(FATAL_ERROR "Library ${LIB} not found in ${OpenCASCADE_LIBRARY_DIR}${DEBUG_SUFFIX}")
    endif ()
  elseif (APPLE)
    if (EXISTS ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.dylib)
      message(STATUS "Adding ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.dylib")
      target_link_libraries(OCXReader debug ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.dylib)
      target_link_libraries(OCXReader optimized ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.dylib)
    else ()
      message(FATAL_ERROR "Library lib${LIB} not found in ${OpenCASCADE_LIBRARY_DIR}")
    endif ()
  elseif (UNIX)
    if (EXISTS ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
      message(STATUS "Adding ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so")
      target_link_libraries(OCXReader debug ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
      target_link_libraries(OCXReader optimized ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
    else ()
      message(FATAL_ERROR "Library lib${LIB} not found in ${OpenCASCADE_LIBRARY_DIR}")
    endif ()
  endif ()
endforeach ()

# Copy runtime dlls to build directory
# cmake-format: off
# ocxr_include_cmake_file("adm/cmake/ocxr_copy_runtime_dlls")
# cmake-format: on

# Adjust runtime environment for Visual Studio
# cmake-format: off
# message(STATUS "OpenCASCADE_BINARY_DIR: ${OpenCASCADE_BINARY_DIR}")
# cmake-format: on
set_property(
  TARGET OCXReader
  PROPERTY
  VS_DEBUGGER_ENVIRONMENT
  "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%"
)
