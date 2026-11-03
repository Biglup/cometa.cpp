Cryptography usage
==================

Signing and deriving keys
-------------------------

Use ``Ed25519PrivateKey`` for a 32-byte seed or a 64-byte extended signing key.
Signatures and public keys are ordinary copyable values.

.. code-block:: cpp

   const Cometa::Ed25519PrivateKey key = Cometa::Ed25519PrivateKey::fromNormalHex(seedHex);
   const Cometa::Ed25519Signature signature = key.sign(message);
   const bool valid = key.publicKey().verify(signature, message);

``Bip32PrivateKey`` supports hardened and non-hardened derivation. Public keys
support only non-hardened derivation. An empty path returns a copy of the key.

.. code-block:: cpp

   const Cometa::Bip32PrivateKey root = Cometa::Bip32PrivateKey::fromBip39Entropy(entropy);
   const Cometa::Bip32PrivateKey account = root.derive({
     Cometa::hardenIndex(1852), Cometa::hardenIndex(1815), Cometa::hardenIndex(0) });
   const Cometa::Ed25519PrivateKey paymentKey = account.derive({ 0, 0 }).toEd25519();
   const Cometa::Blake2bHash credentialHash = paymentKey.publicKey().hash();

``Bip32PublicKey::hash`` hashes the extended key including its chain code.
For a credential hash, convert to an Ed25519 public key before hashing.
The entropy password is a byte string; normalize user-entered text before passing
it if the application requires Unicode normalization.

Private-key objects erase their owned key material when destroyed. Exported
``std::string`` and ``std::vector`` values are ordinary containers and do not erase
their buffers automatically. Avoid exporting or logging secret material unless
it is needed.

Mnemonic and password utilities
-------------------------------

The BIP39 helpers use the English word list and validate the checksum when
converting words back to entropy.

.. code-block:: cpp

   const std::vector<std::string> words = Cometa::entropyToMnemonic(entropy);
   const std::vector<std::uint8_t> recovered = Cometa::mnemonicToEntropy(words);
   const Cometa::Bip32PrivateKey root = Cometa::Bip32PrivateKey::fromBip39Entropy(recovered);

EMIP-003 encryption includes a random salt, nonce, and authentication tag in its
output. Decryption reports an error for a wrong password or altered ciphertext.

.. code-block:: cpp

   const std::vector<std::uint8_t> encrypted = Cometa::encryptEmip3(payload, password);
   const std::vector<std::uint8_t> decrypted = Cometa::decryptEmip3(encrypted, password);

``pbkdf2HmacSha512`` exposes explicit salt, iteration count, and output size.
``crc32`` computes a checksum. Returned secret bytes and mnemonic words use
ordinary standard containers; their memory is not automatically erased.
