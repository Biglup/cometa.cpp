Message Signing usage
=====================

Signing data with CIP-8
-----------------------

``signCip8`` signs bytes with an Ed25519 key and binds the result to an address
or key hash. The result contains COSE_Sign1 and COSE_Key CBOR bytes suitable for
Cardano wallet message-signing interfaces.

.. code-block:: cpp

   const Cometa::Cip8Signature signedMessage =
     Cometa::signCip8(message, address, signingKey);
   const std::vector<std::uint8_t>& coseSign1 = signedMessage.coseSign1;
   const std::vector<std::uint8_t>& coseKey = signedMessage.coseKey;
