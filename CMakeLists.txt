cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ocxreader-distribution)
set(OCXREADER_VERSION 0.1.0)

enable_testing()

include(GNUInstallDirs)

# Application
add_subdirectory(ocxreader)

# Packages
add_subdirectory(libs/ocx)
add_subdirectory(libs/shipxml)
add_subdirectory(libs/occutils)
