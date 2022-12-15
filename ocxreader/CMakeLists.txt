cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ocxreader VERSION ${OCXREADER_VERSION} LANGUAGES CXX)

# Where ocxreader's .h files can be found
set(ocx_reader_include_dirs
    "${OpenCASCADE_INCLUDE_DIR}"
    "${ocxreader_SOURCE_DIR}/include"
    "${ocxreader_SOURCE_DIR}"
    "${ocx_SOURCE_DIR}/include"
    # This directory is needed to build directly from ocx sources.
    "${ocx_SOURCE_DIR}"
    "${shipxml_SOURCE_DIR}/include"
    # This directory is needed to build directly from shipxml sources.
    "${shipxml_SOURCE_DIR}")
include_directories(SYSTEM ${ocx_reader_include_dirs})

# Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options)
if (NOT Boost_FOUND)
  message(STATUS "No local Boost installation found. Loading Boost modules via FetchContent.")
  include(FetchContent)
  FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.80.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
  )
  FetchContent_MakeAvailable(Boost)
endif ()
set(boost_libs Boost::program_options)

# Define helper functions and macros used by ocxreader
include(cmake/internal_utils.cmake)

# Set compile definitions
add_compile_definitions(
  $<$<CONFIG:DEBUG>:OCXREADER_CONFIG_DEBUG>
  $<$<CONFIG:RELEASE>:OCXREADER_CONFIG_RELEASE>)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS")
  set(CMAKE_CXX_EXTENSIONS ON)
endif ()

# Add executable
add_executable(${PROJECT_NAME} src/ocx-reader-all.cc)

target_link_libraries(${PROJECT_NAME} libocx libshipxml spdlog::spdlog ${boost_libs})

target_link_occt_libraries(${PROJECT_NAME})
