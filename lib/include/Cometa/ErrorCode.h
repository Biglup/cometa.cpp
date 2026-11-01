/**
 * @file ErrorCode.h
 *
 * @brief Operation error codes.
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

#include <system_error>

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/**
 * @brief Identifies an operation failure.
 */
enum class ErrorCode
{
  /** @brief Successful operation. */
  Success = 0,

  /** @brief Generic error. */
  Generic = 1,

  /** @brief Insufficient buffer size. */
  InsufficientBufferSize = 2,

  /** @brief Operation over a null pointer. */
  PointerIsNull = 3,

  /** @brief Memory could not be allocated. */
  MemoryAllocationFailed = 4,

  /** @brief Out of bounds memory read. */
  OutOfBoundsMemoryRead = 5,

  /** @brief Out of bounds memory write. */
  OutOfBoundsMemoryWrite = 6,

  /** @brief Invalid argument. */
  InvalidArgument = 7,

  /** @brief Invalid URL. */
  InvalidUrl = 8,

  /** @brief Element not found. */
  ElementNotFound = 9,

  /** @brief Encoding failure. */
  Encoding = 10,

  /** @brief Decoding failure. */
  Decoding = 11,

  /** @brief Invalid checksum. */
  ChecksumMismatch = 12,

  /** @brief Invalid JSON. */
  InvalidJson = 13,

  /** @brief Size overflow. */
  IntegerOverflow = 14,

  /** @brief Size underflow. */
  IntegerUnderflow = 15,

  /** @brief Conversion error. */
  ConversionFailed = 16,

  /** @brief Index out of bounds. */
  IndexOutOfBounds = 17,

  /** @brief Invalid certificate type. */
  InvalidCertificateType = 18,

  /** @brief The operation is not implemented. */
  NotImplemented = 19,

  /** @brief The passphrase is invalid. */
  InvalidPassphrase = 20,

  /** @brief The state of the object is illegal. */
  IllegalState = 21,

  /** @brief The element is already present. */
  DuplicatedKey = 22,

  /** @brief The JSON type is unexpected. */
  JsonTypeMismatch = 23,

  /** @brief The serialization or deserialization process resulted in a loss of precision. */
  LossOfPrecision = 100,

  /** @brief The magic number is invalid. */
  InvalidMagic = 101,

  /** @brief The checksum is invalid. */
  InvalidChecksum = 102,

  /** @brief The hash size is invalid. */
  InvalidBlake2bHashSize = 200,

  /** @brief The Ed25519 signature size is invalid. */
  InvalidEd25519SignatureSize = 201,

  /** @brief The Ed25519 public key size is invalid. */
  InvalidEd25519PublicKeySize = 202,

  /** @brief The Ed25519 private key size is invalid. */
  InvalidEd25519PrivateKeySize = 203,

  /** @brief The BIP32 public key size is invalid. */
  InvalidBip32PublicKeySize = 204,

  /** @brief The BIP32 private key size is invalid. */
  InvalidBip32PrivateKeySize = 205,

  /** @brief The BIP32 derivation index is invalid. */
  InvalidBip32DerivationIndex = 206,

  /** @brief The CBOR type is unexpected. */
  UnexpectedCborType = 300,

  /** @brief The CBOR value is outside the permitted range. */
  InvalidCborValue = 301,

  /** @brief The CBOR array size is invalid. */
  InvalidCborArraySize = 302,

  /** @brief The CBOR map size is invalid. */
  InvalidCborMapSize = 303,

  /** @brief The CBOR map key is duplicated. */
  DuplicatedCborMapKey = 304,

  /** @brief The CBOR map key is invalid. */
  InvalidCborMapKey = 305,

  /** @brief The address type is invalid. */
  InvalidAddressType = 400,

  /** @brief The address format is invalid. */
  InvalidAddressFormat = 401,

  /** @brief The credential type is invalid. */
  InvalidCredentialType = 500,

  /** @brief The Plutus data conversion is invalid. */
  InvalidPlutusDataConversion = 600,

  /** @brief The Plutus datum type is invalid. */
  InvalidDatumType = 601,

  /** @brief The script language is invalid. */
  InvalidScriptLanguage = 700,

  /** @brief The native script type is invalid. */
  InvalidNativeScriptType = 701,

  /** @brief The Plutus cost model is invalid. */
  InvalidPlutusCostModel = 702,

  /** @brief The governance action type is invalid. */
  InvalidProcedureProposalType = 800,

  /** @brief The metadatum conversion is invalid. */
  InvalidMetadatumConversion = 900,

  /** @brief The metadata text exceeds the permitted size. */
  InvalidMetadatumTextStringSize = 901,

  /** @brief The metadata bytes exceed the permitted size. */
  InvalidMetadatumBoundedBytesSize = 902,

  /** @brief The HTTP request is invalid. */
  InvalidHttpRequest = 1000,

  /** @brief The available balance cannot cover the requested outputs. */
  BalanceInsufficient = 1100,

  /** @brief There are too few UTxOs for the required output selection. */
  UtxoNotFragmentedEnough = 1101,

  /** @brief Coin selection exhausted the eligible UTxOs. */
  UtxoFullyDepleted = 1102,

  /** @brief Coin selection would exceed the input limit. */
  MaximumInputCountExceeded = 1103,

  /** @brief Sub-transactions do not conserve value as required by the scripts in the batch. */
  UnbalancedSubTransactions = 1104,

  /** @brief The transaction contains witness datums that are not allowed. */
  NotAllowedSupplementalDatums = 1105,

  /** @brief A Plutus script purpose has no redeemer. */
  MissingRedeemer = 1106,

  /** @brief The transaction script evaluation failed. */
  ScriptEvaluationFailure = 1200
};
} // namespace Cometa

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Creates a standard error code for a Cometa status.
 *
 * @param code The operation status.
 *
 * @return The status in the Cometa error category.
 */
std::error_code makeErrorCode(ErrorCode code) noexcept;
} // namespace Cometa
