CMAKE_MINIMUM_REQUIRED (VERSION 3.22)
PROJECT (cometa-docs LANGUAGES NONE)

# Documentation toolchain.
FIND_PACKAGE (Doxygen REQUIRED)
FIND_PROGRAM (SPHINX_BUILD sphinx-build REQUIRED)
GET_FILENAME_COMPONENT (COMETA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
INCLUDE ("${COMETA_ROOT}/cmake/Version.cmake")

# Generate build-local configuration and copy the documentation sources.
CONFIGURE_FILE (Doxyfile.in Doxyfile @ONLY)
CONFIGURE_FILE (Doxyfile.check.in Doxyfile.check @ONLY)
CONFIGURE_FILE (conf.py.in conf.py @ONLY)
FILE (COPY src/ DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
FILE (COPY coverage/ DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/coverage")

ADD_CUSTOM_TARGET (check_docs
  COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.check"
)

ADD_CUSTOM_TARGET (doc_doxygen
  COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
)

ADD_CUSTOM_TARGET (doc
  COMMAND ${SPHINX_BUILD} -W --keep-going -b html "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/html"
  DEPENDS check_docs doc_doxygen
)
