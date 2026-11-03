Assets usage
============

Identifying assets
------------------

An ``AssetId`` combines a 28-byte policy ID and an asset name of up to 32 bytes.
The byte and hexadecimal representations concatenate those fields.
``AssetId::lovelace()`` represents lovelace with empty bytes and hex; parsing
an empty representation returns that same identifier. A policy with an empty
asset name remains a native token.

.. code-block:: cpp

   const Cometa::AssetId     token(policyId, Cometa::AssetName::fromString("Cometa"));
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

   const std::vector<std::uint8_t> cbor    = payment.toCbor();
   const Cometa::Value             decoded = Cometa::Value::fromCbor(cbor);

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
