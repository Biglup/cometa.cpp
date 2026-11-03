Message Signing
===============

CIP-8 message signing produces COSE_Sign1 and COSE_Key encodings for wallet-compatible
data signatures. Bind the signed payload to an address or key hash using signCip8. This
operation is distinct from signing a transaction body.

.. toctree::
   :maxdepth: 1

   usage
   cip8
