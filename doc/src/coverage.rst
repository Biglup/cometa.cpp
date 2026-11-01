Native API mapping
==================

This is an initial review slice, not a complete binding. The native revision is
pinned in ``cmake/Cardano.cmake``. Other domain types, builders, providers, signing,
and callback adapters are not implemented yet.

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

Error mapping
-------------

Operation failures throw ``Error`` with a documented ``ErrorCode``. Allocation
failures reported as such throw ``std::bad_alloc``. Unit tests verify credential
and error code values against cardano-c.

Tests cover encodings, known hash digests, independent copies, moves, allocation
failures, and cleanup. Transaction conformance fixtures will apply when transaction
types are implemented.
