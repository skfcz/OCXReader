cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

option(ocx_build_tests "Build OCX tests." OFF)
option(occutils_build_tests "Build OCCUtils tests." OFF)
if (ocx_build_tests OR occutils_build_tests)
  list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif ()

set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_LIST_DIR}/vcpkg/triplets" CACHE STRING "")

project(OCXReader)
set(OCXREADER_VERSION 0.1.0)

enable_testing()

include(GNUInstallDirs)

# Define helper functions and macros used by ocxreader-distribution
include(cmake/internal_utils.cmake)

# Licensing
build_3rd_party_copyright()

# Dependencies
find_package(spdlog CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(OpenCASCADE CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED) # freetype is a dependency of OpenCASCADE
find_package(Boost REQUIRED COMPONENTS program_options system filesystem)
find_package(GTest CONFIG REQUIRED)

# Resolve dependencies using git submodules
get_filename_component(SUBMODULE_DIR ${CMAKE_CURRENT_LIST_DIR}/deps ABSOLUTE)

# Local libraries
add_subdirectory(libs/occutils)
add_subdirectory(libs/ocx)
add_subdirectory(libs/shipxml)

# Application
add_subdirectory(ocxreader)
