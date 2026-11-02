# C++ coding conventions

These conventions apply to Cometa.cpp source, public headers, tests,
CMake files, and API documentation. Use the checked-in `.clang-format` with
**clang-format 15**. The language baseline is **C++17**; retain modern C++17
features rather than introducing older-language workarounds.

## Before editing

Read this guide, then inspect neighboring files before adding or changing code.
Open a comparable header, its implementation, and its tests in the same domain.
Match their naming, include groups, section banners, access groups, spacing, and
Doxygen comments. For CMake or CI changes, inspect the adjacent build definitions
and existing workflows too. Do not introduce a new pattern just because it is
common in another C++ project.

Useful starting points are
[Credential.h](lib/include/Cometa/Common/Credential.h),
[Credential.cpp](lib/src/Common/Credential.cpp), and
[CredentialTest.cpp](lib/tests/Common/CredentialTest.cpp).
For shared cleanup code, read [CardanoInterop.h](lib/src/CardanoInterop.h).
These examples complement this guide; the written rules and the user's current
instructions take precedence if an older file differs.

Keep changes focused. Do not add unrequested examples, standalone smoke programs,
helper scripts, or embedded workflow code when an existing test or build target
can cover the change. Update this guide when an agreed convention changes.

## Names and files

- Public namespace: `Cometa`. Implementation helpers remain in private headers and are not installed.
- Give each public type and enum its own header.
- Types and enums: PascalCase, such as `Blake2bHash` and `CredentialType`.
- Functions, methods, and local variables: camelCase.
- Nonstatic data members: `m_` prefix, such as `m_value`.
- Constants: uppercase with underscores, such as `HASH_HEX`.
- Headers: `.h`; implementations: `.cpp`. Use PascalCase basenames matching
  the primary type, such as `Credential.h` and `Credential.cpp`.
- Group domain headers under `Cometa/Assets`, `Cometa/Common`, `Cometa/Crypto`,
  and corresponding domain folders as the library grows. Mirror those folders in
  `lib/src` and `lib/tests`; keep shared error types at the root.
- Test filenames end in `Test.cpp`. Keep public headers in `lib/include/Cometa`,
  implementations and internal headers in `lib/src`, and tests in `lib/tests`.
- Public includes use `<Cometa/Common/Credential.h>`; the umbrella header is
  `<Cometa/Cometa.h>`.

## Expressions and control flow

Use explicit types instead of `auto` in implementations, tests, and documentation
snippets. Use named functions and direct calls instead of lambdas. Do not add a
`Detail` namespace or factory adapters that obscure a straightforward C call.
Avoid generic machinery when a few explicit statements make the operation clearer.

Keep each logical step distinct: validate inputs, obtain resources, check results,
perform the operation, and return or transfer the result. Separate those groups
with blank lines. Keep related declarations together, but split groups when the
formatter would otherwise create large, distracting alignment gaps. Formatting
is not a substitute for choosing readable groups of statements.

## Formatting

Use two spaces, Allman braces, and the checked-in formatter's spacing and
alignment rules. Keep braces around control-flow bodies. Put function definition
return types on their own line. Preserve intentional line breaks in long calls:
`ColumnLimit: 0` does not mean that an entire expression should be one line.
Keep control flow visible and avoid callback adapters around ordinary function calls.

Group includes with blank lines: the corresponding public header first, other
project/internal headers, native or third-party headers, then standard-library
headers. The formatter sorts within each group. Headers must compile on their
own; include every standard header used directly. Never rely on transitive
includes for types such as `std::uint8_t`.

Keep Cometa's Apache license and Biglup Labs copyright in file headers. Borrowing
formatting conventions does not justify copying another project's implementation
or proprietary notices.

## File sections

Use the existing full-width section comments, omitting sections that are empty:

```cpp
/* INCLUDES ******************************************************************/
/* FORWARD DECLARATIONS ******************************************************/
/* DEFINITIONS ***************************************************************/
/* CLASS DECLARATION *********************************************************/
/* PROTOTYPES ****************************************************************/
/* INTERNAL IMPLEMENTATION ***************************************************/
/* CLASS IMPLEMENTATION ******************************************************/
/* PRIVATE FUNCTIONS *********************************************************/
/* FUNCTIONS IMPLEMENTATION **************************************************/
```

Headers contain includes, optional forward declarations or enum definitions,
class declarations, then free-function prototypes. Source files place anonymous
namespace helpers before class implementations and nonmember functions after
class implementations. Tests separate setup, the operation, and expectations with `// Arrange`, `// Act`,
and `// Assert`; use `// Act & Assert` for exception assertions.
Test files use `USINGS`, `INTERNAL IMPLEMENTATION`, and
`IMPLEMENTATION` sections; place test support before test cases.

## Class organization

Use separate access-label groups in this order, omitting groups that do not apply:

1. Public type aliases and constants.
2. Public constructors, copy/move constructors, and destructor.
3. Public static factories.
4. Public observers, modifiers, serialization, and swap.
5. Public assignment and comparison operators.
6. Private constructors used for internal ownership transfer.
7. Private helper methods.
8. Private data members.

Repeat `public:` and `private:` to separate those groups. Keep blank lines after
access labels and between documented declarations. Keep implementation order close
to declaration order, with private helpers and nonmember functions in their own
sections. Do not invent empty access groups or placeholder members.

## Value semantics and C interoperability

Expose idiomatic C++ types and operations. Values may be mutable; do not impose
immutability merely because they wrap C objects. Builders and other resources
without meaningful copy semantics may be noncopyable.

Ordinary values use RAII and independent copy semantics; moves and swaps transfer
ownership without allocating. Native C references may be owned privately by these
RAII value classes. Temporary native allocations belong in scoped ownership guards.
Do not expose reference counting, disposal, or raw owning pointers in the public API.

Call C constructors directly and put any returned reference into a scoped owner
before checking an error that can throw. Release ownership only when transferring
the successful result into its destination. Preserve cleanup on partial failures
and leave the destination unchanged when copy assignment fails. Shared helpers
should have narrow, descriptive names; keep them in private implementation files.

Use documented `ErrorCode` values and `Error` for operation failures instead of
requiring callers to interpret integers or include C headers. Keep each enum in
its own header. Verify C/C++ enum mappings in unit tests, rather than scattering
mapping `static_assert` declarations through library implementation files.

## Documentation

Follow the structured Doxygen pattern used by the headers:

```cpp
/**
 * @brief Replaces the credential hash.
 *
 * @param hash The 28-byte hash to copy.
 *
 * @throws std::invalid_argument The hash length is not 28 bytes.
 */
void setHash(const Blake2bHash& hash);
```

- File comments include `@file`, `@brief`, author/date, and the project license.
- Document every type, enum variant, constructor, method, function, and member,
  including private declarations and implementation helpers.
- Start with a direct `@brief`. Describe the operation rather than restating its name.
- Document every parameter with `@param`; use `@return` for non-void results.
- Use `@throws` for operation-specific failure conditions. Keep the class summary
  about the domain concept; do not fill it with generic lifetime explanations.
- Document only the declaration belonging to each comment; describe move behavior
  on move operations, not in the class summary.
- Keep public documentation about observable behavior. Do not describe C handles,
  reference counts, native storage, or cloning mechanisms in interface documentation.
- Use `@remark` only for useful behavior guarantees and important limits.
- Distinguish owned return values from borrowed internal references. Describe
  mutation and moved-from behavior accurately.
- Explain units, size limits, encodings, empty-input behavior, and whether input is
  copied. Do not claim validation or exception guarantees that the code does not provide.
- Keep implementation contracts in private/internal comments. These are checked
  by `check_docs` but excluded from the published API reference.
- Keep documentation concise and specific. The [Microsoft writing style guide](https://learn.microsoft.com/en-us/style-guide/)
  is a useful reference for clear engineering documentation.

Keep user guides in `doc/src`. Public API comments feed Doxygen, Breathe, and
Sphinx; update documentation snippets and API mappings when the interface changes.
Public documentation describes how to use the library. Private comments document
implementation contracts without exposing them in the published reference.

## CMake

Use uppercase command names and a space before `(`, including user-defined CMake
commands. Use two-space indentation inside control-flow blocks and multiline
argument lists. Write `ELSE ()` and `ENDIF ()` on separate lines.

Group related commands under short comments. Expand source lists, target include
and link settings, install rules, and long dependency declarations across lines.
Align adjacent keyword/value pairs when it improves readability. Preserve target
names, argument case, variable names, and quoted strings: CMake command names are
case-insensitive, but those arguments are not.

Use target-scoped settings, retain `cxx_std_17`, and disable compiler extensions.
A formatting change must not alter build options or dependency behavior.

## Unit tests

Give each domain type its own `TypeNameTest.cpp` in the matching domain folder.
Keep reusable copy/move and allocation-failure scenarios in the shared value-contract
suite. Do not collect unrelated domain behavior into a large catch-all test file.

Use the cardano-c Arrange/Act/Assert style, with blank lines between phases:
`// Arrange`, `// Act`, and `// Assert`. Use `// Act & Assert` when an exception
assertion necessarily performs the operation. Omit phases with no work to do.
Split tests that mix unrelated behaviors into focused cases.

Cover the supported constructors, conversions, comparisons, mutation, and invalid
inputs. Check known encodings and values, not only round trips. For overloads such
as byte-vector and hexadecimal CBOR parsing, test valid inputs and their distinct
invalid-input cases. Test copy independence, failure cleanup, and ownership
transfers where applicable. Run allocation-failure tests under Valgrind.

## Verification

Run `clang-format-15 -i` on changed `.h` and `.cpp` files, then check the diff.
Build and run the existing tests after declaration reordering, renames, or namespace
changes. Build docs with warnings treated as errors after API documentation edits.
The `doc` target also runs `check_docs`, covering public and private library code.
For public header/path changes, also compile a separate installed-package consumer.
CI must retain clang-format and cppcheck checks, public and internal documentation
checks, platform builds, and full-suite Valgrind checks. Missing private comments
must fail `check_docs`; do not disable extraction or warnings to hide omissions.
Valgrind must fail on memory errors and leaks and retain its report as an artifact.
Do not weaken checks or add broad suppressions to make a change pass.

Use the commands in the current CI workflows as the reference for validation.
For documentation-only changes to this guide, check accuracy, links, and the diff;
there is no need to rerun unrelated behavioral tests.

Keep formatting and organization changes separate from behavioral fixes in commits.
Use lowercase Conventional Commit messages, such as `docs(style): clarify code
organization`. Use the configured maintainer identity and signing key for commits;
verify the signature before pushing. Do not silently substitute an agent identity
or create unsigned commits when signing is required.

## Implementation PRs

Stack successive implementation PRs on the preceding branch rather than waiting
for each merge. State the dependency and review order in each PR. Keep each diff
focused on its own slice, and update the stack as its base PRs merge. After opening
or updating a PR, track its CI checks, fix failures, and confirm lint, tests,
documentation, and memory checks pass before starting the next implementation PR.
Do not treat passing local checks as a substitute for CI. Once checks pass,
continue with the next planned slice without ending work to request another
instruction or waiting for a merge. Continue until the agreed PR sequence is
open, unless a blocker requires user input. Keep the
coverage ledger in the repository until the bindings are complete.
