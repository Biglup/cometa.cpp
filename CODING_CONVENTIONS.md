# C++ coding conventions

These conventions apply to Cometa.cpp source, public headers, tests, examples,
CMake files, and API documentation. Use the checked-in `.clang-format` with
**clang-format 15**. The language baseline is **C++17**; retain modern C++17
features rather than introducing older-language workarounds.

## Names and files

- Public namespace: `Cometa`. Internal shared helpers: `Cometa::Detail`.
- Types and enums: PascalCase, such as `Blake2bHash` and `CredentialType`.
- Functions, methods, and local variables: camelCase.
- Nonstatic data members: `m_` prefix, such as `m_value`.
- Constants: uppercase with underscores, such as `HASH_HEX`.
- Headers: `.h`; implementations: `.cpp`. Use PascalCase basenames matching
  the primary type, such as `Credential.h` and `Credential.cpp`.
- Test filenames end in `Test.cpp`. Keep public headers in `lib/include/cometa`,
  implementations and internal headers in `lib/src`, and tests in `lib/tests`.
- Public includes use `<cometa/Credential.h>`; the umbrella header is
  `<cometa/Cometa.h>`.

## Formatting

Use two spaces, Allman braces, and the checked-in formatter's spacing and
alignment rules. Keep braces around control-flow bodies. Put function definition
return types on their own line. Preserve intentional line breaks in long calls:
`ColumnLimit: 0` does not mean that an entire expression should be one line.
For nested factory calls and lambdas, put arguments and lambda bodies on separate
lines so indentation reflects the expression structure.

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
class implementations. Test files use `USINGS`, `INTERNAL IMPLEMENTATION`, and
`IMPLEMENTATION` sections; place test support before test cases.

## Class organization

Use separate access-label groups in this order, omitting groups that do not apply:

1. Public nested types and constants.
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

Ordinary values use RAII and independent copy semantics; moves and swaps transfer
ownership without allocating. Native C references may be owned privately by these
RAII value classes. Temporary native allocations belong in scoped ownership guards.
Do not expose reference counting, disposal, or raw owning pointers in the public API.

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
- Document each public type, enum variant, constructor, method, and free function.
- Start with a direct `@brief`. Describe the operation rather than restating its name.
- Document every parameter with `@param`; use `@return` for non-void results.
- Use `@throws` for operation-specific failure conditions. Class-level text may
  explain shared native-error and allocation-error behavior without repeating it
  verbatim on every member.
- Use `@remark` for copy/move guarantees, ownership, lifetime, and important limits.
- Distinguish owned return values from borrowed internal references. Describe
  mutation and moved-from behavior accurately.
- Explain units, size limits, encodings, empty-input behavior, and whether input is
  copied. Do not claim validation or exception guarantees that the code does not provide.
- Document nontrivial internal helpers, especially ownership transfers and cleanup.
- Keep documentation concise and specific. The [Microsoft writing style guide](https://learn.microsoft.com/en-us/style-guide/)
  is a useful reference for clear engineering documentation.

Keep user guides in `doc/src`. Public API comments feed Doxygen, Breathe, and
Sphinx; update examples and native API mappings when the interface changes.

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

## Verification

Run `clang-format-15 -i` on changed `.h` and `.cpp` files, then check the diff.
Build and run the existing tests after declaration reordering, renames, or namespace
changes. Build docs with warnings treated as errors after API documentation edits.
For public header/path changes, also compile a separate installed-package consumer.
Keep formatting and organization changes separate from behavioral fixes in commits.
