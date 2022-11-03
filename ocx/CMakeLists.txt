cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(ocx VERSION ${OCXREADER_VERSION} LANGUAGES CXX)

# Where ocx's .h files can be found
set(ocx_include_dirs
    "${OpenCASCADE_INCLUDE_DIR}"
    "${ocx_SOURCE_DIR}/include"
    "${ocx_SOURCE_DIR}")
include_directories(SYSTEM ${ocx_include_dirs})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(${PROJECT_NAME} STATIC src/ocx-all.cc)

add_subdirectory(test)

# Set target link libraries
foreach (LIB ${OpenCASCADE_LIBRARIES})
  if (WIN32)
    if (EXISTS ${OpenCASCADE_LIBRARY_DIR}${DEBUG_SUFFIX}/${LIB}.lib)
      if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(DEBUG_SUFFIX "d")
      endif ()
      message(STATUS "Adding ${OpenCASCADE_LIBRARY_DIR}${DEBUG_SUFFIX}/${LIB}.lib")
      target_link_libraries(${PROJECT_NAME} debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
      target_link_libraries(${PROJECT_NAME} optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
    else ()
      message(FATAL_ERROR "Library ${LIB} not found in ${OpenCASCADE_LIBRARY_DIR}${DEBUG_SUFFIX}")
    endif ()
  elseif (APPLE)
    if (EXISTS ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.dylib)
      message(STATUS "Adding ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.dylib")
      target_link_libraries(${PROJECT_NAME} debug ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.dylib)
      target_link_libraries(${PROJECT_NAME} optimized ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.dylib)
    else ()
      message(FATAL_ERROR "Library lib${LIB} not found in ${OpenCASCADE_LIBRARY_DIR}")
    endif ()
  elseif (UNIX)
    if (EXISTS ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
      message(STATUS "Adding ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so")
      target_link_libraries(${PROJECT_NAME} debug ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
      target_link_libraries(${PROJECT_NAME} optimized ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
    else ()
      message(FATAL_ERROR "Library lib${LIB} not found in ${OpenCASCADE_LIBRARY_DIR}")
    endif ()
  endif ()
endforeach ()

# Adjust runtime environment for Visual Studio
set_property(
  TARGET ${PROJECT_NAME}
  PROPERTY
  VS_DEBUGGER_ENVIRONMENT
  "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%"
)
