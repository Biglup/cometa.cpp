API reference
=============

.. doxygenclass:: Cometa::Blake2bHash
   :members:

.. doxygenclass:: Cometa::AssetName
   :members:

.. doxygenenum:: Cometa::CredentialType

.. doxygenclass:: Cometa::Credential
   :members:

.. doxygenclass:: Cometa::Error
   :members:

.. doxygenenum:: Cometa::ErrorCode

.. doxygenfunction:: Cometa::makeErrorCode

.. doxygenclass:: Cometa::ExUnits
   :members:

.. doxygenclass:: Cometa::ProtocolVersion
   :members:

.. doxygenstruct:: Cometa::SlotConfig
   :members:

.. doxygenenum:: Cometa::ByteOrder

.. doxygenenum:: Cometa::NetworkId

.. doxygenenum:: Cometa::NetworkMagic

.. doxygenenum:: Cometa::GovernanceKeyType

.. doxygenfunction:: Cometa::toString(NetworkMagic)

.. doxygenclass:: Cometa::UnitInterval
   :members:

.. doxygenclass:: Cometa::Anchor
   :members:

.. doxygenclass:: Cometa::GovernanceActionId
   :members:

.. doxygenclass:: Cometa::BigInt
   :members:

.. doxygenfunction:: Cometa::encodeBase58

.. doxygenfunction:: Cometa::decodeBase58

.. doxygenfunction:: Cometa::encodeBech32

.. doxygenfunction:: Cometa::decodeBech32

.. doxygenstruct:: Cometa::Bech32Data
   :members:

.. doxygenenum:: Cometa::CborTag

.. doxygenenum:: Cometa::CborMajorType

.. doxygenenum:: Cometa::CborSimpleValue

.. doxygenenum:: Cometa::CborReaderState

.. doxygenclass:: Cometa::CborReader
   :members:

.. doxygenclass:: Cometa::CborWriter
   :members:

.. doxygenfunction:: Cometa::toString(CborTag)

.. doxygenfunction:: Cometa::toString(CborMajorType)

.. doxygenfunction:: Cometa::toString(CborSimpleValue)

.. doxygenfunction:: Cometa::toString(CborReaderState)

.. doxygenenum:: Cometa::JsonType

.. doxygenenum:: Cometa::JsonFormat

.. doxygenenum:: Cometa::JsonContext

.. doxygenclass:: Cometa::JsonValue
   :members:

.. doxygenclass:: Cometa::JsonWriter
   :members:

.. doxygenclass:: Cometa::Ed25519Signature
   :members:

.. doxygenclass:: Cometa::Ed25519PublicKey
   :members:

.. doxygenclass:: Cometa::Ed25519PrivateKey
   :members:

.. doxygenclass:: Cometa::Bip32PublicKey
   :members:

.. doxygenclass:: Cometa::Bip32PrivateKey
   :members:

.. doxygenfunction:: Cometa::hardenIndex

.. doxygenfunction:: Cometa::entropyToMnemonic

.. doxygenfunction:: Cometa::mnemonicToEntropy

.. doxygenfunction:: Cometa::pbkdf2HmacSha512

.. doxygenfunction:: Cometa::encryptEmip3

.. doxygenfunction:: Cometa::decryptEmip3

.. doxygenfunction:: Cometa::crc32

.. doxygenenum:: Cometa::Blake2bHashSize
