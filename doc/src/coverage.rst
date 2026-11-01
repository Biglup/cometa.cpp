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

Ownership and error mappings
----------------------------

``*_ref``, ``*_unref``, reference counts, raw pointer getters, and buffer-size
queries are internal implementation details. Native constructors and decoders
return one owned reference. The internal unique-pointer guard releases it on
failure; successful construction transfers it to the C++ object's private owner.
Destructors unref exactly once. Moves clear the source handle explicitly.

Hash getters and setters on ``Credential`` copy bytes into fresh native values;
no mutable child is shared with the caller. Bytes and strings returned to callers
own their storage. CBOR readers, writers, and JSON writers are scoped internally
and released before returning or throwing.

Native declarations come from the pinned C headers rather than a handwritten ABI.
Credential and error enum values are checked at compile time. Errors map to
``Error`` with a documented ``ErrorCode`` in the Cometa category, and
native allocation-error codes to ``std::bad_alloc``. Raw diagnostic setters are
not public operations; exceptions name the failed operation and retain its status.

The value tests cover native encodings, a known hash digest, mutation isolation,
copy/move/self-assignment, source destruction, allocation failure, parser cleanup,
and owned return values. Broader shared transaction fixtures will apply when those
types are implemented; this slice does not claim full corpus conformance.
