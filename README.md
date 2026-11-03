<div align="center">
  <a href="" target="_blank">
    <img align="center" width="300" src="https://raw.githubusercontent.com/Biglup/cometa.cpp/main/assets/cometa_cpp.png">
  </a>
</div>

<br>

<div align="center">

![License](https://img.shields.io/badge/license-Apache%202.0-blue.svg)
![Post-Integration](https://github.com/Biglup/cometa.cpp/actions/workflows/ci.yml/badge.svg)
[![Documentation Status](https://app.readthedocs.org/projects/cometacpp/badge/?version=latest)](https://cometacpp.readthedocs.io/en/latest/?badge=latest)
[![Twitter Follow](https://img.shields.io/twitter/follow/BiglupLabs?style=social)](https://x.com/BiglupLabs)

</div>

<hr>

- [Official Website](https://cometa.dev/)
- [Installation](#installation)
- [Documentation](https://cometacpp.readthedocs.io/en/latest/)

<hr>

Cometa.cpp is a C++17 Cardano library built on cardano-c. Values use ordinary
copy and move semantics, and their native resources are released automatically.
It includes transactions, scripts, certificates, governance values, transaction
builders, signing, and a Blockfrost provider. Release ZIPs provide static and
shared libraries for each supported platform.

```cpp
#include <Cometa/Cometa.h>
#include <Cometa/Providers/DefaultHttpTransport.h>
#include <Cometa/TransactionBuilder/TransactionBuilder.h>

#include <cstdlib>
#include <memory>
#include <stdexcept>

int
main()
{
  const char* projectId  = std::getenv("BLOCKFROST_API_KEY");
  const char* signingKey = std::getenv("PAYMENT_PRIVATE_KEY");

  if (projectId == nullptr || signingKey == nullptr)
  {
    throw std::runtime_error("Set BLOCKFROST_API_KEY and PAYMENT_PRIVATE_KEY");
  }

  Cometa::BlockfrostProvider provider(
    Cometa::NetworkMagic::Preprod,
    projectId,
    std::make_shared<Cometa::DefaultHttpTransport>());
  Cometa::Ed25519PrivateKey key = Cometa::Ed25519PrivateKey::fromNormalHex(signingKey);
  Cometa::Credential        paymentKey(Cometa::CredentialType::KeyHash, key.publicKey().hash());
  Cometa::Address           sender =
    Cometa::EnterpriseAddress(Cometa::NetworkId::Testnet, paymentKey).toAddress();

  Cometa::ProtocolParameters parameters = provider.getParameters();
  Cometa::UtxoList           available  = provider.getUnspentOutputs(sender);

  Cometa::TransactionBuilder builder(
    parameters, Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod));
  builder.setUtxos(available)
    .setChangeAddress(sender)
    .sendLovelace(sender, 2000000);

  Cometa::Transaction    transaction = builder.build();
  Cometa::VkeyWitnessSet witnesses;
  witnesses.add(Cometa::VkeyWitness(key.publicKey(), key.sign(transaction.id().toBytes())));
  transaction.applyVkeyWitnesses(witnesses);
  provider.submitTransaction(transaction);
  return 0;
}
```

The example above sends to the signing address; choose the intended recipient
in real use. See [send_lovelace_example.cpp](examples/send_lovelace_example.cpp)
for a complete runnable flow.

## Building

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

Tests require GoogleTest. Use `-DCOMETA_BUILD_TESTS=OFF` to build without it.
The build fetches a pinned cardano-c commit with the deep-clone APIs; a local
checkout can be selected with `-DCOMETA_CARDANO_SOURCE_DIR=/path/to/cardano-c`.
The Blockfrost HTTP transport uses WinHTTP on Windows and libcurl on Linux and
macOS. Set `-DCOMETA_BUILD_CURL_TRANSPORT=OFF` or
`-DCOMETA_BUILD_WINHTTP_TRANSPORT=OFF` when supplying your own `HttpTransport`.
Set `-DCOMETA_BUILD_SHARED=OFF` when only static libraries are needed.

## Installation

```sh
cmake --install build --prefix /path/to/cometa
```

Set `CMAKE_PREFIX_PATH` to that prefix and link the desired CMake target:

```cmake
FIND_PACKAGE (Cometa CONFIG REQUIRED COMPONENTS http)
TARGET_LINK_LIBRARIES (my_application PRIVATE Cometa::http_shared)
```

| Target | Link mode | Network transport |
| --- | --- | --- |
| `Cometa::cometa` | Static | Your own, or none |
| `Cometa::cometa_shared` | Shared | Your own, or none |
| `Cometa::http` | Static Cometa | WinHTTP on Windows; libcurl elsewhere |
| `Cometa::http_shared` | Shared Cometa | WinHTTP on Windows; libcurl elsewhere |

Omit `COMPONENTS http` when using a core target. On Linux and macOS, the HTTP
targets require a libcurl development package at build time and its runtime
library when that installation is shared. Windows uses the operating system's
WinHTTP service. The shared Cometa library contains its own cardano-c code; the
static target links the included cardano-c archive.

On Windows, copy the Cometa DLLs beside your executable. CMake can copy its
runtime DLL dependencies after building:

```cmake
IF (WIN32)
  ADD_CUSTOM_COMMAND (TARGET my_application POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_RUNTIME_DLLS:my_application>
      $<TARGET_FILE_DIR:my_application>
    COMMAND_EXPAND_LISTS
  )
ENDIF ()
```

For installed Linux and macOS applications, configure the application's
runtime search path to the directory containing the Cometa shared libraries.

Release workflows produce platform-specific ZIP packages with headers, static
and shared libraries, licenses, and CMake configuration. They are validated on
Linux x64/ARM64, macOS x64/ARM64, and Windows x64. No C++ binary releases exist
yet.

## Examples

The `examples/` directory ports all 21 cardano-c examples. It covers payments,
staking, minting, governance, deferred redeemers, and subtransactions. Network
examples use preprod and require `BLOCKFROST_API_KEY` at runtime. They use the
sample signing key from cardano-c, so use a dedicated test wallet. The Plutus V4
guarded subtransaction example builds transactions offline from the funding UTxO,
signing key, and cost model supplied to it. See each source file for its inputs.

## Documentation

The Doxygen/Breathe/Sphinx build follows cardano-c's documentation recipe:

```sh
python3 -m pip install -r doc/requirements.txt
cmake -S doc -B build/docs
cmake --build build/docs --target doc
```

Open `build/docs/html/index.html`. Read the Docs configuration is included for
publishing the same documentation at [cometacpp.readthedocs.io](https://cometacpp.readthedocs.io/).

## License

Apache 2.0. See [LICENSE](LICENSE).
