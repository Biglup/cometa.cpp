Auxiliary Data usage
====================

Auxiliary Plutus script lists
-----------------------------

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

   const std::vector<std::uint8_t> cbor      = metadata.toCbor();
   const std::string               plainJson = metadata.toPlainJson();

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

   metadata.set(674, Cometa::Metadatum::fromPlainJson(R"({"msg":["Hello from Cometa"]})"));

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
