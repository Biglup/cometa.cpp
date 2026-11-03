CMAKE_MINIMUM_REQUIRED (VERSION 3.22)
PROJECT (cometa-consumer LANGUAGES CXX)

FIND_PACKAGE (Cometa CONFIG REQUIRED)

ADD_EXECUTABLE (consumer PackageTest.cpp)
TARGET_LINK_LIBRARIES (consumer PRIVATE Cometa::CometaStatic)

IF (TARGET Cometa::HttpStatic)
  TARGET_LINK_LIBRARIES (consumer PRIVATE Cometa::HttpStatic)
  TARGET_COMPILE_DEFINITIONS (consumer PRIVATE COMETA_TEST_HTTP_TRANSPORT)
ENDIF ()

IF (TARGET Cometa::cometa_shared)
  ADD_EXECUTABLE (consumer-shared PackageTest.cpp)
  TARGET_LINK_LIBRARIES (consumer-shared PRIVATE Cometa::Cometa)

  IF (TARGET Cometa::Http)
    TARGET_LINK_LIBRARIES (consumer-shared PRIVATE Cometa::Http)
    TARGET_COMPILE_DEFINITIONS (consumer-shared PRIVATE COMETA_TEST_HTTP_TRANSPORT)
  ENDIF ()

  IF (WIN32)
    ADD_CUSTOM_COMMAND (TARGET consumer-shared POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:consumer-shared>
        $<TARGET_FILE_DIR:consumer-shared>
      COMMAND_EXPAND_LISTS
    )
  ENDIF ()
ENDIF ()

ENABLE_TESTING ()
ADD_TEST (NAME installed-consumer COMMAND consumer)

IF (TARGET consumer-shared)
  ADD_TEST (NAME installed-consumer-shared COMMAND consumer-shared)
ENDIF ()
