Native API mapping
==================

This binding is under development. The native revision is
pinned in ``cmake/Cardano.cmake``. Remaining domain types, builders, providers, signing,
and callback adapters are tracked in :doc:`implementation_plan`.

.. list-table:: Implemented domain operations
   :header-rows: 1
   :widths: 30 70

   * - Native API
     - C++ API
   * - ``cardano_blake2b_hash_t`` and hash constructors
     - ``Blake2bHash``, byte constructor, ``fromHex``, ``compute``, ``fromCbor``
   * - Hash bytes, hex, CBOR, and CIP-116 JSON
     - ``toBytes``, ``toHex``, ``toCbor``, ``toJson`` return owned C++ values
   * - Hash equality and comparison
     - ``==``, ``!=``, ``<``
   * - ``cardano_credential_t`` and hash-based constructors
     - ``Credential(type, hash)`` and ``fromCbor``
   * - Credential hash/type access and mutation
     - ``hash``, ``type``, ``setHash``, ``setType``
   * - Credential comparison, CBOR, and CIP-116 JSON
     - ``==``, ``!=``, ``<``, ``toCbor``, ``toJson``
   * - ``cardano_credential_type_t``
     - ``CredentialType::KeyHash`` and ``CredentialType::ScriptHash``; unknown values are rejected
   * - ``cardano_asset_name_t`` and its constructors
     - ``AssetName``, byte constructor, ``fromHex``, ``fromString``, ``fromCbor``
   * - Asset name bytes, string, hex, CBOR, and CIP-116 JSON
     - ``toBytes``, ``toString``, ``toHex``, ``toCbor``, ``toJson``
   * - ``cardano_blake2b_hash_size_t`` size constants
     - Digest size expressed as a byte count rather than a closed enum
   * - ``*_deep_clone``
     - Copy construction and assignment; independent ownership

   * - ``cardano_ex_units_t``
     - ``ExUnits`` construction, mutation, CBOR/hex parsing, JSON, and independent copies
   * - ``cardano_protocol_version_t``
     - ``ProtocolVersion`` with a 64-bit major version and checked 32-bit minor version
   * - Network, byte-order, and governance-key enums
     - ``NetworkId``, ``NetworkMagic``, ``ByteOrder``, and ``GovernanceKeyType``
   * - Slot configuration and time conversion
     - ``SlotConfig``; seconds and milliseconds are explicit, and overflow is rejected

   * - ``cardano_unit_interval_t``
     - ``UnitInterval`` preserves unsigned components and checks floating-point conversion
   * - ``cardano_anchor_t``
     - ``Anchor`` URL/hash access, mutation, CBOR, and JSON
   * - ``cardano_governance_action_id_t``
     - ``GovernanceActionId`` hash/index access, mutation, CBOR, JSON, and checked CIP-129 encoding

   * - ``cardano_bigint_t``
     - ``BigInt`` arithmetic, comparisons, checked narrowing, byte/string encoding, and bitwise/modular operations

   * - Base58 and Bech32 encoding
     - ``encodeBase58``, ``decodeBase58``, ``encodeBech32``, and ``decodeBech32``; strings, byte vectors, and ``Bech32Data``

   * - CBOR readers, writers, and enums
     - ``CborReader``, ``CborWriter``, ``CborTag``, ``CborMajorType``, ``CborSimpleValue``, and ``CborReaderState``; independent cursor/buffer copies

   * - JSON values, writers, and enums
     - ``JsonValue``, ``JsonWriter``, ``JsonType``, ``JsonFormat``, and ``JsonContext``; owned children, exact number tokens, checked writes

   * - Ed25519 and BIP32 keys and signatures
     - ``Ed25519Signature``, ``Ed25519PublicKey``, ``Ed25519PrivateKey``, ``Bip32PublicKey``, ``Bip32PrivateKey``, and ``hardenIndex``

Coverage ledger
---------------

The inventory targets cardano-c commit
``6fe1ded7f81e259631bb06f2f0a33601f33f9df7``, matching the dependency pin.
It contains 231 public headers, 418 implementation files, 235 distinct public
typedefs, and 3,335 exported function declarations. Tests and vendored dependencies
are outside the file inventory.

* :download:`File inventory <coverage/files.csv>`
* :download:`Type coverage <coverage/types.csv>`
* :download:`Function coverage <coverage/functions.csv>`
* :download:`PR sequence and acceptance criteria <coverage/prs.csv>`

``implemented`` identifies a C++ equivalent, which may combine several C operations.
``internal`` identifies C plumbing represented through automatic ownership,
standard containers, or private adapters; it is not a claim that every internal
C operation has a dedicated C++ test. ``planned`` entries remain unfinished.
Function rows name the C++ operation and its associated test suite when available.
Detailed ownership audits remain part of implementing each planned operation.

Update the ledger when adding an API or changing the native pin. Passing tests
or the shared corpus does not establish complete API coverage.

Error mapping
-------------

Operation failures throw ``Error`` with a documented ``ErrorCode``. Allocation
failures reported as such throw ``std::bad_alloc``. Unit tests verify credential
and error code values against cardano-c.

Tests cover encodings, known hash digests, independent copies, moves, allocation
failures, and cleanup. Transaction conformance fixtures will apply when transaction
types are implemented.
