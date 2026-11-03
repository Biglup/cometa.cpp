Key Handlers usage
==================

Custom signing handlers
-----------------------

``SecureKeyHandler`` accepts C++ signing operations, including signers backed by
hardware wallets or remote services. Supply transaction and sub-transaction
signing functions and the matching public-key lookup. The handler keeps these
operations available for its lifetime and releases its native resources when
destroyed.

.. code-block:: cpp

   Cometa::Bip32KeyCallbacks callbacks;
   callbacks.signTransaction = signTransaction;
   callbacks.signSubTransaction = signSubTransaction;
   callbacks.accountPublicKey = accountPublicKey;

   const Cometa::SecureKeyHandler signer =
     Cometa::SecureKeyHandler::bip32("hardware-wallet", callbacks);
   const Cometa::DerivationPath paymentPath {
     1852, 1815, 0, Cometa::DerivationRole::External, 0 };
   const Cometa::VkeyWitnessSet witnesses =
     signer.signTransaction(transaction, { paymentPath });

The assigned functions return ``VkeyWitnessSet`` and receive ``Transaction`` or
``SubTransaction`` values. A BIP32 public-key function receives
``AccountDerivationPath`` and returns ``Bip32PublicKey``. Use
``Ed25519KeyCallbacks`` and ``SecureKeyHandler::ed25519`` for a single-key signer.
Exceptions thrown by a callback become ``Cometa::Error`` at the signing call;
``lastError()`` provides the callback's message.

For a local encrypted signer, create a software handler with entropy or an
Ed25519 key and a passphrase provider. The provider returns passphrase bytes
when the signer needs to decrypt the key. The returned handler supports the
same signing operations and can be serialized for encrypted storage.

.. code-block:: cpp

   const Cometa::SecureKeyHandler signer =
     Cometa::SecureKeyHandler::softwareBip32(entropy, passphrase, getPassphrase);
   const Cometa::SecureKeyHandler restored =
     Cometa::SecureKeyHandler::deserializeSoftware(signer.serialize(), getPassphrase);

The passphrase provider must return a nonempty value of at most 128 bytes.
The handler clears its temporary passphrase copy after each operation; callers
remain responsible for their original passphrase storage.

Wallet derivation paths
-----------------------

For wallet keys, ``AccountDerivationPath`` represents the first three CIP-1852 path segments. ``DerivationPath`` adds a typed ``DerivationRole`` and key index. Supply segment values before hardening; software key handlers harden purpose, coin type, and account during derivation.
