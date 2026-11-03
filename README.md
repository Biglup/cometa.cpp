<div align="center">
  <img width="300" src="assets/cometa_cpp.png" alt="Cometa.cpp logo">

  <p><strong>Cardano development in modern C++</strong></p>

  <p>
    <a href="https://github.com/Biglup/cometa.cpp/actions/workflows/ci.yml"><img src="https://github.com/Biglup/cometa.cpp/actions/workflows/ci.yml/badge.svg" alt="Build and tests"></a>
    <a href="https://cometacpp.readthedocs.io/en/latest/"><img src="https://app.readthedocs.org/projects/cometacpp/badge/?version=latest" alt="Documentation"></a>
    <a href="LICENSE"><img src="https://img.shields.io/badge/license-Apache%202.0-blue.svg" alt="Apache 2.0 license"></a>
  </p>

  <p>
    <a href="https://cometacpp.readthedocs.io/en/latest/">Documentation</a> ·
    <a href="examples/">Examples</a> ·
    <a href="https://github.com/Biglup/cometa.cpp/releases">Releases</a> ·
    <a href="https://cometa.dev/">Cometa website</a>
  </p>
</div>

Cometa.cpp is a C++17 binding for [cardano-c](https://github.com/Biglup/cardano-c). It provides C++ value types for Cardano data, transaction and subtransaction builders, signing, local Plutus evaluation, and a Blockfrost provider. Objects manage their own resources and support ordinary copy and move operations where those operations make sense.

## Quick start

Build a payment with the fluent transaction builder:

```cpp
Cometa::TransactionBuilder builder(parameters, slots);

Cometa::Transaction transaction = builder
  .setUtxos(utxos)
  .setChangeAddress(sender)
  .sendLovelace(recipient, 2'000'000)
  .build();
```

`parameters` and `utxos` come from your network provider; `slots`, `sender`, and `recipient` describe the network and addresses. See the [complete payment example](examples/send_lovelace_example.cpp) for signing and submission.

## Installation

Download the ZIP for your operating system and architecture from [GitHub Releases](https://github.com/Biglup/cometa.cpp/releases). Packages are built for Linux x64/ARM64, macOS x64/ARM64, and Windows x64. Set `CMAKE_PREFIX_PATH` to the extracted package directory, then link the CMake target:

```cmake
FIND_PACKAGE (Cometa CONFIG REQUIRED)
TARGET_LINK_LIBRARIES (my_application PRIVATE Cometa::Cometa)
```

`Cometa::Cometa` uses the shared library when available. Choose `Cometa::CometaStatic` for static linking. If you use the packaged HTTP transport, request `COMPONENTS http` and link `Cometa::Http` or `Cometa::HttpStatic` instead. The [installation guide](https://cometacpp.readthedocs.io/en/latest/getting_started.html) covers runtime library paths and platform dependencies.

## Transaction examples

The snippets below assume you have protocol parameters, a slot configuration, spendable UTxOs, and the addresses or script values used in each example. Each starts with a fresh builder. The linked files contain complete, runnable flows.

### Multiple payments

```cpp
Cometa::TransactionBuilder builder(parameters, slots);

Cometa::Transaction transaction = builder
  .setUtxos(utxos)
  .setChangeAddress(sender)
  .sendLovelace(alice, 5'000'000)
  .sendLovelace(bob, 10'000'000)
  .build();
```

See [send_lovelace_example.cpp](examples/send_lovelace_example.cpp).

### Minting a native asset

```cpp
Cometa::Value output(2'000'000);
output.addAsset(asset, 1);

Cometa::TransactionBuilder builder(parameters, slots);

Cometa::Transaction transaction = builder
  .setUtxos(utxos)
  .setChangeAddress(sender)
  .mintToken(asset, 1)
  .addScript(policyScript)
  .sendValue(recipient, output)
  .build();
```

`asset` identifies a token under `policyScript`. See the [mint and burn example](examples/mint_burn_native_script_example.cpp) for policy construction, metadata, signing, and burning.

### Treasury donation and governance

```cpp
Cometa::TransactionBuilder builder(parameters, slots);

Cometa::Transaction transaction = builder
  .setUtxos(utxos)
  .setChangeAddress(sender)
  .setDonation(2'000'000)
  .build();
```

For voting, add `builder.vote(voter, actionId, Cometa::VotingProcedure(Cometa::Vote::Yes))` before `build()`. See the [treasury donation](examples/donate_to_treasury_example.cpp), [DRep registration](examples/drep_pubkey_example.cpp), [proposal](examples/propose_treasury_withdrawal_example.cpp), and [voting](examples/vote_for_proposal_pubkey_example.cpp) examples.

### Direct deposit

On a Dijkstra network, a transaction can deposit directly into a reward account:

```cpp
Cometa::TransactionBuilder builder(parameters, slots);

Cometa::Transaction transaction = builder
  .setUtxos(utxos)
  .setChangeAddress(sender)
  .addDirectDeposit(rewardAddress, 2'000'000)
  .build();
```

`addAccountBalanceInterval()` can also constrain the account balance in the same transaction.

### Dijkstra subtransactions

A party can build and sign an intent independently. The batch builder then includes that intent and its resolved input:

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

The [batch example](examples/sub_transaction_batch_example.cpp) settles two signed intents. The [Plutus V4 guard example](examples/plutus_v4_guarded_sub_transaction_example.cpp) covers script guards. Dijkstra features require protocol parameters from a network that supports them.

### Deferred redeemers

A redeemer can depend on the final input order, which is only known after balancing. The callback receives the current balanced draft and resolved inputs:

```cpp
builder.addInputWithDeferredRedeemer(
  scriptUtxo,
  [scriptInput](const Cometa::Transaction& draft, const Cometa::UtxoList&)
  {
    const std::optional<std::uint64_t> index = draft.findInputIndex(scriptInput);

    if (!index.has_value())
    {
      throw std::runtime_error("Script input is absent from the balanced transaction");
    }

    return Cometa::PlutusData::fromUnsigned(*index);
  });
```

The callback may run more than once while the builder balances, so it must be deterministic and avoid side effects. See the [self-index](examples/deferred_redeemer_self_index_example.cpp), [input/output pairing](examples/deferred_redeemer_batch_example.cpp), and [zero-withdrawal](examples/deferred_redeemer_withdraw_zero_example.cpp) examples.

## Extension points

The builder defaults to random-improve coin selection and local Plutus evaluation. To choose another built-in strategy or evaluate through a provider:

```cpp
builder.setCoinSelector(std::make_shared<Cometa::LargeFirstCoinSelector>());
builder.setTransactionEvaluator(
  std::make_shared<Cometa::ProviderTransactionEvaluator>(networkProvider));
```

`networkProvider` is a `std::shared_ptr<Cometa::Provider>`. For your own strategy, implement `CoinSelector::name()` and `CoinSelector::select()`. For another evaluator, implement `TransactionEvaluator::name()` and `TransactionEvaluator::evaluate()`. A custom network source implements `Provider`; a different HTTP stack implements `HttpTransport`. The [API reference](https://cometacpp.readthedocs.io/en/latest/) documents these interfaces.

The [examples directory](examples/) contains C++ counterparts of all 21 cardano-c examples, including scripts, staking, governance, deferred redeemers, and subtransactions. Network examples use preprod and read `BLOCKFROST_API_KEY` at runtime. Use a dedicated test wallet when running examples that submit transactions.

## Build from source

Use CMake 3.22 or newer and a C++17 compiler:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
cmake --install build --prefix /path/to/cometa
```

Tests require GoogleTest. Pass `-DCOMETA_BUILD_TESTS=OFF` if it is unavailable. The build downloads a pinned cardano-c revision; to use a local checkout, pass `-DCOMETA_CARDANO_SOURCE_DIR=/path/to/cardano-c`. Pass `-DCOMETA_BUILD_SHARED=OFF` for a static-only build.

## Documentation

The [C++ guide and API reference](https://cometacpp.readthedocs.io/en/latest/) cover values, builders, providers, errors, and installation. To build them locally:

```bash
python3 -m pip install -r doc/requirements.txt
cmake -S doc -B build/docs
cmake --build build/docs --target doc
```

## License

Apache 2.0. See [LICENSE](LICENSE).
