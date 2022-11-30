cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ocxreader-distribution)
set(OCXREADER_VERSION 0.1.0)

enable_testing()

include(GNUInstallDirs)

# Set OCCT cmake directory on windows
if (WIN32)
  set(OpenCASCADE_DIR "C:/Program Files/OCCT/cmake")
endif ()

find_package(OpenCASCADE REQUIRED)

add_subdirectory(ocx)
add_subdirectory(ocxreader)
add_subdirectory(shipxml)
