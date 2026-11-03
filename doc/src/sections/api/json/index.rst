JSON
====

JsonValue represents a parsed JSON document or value; JsonWriter constructs a document
incrementally. Transaction inspection uses the domain types’ toJson methods. Number
tokens retain their decimal representation until explicitly converted to a numeric C++
type.

.. toctree::
   :maxdepth: 1

   usage
   json_context
   json_format
   json_type
   json_value
   json_writer
