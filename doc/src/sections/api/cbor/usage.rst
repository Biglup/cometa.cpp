CBOR usage
==========

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
