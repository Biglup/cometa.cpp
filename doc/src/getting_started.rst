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

Reading and writing CBOR
------------------------

Readers copy their input. A copied reader continues from the same position
independently; a copied writer has its own encoded bytes.

.. code-block:: cpp

   Cometa::CborWriter writer;
   writer.writeStartArray(2);
   writer.writeUInt(42);
   writer.writeText("Cometa");

   Cometa::CborReader reader(writer.toBytes());
   const std::optional<std::uint64_t> count = reader.readStartArray();
   const std::uint64_t number = reader.readUInt();
   const std::string label = reader.readText();
   reader.readEndArray();

Pass ``std::nullopt`` to write an indefinite array or map and finish it with
``writeEndArray`` or ``writeEndMap``. Definite containers have no break marker;
do not call those writer methods for a definite container. Readers finish both
forms with ``readEndArray`` or ``readEndMap`` after consuming their elements.

``writeBigInt`` writes CBOR tags 2 or 3 even for small integers. Use ``writeInt``
or ``writeUInt`` for ordinary integer encodings. ``readNegativeInt`` returns
the unsigned argument ``n`` representing ``-1-n``, covering the full CBOR
negative integer range. ``readInt`` throws ``std::out_of_range`` after consuming
an integer outside the signed 64-bit range. Semantic tags above ``INT32_MAX``
are rejected. ``writeEncoded`` appends raw bytes without validating them.

Reading and writing JSON
------------------------

``JsonValue`` represents any JSON value. Child access returns independent values
that remain usable after their parent is replaced or destroyed.

.. code-block:: cpp

   const Cometa::JsonValue value = Cometa::JsonValue::parse(
     R"({"name":"Cometa","amount":18446744073709551616})");
   const std::string name = value.at("name").asString();
   const std::string amount = value.at("amount").toJson();

   Cometa::JsonWriter writer;
   writer.writeStartObject();
   writer.writePropertyName("name");
   writer.writeString(name);
   writer.writePropertyName("amount");
   writer.writeValue(value.at("amount"));
   writer.writeEndObject();
   const std::string json = writer.toString();

Parsed number tokens retain their decimal spelling, including precision and
exponents. Numeric conversions report overflow or invalid conversions through
``Cometa::Error``. ``writeBigInt`` emits a number; ``writeBigIntAsString`` emits
a quoted decimal string. ``writeRaw`` validates one complete JSON value before
appending it. ``toString`` requires a complete document; ``reset`` clears the
document and any write error while preserving the output format.

Signing and deriving keys
-------------------------

Use ``Ed25519PrivateKey`` for a 32-byte seed or a 64-byte extended signing key.
Signatures and public keys are ordinary copyable values.

.. code-block:: cpp

   const Cometa::Ed25519PrivateKey key = Cometa::Ed25519PrivateKey::fromNormalHex(seedHex);
   const Cometa::Ed25519Signature signature = key.sign(message);
   const bool valid = key.publicKey().verify(signature, message);

``Bip32PrivateKey`` supports hardened and non-hardened derivation. Public keys
support only non-hardened derivation. An empty path returns a copy of the key.

.. code-block:: cpp

   const Cometa::Bip32PrivateKey root = Cometa::Bip32PrivateKey::fromBip39Entropy(entropy);
   const Cometa::Bip32PrivateKey account = root.derive({
     Cometa::hardenIndex(1852), Cometa::hardenIndex(1815), Cometa::hardenIndex(0) });
   const Cometa::Ed25519PrivateKey paymentKey = account.derive({ 0, 0 }).toEd25519();
   const Cometa::Blake2bHash credentialHash = paymentKey.publicKey().hash();

``Bip32PublicKey::hash`` hashes the extended key including its chain code.
For a credential hash, convert to an Ed25519 public key before hashing.
The entropy password is a byte string; normalize user-entered text before passing
it if the application requires Unicode normalization.

Private-key objects erase their owned key material when destroyed. Exported
``std::string`` and ``std::vector`` values are ordinary containers and do not erase
their buffers automatically. Avoid exporting or logging secret material unless
it is needed.

Mnemonic and password utilities
-------------------------------

The BIP39 helpers use the English word list and validate the checksum when
converting words back to entropy.

.. code-block:: cpp

   const std::vector<std::string> words = Cometa::entropyToMnemonic(entropy);
   const std::vector<std::uint8_t> recovered = Cometa::mnemonicToEntropy(words);
   const Cometa::Bip32PrivateKey root = Cometa::Bip32PrivateKey::fromBip39Entropy(recovered);

EMIP-003 encryption includes a random salt, nonce, and authentication tag in its
output. Decryption reports an error for a wrong password or altered ciphertext.

.. code-block:: cpp

   const std::vector<std::uint8_t> encrypted = Cometa::encryptEmip3(payload, password);
   const std::vector<std::uint8_t> decrypted = Cometa::decryptEmip3(encrypted, password);

``pbkdf2HmacSha512`` exposes explicit salt, iteration count, and output size.
``crc32`` computes a checksum. Returned secret bytes and mnemonic words use
ordinary standard containers; their memory is not automatically erased.

Constructing and inspecting addresses
-------------------------------------

Build an address from credentials or parse its bytes or text. Credentials passed
to a constructor and returned by getters are independent values.

.. code-block:: cpp

   const Cometa::BaseAddress paymentAddress(
     Cometa::NetworkId::Testnet, paymentCredential, stakeCredential);
   const std::string text = paymentAddress.toString();
   const Cometa::Address parsed = Cometa::Address::fromString(text);
   const std::optional<Cometa::BaseAddress> base = parsed.asBase();

``Address`` accepts Bech32 Shelley addresses and Base58 Byron addresses.
``asBase``, ``asEnterprise``, ``asPointer``, ``asReward``, and ``asByron`` return
``std::nullopt`` for a different address format; allocation failures still throw.
The specialized ``fromAddress`` factories report a wrong format as ``Cometa::Error``.

Shelley byte input must be canonical and contain exactly one address. Text input
checks the network/type prefix and rejects mixed case. Uppercase Bech32 input is
accepted and formatted as lowercase. Network IDs preserve their four-bit value.

``PointerAddress`` currently accepts slot, transaction index, and certificate index
values up to ``UINT32_MAX``. Larger values are rejected rather than truncated.
Byron attributes use an owned encrypted derivation path of at most 64 bytes and
an optional nonnegative network magic; absence denotes mainnet.
