include("${CMAKE_CURRENT_LIST_DIR}/Version.cmake")
if(NOT "$ENV{RELEASE_TAG}" STREQUAL "v${COMETA_FULL_VERSION}")
  message(FATAL_ERROR "Release tag must match VERSION: v${COMETA_FULL_VERSION}")
endif()
