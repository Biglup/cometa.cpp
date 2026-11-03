Transaction Builder usage
=========================

Building a transaction
----------------------

``TransactionBuilder`` balances fees and change from the available UTxOs and
current protocol parameters. Supply a change address and the outputs to create.
For example, with an existing ``provider``, ``sender``, ``recipient``, and
``signer``::

   const Cometa::ProtocolParameters parameters = provider.getParameters();
   const Cometa::UtxoList           available  = provider.getUnspentOutputs(sender);

   Cometa::TransactionBuilder builder(
     parameters,
     Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod));

   builder.setUtxos(available)
     .setChangeAddress(sender)
     .sendLovelace(recipient, 2'000'000);

   Cometa::Transaction transaction = builder.build();
   transaction.applyVkeyWitnesses(signer.signTransaction(transaction));

   provider.submitTransaction(transaction);

Use the network's slot configuration and protocol parameters. An application
should set a validity interval and confirm submission as appropriate for its
flow. ``send_lovelace_example.cpp`` in the repository is a complete preprod
example.

Coin selection and evaluation
-----------------------------

Builders use the built-in coin selector and native script evaluator unless
another service is supplied. Implement ``CoinSelector`` or
``TransactionEvaluator`` to use a different strategy. The builder retains the
shared service during construction and building::

   builder.setCoinSelector(std::make_shared<Cometa::LargeFirstCoinSelector>());

   builder.setTransactionEvaluator(
     std::make_shared<Cometa::ProviderTransactionEvaluator>(providerPtr));

``providerPtr`` is a ``std::shared_ptr<Cometa::Provider>``. The native evaluator
is the default. Use provider-backed evaluation only when the application needs
network-side execution budgets.

Subtransactions and deferred redeemers
--------------------------------------

``SubTransactionBuilder`` constructs an intent that its participant signs.
Subtransactions require a network running Dijkstra. It uses the same value types
as ``TransactionBuilder``. A top-level builder can add the signed intent and its
resolved UTxOs::

   Cometa::SubTransactionBuilder intentBuilder(parameters, slots);

   intentBuilder.addInput(partyUtxo)
     .sendLovelace(partyAddress, 5'000'000)
     .requireTopLevelGuard(batcherCredential);

   Cometa::SubTransaction intent = intentBuilder.build();
   intent.applyVkeyWitnesses(partySigner.signSubTransaction(intent));

   Cometa::TransactionBuilder batch(parameters, slots);

   batch.setUtxos(batcherUtxos)
     .setChangeAddress(batcherAddress)
     .addSubTransaction(intent, Cometa::UtxoList { partyUtxo })
     .addGuard(batcherCredential);

   Cometa::Transaction transaction = batch.build();
   transaction.applyVkeyWitnesses(batcherSigner.signTransaction(transaction));

Use ``addInputWithDeferredRedeemer`` or ``mintTokenWithDeferredRedeemer`` when
redeemer data depends on the balanced transaction. The callback receives the
current draft and resolved inputs; the builder may invoke it more than once, so
it should return the same data for the same draft. The complete batch and
deferred-redeemer examples are in ``examples/``.

Transaction utilities
---------------------

Use ``computeMinAdaRequired`` to price an output before adding it to a transaction.
``computeTransactionFee`` prices the current transaction, including its existing
witnesses and execution budgets. It does not reserve space for signatures that
have not yet been added; the builder and standalone balancer account for those.

.. code-block:: cpp

   const std::uint64_t minimum = Cometa::computeMinAdaRequired(
     output, parameters.adaPerUtxoByte());

   const Cometa::Value imbalance = Cometa::computeTransactionImbalance(
     transaction, resolvedInputs, parameters);

A positive imbalance is surplus value; a negative imbalance is a deficit.
``computeTransactionBatchImbalance`` includes sub-transactions.
``computeImplicitCoin`` reports withdrawals, certificate deposits and refunds,
and governance proposal deposits. Pool registration state still needs to be
checked against the chain when deciding whether a pool deposit is due.

To balance an existing unsigned draft, provide its resolved spending inputs and
available funds. The result includes the fee and change; the original draft is
unchanged. Sign the returned transaction. Use ``TransactionBuilder`` when a
redeemer must be computed after balancing.

.. code-block:: cpp

   Cometa::TransactionBalanceRequest request(
     draft, parameters, changeAddress, slotConfig);

   request.available = availableUtxos;
   request.collateral = collateralUtxos;
   request.coinSelector = std::make_shared<Cometa::LargeFirstCoinSelector>();

   const Cometa::Transaction balanced = Cometa::balanceTransaction(request);

``request.evaluator`` accepts the same ``TransactionEvaluator`` interface as the
builder. When no services are supplied, balancing uses random-improve selection
and local Plutus evaluation with the request's network parameters and slot timeline.
