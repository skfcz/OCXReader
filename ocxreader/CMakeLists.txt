cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(OCXReader VERSION ${OCXREADER_VERSION} LANGUAGES CXX)

# Where ocxreader's .h files can be found
set(ocxreader_include_dirs
    "${OpenCASCADE_INCLUDE_DIR}"
    "${ocx_SOURCE_DIR}/include"
    "${shipxml_SOURCE_DIR}/include"
    "${OCXReader_SOURCE_DIR}/include"
    "${OCXReader_SOURCE_DIR}")
include_directories(SYSTEM ${ocxreader_include_dirs})

# Define helper functions and macros used by ocxreader
include(cmake/internal_utils.cmake)

# Set compile definitions
add_compile_definitions(
  $<$<CONFIG:DEBUG>:OCXREADER_CONFIG_DEBUG>
  $<$<CONFIG:RELEASE>:OCXREADER_CONFIG_RELEASE>)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS")
  set(CMAKE_CXX_EXTENSIONS ON)
endif ()

# Set libs to link against
list(APPEND ocxreader_libs
     ocxreader::ocx
     ocxreader::shipxml
     spdlog::spdlog
     Boost::boost
     Boost::program_options
     ${OpenCASCADE_LIBRARIES})

# Add executable
cxx_executable(${PROJECT_NAME} src "${ocxreader_libs}")

# Install
install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_SOURCE_DIR}/README.md ${CMAKE_SOURCE_DIR}/LICENSE
        DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES ${CMAKE_BINARY_DIR}/LICENSE-3RD-PARTY.txt
        DESTINATION ${CMAKE_INSTALL_DOCDIR})
