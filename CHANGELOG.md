Next
---------------------
- Add C++17 hash, credential, and asset-name value types with native deep copies and automatic memory management.
- Add static cardano-c builds, relocatable CMake packages, platform ZIP packaging, unit tests, and Valgrind CI.
- Add Doxygen/Breathe/Sphinx documentation and Read the Docs configuration.
- Use the Cometa namespace and PascalCase .h headers; document C++17 coding and API documentation conventions.
- Capitalize the Cometa include directory, separate enum headers, and expose documented ErrorCode values.
- Accept hexadecimal CBOR through string-view overloads and add per-type unit test suites.
- Report empty CBOR as an invalid argument instead of an allocation failure.
- Simplify C interoperability with direct calls, remove the example, and enforce formatting and static analysis in CI.
