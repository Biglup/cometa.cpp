Key Handlers
============

SecureKeyHandler signs transactions and subtransactions using software keys or
application-provided callbacks. BIP32 handlers support derivation paths; Ed25519
handlers use a single key. Software handlers support encrypted storage and request a
passphrase when needed.

.. toctree::
   :maxdepth: 1

   usage
   account_derivation_path
   bip32_key_callbacks
   cip1852
   derivation_path
   derivation_role
   ed25519_key_callbacks
   passphrase_provider
   secure_key_handler
   secure_key_handler_type
