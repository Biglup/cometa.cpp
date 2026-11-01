CMAKE_MINIMUM_REQUIRED (VERSION 3.22)

# Project metadata.
INCLUDE (cmake/Version.cmake)
PROJECT (cometa VERSION ${COMETA_VERSION} LANGUAGES C CXX)

INCLUDE (GNUInstallDirs)
INCLUDE (CMakePackageConfigHelpers)

# Build options and native dependency.
OPTION (COMETA_BUILD_TESTS "Build unit tests" ON)
OPTION (COMETA_BUILD_EXAMPLES "Build examples" ON)
OPTION (COMETA_BUILD_DOCS "Build documentation" OFF)
SET (COMETA_CARDANO_SOURCE_DIR "" CACHE PATH "Local cardano-c checkout with deep-clone APIs")

INCLUDE (cmake/Cardano.cmake)

# Library sources.
ADD_LIBRARY (cometa STATIC
  lib/src/ErrorCode.cpp
  lib/src/Error.cpp
  lib/src/Blake2bHash.cpp
  lib/src/AssetName.cpp
  lib/src/Credential.cpp
)

ADD_LIBRARY (Cometa::cometa ALIAS cometa)

TARGET_COMPILE_FEATURES (cometa PUBLIC cxx_std_17)

TARGET_INCLUDE_DIRECTORIES (cometa
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    "${cardano_source}/lib/include"
)

TARGET_LINK_LIBRARIES (cometa
  PRIVATE
    $<BUILD_INTERFACE:cometa_cardano>
    $<INSTALL_INTERFACE:Cometa::cardano>
)

SET_TARGET_PROPERTIES (cometa
  PROPERTIES
    CXX_EXTENSIONS            OFF
    POSITION_INDEPENDENT_CODE ON
)

IF (MSVC)
  TARGET_COMPILE_OPTIONS (cometa PRIVATE /W4 /WX)
ELSE ()
  TARGET_COMPILE_OPTIONS (cometa PRIVATE -Wall -Wextra -Wpedantic -Werror)
ENDIF ()

# Unit tests.
IF (COMETA_BUILD_TESTS)
  ENABLE_TESTING ()
  FIND_PACKAGE (GTest CONFIG REQUIRED)

  ADD_EXECUTABLE (cometa-tests
    lib/tests/ErrorTest.cpp
    lib/tests/AssetNameTest.cpp
    lib/tests/Blake2bHashTest.cpp
    lib/tests/CredentialTest.cpp
    lib/tests/ValueContractTest.cpp
  )

  TARGET_INCLUDE_DIRECTORIES (cometa-tests
    PRIVATE
      "${cardano_source}/lib"
      "${cardano_source}/lib/include"
  )

  TARGET_LINK_LIBRARIES (cometa-tests PRIVATE Cometa::cometa GTest::gtest_main)

  ADD_TEST (NAME cometa-tests COMMAND cometa-tests)
ENDIF ()

# Examples and documentation.
IF (COMETA_BUILD_EXAMPLES)
  ADD_EXECUTABLE (cometa-value-types examples/ValueTypes.cpp)
  TARGET_LINK_LIBRARIES (cometa-value-types PRIVATE Cometa::cometa)
ENDIF ()

IF (COMETA_BUILD_DOCS)
  ADD_SUBDIRECTORY (doc)
ENDIF ()

# Installed headers, archives, and license notices.
INSTALL (
  TARGETS cometa
  EXPORT  CometaTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

INSTALL (
  DIRECTORY   lib/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

INSTALL (
  FILES       "${cardano_archive}"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cometa
  RENAME      "${CMAKE_STATIC_LIBRARY_PREFIX}cardano-c${CMAKE_STATIC_LIBRARY_SUFFIX}"
)

INSTALL (
  FILES       LICENSE README.md
  DESTINATION ${CMAKE_INSTALL_DATADIR}/cometa
)

INSTALL (
  FILES       "${cardano_source}/COPYING"
  DESTINATION ${CMAKE_INSTALL_DATADIR}/cometa
  RENAME      cardano-c-LICENSE
)

INSTALL (
  DIRECTORY   "${cardano_source}/lib/external/"
  DESTINATION ${CMAKE_INSTALL_DATADIR}/cometa/third-party
  FILES_MATCHING
    PATTERN "*LICENSE*"
    PATTERN "*COPYING*"
    PATTERN "*NOTICE*"
    PATTERN "*license*"
    PATTERN "*copyright*"
)

# Relocatable CMake package.
INSTALL (
  EXPORT      CometaTargets
  NAMESPACE   Cometa::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Cometa
)

CONFIGURE_PACKAGE_CONFIG_FILE (
  cmake/CometaConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/CometaConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Cometa
)

WRITE_BASIC_PACKAGE_VERSION_FILE (
  "${CMAKE_CURRENT_BINARY_DIR}/CometaConfigVersion.cmake"
  VERSION       ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

INSTALL (
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CometaConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CometaConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Cometa
)

# Platform ZIP packages.
SET (CPACK_GENERATOR ZIP)
SET (CPACK_PACKAGE_VERSION "${COMETA_FULL_VERSION}")
SET (CPACK_PACKAGE_FILE_NAME
  "cometa-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_CXX_COMPILER_ID}"
)

INCLUDE (CPack)
