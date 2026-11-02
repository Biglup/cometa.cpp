# Pin the native API used by the binding coverage inventory.
SET (COMETA_CARDANO_REVISION 5fe0d3acda2d7f14c080729e441e71b382f61850)

INCLUDE (FetchContent)
INCLUDE (ExternalProject)

# Resolve the native source checkout.
IF (COMETA_CARDANO_SOURCE_DIR)
  GET_FILENAME_COMPONENT (cardano_source "${COMETA_CARDANO_SOURCE_DIR}" ABSOLUTE)
ELSE ()
  FETCHCONTENT_DECLARE (cardano_source
    GIT_REPOSITORY https://github.com/Biglup/cardano-c.git
    GIT_TAG        ${COMETA_CARDANO_REVISION}
  )

  FETCHCONTENT_GETPROPERTIES (cardano_source)

  IF (NOT cardano_source_POPULATED)
    FETCHCONTENT_POPULATE (cardano_source)
  ENDIF ()

  SET (cardano_source "${cardano_source_SOURCE_DIR}")
ENDIF ()

SET (cardano_binary "${CMAKE_CURRENT_BINARY_DIR}/cardano-build")

# Native Debug builds enable coverage unconditionally. Retain symbols without
# imposing that coverage runtime on applications using the static archive.
SET (cardano_mode release)

IF (MSVC)
  SET (cardano_archive "${cardano_binary}/build/${cardano_mode}/lib/$<CONFIG>/cardano-c-static.lib")
ELSE ()
  SET (cardano_archive "${cardano_binary}/build/${cardano_mode}/lib/libcardano-c.a")
ENDIF ()

# Build only the native static target with the consumer's toolchain.
SET (cardano_args
  -DCMAKE_BUILD_TYPE=RelWithDebInfo
  -DBUILD_SHARED_LIBS=ON
  -DBUILD_STATIC_LIBS=ON
  -DTESTING_ENABLED=OFF
  -DDOXYGEN_ENABLED=OFF
  -DEXAMPLES_ENABLED=OFF
  -DCARDANO_DISABLE_IPO=ON
  -DCLANG_TIDY_EXE=
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
)

FOREACH (setting
  CMAKE_TOOLCHAIN_FILE
  CMAKE_OSX_ARCHITECTURES
  CMAKE_OSX_DEPLOYMENT_TARGET
  CMAKE_MSVC_RUNTIME_LIBRARY
)
  IF (DEFINED ${setting})
    LIST (APPEND cardano_args "-D${setting}=${${setting}}")
  ENDIF ()
ENDFOREACH ()

EXTERNALPROJECT_ADD (cardano-native
  SOURCE_DIR       "${cardano_source}"
  BINARY_DIR       "${cardano_binary}"
  CMAKE_ARGS       ${cardano_args}
  BUILD_COMMAND    ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG> --target cardano-c-static --parallel 2
  BUILD_ALWAYS     TRUE
  BUILD_BYPRODUCTS "${cardano_archive}"
  INSTALL_COMMAND ""
)

# Expose the archive and platform linker requirements to the C++ target.
ADD_LIBRARY (cometa_cardano INTERFACE)
ADD_DEPENDENCIES (cometa_cardano cardano-native)
TARGET_LINK_LIBRARIES (cometa_cardano INTERFACE "${cardano_archive}")

IF (UNIX)
  TARGET_LINK_LIBRARIES (cometa_cardano INTERFACE m)
ENDIF ()
