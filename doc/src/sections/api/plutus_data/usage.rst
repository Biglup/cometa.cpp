Plutus Data usage
=================

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
