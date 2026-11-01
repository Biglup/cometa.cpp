Design and next steps
=====================

Value semantics
---------------

Classes mirror native domain concepts using C++ names and standard-library types.
Copies are independent values; changing one does not change another.
No public native pointers, reference counts, disposal methods, or handle wrappers
are needed. Builders and provider sessions will have separate noncopyable lifetimes
where copying does not have meaningful semantics.

The initial types deliberately exercise different cases: a cryptographic value,
a binary asset name, and a mutable credential containing another native value.
Thread synchronization remains the caller's responsibility when mutating an object.

Static distribution
-------------------

Each ZIP contains C++ headers, the Cometa archive, the native cardano-c archive,
licenses, a README, and a relocatable CMake package. Keeping the archives separate preserves
normal linker extraction and avoids platform-specific archive-merging tools. The
exported target supplies both archives automatically.

The initial CI and release matrix covers Linux x64/ARM64, macOS x64/ARM64, and
Windows x64 with MSVC. Additional compiler and 32-bit ARM packages can be added
after the initial API review; they are not advertised as tested in this slice.

The dependency revision is pinned in ``cmake/Cardano.cmake``. Replace that pin with
the corresponding release commit once the native deep-clone API is released.
Local source overrides are intended for development and do not change the pin.

Blockfrost transport
--------------------

The provider is not implemented in this initial API review. The planned transport
is libcurl on Linux, macOS, and Windows. There is no need for a separate WinHTTP
implementation: curl supports Schannel on Windows.

Build curl as a static library and keep HTTP dependencies in a separate provider
target. On Windows, Schannel uses the operating system TLS implementation and trust
store; those system libraries remain dynamic. On Linux and macOS, choose and pin
the TLS backend and include its static dependencies and license notices in provider
packages. Certificate verification and hostname verification must remain enabled.
A bundled TLS library still needs an appropriate CA trust source.

The transport should expose timeouts, cancellation, HTTP status, and bounded error
bodies, keep API keys out of diagnostics, and support injection for offline tests.
Blockfrost operations should implement the language-level provider interface before
adapting it to the native callback API. Callbacks must catch C++ exceptions and
preserve ownership until all in-flight native calls finish.

See `curl's build documentation <https://curl.se/docs/install.html>`_ for static
linking and TLS backend options.
