2.0.0-rc.1
---------------------

First release candidate of Cometa.cpp, a C++17 library for Cardano.

- Add Cardano domain values, cryptography, addresses, scripts, metadata, certificates, governance, and transaction inspection.
- Add transaction and subtransaction builders, balancing utilities, deferred redeemers, Dijkstra guards, and direct account deposits.
- Add native Plutus evaluation and script parameter application, with injectable coin selectors, evaluators, providers, and signing callbacks.
- Add Blockfrost access and platform HTTP transports with TLS verification, timeouts, and bounded responses.
- Provide independent value copies, automatic resource management, standard collection iterators, and implicit conversions between related domain types.
- Expose explicit native borrowing and ownership transfer through `nativeHandle()`, `adopt()`, and `release()` where applicable.
- Package static and shared libraries, matching C headers, and relocatable CMake targets for Linux and macOS x64/ARM64, and Windows x64.
- Publish API documentation and usage guides, port the cardano-c examples, and validate the binding with shared conformance fixtures, allocation-failure tests, and Valgrind.
