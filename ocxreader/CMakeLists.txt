cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ocxreader VERSION ${OCXREADER_VERSION} LANGUAGES CXX)

# Where ocxreader's .h files can be found
set(ocxreader_include_dirs
    "${OpenCASCADE_INCLUDE_DIR}"
    "${ocx_SOURCE_DIR}/include"
    "${shipxml_SOURCE_DIR}/include"
    "${ocxreader_SOURCE_DIR}/include"
    "${ocxreader_SOURCE_DIR}")
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
install(FILES ${CMAKE_SOURCE_DIR}/README.md ${CMAKE_SOURCE_DIR}/LICENSE
        DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES ${CMAKE_BINARY_DIR}/LICENSE-3RD-PARTY.txt
        DESTINATION ${CMAKE_INSTALL_DOCDIR})

if (WIN32)
  install(FILES $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
          DESTINATION ${CMAKE_INSTALL_BINDIR})
else ()
  # Transfer the value of ${PROJECT_NAME} into the install script
  install(CODE "set(PROJECT_NAME \"${PROJECT_NAME}\")")

  install(CODE [[
  function(install_with_deps LIBRARY)
    file(INSTALL
      DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
      TYPE SHARED_LIBRARY
      FOLLOW_SYMLINK_CHAIN
      FILES "${LIBRARY}"
    )
    file(GET_RUNTIME_DEPENDENCIES
      LIBRARIES ${LIBRARY}
      RESOLVED_DEPENDENCIES_VAR RESOLVED_DEPS
      UNRESOLVED_DEPENDENCIES_VAR UNRESOLVED_DEPS
    )
    foreach(FILE ${RESOLVED_DEPS})
      if(NOT IS_SYMLINK ${FILE})
        install_with_deps(${FILE})
      endif()
    endforeach()
    foreach(FILE ${UNRESOLVED_DEPS})
      message(STATUS "Unresolved from ${LIBRARY}: ${FILE}")
    endforeach()
  endfunction()
  file(GET_RUNTIME_DEPENDENCIES
    EXECUTABLES $<TARGET_FILE:${PROJECT_NAME}>
    RESOLVED_DEPENDENCIES_VAR RESOLVED_DEPS
    UNRESOLVED_DEPENDENCIES_VAR UNRESOLVED_DEPS
  )
  foreach(FILE ${RESOLVED_DEPS})
    install_with_deps(${FILE})
  endforeach()
  foreach(FILE ${UNRESOLVED_DEPS})
    message(STATUS "Unresolved: ${FILE}")
  endforeach()
]])
endif ()
