cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ocx VERSION ${OCXREADER_VERSION} LANGUAGES CXX)

# Spdlog
include(FetchContent)
FetchContent_Declare(
  spdlog # Specify the commit you depend on and update it regularly.
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.x)
FetchContent_MakeAvailable(spdlog)

# Where ocx's .h files can be found
set(ocx_include_dirs
    "${OpenCASCADE_INCLUDE_DIR}"
    "${ocx_SOURCE_DIR}/include"
    "${ocx_SOURCE_DIR}")
include_directories(SYSTEM ${ocx_include_dirs})

# Set compile definitions
add_compile_definitions(
  $<$<CONFIG:DEBUG>:OCX_CONFIG_DEBUG>
  $<$<CONFIG:RELEASE>:OCX_CONFIG_RELEASE>
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(${PROJECT_NAME} STATIC src/ocx-all.cc)

add_subdirectory(test)

# Set target link libraries
foreach (LIB ${OpenCASCADE_LIBRARIES})
  if (WIN32)
    if (EXISTS ${OpenCASCADE_LIBRARY_DIR}${DEBUG_SUFFIX}/${LIB}.lib)
      if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(DEBUG_SUFFIX "d")
      endif ()
      set(occt_lib_dir ${OpenCASCADE_LIBRARY_DIR}${DEBUG_SUFFIX}/${LIB}.lib)
      message(STATUS "Adding ${occt_lib_dir}")
      target_link_libraries(${PROJECT_NAME} debug ${occt_lib_dir} spdlog::spdlog)
      target_link_libraries(${PROJECT_NAME} optimized ${occt_lib_dir} spdlog::spdlog)
    else ()
      message(FATAL_ERROR "Library ${LIB} not found in ${OpenCASCADE_LIBRARY_DIR}${DEBUG_SUFFIX}")
    endif ()
  elseif (APPLE)
    if (EXISTS ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.dylib)
      set(occt_lib_dir ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.dylib)
      message(STATUS "Adding ${occt_lib_dir}")
      target_link_libraries(${PROJECT_NAME} debug ${occt_lib_dir})
      target_link_libraries(${PROJECT_NAME} optimized ${occt_lib_dir})
    else ()
      message(FATAL_ERROR "Library lib${LIB} not found in ${OpenCASCADE_LIBRARY_DIR}")
    endif ()
  elseif (UNIX)
    if (EXISTS ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
      set(occt_lib_dir ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
      message(STATUS "Adding ${occt_lib_dir}")
      target_link_libraries(${PROJECT_NAME} debug ${occt_lib_dir})
      target_link_libraries(${PROJECT_NAME} optimized ${occt_lib_dir})
    else ()
      message(FATAL_ERROR "Library lib${LIB} not found in ${OpenCASCADE_LIBRARY_DIR}")
    endif ()
  endif ()
endforeach ()

# Adjust runtime environment for Visual Studio
set_property(
  TARGET ${PROJECT_NAME}
  PROPERTY
  VS_DEBUGGER_ENVIRONMENT
  "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%"
)
