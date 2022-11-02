cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(OCXReaderTest CXX)

set(SOURCES OCXReaderTest.cpp OCXHelperTest.cpp)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/3026483ae575e2de942db5e760cf95e973308dd5.zip)
# For Windows: Prevent overriding the parent project's compiler/linker settings
if (WIN32)
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
endif ()
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} OCXReader gtest_main)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
