:orphan:

Implementation record
=====================

The planned cardano-c binding slices are implemented. The coverage ledger in
:doc:`coverage` accounts for every exported declaration in the pinned native
revision; no entries remain planned. This page records the implementation
sequence and its acceptance criteria, rather than an outstanding PR backlog.

Ordinary C++ operations manage ownership automatically. Explicit native
interoperability is documented in :doc:`design`.

Implementation sequence
-----------------------

The following slices were delivered through reviewed implementation PRs. Later
completion work added the remaining transaction utilities, governance builder
operations, and native interoperability.

.. list-table:: Implementation slices
   :header-rows: 1
   :widths: 12 55 33

   * - PR
     - Scope
     - Prerequisites
   * - P01a.1
     - Dependency pin and coverage ledger; enums; ExUnits; ProtocolVersion; checked slot/time conversion
     - merged foundation
   * - P01a.2
     - BigInt; UnitInterval; Anchor; GovernanceActionId
     - P01a.1
   * - P01a.3
     - Base58 and Bech32; native encoding boundary fixes where needed
     - P01a.1
   * - P01b
     - CBOR and JSON readers/writers and enums
     - P01a.2 P01a.3
   * - P02.1
     - Cryptographic keys/signatures and BIP32
     - P01a.2 P01a.3
   * - P02.2
     - BIP39; PBKDF2; EMIP3; CRC32; hash size utilities
     - P02.1
   * - P03
     - Shelley and Byron addresses; attributes; stake pointers; address enums
     - P01a.2 P01a.3 P02
   * - P04.1
     - Asset IDs and standard vector aliases for asset IDs, names, and policies
     - P01a.2 P02
   * - P04.2
     - Asset maps; multi-assets; Value; checked arithmetic and serialization
     - P04.1
   * - P04.3
     - Serialized hash and credential sets
     - P02 P04.1
   * - P05
     - Plutus data; constructors/lists/maps; Datum
     - P01a.2 P01a.3 P01b
   * - P06.1
     - Signature requirements; timelocks; required guards
     - P02 P05
   * - P06.2
     - Native script composites; list; union; discriminator
     - P06.1
   * - P06.3
     - Plutus V1-V4 scripts; language versions; parameter application
     - P05
   * - P06.4
     - Script union and language discriminator
     - P06.2 P06.3
   * - P07
     - Transaction inputs/outputs; UTxOs; withdrawals; address/output collections
     - P03 P04 P05 P06.4
   * - P08
     - Metadata and auxiliary data; Plutus script lists
     - P05 P06.4
   * - P09
     - Pool parameters; relays; IPv4/IPv6; metadata; BLS key representation
     - P02 P03 P04
   * - P10
     - Stake/pool/legacy certificates and certificate containers
     - P03 P04 P09
   * - P11
     - DRep and committee certificates; vote delegation combinations; remaining certificate variants
     - P01a.2 P01a.3 P10
   * - P12
     - Protocol parameters/updates; cost models; prices and voting thresholds
     - P01a.2 P01a.3 P04
   * - P13
     - Governance proposals/actions; committee and constitution
     - P03 P04 P11 P12
   * - P14
     - Voters; votes; voting procedures and collections
     - P01a.2 P01a.3 P11
   * - P15
     - Witness sets; vkey/bootstrap witnesses; redeemers; script/data sets
     - P02 P05 P06.4
   * - P16
     - Subtransaction body; account balance intervals; deposits; required guards
     - P07 P11 P13 P14 P15
   * - P17
     - Transaction body; transaction; subtransaction; subtransaction set
     - P08 P16
   * - P18
     - Derivation paths; software key handler; extensible signing interface; CIP-8
     - P02 P03 P17
   * - P19
     - Provider C++ interface and private C adapter; deterministic test provider
     - P07 P12 P17
   * - P20
     - CoinSelector interface; requests/results; large-first and random-improve strategies
     - P07 P12 P19
   * - P21
     - TransactionEvaluator interface; native and provider-backed evaluators
     - P12 P15 P17 P19
   * - P22
     - TransactionBuilder; balancing; fees; script-data hashes; implicit coin
     - P18 P20 P21
   * - P23
     - SubTransactionBuilder; batch/deferred-redeemer workflows; Dijkstra builder completion
     - P22
   * - P24
     - Optional static libcurl transport target and platform packaging
     - P19
   * - P25
     - BlockfrostProvider implementation and end-to-end validation
     - P23 P24

Encoding validation
-------------------

P01a.3 pins the native Base58 fix for short inputs, embedded zero digits, and
leading zero bytes. Tests include known text/byte vectors, every single-byte
value, exact decoded lengths, malformed characters, and buffer boundaries.
Bech32 covers checksum validation, empty and long payloads, prefix bounds,
case handling, and allocation-failure cleanup.

Extensible services
-------------------

Coin selection, transaction evaluation, providers, signing, and deferred redeemers
use named C++ interfaces with virtual destructors. Builders retain injected
services through shared ownership. The callback bridge remains private; user code
receives C++ objects and returns owned C++ values. Catch exceptions before crossing
the C ABI and preserve their original cause for the outer operation.

Coin selection receives available and preselected UTxOs, the authoritative target,
protocol parameters, a change address, and optional output-shape hints. Its result
contains selected UTxOs, remaining UTxOs, and explicit change outputs. Test value
conservation and minimum ADA; fee balancing belongs to the builder.

Transaction evaluation defaults to the built-in evaluator. An injected evaluator
can replace it, including an explicitly selected provider-backed evaluator.
Evaluation receives a read-only transaction snapshot and resolved inputs and
returns redeemers with execution budgets. Ordinary transaction values remain
mutable; callback snapshots must not share mutable state with an active builder.

Use synchronous C++17 service methods first. Do not promise asynchronous or
concurrent use of a mutable builder without a separate execution and lifetime
contract. Deferred redeemers must be deterministic across balancing iterations.

Acceptance criteria
-------------------

Every slice includes per-type tests, malformed-input and integer-boundary cases,
ownership and failure-path tests, documentation, and coverage updates. Preserve
clang-format, cppcheck, platform builds, package-consumer checks, and Valgrind CI.
Port relevant native unit tests and compare known encodings, not only round trips.

The C++ conformance adapter covers all 229 cases in the pinned shared corpus:
serialization, invalid inputs, primitives, transactions, builders, and lifecycle
scenarios. The corpus is pinned alongside the native dependency. Expected values
come from cardano-c, not from the binding under test.

Blockfrost and the platform HTTP transports are implemented with
verified TLS, bounded responses, timeouts, and offline transport injection. Tests
cover provider operations, pagination, HTTP failures, and exact amounts.
The opt-in live test must fetch UTxOs and parameters, build, sign, submit, and
confirm a transaction. Test custom coin selectors and evaluators as well as the
default native evaluator. Exercise Dijkstra submissions only on networks that
support them; local conformance is not evidence of on-chain acceptance.

Static transport dependencies must be validated on the supported Linux, macOS,
and Windows targets. Platform TLS libraries may remain system dependencies.
Additional architectures require explicit build and package-consumer validation
before they are advertised as supported.
