# Install 3rd party copyright notices
function (build_3rd_party_copyright)
  set(LICENSE_3RD_PARTY_FILE ${CMAKE_CURRENT_BINARY_DIR}/LICENSE-3RD-PARTY.txt)
  file(REMOVE ${LICENSE_3RD_PARTY_FILE}) # Delete the old file

  file(GLOB SEARCH_RESULT
       "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share/**/copyright"
       "${CMAKE_SOURCE_DIR}/deps/**/LICENSE")
  set(COPYRIGHT_FILES ${SEARCH_RESULT} CACHE INTERNAL "copyright files")

  # Handle duplicate copyright files
  remove_duplicate_by_file_content("${COPYRIGHT_FILES}" "COPYRIGHT_FILES" "Boost Software License")

  foreach (copyright_file ${COPYRIGHT_FILES})
    string(REGEX MATCH ".*/(.*)/copyright" _ ${copyright_file})
    if ("${CMAKE_MATCH_1}" STREQUAL "")
      string(REGEX MATCH ".*/(.*)/LICENSE" _ ${copyright_file})
    endif ()
    set(LIBRARY_NAME ${CMAKE_MATCH_1})

    file(APPEND ${LICENSE_3RD_PARTY_FILE} "-------------------------------------------------\n")
    file(APPEND ${LICENSE_3RD_PARTY_FILE} "${LIBRARY_NAME}\n")
    file(APPEND ${LICENSE_3RD_PARTY_FILE} "-------------------------------------------------\n")
    file(READ ${copyright_file} COPYRIGHT_CONTENTS)
    file(APPEND ${LICENSE_3RD_PARTY_FILE} "${COPYRIGHT_CONTENTS}\n")
  endforeach ()
endfunction ()

# Remove duplicate from list by content
function (remove_duplicate_by_file_content list list_name filter_by)
  set(INCLUDED OFF)
  foreach (file ${list})
    file(STRINGS ${file} content)
    string(FIND "${content}" ${filter_by} FOUND)
    if (NOT ${FOUND} EQUAL -1)
      if (INCLUDED)
        list(REMOVE_ITEM list ${file})
      else ()
        set(INCLUDED ON)
      endif ()
    endif ()
  endforeach ()
  set(${list_name} ${list} PARENT_SCOPE)
endfunction ()
