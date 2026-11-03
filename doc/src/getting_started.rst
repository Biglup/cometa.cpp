Getting Started
===============

Install Cometa.cpp, link it to your application, and start building transactions.

Building and installing from source
-----------------------------------

Use CMake 3.22 or newer and a C++17 compiler. Unit tests require GoogleTest.
The build downloads the required cardano-c source revision.
To use a local checkout, set ``COMETA_CARDANO_SOURCE_DIR`` instead.

.. code-block:: console

   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build --parallel 2
   ctest --test-dir build --output-on-failure
   cmake --install build --prefix /path/to/cometa

Pass ``-DCOMETA_BUILD_TESTS=OFF`` if GoogleTest is not installed. Tests
are excluded from the installed package. Pass ``-DCOMETA_BUILD_SHARED=OFF``
for a static-only build.

Configuration options
~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 35 15 50

   * - Option
     - Default
     - Purpose
   * - ``COMETA_BUILD_TESTS``
     - ``ON``
     - Build the unit tests; requires GoogleTest.
   * - ``COMETA_BUILD_EXAMPLES``
     - ``ON``
     - Build the example programs.
   * - ``COMETA_BUILD_SHARED``
     - ``ON``
     - Build shared libraries in addition to static libraries.
   * - ``COMETA_BUILD_DOCS``
     - ``OFF``
     - Enable the documentation targets.
   * - ``COMETA_BUILD_CURL_TRANSPORT``
     - ``ON``
     - Build the libcurl transport when libcurl is available.
   * - ``COMETA_BUILD_WINHTTP_TRANSPORT``
     - ``ON``
     - Build the WinHTTP transport on Windows.
   * - ``COMETA_CARDANO_SOURCE_DIR``
     - Unset
     - Use a local cardano-c source checkout.
   * - ``CMAKE_INSTALL_PREFIX``
     - Platform default
     - Choose the installation directory.

Consuming an installed package
------------------------------

Set ``CMAKE_PREFIX_PATH`` to the extracted ZIP or installation prefix.
``Cometa::Cometa`` links the shared library when available; use
``Cometa::CometaStatic`` to link the static archives. The shared library
includes its own cardano-c code.

.. code-block:: cmake

   FIND_PACKAGE (Cometa CONFIG REQUIRED)
   TARGET_LINK_LIBRARIES (my_application PRIVATE Cometa::Cometa)

For the default HTTPS transport, request the HTTP component and link
``Cometa::Http``. Use ``Cometa::HttpStatic`` for static linking.

.. code-block:: cmake

   FIND_PACKAGE (Cometa CONFIG REQUIRED COMPONENTS http)
   TARGET_LINK_LIBRARIES (my_application PRIVATE Cometa::Http)

The HTTP component uses WinHTTP on Windows and libcurl on Linux and macOS.
Unix builds need a libcurl development package. Core targets do not require
an HTTP dependency. On Windows, place the Cometa DLLs beside the executable.
On Linux and macOS, set the installed executable's runtime search path to the
directory containing the shared libraries. CMake's
``$<TARGET_RUNTIME_DLLS:...>`` can copy the Windows DLLs after a build.

Use an archive built for your compiler, standard library, architecture, and runtime.
MSVC packages use the default dynamic C/C++ runtime; native cardano-c code is static.
A ZIP is not a universal binary compatible with every C++ ABI.

Next steps
----------

* :doc:`sections/api/transaction_builder/usage` — build, sign, and submit transactions.
* :doc:`sections/api/providers/usage` — connect to Blockfrost.
* :doc:`design` — C++ values, ownership, and extension points.
* :doc:`error_handling` — handle operation errors.
