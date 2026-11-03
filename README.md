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

Create a transaction with network parameters and UTxOs from a provider. The builder selects inputs, balances outputs, and calculates the fee:

```cpp
Cometa::TransactionBuilder builder(
  parameters,
  Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod));

builder.setUtxos(utxos)
  .setChangeAddress(sender)
  .sendLovelace(recipient, 2'000'000)
  .setInvalidAfter(expirySlot);

Cometa::Transaction transaction = builder.build();
transaction.applyVkeyWitnesses(signer.signTransaction(transaction));
provider.submitTransaction(transaction);
```

Here, `parameters` and `utxos` come from the provider, `sender` and `recipient` are `Cometa::Address` values, `expirySlot` is a network slot, and `signer` is a `Cometa::SecureKeyHandler`. The [complete payment example](examples/send_lovelace_example.cpp) shows setup, signing, and submission on preprod.

## Install

Download the ZIP for your operating system and architecture from [GitHub Releases](https://github.com/Biglup/cometa.cpp/releases), then extract it. Packages are built for Linux x64/ARM64, macOS x64/ARM64, and Windows x64. Each includes headers, static and shared libraries, and a CMake package.

Set `CMAKE_PREFIX_PATH` to the extracted package directory. In your application's `CMakeLists.txt`:

```cmake
FIND_PACKAGE (Cometa CONFIG REQUIRED COMPONENTS http)
TARGET_LINK_LIBRARIES (my_application PRIVATE Cometa::http_shared)
```

Choose the target that matches your link mode:

| Target | Link mode | Includes HTTP transport |
| --- | --- | --- |
| `Cometa::cometa` | Static | No |
| `Cometa::cometa_shared` | Shared | No |
| `Cometa::http` | Static | Yes |
| `Cometa::http_shared` | Shared | Yes |

The HTTP targets use WinHTTP on Windows and libcurl on Linux and macOS. Unix consumers need libcurl development files when using those targets. The core targets have no HTTP dependency. For shared linking, place the Cometa DLLs beside your Windows executable or configure the runtime library path on Linux and macOS. See the [installation guide](https://cometacpp.readthedocs.io/en/latest/getting_started.html) for details.

### Blockfrost

Set `BLOCKFROST_API_KEY` in your environment and pass the platform transport to the provider:

```cpp
const char* projectId = std::getenv("BLOCKFROST_API_KEY");

if (projectId == nullptr)
{
  throw std::runtime_error("Set BLOCKFROST_API_KEY");
}

Cometa::BlockfrostProvider provider(
  Cometa::NetworkMagic::Preprod,
  projectId,
  std::make_shared<Cometa::DefaultHttpTransport>());

Cometa::ProtocolParameters parameters = provider.getParameters();
```

This snippet needs `<Cometa/Cometa.h>`, `<Cometa/Providers/DefaultHttpTransport.h>`, `<cstdlib>`, `<memory>`, and `<stdexcept>`. You can also implement `Cometa::HttpTransport` to use another HTTP stack.

## What you can build

**Conway:** payments, native assets, Plutus scripts, staking, DRep registration, proposals, and voting. The examples include [native minting](examples/mint_burn_native_script_example.cpp), [script spending](examples/spend_from_validator_example.cpp), and [governance voting](examples/vote_for_proposal_pubkey_example.cpp).

**Dijkstra:** Plutus V4, guards, direct deposits, account balance constraints, and independently signed subtransactions. The [subtransaction batch example](examples/sub_transaction_batch_example.cpp) builds two signed intents and settles them in one transaction. The [Plutus V4 guard example](examples/plutus_v4_guarded_sub_transaction_example.cpp) builds a guarded spend offline. These features require protocol parameters from a network that supports them.

**Deferred redeemers:** calculate redeemer data after the builder has balanced the transaction and fixed input and output ordering. See the [self-index](examples/deferred_redeemer_self_index_example.cpp), [input/output pairing](examples/deferred_redeemer_batch_example.cpp), and [zero-withdrawal](examples/deferred_redeemer_withdraw_zero_example.cpp) examples.

The [examples directory](examples/) contains C++ counterparts of all 21 cardano-c examples. Network examples use preprod and read `BLOCKFROST_API_KEY` at runtime. Use a dedicated test wallet when running examples that submit transactions.

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
