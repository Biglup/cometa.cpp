Scripts usage
=============

Native script requirements
--------------------------

Use ``ScriptPubkey`` for a key signature, ``ScriptInvalidBefore`` and
``ScriptInvalidAfter`` for slot bounds, and ``ScriptRequireGuard`` for a Dijkstra
guard credential. Key hashes must contain 28 bytes.

.. code-block:: cpp

   const Cometa::Blake2bHash keyHash = publicKey.hash();
   const Cometa::ScriptPubkey signature(keyHash);
   const Cometa::ScriptInvalidBefore startsAt(1000);
   const Cometa::ScriptInvalidAfter endsAt(2000);
   const Cometa::ScriptRequireGuard guard(
     Cometa::Credential(Cometa::CredentialType::KeyHash, keyHash));

``fromJson`` accepts the cardano-cli native-script format: ``sig`` with
``keyHash``, ``after`` with ``slot`` for an invalid-before bound, ``before`` with
``slot`` for an invalid-after bound, and ``guard`` with exactly one of ``keyHash``
or ``scriptHash``. Slot values must be unsigned integers without fractional or
exponent notation. ``toJson`` produces CIP-116 inspection JSON, which uses a
different schema.

Copies and returned credentials are independent. CBOR decoded with ``fromCbor``
retains its original encoding until a setter or ``normalizeEncoding`` is used.

Composite native scripts
------------------------

Combine requirements with ``ScriptAll``, ``ScriptAny``, or ``ScriptNOfK`` and
wrap the result in ``NativeScript``. A threshold may exceed the child count;
that describes an unsatisfiable requirement rather than an invalid value.

.. code-block:: cpp

   const Cometa::NativeScriptList requirements {
     Cometa::NativeScript(Cometa::ScriptPubkey(publicKey.hash())),
     Cometa::NativeScript(Cometa::ScriptInvalidBefore(1000))
   };
   const Cometa::NativeScript policy { Cometa::ScriptAll(requirements) };
   const Cometa::Blake2bHash policyId = policy.hash();

``type()`` identifies the variant. Accessors such as ``asAll()`` return
independent requirements and throw ``Error`` for a different variant. Change a
returned composite with ``setScripts`` and construct a new ``NativeScript`` to
replace the parent value.

Script hashes use the original decoded CBOR, including nonminimal encodings.
``normalizeEncoding()`` recursively rebuilds those encodings and can change the
hash. ``NativeScriptList`` retains its definite or indefinite array form and its
children's encodings, but normalizes nonminimal array-length headers. Appending a
child does not normalize existing children.

Native-script JSON uses the cardano-cli schema: ``all`` and ``any`` objects have
a ``scripts`` array; ``atLeast`` additionally has an unsigned ``required`` count.
``NativeScriptList::fromJson`` accepts an object containing a ``scripts`` array.

Plutus scripts and parameters
-----------------------------

``PlutusV1Script`` through ``PlutusV4Script`` accept compiled script bytes.
``fromHex`` decodes those bytes from hexadecimal; ``fromCbor`` decodes an outer
CBOR bytestring containing them. ``toBytes`` and ``toHex`` return the compiled
bytes, while ``toCbor`` adds that outer bytestring.

.. code-block:: cpp

   const Cometa::PlutusV4Script script =
     Cometa::PlutusV4Script::fromHex("450100002499");
   Cometa::PlutusList parameters;
   parameters.pushBack(Cometa::PlutusData(std::int64_t { 42 }));

   const Cometa::PlutusV4Script applied = script.applyParameters(parameters);
   const Cometa::Blake2bHash scriptHash = applied.hash();

``applyParameters`` requires a CBOR-wrapped Flat program, applies arguments in
list order, and returns a new script. An empty list canonicalizes the program.
Malformed programs throw ``Error``. Applying parameters does not execute the
script or change its ledger language version.

``hash`` includes the language prefix, so identical bytes in different Plutus
versions have different hashes. Decoding an outer CBOR representation normalizes
its bytestring header without changing the compiled bytes or script hash.

Working with either script language
-----------------------------------

``Script`` holds a ``NativeScript`` or any of the four Plutus script types.
Use ``language()`` to identify the variant and a typed accessor to obtain it.
An accessor for a different language throws ``Error``.

.. code-block:: cpp

   const Cometa::Script script(
     Cometa::PlutusV4Script::fromHex("450100002499"));

   if (script.language() == Cometa::ScriptLanguage::PlutusV4)
   {
     const Cometa::PlutusV4Script validator = script.asPlutusV4();
     const std::string bytes = validator.toHex();
   }

Constructors and accessors copy values. Replacing a returned script does not
modify its parent. ``toCbor`` includes both the language discriminator and the
script payload; ``hash`` uses the underlying script's language-specific hash.
Native payloads retain their decoded encoding, including encodings that affect
the hash. To normalize a native payload, normalize the value returned by
``asNative()`` and construct a replacement ``Script`` from it.
