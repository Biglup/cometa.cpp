Witness Set usage
=================

Script redeemers
----------------

A ``Redeemer`` contains its script purpose, index, Plutus data, and execution budget::

   Cometa::Redeemer redeemer(Cometa::RedeemerTag::Spend, 0, Cometa::PlutusData(42), Cometa::ExUnits(500, 700));
   const std::vector<std::uint8_t> cbor = redeemer.toCbor();

``RedeemerList`` stores redeemers in purpose and index order and rejects duplicate keys. ``setExUnits`` updates an existing redeemer's budget.

``VkeyWitness`` pairs an Ed25519 verification key with its transaction signature and supports CBOR and CIP-116 JSON output.

``VkeyWitnessSet`` holds key witnesses. Adding or applying a witness with an existing verification key updates its signature. ``setUseTag`` controls the CBOR set tag.

``BootstrapWitness`` adds a 32-byte chain code and bootstrap address attributes to a key witness.

``BootstrapWitnessSet`` groups bootstrap witnesses by Byron address root and updates the signature when a new witness has the same root.

``NativeScriptSet`` stores distinct native scripts by script hash and controls the CBOR set tag.

``PlutusDataSet`` stores distinct datum values. Adding to a decoded set updates its CBOR, and ``setUseTag`` controls the set tag.

``PlutusV1ScriptSet`` through ``PlutusV4ScriptSet`` hold distinct compiled scripts by hash. ``WitnessSet`` groups signatures, scripts, data, and redeemers; its optional fields can be read, replaced, or cleared.
