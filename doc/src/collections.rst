Collections
===========

Cometa collections support range-based loops, iterator pairs, and C++17 standard
algorithms. Keep using domain types such as ``TransactionOutputList``,
``AssetNameMap``, and ``GuardSet``; converting to a different container is optional.

Sequences
---------

Lists expose mutable random-access iterators, ``at``, ``operator[]``, ``front``,
``back``, ``push_back``, ``emplace_back``, ``insert``, ``erase``, and ``clear``.
They also support range construction, ``assign``, and capacity operations.
``CertificateSet`` and ``ProposalProcedureSet`` expose the same sequence interface:
certificate and proposal positions are significant, and duplicate entries are retained.

.. code-block:: cpp

   Cometa::PlutusList values { Cometa::PlutusData(1), Cometa::PlutusData(2) };

   values.emplace_back(3);
   std::reverse(values.begin(), values.end());

   for (Cometa::PlutusData& value: values)
   {
     value = Cometa::PlutusData(4);
   }

   const std::vector<std::uint8_t> cbor = values.toCbor();

Use ``std::back_inserter`` to append through algorithms:

.. code-block:: cpp

   Cometa::PlutusList copy;

   std::copy(values.cbegin(), values.cend(), std::back_inserter(copy));

References obtained from mutable accessors refer to collection elements. Assigning
to an element changes the collection. Copy an element explicitly when an independent
value is needed.

Maps
----

Keyed maps expose bidirectional iterators over ``std::pair<const key_type,
mapped_type>``. Keys cannot be changed through an iterator; mapped values can.
``find``, ``count``, ``contains``, ``at``, ``insert``, ``insert_or_assign``,
``try_emplace``, ``erase``, and ``clear`` provide the usual lookup and mutation
operations. ``operator[]`` is available where the mapped type has a useful default
value.

.. code-block:: cpp

   const Cometa::AssetName name = Cometa::AssetName::fromString("Cometa");
   Cometa::AssetNameMap amounts;

   amounts.try_emplace(name, 10);

   for (Cometa::AssetNameMap::value_type& entry: amounts)
   {
     entry.second += 5;
   }

   const std::int64_t amount = amounts.at(name);

``insert`` returns an iterator and a Boolean indicating whether insertion occurred.
An existing key is left unchanged; use ``insert_or_assign`` to replace its value.
``at`` throws ``std::out_of_range`` when a key is absent.

``MultiAsset`` maps policy hashes to ``AssetNameMap`` values. ``CostModels`` maps
language IDs to cost models; each model must have the same language ID as its key.
``VotingProcedures`` uses a ``std::pair<Voter, GovernanceActionId>`` key for each vote.

``PlutusMap`` and ``MetadatumMap`` are ordered sequences of key/value pairs. They
retain duplicate keys and expose the sequence operations above. Their existing
two-argument ``insert`` appends an entry. They do not have the unique-key semantics
of ``AssetNameMap``.

Sets and identity-bearing collections
-------------------------------------

Sets expose read-only element references. Use ``insert``, ``emplace``, ``find``,
``count``, ``erase``, and ``clear`` to change membership. They work with
``std::inserter`` and algorithms that read elements. An element cannot be modified
in place because that could change its identity or ordering.

.. code-block:: cpp

   Cometa::GuardSet guards;
   const Cometa::Credential guard(
     Cometa::CredentialType::KeyHash,
     Cometa::Blake2bHash::fromHex(std::string(56, '1')));

   const std::pair<Cometa::GuardSet::iterator, bool> inserted = guards.insert(guard);

   if (inserted.second)
   {
     // The guard was added.
   }

Iteration follows each collection's domain order; a type named ``Set`` does not
necessarily sort its elements. Guard order and subtransaction order are retained.
Witnesses are identified by their signing key or Byron address root, scripts by
hash, subtransactions by ID, and redeemers by purpose and index. Standard ``insert``
leaves an existing identity unchanged. Existing domain operations such as witness
``add`` retain their documented replacement behavior.

Lifetime and serialization
--------------------------

* Reading a collection through iterators does not change its CBOR encoding.
* Element edits are included in serialization and in values passed to parent objects.
  Copying a collection or assigning it to a parent produces an independent value.
* Sequence iterators follow vector invalidation rules: reallocation invalidates all
  references; insertion or erasure invalidates positions at or after the change.
  ``assign`` and ``clear`` invalidate all element references.
* Standard map insertion preserves references. Erasure invalidates only references
  to erased entries. Existing domain mutators such as ``set`` and two-argument
  ``insert`` invalidate all map iterators and references.
* Successful set membership changes invalidate all set iterators and references.
  Inserting a duplicate through standard ``insert`` leaves them valid.
* Serialization does not invalidate element references. Move construction and swap
  transfer them with their elements. Assignment invalidates references to the
  destination's previous elements.

The first iterator or reference access may allocate and copy elements; subsequent
iterator increments and dereferences do not allocate. Map lookup uses ordered-key
comparison; set lookup is linear. Set insertion and erasure may copy the collection.
Serialization after mutable access checks element encodings and can require work
proportional to the collection size. Domain constraints are checked when entries
are inserted and when edited values are serialized or passed to a parent.

Include ``<algorithm>`` and ``<iterator>`` for the algorithms and iterator adapters
used above. Coordinate access to the same collection across threads, including
serialization and first access. Independent copies can be used independently.
