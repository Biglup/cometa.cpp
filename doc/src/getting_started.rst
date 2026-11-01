Getting started
===============

Building
--------

Use CMake 3.22 or newer and a C++17 compiler. Unit tests require GoogleTest.
The build downloads a pinned cardano-c commit containing the deep-clone API.
To use a local checkout, set ``COMETA_CARDANO_SOURCE_DIR`` instead.

.. code-block:: console

   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build --parallel 2
   ctest --test-dir build --output-on-failure
   cmake --install build --prefix /path/to/cometa

Pass ``-DCOMETA_BUILD_TESTS=OFF`` if GoogleTest is not installed. Tests
are excluded from the installed package.

Consuming an installed package
------------------------------

Set ``CMAKE_PREFIX_PATH`` to the extracted ZIP or installation prefix. Use the
CMake target to link both static archives and the platform libraries correctly.
The package does not require a cardano-c shared library at runtime.

.. code-block:: cmake

   FIND_PACKAGE (Cometa CONFIG REQUIRED)
   TARGET_LINK_LIBRARIES (my_application PRIVATE Cometa::cometa)

Use an archive built for your compiler, standard library, architecture, and runtime.
MSVC packages use the default dynamic C/C++ runtime; native cardano-c code is static.
A ZIP is not a universal binary compatible with every C++ ABI.

Using values
------------

.. code-block:: cpp

   #include <Cometa/Cometa.h>

   using namespace Cometa;

   Blake2bHash hash = Blake2bHash::fromHex(std::string(56, '0'));
   Credential original(CredentialType::KeyHash, hash);

   Credential scriptCredential = original;

   scriptCredential.setType(CredentialType::ScriptHash);

   // original is still a key credential.
   std::vector<std::uint8_t> encoded = scriptCredential.toCbor();
   Credential decoded = Credential::fromCbor(encoded);

   AssetName asset = AssetName::fromString("Cometa");

CBOR accepts byte vectors or hexadecimal strings through ``fromCbor``.
For example, ``AssetName::fromCbor("46436f6d657461")`` decodes the name Cometa.
Hexadecimal strings contain bytes, without a ``0x``
prefix. Asset names can contain arbitrary bytes, including embedded NULs; use
``toBytes`` for binary data and ``fromString`` for UTF-8 input.

Errors
------

Operation failures throw ``Cometa::Error``, derived from ``std::system_error``.
Use ``ErrorCode`` to identify the failure:

.. code-block:: cpp

   try
   {
     AssetName asset = AssetName::fromCbor(bytes);
   }
   catch (const Error& error)
   {
     if (error.code() == makeErrorCode(ErrorCode::Decoding))
     {
       // Handle invalid CBOR.
     }
   }

Invalid arguments throw ``std::invalid_argument``. Allocation failures reported
as such throw ``std::bad_alloc``. CBOR parsing rejects trailing bytes, and
``Blake2bHash::compute`` requires nonempty input.

Copy assignment leaves the destination unchanged when copying fails. Destruction,
moves, and swaps do not throw.

Testing and documentation
-------------------------

.. code-block:: console

   valgrind --leak-check=full --errors-for-leak-kinds=all --error-exitcode=1 build/cometa-tests
   python3 -m pip install -r doc/requirements.txt
   cmake -S doc -B build/docs
   cmake --build build/docs --target doc

Documentation uses Doxygen XML, Breathe, Sphinx, and the Immaterial theme, matching
the cardano-c documentation pipeline. Read the Docs builds use the same configuration.
