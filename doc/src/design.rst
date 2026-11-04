Design
======

Values and ownership
--------------------

Public classes mirror Cardano concepts with C++17 names and standard-library
values. Copying a value creates an independent copy. Moving transfers the value.
Destructors release native resources; applications do not handle reference counts
or call disposal functions. Builders have move-only lifetimes because a partially
built transaction has no useful copy operation.

Domain getters return independent values. A returned transaction body, script, or
witness remains usable after its parent is destroyed. Collection accessors and
iterators can return references; see :doc:`collections`. Mutating an object from
multiple threads requires application-level synchronization.

C interoperability
------------------

Owning value wrappers expose three operations for integrating existing cardano-c
code:

* ``nativeHandle()`` borrows a pointer. The const overload returns a pointer to
  const. Do not unref a borrowed pointer or retain it beyond the next operation on
  its C++ owner.
* ``release()`` transfers the owned reference and leaves the wrapper empty. The
  caller must adopt it into another wrapper or call the matching C unref function.
* ``Type::adopt(pointer)`` takes ownership without copying or incrementing the
  reference count. It rejects null with ``std::invalid_argument``.

``JsonWriter::adopt(pointer, format)`` additionally requires the format used to
create the writer, so ``reset()`` preserves that choice.

Borrowing or releasing an empty wrapper returns ``nullptr``. Empty wrappers can
be assigned, swapped, or destroyed; ordinary value operations reject them.

.. code-block:: cpp

   #include <Cometa/Assets/AssetName.h>
   #include <cardano/assets/asset_name.h>

   Cometa::AssetName name = Cometa::AssetName::fromString("Cometa");

   cardano_asset_name_t* borrowed = name.nativeHandle();
   const std::size_t length = cardano_asset_name_get_bytes_size(borrowed);

   Cometa::AssetName transferred = Cometa::AssetName::adopt(name.release());

Collections synchronize edits through C++ element references before exposing a
native pointer. Synchronization can allocate and throw; on failure, ownership and
pending edits remain with the collection. A mutable borrow invalidates all C++
iterators and element references. Finish native access before using C++ collection
operations again. Do not retain native pointers across later C++ operations or
mutate through a pointer obtained from a const wrapper.

``TransactionBuilder``, ``SubTransactionBuilder``, and ``SecureKeyHandler`` expose
borrowed handles only: their C++ callback state cannot be transferred by a raw
pointer. Keep these wrappers alive during native calls. Use C++ signing methods
when a software signer needs its C++ passphrase provider; direct C calls do not
activate that provider.

Use the matching cardano-c headers and library shipped with Cometa. Do not mix
objects from different cardano-c builds or change global allocators while native
objects are alive. Native mutations must respect cardano-c's invariants and any
required parent CBOR-cache invalidation.

Concrete and general values
---------------------------

Functions taking a general domain value accept its concrete forms directly:

.. code-block:: cpp

   Cometa::Blake2bHash
   scriptHash(const Cometa::Script& script)
   {
     return script.hash();
   }

   const Cometa::ScriptInvalidBefore requirement(1000);
   const Cometa::Blake2bHash hash = scriptHash(requirement);

Concrete native-script forms convert to both ``NativeScript`` and ``Script``.
Plutus script versions convert to ``Script``. The same convention applies to
concrete addresses, certificates, MIR certificates, relays, Plutus lists/maps/
constructors, and metadata lists/maps. Individual MIR forms also convert directly
to ``Certificate``.

Conversions work in assignments and collection insertion:

.. code-block:: cpp

   const Cometa::NativeScriptList requirements {
     Cometa::ScriptInvalidBefore(1000),
     Cometa::ScriptInvalidAfter(2000)
   };

   const Cometa::Script policy = Cometa::ScriptAll(requirements);

Converting an lvalue creates an independent copy and can allocate. Move overloads
are provided for direct alternatives; passing ``std::move(value)`` to one of these
overloads transfers the value after successful construction. Address conversions
and direct native-script/MIR leaf-to-root conversions accept rvalues through their
copying overloads. Converting a value does not make extraction implicit: use the
appropriate ``asX()`` accessor to obtain a specific form.

Strings, byte vectors, and numbers still require explicit construction or parsing.
Choosing a datum representation or a DRep role also remains explicit. A governance
action requires a proposal's deposit, return address, and anchor before it can form
a ``ProposalProcedure``.

If an application overloads the same function for both ``NativeScript`` and
``Script``, a native-script leaf can convert to either parameter. Select the desired
overload explicitly:

.. code-block:: cpp

   void inspect(const Cometa::NativeScript& script);
   void inspect(const Cometa::Script& script);

   inspect(Cometa::NativeScript(requirement));
   inspect(Cometa::Script(requirement));

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
``HttpTransport``, so applications can use the supplied platform transport or
another HTTP client. The supplied transport uses WinHTTP on Windows and
libcurl on Linux and macOS. It accepts HTTPS only, verifies certificates and
hostnames, limits response size, and applies connection and request timeouts.
The project key is supplied at runtime.

Distribution
------------

Each platform ZIP contains the C++ and matching C headers, static and shared Cometa libraries,
the static cardano-c archive, licenses, and a relocatable CMake package. The
shared Cometa library includes cardano-c. The HTTP transport is available in
both link modes. Windows uses the system WinHTTP service. Unix packages use
the platform's libcurl installation; libcurl and its transitive dependencies
are not bundled.

The CI and release matrix covers Linux x64/ARM64, macOS x64/ARM64, and Windows
x64 with MSVC. The cardano-c source revision is pinned in
``cmake/Cardano.cmake``. A local source checkout can be selected for development
without changing that pin.
