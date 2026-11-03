Protocol Parameters usage
=========================

Protocol parameters
-------------------

``ExUnitPrices`` holds the rational memory and CPU step prices used by Plutus
execution. Both prices can be read and replaced independently::

   Cometa::ExUnitPrices prices(Cometa::UnitInterval(1, 2), Cometa::UnitInterval(1, 3));

   prices.setMemoryPrice(Cometa::UnitInterval(2, 3));

``CostModel`` holds operation costs for a Plutus language. Decoding retains
unknown language IDs so a model can be read and re-encoded without loss::

   Cometa::CostModel model(Cometa::PlutusLanguageVersion::V4, { 1, 2, 3 });

   model.setCost(1, 7);

   const std::vector<std::int64_t> costs = model.costs();

``CostModels`` stores cost models by language ID. The collection retains models
for future language IDs when decoding CBOR::

   Cometa::CostModels models;

   models.insert(model);

   const std::optional<Cometa::CostModel> v4 =
     models.get(Cometa::PlutusLanguageVersion::V4);

``DRepVotingThresholds`` and ``PoolVotingThresholds`` expose each governance
threshold by name. Both accept rational ``UnitInterval`` values and support
independent copies and CBOR serialization::

   Cometa::DRepVotingThresholds drep =
     Cometa::DRepVotingThresholds::fromCbor(drepThresholdCbor);

   drep.setTreasuryWithdrawal(Cometa::UnitInterval(2, 3));

   Cometa::PoolVotingThresholds pool =
     Cometa::PoolVotingThresholds::fromCbor(poolThresholdCbor);

   pool.setSecurityRelevantParam(Cometa::UnitInterval(1, 2));

``ProtocolParamUpdate`` stores proposed changes as optional values. A missing
value is omitted from the update; ``std::nullopt`` removes an existing proposal::

   Cometa::ProtocolParamUpdate update;

   update.setMinFeeA(std::uint64_t(44));
   update.setPoolPledgeInfluence(Cometa::UnitInterval(1, 2));
   update.setMinFeeA(std::nullopt);

Max pledge leverage and the Peras bootstrap round also have an explicit
ledger-null value, separate from absence. Use
``setMaxPledgeLeverageUnbounded()`` or ``setPerasBootstrapRoundNull()`` to
propose that value; use the matching ``clear`` method to omit the field.

``ProposedParamUpdates`` associates proposed changes with genesis delegate key
hashes. Entries are sorted by hash, and accessors return independent values::

   Cometa::ProposedParamUpdates proposals;
   const Cometa::Blake2bHash    delegate =
     Cometa::Blake2bHash::fromHex(delegateHashHex);

   proposals.add(delegate, update);

   const std::optional<Cometa::ProtocolParamUpdate> found =
     proposals.get(delegate);

``Update`` combines the proposals with the epoch in which they take effect::

   Cometa::Update epochUpdate(500, proposals);

   const std::vector<std::uint8_t> cbor    = epochUpdate.toCbor();
   const Cometa::Update            decoded = Cometa::Update::fromCbor(cbor);

``ProtocolParameters`` holds the current network settings. It includes the
Conway, Leios, and Peras fields, with typed values for prices, execution units,
thresholds, and cost models::

   Cometa::ProtocolParameters parameters;

   parameters.setMinFeeA(44);
   parameters.setMaxTxExUnits(Cometa::ExUnits(10'000'000, 10'000'000'000));
   parameters.setPerasBootstrapRound(std::nullopt);

Copies own independent values. A missing max pledge leverage or Peras bootstrap
round is represented by ``std::nullopt``.
