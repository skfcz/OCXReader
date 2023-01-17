cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ocx-test VERSION ${OCXREADER_VERSION} LANGUAGES CXX)

# Define helper functions and macros used by ocx-test
include(cmake/internal_utils.cmake)

# Set libs to link against
list(APPEND ocx_test_libs
     ocxreader::ocx
     GTest::gtest
     GTest::gtest_main
     ${OpenCASCADE_LIBRARIES})

# Add executable
cxx_executable("ocx-all-test" src "${ocx_test_libs}")

add_test(NAME ${PROJECT_NAME} COMMAND "ocx-all-test")
