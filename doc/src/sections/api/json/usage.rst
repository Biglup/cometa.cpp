JSON usage
==========

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
