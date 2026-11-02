CMAKE_MINIMUM_REQUIRED (VERSION 3.22)

# Project metadata.
INCLUDE (cmake/Version.cmake)
PROJECT (cometa VERSION ${COMETA_VERSION} LANGUAGES C CXX)

INCLUDE (GNUInstallDirs)
INCLUDE (CMakePackageConfigHelpers)

# Build options and native dependency.
OPTION (COMETA_BUILD_TESTS "Build unit tests" ON)
OPTION (COMETA_BUILD_DOCS "Build documentation" OFF)
SET (COMETA_CARDANO_SOURCE_DIR "" CACHE PATH "Local cardano-c checkout with deep-clone APIs")

INCLUDE (cmake/Cardano.cmake)

# Library sources.
ADD_LIBRARY (cometa STATIC
  lib/src/AuxiliaryData/TransactionMetadata.cpp
  lib/src/PoolParams/SingleHostAddrRelay.cpp
  lib/src/PoolParams/SingleHostNameRelay.cpp
  lib/src/PoolParams/MultiHostNameRelay.cpp
  lib/src/PoolParams/Relay.cpp
  lib/src/PoolParams/Relays.cpp
  lib/src/PoolParams/RelayType.cpp
  lib/src/PoolParams/PoolOwners.cpp
  lib/src/PoolParams/PoolParams.cpp
  lib/src/Certificates/CertificateType.cpp
  lib/src/Certificates/StakeRegistrationCert.cpp
  lib/src/Certificates/StakeDeregistrationCert.cpp
  lib/src/Certificates/StakeDelegationCert.cpp
  lib/src/Certificates/StakeRegistrationDelegationCert.cpp
  lib/src/Certificates/PoolRegistrationCert.cpp
  lib/src/Certificates/PoolRetirementCert.cpp
  lib/src/Certificates/GenesisKeyDelegationCert.cpp
  lib/src/Certificates/MirToPotCert.cpp
  lib/src/Certificates/MirToStakeCredentialsCert.cpp
  lib/src/Certificates/MirCert.cpp
  lib/src/Certificates/VoteDelegationCert.cpp
  lib/src/Certificates/StakeVoteDelegationCert.cpp
  lib/src/Certificates/VoteRegistrationDelegationCert.cpp
  lib/src/Certificates/StakeVoteRegistrationDelegationCert.cpp
  lib/src/Certificates/UnregisterDRepCert.cpp
  lib/src/Certificates/RegisterDRepCert.cpp
  lib/src/Certificates/UpdateDRepCert.cpp
  lib/src/Certificates/RegistrationCert.cpp
  lib/src/Certificates/UnregistrationCert.cpp
  lib/src/Common/DRep.cpp
  lib/src/PoolParams/Ipv4.cpp
  lib/src/PoolParams/Ipv6.cpp
  lib/src/PoolParams/BlsKey.cpp
  lib/src/PoolParams/PoolMetadata.cpp
  lib/src/AuxiliaryData/AuxiliaryData.cpp
  lib/src/AuxiliaryData/Metadatum.cpp
  lib/src/AuxiliaryData/MetadatumList.cpp
  lib/src/AuxiliaryData/MetadatumMap.cpp

  lib/src/AuxiliaryData/PlutusV1ScriptList.cpp
  lib/src/AuxiliaryData/PlutusV2ScriptList.cpp
  lib/src/AuxiliaryData/PlutusV3ScriptList.cpp
  lib/src/AuxiliaryData/PlutusV4ScriptList.cpp

  lib/src/TransactionBody/TransactionInputSet.cpp
  lib/src/TransactionBody/TransactionOutputList.cpp
  lib/src/Common/WithdrawalMap.cpp
  lib/src/TransactionBody/TransactionInput.cpp
  lib/src/TransactionBody/TransactionOutput.cpp
  lib/src/Common/Utxo.cpp
  lib/src/Scripts/Script.cpp
  lib/src/Scripts/PlutusScripts/PlutusV1Script.cpp
  lib/src/Scripts/PlutusScripts/PlutusV2Script.cpp
  lib/src/Scripts/PlutusScripts/PlutusV3Script.cpp
  lib/src/Scripts/PlutusScripts/PlutusV4Script.cpp
  lib/src/Scripts/NativeScripts/NativeScriptType.cpp
  lib/src/Scripts/NativeScripts/NativeScript.cpp
  lib/src/Scripts/NativeScripts/NativeScriptList.cpp
  lib/src/Scripts/NativeScripts/ScriptAll.cpp
  lib/src/Scripts/NativeScripts/ScriptAny.cpp
  lib/src/Scripts/NativeScripts/ScriptNOfK.cpp

  lib/src/Scripts/NativeScripts/ScriptPubkey.cpp
  lib/src/Scripts/NativeScripts/ScriptInvalidBefore.cpp
  lib/src/Scripts/NativeScripts/ScriptInvalidAfter.cpp
  lib/src/Scripts/NativeScripts/ScriptRequireGuard.cpp

  lib/src/PlutusData/PlutusData.cpp
  lib/src/PlutusData/PlutusList.cpp
  lib/src/PlutusData/PlutusMap.cpp
  lib/src/PlutusData/ConstrPlutusData.cpp
  lib/src/Common/Datum.cpp

  lib/src/Cbor/CborTag.cpp
  lib/src/Cbor/CborMajorType.cpp
  lib/src/Cbor/CborSimpleValue.cpp
  lib/src/Cbor/CborReaderState.cpp
  lib/src/Cbor/CborReader.cpp
  lib/src/Cbor/CborWriter.cpp
  lib/src/Json/JsonValue.cpp
  lib/src/Json/JsonWriter.cpp
  lib/src/ErrorCode.cpp
  lib/src/Error.cpp
  lib/src/SlotConfig.cpp
  lib/src/Encoding/Base58.cpp
  lib/src/Encoding/Bech32.cpp
  lib/src/Common/BigInt.cpp
  lib/src/Common/UnitInterval.cpp
  lib/src/Common/Anchor.cpp
  lib/src/Common/GovernanceActionId.cpp
  lib/src/Common/ExUnits.cpp
  lib/src/Common/ProtocolVersion.cpp
  lib/src/Common/NetworkMagic.cpp
  lib/src/Crypto/Bip39.cpp
  lib/src/Crypto/Pbkdf2.cpp
  lib/src/Crypto/Emip3.cpp
  lib/src/Crypto/Crc32.cpp
  lib/src/Address/Address.cpp
  lib/src/Address/BaseAddress.cpp
  lib/src/Address/EnterpriseAddress.cpp
  lib/src/Address/RewardAddress.cpp
  lib/src/Address/PointerAddress.cpp
  lib/src/Address/ByronAddress.cpp
  lib/src/Crypto/Bip32.cpp
  lib/src/Crypto/Ed25519Signature.cpp
  lib/src/Crypto/Ed25519PublicKey.cpp
  lib/src/Crypto/Ed25519PrivateKey.cpp
  lib/src/Crypto/Bip32PublicKey.cpp
  lib/src/Crypto/Bip32PrivateKey.cpp
  lib/src/Crypto/Blake2bHashSet.cpp
  lib/src/Crypto/Blake2bHash.cpp
  lib/src/Assets/AssetArithmetic.cpp
  lib/src/Assets/AssetIdMap.cpp
  lib/src/Assets/AssetNameMap.cpp
  lib/src/Assets/MultiAsset.cpp
  lib/src/Assets/AssetId.cpp
  lib/src/Assets/AssetName.cpp
  lib/src/TransactionBody/Value.cpp
  lib/src/Common/CredentialSet.cpp
  lib/src/Common/Credential.cpp
)

ADD_LIBRARY (Cometa::cometa ALIAS cometa)

TARGET_COMPILE_FEATURES (cometa PUBLIC cxx_std_17)

TARGET_INCLUDE_DIRECTORIES (cometa
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    "${cardano_source}/lib/include"
)

TARGET_LINK_LIBRARIES (cometa
  PRIVATE
    $<BUILD_INTERFACE:cometa_cardano>
    $<INSTALL_INTERFACE:Cometa::cardano>
)

SET_TARGET_PROPERTIES (cometa
  PROPERTIES
    CXX_EXTENSIONS            OFF
    POSITION_INDEPENDENT_CODE ON
)

IF (MSVC)
  TARGET_COMPILE_OPTIONS (cometa PRIVATE /W4 /WX)
ELSE ()
  TARGET_COMPILE_OPTIONS (cometa PRIVATE -Wall -Wextra -Wpedantic -Werror)
ENDIF ()

# Unit tests.
IF (COMETA_BUILD_TESTS)
  ENABLE_TESTING ()
  FIND_PACKAGE (GTest CONFIG REQUIRED)

  ADD_EXECUTABLE (cometa-tests
    lib/tests/AuxiliaryData/TransactionMetadataTest.cpp
    lib/tests/AuxiliaryData/MetadatumLabelListTest.cpp
    lib/tests/PoolParams/SingleHostAddrRelayTest.cpp
    lib/tests/PoolParams/SingleHostNameRelayTest.cpp
    lib/tests/PoolParams/MultiHostNameRelayTest.cpp
    lib/tests/PoolParams/RelayTest.cpp
    lib/tests/PoolParams/RelaysTest.cpp
    lib/tests/PoolParams/RelayTypeTest.cpp
    lib/tests/PoolParams/PoolOwnersTest.cpp
    lib/tests/PoolParams/PoolParamsTest.cpp
    lib/tests/Certificates/CertificateTypeTest.cpp
    lib/tests/Certificates/StakeRegistrationCertTest.cpp
    lib/tests/Certificates/StakeDeregistrationCertTest.cpp
    lib/tests/Certificates/StakeDelegationCertTest.cpp
    lib/tests/Certificates/StakeRegistrationDelegationCertTest.cpp
    lib/tests/Certificates/PoolRegistrationCertTest.cpp
    lib/tests/Certificates/PoolRetirementCertTest.cpp
    lib/tests/Certificates/GenesisKeyDelegationCertTest.cpp
    lib/tests/Certificates/MirPotTypeTest.cpp
    lib/tests/Certificates/MirCertTypeTest.cpp
    lib/tests/Certificates/MirToPotCertTest.cpp
    lib/tests/Certificates/MirToStakeCredentialsCertTest.cpp
    lib/tests/Certificates/MirCertTest.cpp
    lib/tests/Certificates/VoteDelegationCertTest.cpp
    lib/tests/Certificates/StakeVoteDelegationCertTest.cpp
    lib/tests/Certificates/VoteRegistrationDelegationCertTest.cpp
    lib/tests/Certificates/StakeVoteRegistrationDelegationCertTest.cpp
    lib/tests/Certificates/UnregisterDRepCertTest.cpp
    lib/tests/Certificates/RegisterDRepCertTest.cpp
    lib/tests/Certificates/UpdateDRepCertTest.cpp
    lib/tests/Certificates/RegistrationCertTest.cpp
    lib/tests/Certificates/UnregistrationCertTest.cpp
    lib/tests/Common/DRepTypeTest.cpp
    lib/tests/Common/DRepTest.cpp
    lib/tests/PoolParams/Ipv4Test.cpp
    lib/tests/PoolParams/Ipv6Test.cpp
    lib/tests/PoolParams/BlsKeyTest.cpp
    lib/tests/PoolParams/PoolMetadataTest.cpp
    lib/tests/AuxiliaryData/AuxiliaryDataTest.cpp

    lib/tests/AuxiliaryData/MetadatumTest.cpp
    lib/tests/AuxiliaryData/MetadatumListTest.cpp
    lib/tests/AuxiliaryData/MetadatumMapTest.cpp
    lib/tests/AuxiliaryData/MetadatumKindTest.cpp

    lib/tests/AuxiliaryData/PlutusV1ScriptListTest.cpp
    lib/tests/AuxiliaryData/PlutusV2ScriptListTest.cpp
    lib/tests/AuxiliaryData/PlutusV3ScriptListTest.cpp
    lib/tests/AuxiliaryData/PlutusV4ScriptListTest.cpp

    lib/tests/TransactionBody/TransactionInputTest.cpp
    lib/tests/TransactionBody/TransactionOutputTest.cpp
    lib/tests/Common/UtxoTest.cpp
    lib/tests/TransactionBody/TransactionInputSetTest.cpp
    lib/tests/TransactionBody/TransactionOutputListTest.cpp
    lib/tests/Common/WithdrawalMapTest.cpp
    lib/tests/Common/UtxoListTest.cpp
    lib/tests/Common/RewardAddressListTest.cpp
    lib/tests/Scripts/ScriptTest.cpp
    lib/tests/Scripts/ScriptLanguageTest.cpp
    lib/tests/Scripts/PlutusScripts/PlutusV1ScriptTest.cpp
    lib/tests/Scripts/PlutusScripts/PlutusV2ScriptTest.cpp
    lib/tests/Scripts/PlutusScripts/PlutusV3ScriptTest.cpp
    lib/tests/Scripts/PlutusScripts/PlutusV4ScriptTest.cpp
    lib/tests/Scripts/PlutusScripts/PlutusLanguageVersionTest.cpp
    lib/tests/Scripts/NativeScripts/NativeScriptTest.cpp
    lib/tests/Scripts/NativeScripts/NativeScriptListTest.cpp
    lib/tests/Scripts/NativeScripts/ScriptAllTest.cpp
    lib/tests/Scripts/NativeScripts/ScriptAnyTest.cpp
    lib/tests/Scripts/NativeScripts/ScriptNOfKTest.cpp
    lib/tests/Scripts/NativeScripts/NativeScriptTypeTest.cpp

    lib/tests/Scripts/NativeScripts/ScriptPubkeyTest.cpp
    lib/tests/Scripts/NativeScripts/ScriptInvalidBeforeTest.cpp
    lib/tests/Scripts/NativeScripts/ScriptInvalidAfterTest.cpp
    lib/tests/Scripts/NativeScripts/ScriptRequireGuardTest.cpp

    lib/tests/PlutusData/PlutusDataTest.cpp
    lib/tests/PlutusData/PlutusListTest.cpp
    lib/tests/PlutusData/PlutusMapTest.cpp
    lib/tests/PlutusData/ConstrPlutusDataTest.cpp
    lib/tests/Common/DatumTest.cpp
    lib/tests/Common/DatumTypeTest.cpp
    lib/tests/PlutusData/PlutusDataKindTest.cpp

    lib/tests/Cbor/CborTagTest.cpp
    lib/tests/Cbor/CborMajorTypeTest.cpp
    lib/tests/Cbor/CborSimpleValueTest.cpp
    lib/tests/Cbor/CborReaderStateTest.cpp
    lib/tests/Cbor/CborReaderTest.cpp
    lib/tests/Cbor/CborWriterTest.cpp
    lib/tests/Json/JsonTypeTest.cpp
    lib/tests/Json/JsonFormatTest.cpp
    lib/tests/Json/JsonContextTest.cpp
    lib/tests/Json/JsonValueTest.cpp
    lib/tests/Json/JsonWriterTest.cpp
    lib/tests/ErrorTest.cpp
    lib/tests/SlotConfigTest.cpp
    lib/tests/Encoding/Base58Test.cpp
    lib/tests/Encoding/Bech32Test.cpp
    lib/tests/Common/BigIntTest.cpp
    lib/tests/Common/UnitIntervalTest.cpp
    lib/tests/Common/AnchorTest.cpp
    lib/tests/Common/GovernanceActionIdTest.cpp
    lib/tests/Common/ExUnitsTest.cpp
    lib/tests/Common/ProtocolVersionTest.cpp
    lib/tests/Common/ByteOrderTest.cpp
    lib/tests/Common/NetworkIdTest.cpp
    lib/tests/Common/NetworkMagicTest.cpp
    lib/tests/Common/GovernanceKeyTypeTest.cpp
    lib/tests/Assets/AssetIdMapTest.cpp
    lib/tests/Assets/AssetNameMapTest.cpp
    lib/tests/Assets/MultiAssetTest.cpp
    lib/tests/Assets/AssetIdTest.cpp
    lib/tests/Assets/AssetIdListTest.cpp
    lib/tests/Assets/AssetNameListTest.cpp
    lib/tests/Assets/PolicyIdListTest.cpp
    lib/tests/Assets/AssetNameTest.cpp
    lib/tests/Crypto/Bip39Test.cpp
    lib/tests/Crypto/Pbkdf2Test.cpp
    lib/tests/Crypto/Emip3Test.cpp
    lib/tests/Crypto/Crc32Test.cpp
    lib/tests/Address/AddressTypeTest.cpp
    lib/tests/Address/ByronAddressTypeTest.cpp
    lib/tests/Address/AddressTest.cpp
    lib/tests/Address/BaseAddressTest.cpp
    lib/tests/Address/EnterpriseAddressTest.cpp
    lib/tests/Address/RewardAddressTest.cpp
    lib/tests/Address/PointerAddressTest.cpp
    lib/tests/Address/ByronAddressTest.cpp
    lib/tests/Crypto/Bip32Test.cpp
    lib/tests/Crypto/Ed25519SignatureTest.cpp
    lib/tests/Crypto/Ed25519PublicKeyTest.cpp
    lib/tests/Crypto/Ed25519PrivateKeyTest.cpp
    lib/tests/Crypto/Bip32PublicKeyTest.cpp
    lib/tests/Crypto/Bip32PrivateKeyTest.cpp
    lib/tests/Crypto/PrimitiveConformanceTest.cpp
    lib/tests/Crypto/Blake2bHashSizeTest.cpp
    lib/tests/Crypto/Blake2bHashSetTest.cpp
    lib/tests/Crypto/Blake2bHashTest.cpp
    lib/tests/Common/CredentialSetTest.cpp
    lib/tests/Common/CredentialTest.cpp
    lib/tests/TransactionBody/ValueTest.cpp
    lib/tests/ValueContractTest.cpp
  )

  IF (MSVC)
    TARGET_COMPILE_OPTIONS (cometa-tests PRIVATE /bigobj)
  ENDIF ()

  TARGET_INCLUDE_DIRECTORIES (cometa-tests
    PRIVATE
      "${cardano_source}/lib"
      "${cardano_source}/lib/include"
  )

  TARGET_COMPILE_DEFINITIONS (cometa-tests
    PRIVATE
      COMETA_TEST_VECTOR_DIR="${cardano_source}/lib/tests/vectors/bindings/v1"
  )

  TARGET_LINK_LIBRARIES (cometa-tests PRIVATE Cometa::cometa GTest::gtest_main)

  ADD_TEST (NAME cometa-tests COMMAND cometa-tests)
ENDIF ()

# Documentation.
IF (COMETA_BUILD_DOCS)
  ADD_SUBDIRECTORY (doc)
ENDIF ()

# Installed headers, archives, and license notices.
INSTALL (
  TARGETS cometa
  EXPORT  CometaTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

INSTALL (
  DIRECTORY   lib/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

INSTALL (
  FILES       "${cardano_archive}"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cometa
  RENAME      "${CMAKE_STATIC_LIBRARY_PREFIX}cardano-c${CMAKE_STATIC_LIBRARY_SUFFIX}"
)

INSTALL (
  FILES       LICENSE README.md
  DESTINATION ${CMAKE_INSTALL_DATADIR}/cometa
)

INSTALL (
  FILES       "${cardano_source}/COPYING"
  DESTINATION ${CMAKE_INSTALL_DATADIR}/cometa
  RENAME      cardano-c-LICENSE
)

INSTALL (
  DIRECTORY   "${cardano_source}/lib/external/"
  DESTINATION ${CMAKE_INSTALL_DATADIR}/cometa/third-party
  FILES_MATCHING
    PATTERN "*LICENSE*"
    PATTERN "*COPYING*"
    PATTERN "*NOTICE*"
    PATTERN "*license*"
    PATTERN "*copyright*"
)

# Relocatable CMake package.
INSTALL (
  EXPORT      CometaTargets
  NAMESPACE   Cometa::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Cometa
)

CONFIGURE_PACKAGE_CONFIG_FILE (
  cmake/CometaConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/CometaConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Cometa
)

WRITE_BASIC_PACKAGE_VERSION_FILE (
  "${CMAKE_CURRENT_BINARY_DIR}/CometaConfigVersion.cmake"
  VERSION       ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

INSTALL (
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CometaConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CometaConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Cometa
)

# Platform ZIP packages.
SET (CPACK_GENERATOR ZIP)
SET (CPACK_PACKAGE_VERSION "${COMETA_FULL_VERSION}")
SET (CPACK_PACKAGE_FILE_NAME
  "cometa-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_CXX_COMPILER_ID}"
)

INCLUDE (CPack)
