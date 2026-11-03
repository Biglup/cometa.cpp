CBOR
====

Cardano serializes ledger values with CBOR. Most domain types provide toCbor and
fromCbor; CborReader and CborWriter expose individual values and containers for custom
formats. Parsing a domain type accepts bytes or a hexadecimal CBOR string.

.. toctree::
   :maxdepth: 1

   usage
   cbor_major_type
   cbor_reader
   cbor_reader_state
   cbor_simple_value
   cbor_tag
   cbor_writer
