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

.. doxygenclass:: Cometa::Address
   :members:

.. doxygenclass:: Cometa::BaseAddress
   :members:

.. doxygenclass:: Cometa::EnterpriseAddress
   :members:

.. doxygenclass:: Cometa::RewardAddress
   :members:

.. doxygenclass:: Cometa::PointerAddress
   :members:

.. doxygenclass:: Cometa::ByronAddress
   :members:

.. doxygenenum:: Cometa::AddressType

.. doxygenenum:: Cometa::ByronAddressType

.. doxygenstruct:: Cometa::StakePointer
   :members:

.. doxygenstruct:: Cometa::ByronAddressAttributes
   :members:

.. doxygenclass:: Cometa::AssetId
   :members:

.. doxygentypedef:: Cometa::AssetIdList

.. doxygentypedef:: Cometa::AssetNameList

.. doxygentypedef:: Cometa::PolicyIdList

.. doxygenclass:: Cometa::AssetIdMap
   :members:

.. doxygenclass:: Cometa::AssetNameMap
   :members:

.. doxygenclass:: Cometa::MultiAsset
   :members:

.. doxygenclass:: Cometa::Value
   :members:

.. doxygenclass:: Cometa::Blake2bHashSet
   :members:

.. doxygenclass:: Cometa::CredentialSet
   :members:

.. doxygenclass:: Cometa::PlutusData
   :members:

.. doxygenclass:: Cometa::PlutusList
   :members:

.. doxygenclass:: Cometa::PlutusMap
   :members:

.. doxygenclass:: Cometa::ConstrPlutusData
   :members:

.. doxygenclass:: Cometa::Datum
   :members:

.. doxygenenum:: Cometa::PlutusDataKind

.. doxygenenum:: Cometa::DatumType

.. doxygenclass:: Cometa::ScriptPubkey
   :members:

.. doxygenclass:: Cometa::ScriptInvalidBefore
   :members:

.. doxygenclass:: Cometa::ScriptInvalidAfter
   :members:

.. doxygenclass:: Cometa::ScriptRequireGuard
   :members:

.. doxygenclass:: Cometa::NativeScript
   :members:

.. doxygenclass:: Cometa::NativeScriptList
   :members:

.. doxygenclass:: Cometa::ScriptAll
   :members:

.. doxygenclass:: Cometa::ScriptAny
   :members:

.. doxygenclass:: Cometa::ScriptNOfK
   :members:

.. doxygenenum:: Cometa::NativeScriptType

.. doxygenfunction:: Cometa::toString(NativeScriptType)

.. doxygenclass:: Cometa::PlutusV1Script
   :members:

.. doxygenclass:: Cometa::PlutusV2Script
   :members:

.. doxygenclass:: Cometa::PlutusV3Script
   :members:

.. doxygenclass:: Cometa::PlutusV4Script
   :members:

.. doxygenenum:: Cometa::PlutusLanguageVersion

.. doxygenclass:: Cometa::Script
   :members:

.. doxygenenum:: Cometa::ScriptLanguage

.. doxygenclass:: Cometa::TransactionInput
   :members:

.. doxygenclass:: Cometa::TransactionOutput
   :members:

.. doxygenclass:: Cometa::TransactionInputSet
   :members:

.. doxygenclass:: Cometa::TransactionOutputList
   :members:

.. doxygenclass:: Cometa::Utxo
   :members:

.. doxygenclass:: Cometa::WithdrawalMap
   :members:

.. doxygentypedef:: Cometa::UtxoList

.. doxygentypedef:: Cometa::RewardAddressList

.. doxygenclass:: Cometa::PlutusV1ScriptList
   :members:

.. doxygenclass:: Cometa::PlutusV2ScriptList
   :members:

.. doxygenclass:: Cometa::PlutusV3ScriptList
   :members:

.. doxygenclass:: Cometa::PlutusV4ScriptList
   :members:

.. doxygenclass:: Cometa::Metadatum
   :members:

.. doxygenenum:: Cometa::MetadatumKind

.. doxygenclass:: Cometa::MetadatumList
   :members:

.. doxygenclass:: Cometa::MetadatumMap
   :members:

.. doxygenclass:: Cometa::TransactionMetadata
   :members:

.. doxygentypedef:: Cometa::MetadatumLabelList

.. doxygenclass:: Cometa::AuxiliaryData
   :members:

Pool primitives
---------------

.. doxygenclass:: Cometa::Ipv4
   :members:

.. doxygenclass:: Cometa::Ipv6
   :members:

.. doxygenclass:: Cometa::BlsKey
   :members:

.. doxygenclass:: Cometa::PoolMetadata
   :members:

Pool relays
-----------

.. doxygenclass:: Cometa::SingleHostAddrRelay
   :members:

.. doxygenclass:: Cometa::SingleHostNameRelay
   :members:

.. doxygenclass:: Cometa::MultiHostNameRelay
   :members:

.. doxygenclass:: Cometa::Relay
   :members:

.. doxygenclass:: Cometa::Relays
   :members:

.. doxygenenum:: Cometa::RelayType

Pool owners
-----------

.. doxygenclass:: Cometa::PoolOwners
   :members:

Pool parameters
---------------

.. doxygenclass:: Cometa::PoolParams
   :members:

Certificate types
-----------------

.. doxygenenum:: Cometa::CertificateType

Stake registration
------------------

.. doxygenclass:: Cometa::StakeRegistrationCert
   :members:

Stake deregistration
--------------------

.. doxygenclass:: Cometa::StakeDeregistrationCert
   :members:

Registration certificates
-------------------------

.. doxygenclass:: Cometa::RegistrationCert
   :members:

.. doxygenclass:: Cometa::UnregistrationCert
   :members:

Stake delegation
----------------

.. doxygenclass:: Cometa::StakeDelegationCert
   :members:

Pool registration certificate
-----------------------------

.. doxygenclass:: Cometa::PoolRegistrationCert
   :members:

Pool retirement certificate
---------------------------

.. doxygenclass:: Cometa::PoolRetirementCert
   :members:

Stake registration and delegation
---------------------------------

.. doxygenclass:: Cometa::StakeRegistrationDelegationCert
   :members:

Genesis key delegation certificate
----------------------------------

.. doxygenclass:: Cometa::GenesisKeyDelegationCert
   :members:
