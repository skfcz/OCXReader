cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ocxreader-distribution)
set(OCXREADER_VERSION 0.1.0)

enable_testing()

include(GNUInstallDirs)

# Check if vcpkg is being used
set(VCPKG_IS_USED OFF)
if (CMAKE_TOOLCHAIN_FILE MATCHES ".*/vcpkg/scripts/buildsystems/vcpkg.cmake$")
  set(VCPKG_IS_USED ON)
  # Check if should use local install of OpenCascade
  set(USE_LOCAL_OCC OFF CACHE BOOL "Use local OpenCascade install")
  if (CUSTOM_OpenCASCADE_DIR)
    message(STATUS "Use local OpenCascade install: ${CUSTOM_OpenCASCADE_DIR}")
    set(OpenCASCADE_DIR ${CUSTOM_OpenCASCADE_DIR})
    set(USE_LOCAL_OCC ON)
    # Do not copy dependencies to the output directory
    set(VCPKG_APPLOCAL_DEPS OFF)
  endif ()
endif ()

# Dependencies
if (VCPKG_IS_USED)
  if (USE_LOCAL_OCC)
    find_package(OpenCASCADE REQUIRED NO_CMAKE_PACKAGE_REGISTRY)
  else ()
    find_package(OpenCASCADE CONFIG REQUIRED)
    find_package(freetype CONFIG REQUIRED) # locate freetype when using vcpkg
  endif ()
else ()
  find_package(OpenCASCADE REQUIRED)
endif ()

add_subdirectory(ocx)
add_subdirectory(shipxml)
add_subdirectory(ocxreader)
