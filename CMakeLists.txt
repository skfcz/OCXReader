cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ocxreader-distribution)
set(OCXREADER_VERSION 0.1.0)

enable_testing()

include(GNUInstallDirs)

# Dependencies
find_package(Boost REQUIRED COMPONENTS program_options system filesystem)
find_package(OpenCASCADE CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# Packages
add_subdirectory(libs/occutils)
add_subdirectory(libs/ocx)
add_subdirectory(libs/shipxml)

# Application
add_subdirectory(ocxreader)
