cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ocx VERSION ${OCXREADER_VERSION} LANGUAGES CXX)

# Where ocx's .h files can be found
set(ocx_include_dirs
    "${OpenCASCADE_INCLUDE_DIR}"
    "${occutils_SOURCE_DIR}/include"
    "${ocx_SOURCE_DIR}/include"
    "${ocx_SOURCE_DIR}")
include_directories(SYSTEM ${ocx_include_dirs})

# Define helper functions and macros used by ocx
include(cmake/internal_utils.cmake)

# Set compile definitions
add_compile_definitions(
  $<$<CONFIG:DEBUG>:OCX_CONFIG_DEBUG>
  $<$<CONFIG:RELEASE>:OCX_CONFIG_RELEASE>)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS")
  set(CMAKE_CXX_EXTENSIONS ON)
endif ()

cxx_library(${PROJECT_NAME} src/ocx-all.cc)

add_library(ocxreader::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Set target link libraries
target_link_libraries(${PROJECT_NAME}
                      ocxreader::occutils
                      spdlog::spdlog)

# Adjust runtime environment for Visual Studio
set_property(
  TARGET ${PROJECT_NAME}
  PROPERTY
  VS_DEBUGGER_ENVIRONMENT
  "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%"
)

if (${ocx_build_tests})
  add_subdirectory(test)
endif ()
