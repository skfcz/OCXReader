cmake_minimum_required(VERSION 3.24)

project(occutils VERSION ${OCXREADER_VERSION} LANGUAGES CXX)

# Where occutils's .h files can be found
set(occutils_include_dirs
    "${OpenCASCADE_INCLUDE_DIR}"
    "${occutils_SOURCE_DIR}/include"
    "${occutils_SOURCE_DIR}")
include_directories(SYSTEM ${occutils_include_dirs})

# Define helper functions and macros used by occutils
include(cmake/internal_utils.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS")
  set(CMAKE_CXX_EXTENSIONS ON)
endif ()

cxx_library(${PROJECT_NAME} src/occutils-all.cc)

add_library(ocxreader::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Set target link libraries
target_link_libraries(${PROJECT_NAME}
                      Boost::boost
                      Boost::system
                      Boost::filesystem)

# Adjust runtime environment for Visual Studio
set_property(
  TARGET ${PROJECT_NAME}
  PROPERTY
  VS_DEBUGGER_ENVIRONMENT
  "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%"
)

if (${occutils_build_tests})
  # add_subdirectory(test) # TODO: Add tests
endif ()
