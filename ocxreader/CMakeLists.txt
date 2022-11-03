cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(ocxreader VERSION ${OCXREADER_VERSION} LANGUAGES CXX)

# Where ocxreader's .h files can be found
set(ocx_reader_include_dirs
    "${OpenCASCADE_INCLUDE_DIR}"
    "${ocxreader_SOURCE_DIR}/include"
    "${ocxreader_SOURCE_DIR}"
    "${ocx_SOURCE_DIR}/include"
    # This directory is needed to build directly from ocx sources.
    "${ocx_SOURCE_DIR}")
include_directories(SYSTEM ${ocx_reader_include_dirs})

# Define helper functions and macros used by ocxreader
# include(cmake/internal_utils.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add executable
add_executable(${PROJECT_NAME} src/main.cc)

target_link_libraries(${PROJECT_NAME} debug ocx)
target_link_libraries(${PROJECT_NAME} optimized ocx)

# Copy runtime dlls to build directory
# copy_occt_runtime_dlls_dynamic(${PROJECT_NAME})
