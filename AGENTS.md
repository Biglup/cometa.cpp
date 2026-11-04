# Agent instructions

Read [CODING_CONVENTIONS.md](CODING_CONVENTIONS.md) before editing code or documentation.
Before editing, inspect comparable neighboring headers, implementations, and tests
and match their existing patterns. For build or CI changes, read nearby build files
and workflows. The guide and current user instructions take precedence over older
examples.

It defines the project's C++17 baseline, `Cometa` namespace, `.h` headers, naming,
section banners, access-group organization, Doxygen patterns, and CMake style.

- Prefer explicit types and direct calls; separate logical steps with blank lines.
- Keep domain types, enums, sources, and tests in their matching capitalized folders.
- Document private members and internal helpers as well as public declarations.
- Keep API comments focused on their declaration and free of implementation details.
- Use the checked-in `.clang-format` with clang-format 15.
- Keep ordinary memory management automatic and preserve value semantics. Use the documented native interoperability methods for explicit C access.
- Do not downgrade language features to match older reference projects.
- Follow existing build, test, documentation, and package-consumer checks.
- Keep style-only changes separate from behavioral changes.

When feedback establishes a new convention, update the guide so future changes follow it.
