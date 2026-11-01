# Agent instructions

Read [CODING_CONVENTIONS.md](CODING_CONVENTIONS.md) before editing code or documentation.
It defines the project's C++17 baseline, `Cometa` namespace, `.h` headers, naming,
section banners, access-group organization, Doxygen patterns, and CMake style.

- Use the checked-in `.clang-format` with clang-format 15.
- Keep native memory management private and automatic; preserve value semantics.
- Do not downgrade language features to match older reference projects.
- Follow existing build, test, documentation, and package-consumer checks.
- Keep style-only changes separate from behavioral changes.
