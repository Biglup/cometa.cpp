Design
======

Values and ownership
--------------------

Public classes mirror Cardano concepts with C++17 names and standard-library
values. Copying a value creates an independent copy. Moving transfers the value.
Destructors release native resources; applications do not handle reference counts
or call disposal functions. Builders have move-only lifetimes because a partially
built transaction has no useful copy operation.

Getters return independent values. A returned transaction body, script, or
witness remains usable after its parent is destroyed. Mutating an object from
multiple threads requires application-level synchronization.

Builders and services
---------------------

``TransactionBuilder`` balances top-level transactions and
``SubTransactionBuilder`` builds Dijkstra subtransactions. They accept C++
``CoinSelector`` and ``TransactionEvaluator`` implementations through shared
ownership. The default evaluator uses cardano-c's native Plutus evaluator.
``ProviderTransactionEvaluator`` uses an injected network provider.

Deferred redeemer callbacks receive a C++ transaction draft and resolved UTxOs.
The builder retains the callback until its work is complete. Exceptions from
callbacks are preserved and rethrown by the outer C++ operation. The current
builder API is synchronous; there is no cancellation operation.

Network access
--------------

``BlockfrostProvider`` implements the ``Provider`` interface. It accepts an
``HttpTransport``, so applications can use the optional ``CurlHttpTransport`` or
another HTTP client. The curl transport accepts HTTPS only, verifies certificates
and hostnames, limits response size, and applies connection and request timeouts.
The project key is supplied at runtime.

Distribution
------------

Each platform ZIP contains the C++ headers, static and shared Cometa libraries,
the static cardano-c archive, licenses, and a relocatable CMake package. The
shared Cometa library includes cardano-c. The optional curl transport is
available in both link modes. It uses the platform's libcurl installation;
libcurl and its transitive dependencies are not bundled.

The CI and release matrix covers Linux x64/ARM64, macOS x64/ARM64, and Windows
x64 with MSVC. The cardano-c source revision is pinned in
``cmake/Cardano.cmake``. A local source checkout can be selected for development
without changing that pin.
