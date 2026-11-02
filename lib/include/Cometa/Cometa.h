/**
 * @file Cometa.h
 *
 * @brief Public Cometa value-type includes.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
 *
 * Copyright 2026 Biglup Labs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

/* INCLUDES ******************************************************************/

#include <Cometa/Assets/AssetId.h>
#include <Cometa/Assets/AssetIdList.h>
#include <Cometa/Assets/AssetIdMap.h>
#include <Cometa/Assets/AssetName.h>
#include <Cometa/Assets/AssetNameList.h>
#include <Cometa/Assets/AssetNameMap.h>
#include <Cometa/Assets/MultiAsset.h>
#include <Cometa/Assets/PolicyIdList.h>
#include <Cometa/Common/Anchor.h>
#include <Cometa/Common/BigInt.h>
#include <Cometa/Common/ByteOrder.h>
#include <Cometa/Common/Credential.h>
#include <Cometa/Common/CredentialSet.h>
#include <Cometa/Common/CredentialType.h>
#include <Cometa/Common/ExUnits.h>
#include <Cometa/Common/GovernanceActionId.h>
#include <Cometa/Common/GovernanceKeyType.h>
#include <Cometa/Common/NetworkId.h>
#include <Cometa/Common/NetworkMagic.h>
#include <Cometa/Common/ProtocolVersion.h>
#include <Cometa/Common/UnitInterval.h>
#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/Crypto/Blake2bHashSet.h>
#include <Cometa/Crypto/Blake2bHashSize.h>
#include <Cometa/Encoding/Base58.h>
#include <Cometa/Encoding/Bech32.h>
#include <Cometa/Error.h>
#include <Cometa/ErrorCode.h>
#include <Cometa/SlotConfig.h>
#include <Cometa/TransactionBody/Value.h>

#include <Cometa/Cbor/CborMajorType.h>
#include <Cometa/Cbor/CborReader.h>
#include <Cometa/Cbor/CborReaderState.h>
#include <Cometa/Cbor/CborSimpleValue.h>
#include <Cometa/Cbor/CborTag.h>
#include <Cometa/Cbor/CborWriter.h>

#include <Cometa/Json/JsonContext.h>
#include <Cometa/Json/JsonFormat.h>
#include <Cometa/Json/JsonType.h>
#include <Cometa/Json/JsonValue.h>
#include <Cometa/Json/JsonWriter.h>

#include <Cometa/Address/Address.h>
#include <Cometa/Address/AddressType.h>
#include <Cometa/Address/BaseAddress.h>
#include <Cometa/Address/ByronAddress.h>
#include <Cometa/Address/ByronAddressAttributes.h>
#include <Cometa/Address/ByronAddressType.h>
#include <Cometa/Address/EnterpriseAddress.h>
#include <Cometa/Address/PointerAddress.h>
#include <Cometa/Address/RewardAddress.h>
#include <Cometa/Address/StakePointer.h>
#include <Cometa/Crypto/Bip32.h>
#include <Cometa/Crypto/Bip32PrivateKey.h>
#include <Cometa/Crypto/Bip32PublicKey.h>
#include <Cometa/Crypto/Bip39.h>
#include <Cometa/Crypto/Crc32.h>
#include <Cometa/Crypto/Ed25519PrivateKey.h>
#include <Cometa/Crypto/Ed25519PublicKey.h>
#include <Cometa/Crypto/Ed25519Signature.h>
#include <Cometa/Crypto/Emip3.h>
#include <Cometa/Crypto/Pbkdf2.h>

#include <Cometa/Common/Datum.h>
#include <Cometa/PlutusData/ConstrPlutusData.h>
#include <Cometa/PlutusData/PlutusData.h>
#include <Cometa/PlutusData/PlutusList.h>
#include <Cometa/PlutusData/PlutusMap.h>

#include <Cometa/Scripts/NativeScripts/ScriptInvalidAfter.h>
#include <Cometa/Scripts/NativeScripts/ScriptInvalidBefore.h>
#include <Cometa/Scripts/NativeScripts/ScriptPubkey.h>
#include <Cometa/Scripts/NativeScripts/ScriptRequireGuard.h>

#include <Cometa/Scripts/NativeScripts/NativeScript.h>
#include <Cometa/Scripts/NativeScripts/NativeScriptList.h>
#include <Cometa/Scripts/NativeScripts/NativeScriptType.h>
#include <Cometa/Scripts/NativeScripts/ScriptAll.h>
#include <Cometa/Scripts/NativeScripts/ScriptAny.h>
#include <Cometa/Scripts/NativeScripts/ScriptNOfK.h>

#include <Cometa/Scripts/PlutusScripts/PlutusLanguageVersion.h>
#include <Cometa/Scripts/PlutusScripts/PlutusV1Script.h>
#include <Cometa/Scripts/PlutusScripts/PlutusV2Script.h>
#include <Cometa/Scripts/PlutusScripts/PlutusV3Script.h>
#include <Cometa/Scripts/PlutusScripts/PlutusV4Script.h>

#include <Cometa/Scripts/Script.h>
#include <Cometa/Scripts/ScriptLanguage.h>

#include <Cometa/Common/Utxo.h>
#include <Cometa/TransactionBody/TransactionInput.h>
#include <Cometa/TransactionBody/TransactionOutput.h>

#include <Cometa/Common/RewardAddressList.h>
#include <Cometa/Common/UtxoList.h>
#include <Cometa/Common/WithdrawalMap.h>
#include <Cometa/TransactionBody/TransactionInputSet.h>
#include <Cometa/TransactionBody/TransactionOutputList.h>
