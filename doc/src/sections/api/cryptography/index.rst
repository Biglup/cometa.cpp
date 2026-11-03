Cryptography
============

Cryptographic primitives cover Blake2b hashing, Ed25519 signatures, BIP32 key
derivation, BIP39 mnemonics, and password-based encryption. Use SecureKeyHandler when an
application needs transaction signing without handling a private key for each operation.

.. toctree::
   :maxdepth: 1

   usage
   bip32
   bip32_private_key
   bip32_public_key
   bip39
   blake2b_hash
   blake2b_hash_set
   blake2b_hash_size
   crc32
   ed25519_private_key
   ed25519_public_key
   ed25519_signature
   emip3
   pbkdf2
