cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ocxreader VERSION ${OCXREADER_VERSION} LANGUAGES CXX)

# Where ocxreader's .h files can be found
set(ocx_reader_include_dirs
    "${OpenCASCADE_INCLUDE_DIR}"
    "${ocx_SOURCE_DIR}/include"
    "${shipxml_SOURCE_DIR}/include"
    "${ocxreader_SOURCE_DIR}/include"
    "${ocxreader_SOURCE_DIR}")
include_directories(SYSTEM ${ocx_reader_include_dirs})

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

target_link_libraries(${PROJECT_NAME}
                      ocx
                      shipxml
                      spdlog::spdlog
                      Boost::boost
                      Boost::program_options)

target_link_occt_libraries_vcpkg(${PROJECT_NAME})
