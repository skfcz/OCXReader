cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(shipxml VERSION ${OCXREADER_VERSION} LANGUAGES CXX)

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
set(shipxml_include_dirs
    "${OpenCASCADE_INCLUDE_DIR}"
    "${ocx_SOURCE_DIR}/include"
    "${ocx_SOURCE_DIR}"
    "${shipxml_SOURCE_DIR}/include"
    "${shipxml_SOURCE_DIR}"
)
include_directories(SYSTEM ${shipXML_include_dirs})

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

cxx_library(${PROJECT_NAME} src/shipxml-all.cc)

# add_subdirectory(test)

# Set target link libraries
target_link_libraries(${PROJECT_NAME} occutils spdlog::spdlog)

# Adjust runtime environment for Visual Studio
set_property(
  TARGET ${PROJECT_NAME}
  PROPERTY
  VS_DEBUGGER_ENVIRONMENT
  "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%"
)
