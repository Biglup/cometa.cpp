Transaction Body usage
======================

Transaction values and withdrawals
----------------------------------

``TransactionInput`` identifies a previous output by its 32-byte transaction ID
and output index. ``TransactionOutput`` contains an address and ``Value``;
``Utxo`` combines an input reference with its output contents.

.. code-block:: cpp

   const Cometa::TransactionInput input = Cometa::TransactionInput::fromHex(
     "0101010101010101010101010101010101010101010101010101010101010101",
     5);

   const Cometa::Address address = Cometa::Address::fromString(
     "addr_test1vqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxrcya6");

   Cometa::TransactionOutput output(address, Cometa::Value(2'000'000));

   output.setDatum(Cometa::Datum(Cometa::PlutusData(std::int64_t { 42 })));
   output.setScriptRef(Cometa::Script(
     Cometa::PlutusV4Script::fromHex("450100002499")));

   const Cometa::Utxo utxo(input, output);

   output.setDatum(std::nullopt);

``datum()`` and ``scriptRef()`` return ``std::optional`` values. Set either field
to ``std::nullopt`` to remove it. Constructors, setters, and getters copy values,
so changing ``output`` above does not change ``utxo``. Output lovelace must be
nonnegative; constructing these values does not establish transaction validity.

``TransactionInputSet`` sorts and deduplicates inputs. Its CBOR always includes
set tag 258; ``wasTagged()`` reports the decoded input's tag presence and returns
true for newly constructed sets. ``TransactionOutputList`` retains output order.
Both serialize as definite-length arrays. Legacy output arrays decode normally
and serialize in the current map form.

``WithdrawalMap`` stores exact unsigned 64-bit lovelace amounts. ``set`` replaces
an existing address's amount, including zero; ``get`` returns ``std::nullopt``
when absent, while ``at`` throws ``std::out_of_range``. ``keys``, ``entries``, and
``entryAt`` return independent values in serialization order.

``UtxoList`` and ``RewardAddressList`` are standard ``std::vector`` aliases. Use
normal vector operations and standard algorithms for sorting, finding, filtering,
concatenation, and slicing; their elements retain independent value semantics.

Dijkstra transaction bodies
---------------------------

``AccountBalanceInterval::exact`` requires one lovelace balance. The range constructor accepts an inclusive lower bound, an exclusive upper bound, or both.

``AccountBalanceIntervalsMap`` associates reward addresses with balance requirements. Entries are returned in canonical CBOR order, and duplicate addresses are rejected.

``DirectDepositMap`` associates reward accounts with unsigned lovelace deposits. It preserves zero deposits, orders entries canonically, and rejects duplicate addresses.

``GuardSet`` preserves guard order because script redeemers refer to guard positions. A key-only set uses the historical key-hash encoding; a set containing a script credential uses the credential encoding. Duplicates are rejected.

``RequiredGuardsMap`` associates guard credentials with optional Plutus data. ``get`` uses two optional layers: the outer layer distinguishes an absent credential, and the inner layer distinguishes a present credential with no datum. ``contains`` checks membership directly.

``SubTransactionBody`` combines inputs and outputs with optional guards, required top-level guards, direct deposits, account balance requirements, validity slots, and the other supported body fields. ``hash()`` computes the sub-transaction body hash.

``TransactionBody`` represents a top-level transaction body, including its fee, collateral, signing requirements, governance fields, and optional Dijkstra sub-transactions. Its fields can be updated before signing; ``hash()`` returns the current body hash.
