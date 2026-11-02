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

Identifying assets
------------------

An ``AssetId`` combines a 28-byte policy ID and an asset name of up to 32 bytes.
The byte and hexadecimal representations concatenate those fields.
``AssetId::lovelace()`` represents lovelace with empty bytes and hex; parsing
an empty representation returns that same identifier. A policy with an empty
asset name remains a native token.

.. code-block:: cpp

   const Cometa::AssetId token(policyId, Cometa::AssetName::fromString("Cometa"));
   const Cometa::AssetIdList assets { Cometa::AssetId::lovelace(), token };
   const std::optional<Cometa::Blake2bHash> policy = token.policyId();

``AssetIdList``, ``AssetNameList``, and ``PolicyIdList`` are named ``std::vector``
aliases. Use ordinary indexing, iteration, insertion, and copying. Their elements
are independent values. Policy lists do not validate hash lengths on insertion;
operations accepting policy IDs enforce the 28-byte requirement.

Working with quantities
-----------------------

``AssetNameMap`` groups signed quantities under one policy; ``MultiAsset`` groups
those maps by policy. ``Value`` adds a lovelace amount. ``AssetIdMap`` provides a
flat identifier-to-quantity view. Getters return independent values.

.. code-block:: cpp

   const Cometa::AssetName name = Cometa::AssetName::fromString("Cometa");
   Cometa::MultiAsset tokens;
   tokens.set(policyId, name, 10);

   Cometa::Value payment(2'000'000, tokens);
   payment.addAsset(policyId, name, 5);
   const std::vector<std::uint8_t> cbor = payment.toCbor();
   const Cometa::Value decoded = Cometa::Value::fromCbor(cbor);

``get`` returns ``std::nullopt`` for an absent asset. Map ``at`` throws
``std::out_of_range`` instead. ``set`` retains zero amounts; addition and
subtraction remove zero results. Quantities use ``std::int64_t``. Arithmetic
throws ``std::overflow_error`` if a result cannot be represented, and failed
mutations leave the original value unchanged.

Negative quantities are useful for differences and minting. Negative lovelace
cannot be encoded by ``Value::toCbor``. ``Value::toAssetMap`` retains nonzero
lovelace of either sign so signed values round-trip through ``fromAssetMap``.
CBOR parsing rejects duplicate asset names or policies, out-of-range amounts,
invalid policy lengths, and trailing input.

``keys``, ``policies``, and ``entries`` provide owned snapshots for ordinary C++
iteration. ``toJson`` uses decimal strings for quantities. The asset-name and
policy maps also offer ``toJsonEntries`` when an array representation is needed.

``Value::assetCount`` includes all stored token entries and positive lovelace.
``intersection`` includes shared token identifiers regardless of amount, and
lovelace only when positive in both values. ``isZero`` requires zero lovelace and
no stored policy entries; a retained zero token entry is not an empty value.

Hash and credential sets
------------------------

``Blake2bHashSet`` and ``CredentialSet`` keep elements sorted and unique.
``insert`` returns false for a duplicate; ``at`` and ``values`` return independent
copies, so changing a returned credential does not alter membership.

.. code-block:: cpp

   Cometa::CredentialSet credentials { paymentCredential };
   const bool inserted = credentials.insert(stakeCredential);
   const std::vector<Cometa::Credential> snapshot = credentials.values();
   const std::vector<std::uint8_t> cbor = credentials.toCbor();

Parsing accepts tagged or untagged arrays and rejects duplicate elements.
Encoding always writes a sorted definite-length array with CBOR set tag 258.
An untagged, unsorted, or indefinite-length input therefore need not round-trip
byte for byte. Other tags, including oversized values that could narrow to 258,
are rejected. ``toJson`` writes the elements as a sorted JSON array.

Plutus data and output datums
-----------------------------

Construct Plutus values with integers, byte vectors, lists, maps, or constructor
fields. ``BigInt`` represents integers outside the machine integer range.

.. code-block:: cpp

   Cometa::PlutusList fields;
   fields.pushBack(Cometa::PlutusData(42));
   fields.pushBack(Cometa::PlutusData::fromHex("aabb"));

   const Cometa::ConstrPlutusData constructor(0, fields);
   const Cometa::PlutusData data(constructor);
   const Cometa::Datum datum(data);
   const Cometa::Blake2bHash datumHash = datum.hash();

``PlutusMap`` retains insertion order and duplicate keys. ``insert`` appends a
pair; ``get`` returns the first matching value or ``std::nullopt``. It does not
replace an earlier pair with the same key.

Decoded Plutus values retain their original CBOR bytes. This matters because
datum hashes depend on the encoding, not only on the decoded fields. Copies
retain that encoding. Mutating a list, map, or constructor rebuilds its encoding
from the new fields. ``normalizeEncoding()`` explicitly rebuilds subsequent
encodings, recursively; this can change the datum hash without changing the
logical value.

Child accessors return independent values. To change constructor fields, modify
the returned list and pass it to ``setFields``. Changing a returned list does not
change its parent.

``Datum`` can hold either a 32-byte hash or inline data. ``inlineData()`` returns
``std::nullopt`` for hash datums. ``hash()`` computes the hash of the preserved
inline encoding when data is present. ``setHash()`` only applies to hash datums;
replace an inline datum with a newly constructed ``Datum`` to change its data.

Native script requirements
--------------------------

Use ``ScriptPubkey`` for a key signature, ``ScriptInvalidBefore`` and
``ScriptInvalidAfter`` for slot bounds, and ``ScriptRequireGuard`` for a Dijkstra
guard credential. Key hashes must contain 28 bytes.

.. code-block:: cpp

   const Cometa::Blake2bHash keyHash = publicKey.hash();
   const Cometa::ScriptPubkey signature(keyHash);
   const Cometa::ScriptInvalidBefore startsAt(1000);
   const Cometa::ScriptInvalidAfter endsAt(2000);
   const Cometa::ScriptRequireGuard guard(
     Cometa::Credential(Cometa::CredentialType::KeyHash, keyHash));

``fromJson`` accepts the cardano-cli native-script format: ``sig`` with
``keyHash``, ``after`` with ``slot`` for an invalid-before bound, ``before`` with
``slot`` for an invalid-after bound, and ``guard`` with exactly one of ``keyHash``
or ``scriptHash``. Slot values must be unsigned integers without fractional or
exponent notation. ``toJson`` produces CIP-116 inspection JSON, which uses a
different schema.

Copies and returned credentials are independent. CBOR decoded with ``fromCbor``
retains its original encoding until a setter or ``normalizeEncoding`` is used.

Composite native scripts
------------------------

Combine requirements with ``ScriptAll``, ``ScriptAny``, or ``ScriptNOfK`` and
wrap the result in ``NativeScript``. A threshold may exceed the child count;
that describes an unsatisfiable requirement rather than an invalid value.

.. code-block:: cpp

   const Cometa::NativeScriptList requirements {
     Cometa::NativeScript(Cometa::ScriptPubkey(publicKey.hash())),
     Cometa::NativeScript(Cometa::ScriptInvalidBefore(1000))
   };
   const Cometa::NativeScript policy { Cometa::ScriptAll(requirements) };
   const Cometa::Blake2bHash policyId = policy.hash();

``type()`` identifies the variant. Accessors such as ``asAll()`` return
independent requirements and throw ``Error`` for a different variant. Change a
returned composite with ``setScripts`` and construct a new ``NativeScript`` to
replace the parent value.

Script hashes use the original decoded CBOR, including nonminimal encodings.
``normalizeEncoding()`` recursively rebuilds those encodings and can change the
hash. ``NativeScriptList`` retains its definite or indefinite array form and its
children's encodings, but normalizes nonminimal array-length headers. Appending a
child does not normalize existing children.

Native-script JSON uses the cardano-cli schema: ``all`` and ``any`` objects have
a ``scripts`` array; ``atLeast`` additionally has an unsigned ``required`` count.
``NativeScriptList::fromJson`` accepts an object containing a ``scripts`` array.

Plutus scripts and parameters
-----------------------------

``PlutusV1Script`` through ``PlutusV4Script`` accept compiled script bytes.
``fromHex`` decodes those bytes from hexadecimal; ``fromCbor`` decodes an outer
CBOR bytestring containing them. ``toBytes`` and ``toHex`` return the compiled
bytes, while ``toCbor`` adds that outer bytestring.

.. code-block:: cpp

   const Cometa::PlutusV4Script script =
     Cometa::PlutusV4Script::fromHex("450100002499");
   Cometa::PlutusList parameters;
   parameters.pushBack(Cometa::PlutusData(std::int64_t { 42 }));

   const Cometa::PlutusV4Script applied = script.applyParameters(parameters);
   const Cometa::Blake2bHash scriptHash = applied.hash();

``applyParameters`` requires a CBOR-wrapped Flat program, applies arguments in
list order, and returns a new script. An empty list canonicalizes the program.
Malformed programs throw ``Error``. Applying parameters does not execute the
script or change its ledger language version.

``hash`` includes the language prefix, so identical bytes in different Plutus
versions have different hashes. Decoding an outer CBOR representation normalizes
its bytestring header without changing the compiled bytes or script hash.

Working with either script language
-----------------------------------

``Script`` holds a ``NativeScript`` or any of the four Plutus script types.
Use ``language()`` to identify the variant and a typed accessor to obtain it.
An accessor for a different language throws ``Error``.

.. code-block:: cpp

   const Cometa::Script script(
     Cometa::PlutusV4Script::fromHex("450100002499"));

   if (script.language() == Cometa::ScriptLanguage::PlutusV4)
   {
     const Cometa::PlutusV4Script validator = script.asPlutusV4();
     const std::string bytes = validator.toHex();
   }

Constructors and accessors copy values. Replacing a returned script does not
modify its parent. ``toCbor`` includes both the language discriminator and the
script payload; ``hash`` uses the underlying script's language-specific hash.
Native payloads retain their decoded encoding, including encodings that affect
the hash. To normalize a native payload, normalize the value returned by
``asNative()`` and construct a replacement ``Script`` from it.

Transaction values and withdrawals
----------------------------------

``TransactionInput`` identifies a previous output by its 32-byte transaction ID
and output index. ``TransactionOutput`` contains an address and ``Value``;
``Utxo`` combines an input reference with its output contents.

.. code-block:: cpp

   const Cometa::TransactionInput input = Cometa::TransactionInput::fromHex(
     "0101010101010101010101010101010101010101010101010101010101010101", 5);
   const Cometa::Address address = Cometa::Address::fromString(
     "addr_test1vqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxrcya6");
   Cometa::TransactionOutput output(address, Cometa::Value(2'000'000));
   output.setDatum(Cometa::Datum(Cometa::PlutusData(std::int64_t { 42 })));
   output.setScriptRef(Cometa::Script(
     Cometa::PlutusV4Script::fromHex("450100002499")));

   const Cometa::Utxo utxo(input, output);
   output.setDatum(std::nullopt);

``datum()`` and ``scriptRef()`` return ``std::optional`` values. Set either field
to ``std::nullopt`` to remove it. Constructors, setters, and getters copy values,
so changing ``output`` above does not change ``utxo``. Output lovelace must be
nonnegative; constructing these values does not establish transaction validity.

``TransactionInputSet`` sorts and deduplicates inputs. Its CBOR always includes
set tag 258; ``wasTagged()`` reports the decoded input's tag presence and returns
true for newly constructed sets. ``TransactionOutputList`` retains output order.
Both serialize as definite-length arrays. Legacy output arrays decode normally
and serialize in the current map form.

``WithdrawalMap`` stores exact unsigned 64-bit lovelace amounts. ``set`` replaces
an existing address's amount, including zero; ``get`` returns ``std::nullopt``
when absent, while ``at`` throws ``std::out_of_range``. ``keys``, ``entries``, and
``entryAt`` return independent values in serialization order.

``UtxoList`` and ``RewardAddressList`` are standard ``std::vector`` aliases. Use
normal vector operations and standard algorithms for sorting, finding, filtering,
concatenation, and slicing; their elements retain independent value semantics.

Auxiliary Plutus script lists
--------------------------------

``PlutusV1ScriptList``, ``PlutusV2ScriptList``, ``PlutusV3ScriptList``, and
``PlutusV4ScriptList`` retain ordered script values, including duplicates.
Use ``pushBack`` to append a script, ``at`` to copy one element, and ``values``
to obtain a standard vector of independent script values.

.. code-block:: cpp

   const Cometa::PlutusV4Script script =
     Cometa::PlutusV4Script::fromHex("450100002499");
   Cometa::PlutusV4ScriptList scripts { script };
   scripts.pushBack(script);
   const std::vector<std::uint8_t> cbor = scripts.toCbor();

``fromCbor`` accepts bytes or hexadecimal text and rejects trailing input.
Serialization produces a definite-length array of script byte strings;
``toJson`` produces the CIP-116 inspection array. These collections do not
execute scripts or establish their validity for a transaction.

Metadata values
---------------

``Metadatum`` represents an integer, text, bytes, ``MetadatumList``, or
``MetadatumMap``. Lists retain order; maps retain insertion order and duplicate
keys. ``get`` returns the first matching map value or ``std::nullopt``.
Constructors and getters provide independent values.

.. code-block:: cpp

   Cometa::MetadatumMap fields;
   fields.insert(Cometa::Metadatum("name"), Cometa::Metadatum("Cometa"));
   fields.insert(Cometa::Metadatum("count"), Cometa::Metadatum(std::int64_t { 42 }));
   const Cometa::Metadatum metadata(fields);
   const std::vector<std::uint8_t> cbor = metadata.toCbor();
   const std::string plainJson = metadata.toPlainJson();

``fromPlainJson`` accepts JSON integers, strings, arrays, and objects. Integer
precision is retained; null, booleans, fractions, and exponents are rejected.
``toPlainJson`` requires text map keys and rejects byte-string values. ``toJson``
produces tagged CIP-116 inspection output for every metadata kind.

Text preserves embedded null characters. Text and bytes may be stored at any
length, but CBOR encoding rejects values longer than 64 bytes. Integers use
``BigInt``; CBOR supports bignum tags for larger values. Constructing or decoding
metadata does not establish its validity under a particular ledger's rules.

CBOR serialization normalizes scalar encodings while retaining each list or
map's definite or indefinite length form, including after appending entries.

Transaction metadata and auxiliary data
---------------------------------------

``TransactionMetadata`` associates unsigned 64-bit labels with metadata values.
``set`` replaces an existing label, ``get`` returns an optional independent value,
and ``at`` throws ``std::out_of_range`` for a missing label. Entries serialize in
ascending label order. Decoding rejects duplicate labels. ``MetadatumLabelList``
is a standard vector of unsigned 64-bit labels.

.. code-block:: cpp

   Cometa::TransactionMetadata metadata;
   metadata.set(674, Cometa::Metadatum::fromPlainJson(
     R"({"msg":["Hello from Cometa"]})"));

   Cometa::AuxiliaryData auxiliary;
   auxiliary.setMetadata(metadata);
   auxiliary.setPlutusV4Scripts(Cometa::PlutusV4ScriptList {
     Cometa::PlutusV4Script::fromHex("450100002499") });
   const Cometa::Blake2bHash hash = auxiliary.hash();
   const std::vector<std::uint8_t> cbor = auxiliary.toCbor();

``AuxiliaryData`` can contain metadata, native scripts, and Plutus V1 through V4
script lists. Getters return optional independent values; setters accept
``std::nullopt`` to remove a field. An absent field and a present empty collection
remain distinct in CBOR.

Decoded auxiliary data preserves its original CBOR and hash. Changing a field
or calling ``normalizeEncoding`` selects the current tagged form for subsequent
serialization; nested script encodings remain intact. ``hash`` always hashes the
bytes returned by ``toCbor``. Equality compares field values, so equivalent values
with different original encodings can have different hashes.

Pool primitives
---------------

``Ipv4`` and ``Ipv6`` accept network-order bytes and provide CBOR, text, and byte
conversions. IPv4 text contains four decimal octets. IPv6 text currently requires
eight groups of four hexadecimal digits; compressed forms such as ``::1`` are
not accepted. Address equality compares bytes, regardless of hexadecimal case.

.. code-block:: cpp

   const Cometa::Ipv4 address = Cometa::Ipv4::fromString("192.0.2.1");
   const std::vector<std::uint8_t> bytes = address.toBytes();

   const Cometa::Blake2bHash hash = Cometa::Blake2bHash::fromHex(std::string(64, '1'));
   Cometa::PoolMetadata metadata("https://example.com/pool.json", hash);
   metadata.setUrl("https://example.com/new.json");

``PoolMetadata`` stores a URL of at most 128 bytes and a 32-byte content hash.
An empty URL is allowed; embedded NUL characters are rejected by constructors and
setters. Copies and returned hashes are independent values.

``BlsKey`` stores a 96-byte public key and a 48-byte proof of possession. Construct
it from two byte vectors, or use ``fromHex`` with 192 and 96 hexadecimal characters.
It supports CBOR and CIP-116 JSON. These operations validate the encoding and
lengths; they do not verify the proof cryptographically.

Pool relays
-----------

``SingleHostAddrRelay`` holds optional IPv4/IPv6 addresses and a port.
``SingleHostNameRelay`` holds a DNS name and optional port; ``MultiHostNameRelay``
holds a DNS name for SRV lookup. DNS names contain 1 to 64 bytes without embedded
NULs. These values describe relay configuration; they do not perform DNS lookup.

.. code-block:: cpp

   Cometa::SingleHostAddrRelay ipRelay(3001, Cometa::Ipv4::fromString("192.0.2.1"));
   ipRelay.setIpv6(Cometa::Ipv6::fromString("0000:0000:0000:0000:0000:0000:0000:0001"));
   ipRelay.setIpv4(std::nullopt);

   Cometa::Relays relays {
     Cometa::Relay(ipRelay),
     Cometa::Relay(Cometa::SingleHostNameRelay("relay.example.com", 3001)),
     Cometa::Relay(Cometa::MultiHostNameRelay("_cardano._tcp.example.com")) };

   const std::vector<std::uint8_t> cbor = relays.toCbor();

``Relay`` holds one of the three variants. Inspect ``type`` before calling
``asSingleHostAddr``, ``asSingleHostName``, or ``asMultiHostName``; a mismatched
accessor throws ``Error``. Returned variants and addresses are independent values.

Optional setters accept ``std::nullopt`` to remove a field. Port zero is distinct
from an absent port. ``Relays`` preserves insertion order and duplicates;
``pushBack`` copies its argument, and ``at`` returns an independent relay.

Pool owners
-----------

``PoolOwners`` stores unique 28-byte pool owner key hashes in sorted order.
Inserting an existing hash returns ``false``. The collection copies inserted
values and returns independent values from ``at`` and ``values``.

.. code-block:: cpp

   Cometa::PoolOwners owners;
   owners.insert(Cometa::Blake2bHash::fromHex(std::string(56, '1')));
   const std::vector<std::uint8_t> cbor = owners.toCbor();
   const Cometa::PoolOwners restored = Cometa::PoolOwners::fromCbor(cbor);

The CBOR encoder applies set tag 258. The decoder also accepts an untagged
array, rejects duplicate hashes, and rejects malformed lengths.

Pool parameters
---------------

``PoolParams`` holds the operator and VRF hashes, pledge, cost, margin, reward
address, owner hashes, relay list, and optional metadata or BLS key. It copies
all supplied values, and each component getter returns an independent value.

.. code-block:: cpp

   Cometa::PoolParams params(operatorHash, vrfHash, 10'000'000, 340'000,
                              Cometa::UnitInterval(1, 20), rewardAddress,
                              owners, relays);
   params.setMetadata(Cometa::PoolMetadata(
     "https://example.com/pool.json", metadataHash));
   const std::vector<std::uint8_t> cbor = params.toCbor();
   const Cometa::PoolParams restored = Cometa::PoolParams::fromCbor(cbor);

CBOR contains the pool parameter fields as a sequence, with no outer array.
``setBlsKey(std::nullopt)`` and ``setMetadata(std::nullopt)`` remove optional fields.

Certificate types
-----------------

``CertificateType`` identifies every supported certificate variant. Use
``toString(type)`` when a readable type name is needed. Legacy staking and
MIR variants remain represented so historical transactions can be inspected.

Legacy stake registration
-------------------------

``StakeRegistrationCert`` represents the historical stake registration form.
It copies the supplied credential; ``credential()`` returns an independent
value. For current transactions, choose the deposit-bearing registration
certificate when that ledger form is required.

.. code-block:: cpp

   Cometa::StakeRegistrationCert cert(stakeCredential);
   const std::vector<std::uint8_t> cbor = cert.toCbor();
   const Cometa::StakeRegistrationCert restored =
     Cometa::StakeRegistrationCert::fromCbor(cbor);

Legacy stake deregistration
---------------------------

``StakeDeregistrationCert`` represents the historical stake deregistration
form. It copies its credential and provides CBOR and CIP-116 JSON conversion.

Deposit-bearing registration certificates
-----------------------------------------

``RegistrationCert`` and ``UnregistrationCert`` hold a stake credential and
a deposit amount in lovelace. They copy credentials and return independent
values. The two forms encode with distinct CBOR certificate tags.

.. code-block:: cpp

   Cometa::RegistrationCert registration(stakeCredential, 2'000'000);
   Cometa::UnregistrationCert unregistration(stakeCredential, 2'000'000);

Stake delegation
----------------

``StakeDelegationCert`` associates a stake credential with a 28-byte pool key
hash. It copies both components and provides CBOR and inspection JSON.

.. code-block:: cpp

   Cometa::StakeDelegationCert delegation(stakeCredential, poolKeyHash);
   const Cometa::StakeDelegationCert restored =
     Cometa::StakeDelegationCert::fromCbor(delegation.toCbor());

Pool registration certificate
-----------------------------

``PoolRegistrationCert`` contains a copied ``PoolParams`` value. The ``params``
getter returns an independent value, which can be changed and supplied to
``setParams``.

.. code-block:: cpp

   Cometa::PoolRegistrationCert registration(params);
   const Cometa::PoolRegistrationCert restored =
     Cometa::PoolRegistrationCert::fromCbor(registration.toCbor());

Pool retirement certificate
---------------------------

``PoolRetirementCert`` stores a 28-byte pool key hash and the retirement epoch.
The hash getter returns an independent value.

.. code-block:: cpp

   Cometa::PoolRetirementCert retirement(poolKeyHash, retirementEpoch);

Combined stake registration and delegation
------------------------------------------

``StakeRegistrationDelegationCert`` registers a stake credential and delegates
to a pool in one certificate. It stores a deposit in lovelace and returns
independent credential and pool hash values.

.. code-block:: cpp

   Cometa::StakeRegistrationDelegationCert cert(
     stakeCredential, poolKeyHash, 2'000'000);

Genesis key delegation
----------------------

``GenesisKeyDelegationCert`` represents the historical delegation of a
genesis key to a delegate key and VRF key. It requires 28-byte genesis and
delegate hashes and a 32-byte VRF hash.

Historical MIR pot transfers
----------------------------

``MirToPotCert`` represents an instantaneous reward transfer between the
reserve and treasury pots. ``MirPotType`` selects the source pot; amounts are
expressed in lovelace. This historical certificate remains available for
inspection of older transactions.

``MirToStakeCredentialsCert`` records a historical distribution from either
pot to stake credentials. Its entries contain a credential and an amount in
lovelace.

``MirCert`` holds either historical form. The ``type()`` discriminator
selects ``asToPot()`` or ``asToStakeCredentials()``.

Delegated representatives
-------------------------

``DRep`` represents a key or script credential, abstention, or no confidence.
Key and script representatives also support bech32 identifiers.

``VoteDelegationCert`` assigns a stake credential's voting rights to a DRep,
including the predefined abstain and no-confidence choices.

``StakeVoteDelegationCert`` combines pool delegation and DRep delegation for
one stake credential.

``VoteRegistrationDelegationCert`` registers a stake credential and delegates
its voting rights in one certificate. It records the registration deposit in
lovelace.

``StakeVoteRegistrationDelegationCert`` combines stake registration, pool
delegation, and DRep delegation with one registration deposit.

``UnregisterDRepCert`` unregisters a DRep credential and records the returned
deposit in lovelace.

``RegisterDRepCert`` registers a DRep credential with a deposit and an optional
metadata anchor. The anchor can be replaced or removed.

``UpdateDRepCert`` records a DRep credential and its optional metadata anchor.

``AuthCommitteeHotCert`` authorizes a committee hot credential with a cold
credential.

``ResignCommitteeColdCert`` records a committee cold credential's resignation
and optional anchor.

``Certificate`` holds any ledger certificate and exposes its
``CertificateType`` and independent typed values.

``CertificateSet`` keeps certificates in insertion order and accepts any
``Certificate`` variant. Accessors return independent values::

   Cometa::CertificateSet certificates;
   certificates.add(Cometa::Certificate(registration));
   const Cometa::Certificate first = certificates.at(0);

Protocol parameters
-------------------

``ExUnitPrices`` holds the rational memory and CPU step prices used by Plutus
execution. Both prices can be read and replaced independently::

   Cometa::ExUnitPrices prices(Cometa::UnitInterval(1, 2),
                               Cometa::UnitInterval(1, 3));
   prices.setMemoryPrice(Cometa::UnitInterval(2, 3));

``CostModel`` holds operation costs for a Plutus language. Decoding retains
unknown language IDs so a model can be read and re-encoded without loss::

   Cometa::CostModel model(Cometa::PlutusLanguageVersion::V4, { 1, 2, 3 });
   model.setCost(1, 7);
   const std::vector<std::int64_t> costs = model.costs();

``CostModels`` stores cost models by language ID. The collection retains models
for future language IDs when decoding CBOR::

   Cometa::CostModels models;
   models.insert(model);
   const std::optional<Cometa::CostModel> v4 =
     models.get(Cometa::PlutusLanguageVersion::V4);

``DRepVotingThresholds`` and ``PoolVotingThresholds`` expose each governance
threshold by name. Both accept rational ``UnitInterval`` values and support
independent copies and CBOR serialization::

   Cometa::DRepVotingThresholds drep =
     Cometa::DRepVotingThresholds::fromCbor(drepThresholdCbor);
   drep.setTreasuryWithdrawal(Cometa::UnitInterval(2, 3));

   Cometa::PoolVotingThresholds pool =
     Cometa::PoolVotingThresholds::fromCbor(poolThresholdCbor);
   pool.setSecurityRelevantParam(Cometa::UnitInterval(1, 2));

``ProtocolParamUpdate`` stores proposed changes as optional values. A missing
value is omitted from the update; ``std::nullopt`` removes an existing proposal::

   Cometa::ProtocolParamUpdate update;
   update.setMinFeeA(std::uint64_t(44));
   update.setPoolPledgeInfluence(Cometa::UnitInterval(1, 2));
   update.setMinFeeA(std::nullopt);

Max pledge leverage and the Peras bootstrap round also have an explicit
ledger-null value, separate from absence. Use
``setMaxPledgeLeverageUnbounded()`` or ``setPerasBootstrapRoundNull()`` to
propose that value; use the matching ``clear`` method to omit the field.

``ProposedParamUpdates`` associates proposed changes with genesis delegate key
hashes. Entries are sorted by hash, and accessors return independent values::

   Cometa::ProposedParamUpdates proposals;
   const Cometa::Blake2bHash delegate =
     Cometa::Blake2bHash::fromHex(delegateHashHex);
   proposals.add(delegate, update);
   const std::optional<Cometa::ProtocolParamUpdate> found =
     proposals.get(delegate);

``Update`` combines the proposals with the epoch in which they take effect::

   Cometa::Update epochUpdate(500, proposals);
   const std::vector<std::uint8_t> cbor = epochUpdate.toCbor();
   const Cometa::Update decoded = Cometa::Update::fromCbor(cbor);

``ProtocolParameters`` holds the current network settings. It includes the
Conway, Leios, and Peras fields, with typed values for prices, execution units,
thresholds, and cost models::

   Cometa::ProtocolParameters parameters;
   parameters.setMinFeeA(44);
   parameters.setMaxTxExUnits(Cometa::ExUnits(10'000'000, 10'000'000'000));
   parameters.setPerasBootstrapRound(std::nullopt);

Copies own independent values. A missing max pledge leverage or Peras bootstrap
round is represented by ``std::nullopt``.

``InfoAction`` is an informational governance proposal. Its CBOR is the
ledger action array ``[6]``::

   const Cometa::InfoAction action;
   const std::vector<std::uint8_t> cbor = action.toCbor();

``NoConfidenceAction`` accepts an optional previous action identifier::

   const Cometa::NoConfidenceAction noConfidence(std::nullopt);
   const std::vector<std::uint8_t> cbor = noConfidence.toCbor();

``HardForkInitiationAction`` combines a proposed protocol version with an
optional previous action identifier::

   const Cometa::HardForkInitiationAction hardFork(
     Cometa::ProtocolVersion(12, 0));

``ParameterChangeAction`` proposes protocol parameter changes. The previous
action identifier and policy hash are optional::

   const Cometa::ParameterChangeAction change(update);
   const std::vector<std::uint8_t> cbor = change.toCbor();

``Constitution`` holds its anchor and an optional script hash::

   const Cometa::Constitution constitution(anchor, std::nullopt);

``NewConstitutionAction`` proposes a constitution and may refer to the
previous constitutional action::

   const Cometa::NewConstitutionAction newConstitution(constitution);

``TreasuryWithdrawalsAction`` holds a withdrawal map and optional policy hash::

   const Cometa::TreasuryWithdrawalsAction treasury(withdrawals);

``CommitteeMembersMap`` associates committee credentials with their term
expiration epochs::

   Cometa::CommitteeMembersMap members;
   const Cometa::Credential coldCredential(
     Cometa::CredentialType::KeyHash,
     Cometa::Blake2bHash::fromHex(std::string(56, '0')));
   members.insert(coldCredential, 200);
   const std::optional<std::uint64_t> expiration = members.get(coldCredential);

``Committee`` adds a quorum threshold to the member terms::

   Cometa::Committee committee(Cometa::UnitInterval(1, 2));
   committee.addMember(coldCredential, 200);
   const Cometa::UnitInterval quorum = committee.quorumThreshold();

``UpdateCommitteeAction`` proposes members to remove or add and a new quorum::

   const Cometa::UpdateCommitteeAction updateCommittee(
     Cometa::CredentialSet(), members, Cometa::UnitInterval(1, 2));
   const std::vector<std::uint8_t> proposalCbor = updateCommittee.toCbor();

``ProposalProcedure`` pairs one governance action with its deposit, return
address, and metadata anchor::

   const Cometa::ProposalProcedure proposal(
     500, Cometa::RewardAddress(Cometa::NetworkId::Testnet, coldCredential),
     Cometa::Anchor("https://a", Cometa::Blake2bHash::fromHex(std::string(64, '0'))),
     Cometa::InfoAction());
   const Cometa::InfoAction action = proposal.asInfoAction();

``ProposalProcedureSet`` stores proposals in insertion order::

   Cometa::ProposalProcedureSet proposals;
   proposals.add(proposal);
   const Cometa::ProposalProcedure first = proposals.at(0);

``Vote`` and ``VoterType`` identify governance choices and voter credentials::

   const Cometa::Vote choice = Cometa::Vote::Yes;
   const Cometa::VoterType voter = Cometa::VoterType::DRepKeyHash;

``Voter`` combines a voter kind with its credential::

   const Cometa::Voter drepVoter(Cometa::VoterType::DRepKeyHash, coldCredential);
