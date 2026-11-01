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

Cometa.cpp is a C++17 library backed by statically linked cardano-c. Its value types
use ordinary copy/move semantics and manage native memory automatically.

This initial implementation provides `Blake2bHash`, `Credential`, and `AssetName`
for API review. Transaction builders and providers are not implemented yet.

```cpp
#include <cometa/Cometa.h>

using namespace Cometa;
auto hash = Blake2bHash::fromHex(std::string(56, '0'));
Credential key(CredentialType::KeyHash, hash);
auto script = key;
script.setType(CredentialType::ScriptHash);
```

## Building

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

Tests require GoogleTest. Use `-DCOMETA_BUILD_TESTS=OFF` to build without it.
The build fetches a pinned cardano-c commit with the deep-clone APIs; a local
checkout can be selected with `-DCOMETA_CARDANO_SOURCE_DIR=/path/to/cardano-c`.

## Installation

```sh
cmake --install build --prefix /path/to/cometa
```

Set `CMAKE_PREFIX_PATH` to that prefix and link the exported CMake target:

```cmake
FIND_PACKAGE (Cometa CONFIG REQUIRED)
TARGET_LINK_LIBRARIES (my_application PRIVATE Cometa::cometa)
```

Release workflows produce platform-specific ZIP packages with headers, static
libraries, licenses, and CMake configuration. No C++ binary releases exist yet.

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
