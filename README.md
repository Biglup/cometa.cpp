<div align="center">
  <img width="300" src="assets/cometa_cpp.png" alt="Cometa.cpp logo">
  <p><strong>A C++17 interface to Cardano</strong></p>
  <p>
    <a href="https://github.com/Biglup/cometa.cpp/actions/workflows/ci.yml"><img src="https://github.com/Biglup/cometa.cpp/actions/workflows/ci.yml/badge.svg" alt="Build and tests"></a>
    <a href="https://cometacpp.readthedocs.io/en/latest/"><img src="https://app.readthedocs.org/projects/cometacpp/badge/?version=latest" alt="Documentation"></a>
    <a href="LICENSE"><img src="https://img.shields.io/badge/license-Apache%202.0-blue.svg" alt="Apache 2.0 license"></a>
  </p>
  <p>
    <a href="#installation">Installation</a> ·
    <a href="#first-transaction">First transaction</a> ·
    <a href="#transaction-recipes">Recipes</a> ·
    <a href="#extension-points">Extension points</a> ·
    <a href="https://cometacpp.readthedocs.io/en/latest/">Documentation</a>
  </p>
</div>

Cometa.cpp binds [cardano-c](https://github.com/Biglup/cardano-c) to C++17. It offers value types for Cardano data, transaction builders, signing, native Plutus evaluation, and a Blockfrost provider. C++ objects manage native resources automatically; ordinary values support copy and move operations.

The library covers Conway transactions and governance, plus Dijkstra features on networks that enable them. The current version is **2.0.0-rc.1**, a release candidate whose API may still change.

## Installation

Download the archive for your platform from [GitHub Releases](https://github.com/Biglup/cometa.cpp/releases). Packages include headers, static and shared libraries, and a CMake package for Linux x64/ARM64, macOS x64/ARM64, and Windows x64.

Set `CMAKE_PREFIX_PATH` to the extracted package directory. Then link Cometa from your application's `CMakeLists.txt`:

```cmake
FIND_PACKAGE (Cometa CONFIG REQUIRED)
TARGET_LINK_LIBRARIES (my_application PRIVATE Cometa::Cometa)
```

`Cometa::Cometa` selects the shared library when one is present. Use `Cometa::CometaStatic` for static linking. If you use the packaged HTTP transport, request `COMPONENTS http` and link `Cometa::Http` or `Cometa::HttpStatic`. The [installation guide](https://cometacpp.readthedocs.io/en/latest/getting_started.html) covers runtime library paths and platform dependencies.

### Build from source

CMake 3.22 or newer and a C++17 compiler are required. Tests also require GoogleTest.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
cmake --install build --prefix /path/to/cometa
```

The build downloads a pinned cardano-c revision. Pass `-DCOMETA_CARDANO_SOURCE_DIR=/path/to/cardano-c` to use a local checkout, `-DCOMETA_BUILD_TESTS=OFF` to skip tests, or `-DCOMETA_BUILD_SHARED=OFF` for a static-only build.

## First transaction

A builder takes current protocol parameters and a slot configuration. Give it spendable UTxOs and a change address, then describe the payment:

```cpp
Cometa::TransactionBuilder builder(parameters, slots);

builder.setUtxos(utxos)
  .setChangeAddress(sender)
  .sendLovelace(recipient, 2'000'000);

Cometa::Transaction transaction = builder.build();
transaction.applyVkeyWitnesses(signer.signTransaction(transaction));

Cometa::Blake2bHash transactionId = provider.submitTransaction(transaction);
```

`parameters` and `utxos` come from the network provider. `sender` and `recipient` are `Address` values; `signer` is a `SecureKeyHandler`. The [runnable payment example](examples/send_lovelace_example.cpp) shows the complete preprod flow. A transaction is built and signed as an ordinary C++ object; no manual disposal is needed.

## Transaction recipes

These snippets show the operation that distinguishes each transaction. Use a fresh builder for each transaction, supply current network parameters, and sign before submitting. The [example catalog](#example-catalog) links complete programs.

### Mint a native asset

Build an output containing the minted token, then include the policy script in the transaction:

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

The [native mint and burn example](examples/mint_burn_native_script_example.cpp) creates the policy, attaches metadata, and burns a token. The [Plutus mint and burn example](examples/mint_burn_plutus_script_example.cpp) uses a redeemer and collateral.

### Spend from a Plutus validator

A script spend needs the script UTxO, its redeemer, the validator, and collateral:

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

See [spend_from_validator_example.cpp](examples/spend_from_validator_example.cpp) for a full reference-script flow.

### Vote on a governance proposal

A `Voter` identifies the credential casting the vote; `GovernanceActionId` identifies the proposal:

```cpp
Cometa::TransactionBuilder builder(parameters, slots);

builder.setUtxos(utxos)
  .setChangeAddress(sender)
  .vote(voter, actionId, Cometa::VotingProcedure(Cometa::Vote::Yes));

Cometa::Transaction transaction = builder.build();
```

The [key DRep vote](examples/vote_for_proposal_pubkey_example.cpp) and [script DRep vote](examples/vote_for_proposal_script_example.cpp) show how the required witnesses differ. The [treasury donation](examples/donate_to_treasury_example.cpp) and [proposal](examples/propose_treasury_withdrawal_example.cpp) examples cover other governance actions.

### Build a Dijkstra batch

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

The [batch example](examples/sub_transaction_batch_example.cpp) settles two independently signed intents. Dijkstra also adds Plutus V4 guards, direct deposits to reward accounts, and account balance constraints; see the [guarded V4 example](examples/plutus_v4_guarded_sub_transaction_example.cpp) and the [API reference](https://cometacpp.readthedocs.io/en/latest/).

### Compute a redeemer after balancing

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

The callback can run more than once, so it must be deterministic and free of side effects. Return the Plutus data shape expected by your validator. See the [self-index](examples/deferred_redeemer_self_index_example.cpp), [input/output pairing](examples/deferred_redeemer_batch_example.cpp), and [zero-withdrawal](examples/deferred_redeemer_withdraw_zero_example.cpp) examples.

## Extension points

The builder uses random-improve coin selection and the native Plutus evaluator by default. Both can be replaced without modifying the library:

```cpp
const std::shared_ptr<Cometa::CoinSelector> selector =
  std::make_shared<Cometa::LargeFirstCoinSelector>();
builder.setCoinSelector(selector);

const std::shared_ptr<Cometa::TransactionEvaluator> evaluator =
  std::make_shared<Cometa::ProviderTransactionEvaluator>(networkProvider);
builder.setTransactionEvaluator(evaluator);
```

`networkProvider` is a `std::shared_ptr<Cometa::Provider>`. Implement `CoinSelector::name()` and `CoinSelector::select()` for a custom coin selection strategy, or `TransactionEvaluator::name()` and `TransactionEvaluator::evaluate()` for a custom execution budget source. Network backends implement `Provider`; alternate HTTP clients implement `HttpTransport`. These are ordinary C++ interfaces held by `std::shared_ptr` while the builder uses them.

## Example catalog

All 21 cardano-c examples have C++ counterparts. Network examples use preprod and read `BLOCKFROST_API_KEY` at runtime; use a dedicated test wallet for examples that submit transactions.

| Area | Runnable examples |
| --- | --- |
| Keys and wallets | [BIP39](examples/bip39_example.cpp), [secure key handler](examples/secure_key_handler_example.cpp) |
| Payments and providers | [Send ADA](examples/send_lovelace_example.cpp), [provider queries](examples/provider_example.cpp), [native script spend](examples/spend_from_native_script_example.cpp), [Plutus spend](examples/spend_from_validator_example.cpp) |
| Assets | [Native mint and burn](examples/mint_burn_native_script_example.cpp), [Plutus mint and burn](examples/mint_burn_plutus_script_example.cpp) |
| Staking | [Key reward account](examples/reward_account_pubkey_example.cpp), [script reward account](examples/reward_account_script_example.cpp) |
| Governance | [Key DRep](examples/drep_pubkey_example.cpp), [script DRep](examples/drep_script_example.cpp), [treasury donation](examples/donate_to_treasury_example.cpp), [treasury proposal](examples/propose_treasury_withdrawal_example.cpp), [key vote](examples/vote_for_proposal_pubkey_example.cpp), [script vote](examples/vote_for_proposal_script_example.cpp) |
| Deferred redeemers | [Self-index](examples/deferred_redeemer_self_index_example.cpp), [input/output pairing](examples/deferred_redeemer_batch_example.cpp), [zero withdrawal](examples/deferred_redeemer_withdraw_zero_example.cpp) |
| Dijkstra | [Signed subtransaction batch](examples/sub_transaction_batch_example.cpp), [Plutus V4 guard](examples/plutus_v4_guarded_sub_transaction_example.cpp) |

## Documentation

The [guide and API reference](https://cometacpp.readthedocs.io/en/latest/) cover values, builders, providers, errors, and the ownership model. Build the docs locally with:

```bash
python3 -m pip install -r doc/requirements.txt
cmake -S doc -B build/docs
cmake --build build/docs --target doc
```

## License

Apache 2.0. See [LICENSE](LICENSE).
