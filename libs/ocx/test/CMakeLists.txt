cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ocx-test VERSION ${OCXREADER_VERSION} LANGUAGES CXX)

# Define helper functions and macros used by ocx-test
include(cmake/internal_utils.cmake)

add_executable(${PROJECT_NAME} ocx-all-test.cc)

target_link_libraries(${PROJECT_NAME}
                      ocx
                      GTest::gtest
                      GTest::gtest_main)

target_link_occt_libraries(${PROJECT_NAME})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
