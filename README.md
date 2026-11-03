<div align="center">
  <a href="https://cometa.dev/">
    <img align="center" width="300" src="assets/cometa_cpp.png" alt="Cometa.cpp logo">
  </a>
</div>

<br>

<div align="center">

[![License](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](LICENSE) [![Post-Integration](https://github.com/Biglup/cometa.cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/Biglup/cometa.cpp/actions/workflows/ci.yml) [![Documentation Status](https://img.shields.io/readthedocs/cometacpp/latest)](https://cometacpp.readthedocs.io/en/latest/) [![Twitter Follow](https://img.shields.io/twitter/follow/BiglupLabs?style=social)](https://x.com/BiglupLabs)

</div>

<hr>

- [Official Website](https://cometa.dev/)
- [Installation](#installation)
- [Documentation](https://cometacpp.readthedocs.io/en/latest/)

<hr>

Cometa.cpp is a C++17 library for Cardano, built on [cardano-c](https://github.com/Biglup/cardano-c). It provides transaction building, signing, staking, governance, and native Plutus evaluation with automatic memory management.

## Installation

Download a package from [GitHub Releases](https://github.com/Biglup/cometa.cpp/releases) and set `CMAKE_PREFIX_PATH` to its directory. Packages include static and shared libraries for Linux and macOS (x64/ARM64), and Windows (x64).

```cmake
FIND_PACKAGE (Cometa CONFIG REQUIRED)
TARGET_LINK_LIBRARIES (my_application PRIVATE Cometa::Cometa)
```

Use `Cometa::CometaStatic` for static linking. See the [installation guide](https://cometacpp.readthedocs.io/en/latest/getting_started.html) for HTTP transport, platform dependencies, and building from source.

## First transaction

Build, sign, and submit a payment:

```cpp
Cometa::TransactionBuilder builder(parameters, slots);

builder.setUtxos(utxos)
  .setChangeAddress(sender)
  .sendLovelace(recipient, 2'000'000);

Cometa::Transaction transaction = builder.build();
transaction.applyVkeyWitnesses(signer.signTransaction(transaction));

Cometa::Blake2bHash transactionId = provider.submitTransaction(transaction);
```

The examples use protocol parameters, slot configuration, and UTxOs from the target network. See the [complete payment example](examples/send_lovelace_example.cpp) for wallet and provider setup.

## Transaction building

### Mint a native asset

```cpp
Cometa::Value output(2'000'000);
output.addAsset(asset, 1);

Cometa::TransactionBuilder builder(parameters, slots);

builder.setUtxos(utxos)
  .setChangeAddress(sender)
  .mintToken(asset, 1)
  .addScript(policyScript)
  .sendValue(recipient, output);

Cometa::Transaction transaction = builder.build();
```

[Full example](examples/mint_burn_native_script_example.cpp)

### Spend from a Plutus validator

```cpp
Cometa::TransactionBuilder builder(parameters, slots);

builder.setUtxos(paymentUtxos)
  .setCollateralUtxos(collateralUtxos)
  .setCollateralChangeAddress(sender)
  .setChangeAddress(sender)
  .addScript(validator)
  .addInput(scriptUtxo, redeemer)
  .sendLovelace(sender, 1'000'000);

Cometa::Transaction transaction = builder.build();
```

[Full example](examples/spend_from_validator_example.cpp)

### Vote on a governance proposal

```cpp
Cometa::TransactionBuilder builder(parameters, slots);

builder.setUtxos(utxos)
  .setChangeAddress(sender)
  .vote(voter, actionId, Cometa::VotingProcedure(Cometa::Vote::Yes));

Cometa::Transaction transaction = builder.build();
```

[Key DRep example](examples/vote_for_proposal_pubkey_example.cpp) · [Script DRep example](examples/vote_for_proposal_script_example.cpp)

## Dijkstra era

Dijkstra adds independently signed subtransactions, direct deposits to reward accounts, account balance constraints, key and script guards, and Plutus V4. These features require a network running Dijkstra.

### Batch subtransactions

A party signs a subtransaction. The batcher includes it in a top-level transaction and satisfies its guard:

```cpp
Cometa::SubTransactionBuilder intent(parameters, slots);

intent.addInput(partyUtxo)
  .sendLovelace(partyAddress, 12'000'000)
  .requireTopLevelGuard(batcherCredential);

Cometa::SubTransaction signedIntent = intent.build();
signedIntent.applyVkeyWitnesses(partySigner.signSubTransaction(signedIntent));

Cometa::TransactionBuilder batch(parameters, slots);

batch.setUtxos(batcherUtxos)
  .setChangeAddress(batcherAddress)
  .addSubTransaction(signedIntent, partyUtxos)
  .addGuard(batcherCredential);

Cometa::Transaction transaction = batch.build();
```

[Two-party batch example](examples/sub_transaction_batch_example.cpp) · [Plutus V4 guard example](examples/plutus_v4_guarded_sub_transaction_example.cpp)

## Deferred redeemers

When a redeemer depends on input or output order, defer its calculation until the builder has balanced a draft:

```cpp
builder.addInputWithDeferredRedeemer(
  scriptUtxo,
  [scriptInput](const Cometa::Transaction& draft, const Cometa::UtxoList&)
  {
    const std::optional<std::uint64_t> index = draft.findInputIndex(scriptInput);

    if (!index.has_value())
    {
      throw std::runtime_error("Script input is absent from the draft");
    }

    return Cometa::PlutusData::fromUnsigned(*index);
  });
```

Callbacks may run more than once and must be deterministic. [Full example](examples/deferred_redeemer_self_index_example.cpp)

## Extension points

The builder defaults to random-improve coin selection and local Plutus evaluation. Override either through its C++ interface:

```cpp
const std::shared_ptr<Cometa::CoinSelector> selector =
  std::make_shared<Cometa::LargeFirstCoinSelector>();
builder.setCoinSelector(selector);

const std::shared_ptr<Cometa::TransactionEvaluator> evaluator =
  std::make_shared<Cometa::ProviderTransactionEvaluator>(networkProvider);
builder.setTransactionEvaluator(evaluator);
```

Implement `CoinSelector` for custom UTxO selection or `TransactionEvaluator` for custom script evaluation. `Provider` supplies network data and submits transactions; `HttpTransport` lets providers use another HTTP client. Here, `networkProvider` is a `std::shared_ptr<Cometa::Provider>`.

## More examples

The [examples directory](examples) includes complete programs for payments, minting, staking, governance, key derivation, and Dijkstra transactions. Network examples use preprod and read `BLOCKFROST_API_KEY` from the environment.

## License

Apache 2.0. See [LICENSE](LICENSE).
