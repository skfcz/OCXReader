option(libocx_build_tests "Build all of libocx's own tests." OFF)

cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ocx VERSION ${OCXREADER_VERSION} LANGUAGES CXX)

set(library_name lib${PROJECT_NAME})

# External dependencies
include(FetchContent)

FetchContent_Declare(
  spdlog # Specify the commit you depend on and update it regularly.
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.x)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
  OCCUtils
  GIT_REPOSITORY https://github.com/paulbuechner/OCCUtils.git
  GIT_TAG master)
FetchContent_MakeAvailable(OCCUtils)

# Where ocx's .h files can be found
set(ocx_include_dirs
    "${OpenCASCADE_INCLUDE_DIR}"
    "${ocx_SOURCE_DIR}/include"
    "${ocx_SOURCE_DIR}"
    )
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

cxx_library(${library_name} src/ocx-all.cc)

if (${libocx_build_tests})
  add_subdirectory(test)
endif ()

# Set target link libraries
target_link_libraries(${library_name} occutils spdlog::spdlog)

# Adjust runtime environment for Visual Studio
set_property(
  TARGET ${library_name}
  PROPERTY
  VS_DEBUGGER_ENVIRONMENT
  "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%"
)
