# Pin the reviewed deep-clone implementation until it is available in a release.
set(COMETA_CARDANO_REVISION 44fcd57607f55d523764ee5fe09ad691feec774f)
include(FetchContent)
include(ExternalProject)
if(COMETA_CARDANO_SOURCE_DIR)
  get_filename_component(cardano_source "${COMETA_CARDANO_SOURCE_DIR}" ABSOLUTE)
else()
  FetchContent_Declare(cardano_source GIT_REPOSITORY https://github.com/Biglup/cardano-c.git GIT_TAG ${COMETA_CARDANO_REVISION})
  FetchContent_GetProperties(cardano_source)
  if(NOT cardano_source_POPULATED)
    FetchContent_Populate(cardano_source)
  endif()
  set(cardano_source "${cardano_source_SOURCE_DIR}")
endif()
set(cardano_binary "${CMAKE_CURRENT_BINARY_DIR}/cardano-build")
# Native Debug builds enable coverage unconditionally. Retain symbols without
# imposing that coverage runtime on applications using the static archive.
set(cardano_mode release)
if(MSVC)
  set(cardano_archive "${cardano_binary}/build/${cardano_mode}/lib/$<CONFIG>/cardano-c-static.lib")
else()
  set(cardano_archive "${cardano_binary}/build/${cardano_mode}/lib/libcardano-c.a")
endif()
set(cardano_args -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=ON
  -DTESTING_ENABLED=OFF -DDOXYGEN_ENABLED=OFF -DEXAMPLES_ENABLED=OFF -DCARDANO_DISABLE_IPO=ON
  -DCLANG_TIDY_EXE= -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})
foreach(setting CMAKE_TOOLCHAIN_FILE CMAKE_OSX_ARCHITECTURES CMAKE_OSX_DEPLOYMENT_TARGET CMAKE_MSVC_RUNTIME_LIBRARY)
  if(DEFINED ${setting})
    list(APPEND cardano_args "-D${setting}=${${setting}}")
  endif()
endforeach()
ExternalProject_Add(cardano-native SOURCE_DIR "${cardano_source}" BINARY_DIR "${cardano_binary}"
  CMAKE_ARGS ${cardano_args}
  BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG> --target cardano-c-static --parallel 2
  BUILD_ALWAYS TRUE
  BUILD_BYPRODUCTS "${cardano_archive}" INSTALL_COMMAND "")
add_library(cometa_cardano INTERFACE)
add_dependencies(cometa_cardano cardano-native)
target_link_libraries(cometa_cardano INTERFACE "${cardano_archive}")
if(UNIX)
  target_link_libraries(cometa_cardano INTERFACE m)
endif()
