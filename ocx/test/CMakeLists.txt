cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ocx-test VERSION ${OCXREADER_VERSION} LANGUAGES CXX)

# External dependencies
include(FetchContent)

# Google Test
FetchContent_Declare(
  googletest # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/0e6aac2571eb1753b8855d8d1f592df64d1a4828.zip)
# For Windows: Prevent overriding the parent project's compiler/linker settings
if (WIN32)
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
endif ()
FetchContent_MakeAvailable(googletest)

# Define helper functions and macros used by ocx-test
include(cmake/internal_utils.cmake)

add_executable(${PROJECT_NAME} ocx-all-test.cc)

target_link_libraries(${PROJECT_NAME} ocx gtest_main)

target_link_occt_libraries(${PROJECT_NAME})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
