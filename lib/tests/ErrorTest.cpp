/**
 * @file ErrorTest.cpp
 *
 * @brief Unit tests for error reporting.
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

/* INCLUDES ******************************************************************/

#include <Cometa/AssetName.h>
#include <Cometa/Error.h>

#include <cardano/error.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Error, ExposesDocumentedCodeAndOperation)
{
  // Arrange
  const ErrorCode code = ErrorCode::InvalidArgument;

  // Act
  const Error error(code, "setHash");

  // Assert
  EXPECT_EQ(error.code(), makeErrorCode(code));
  EXPECT_STREQ(error.code().category().name(), "Cometa");
  EXPECT_NE(std::string(error.what()).find("setHash"), std::string::npos);
  EXPECT_FALSE(error.code().message().empty());
  EXPECT_NE(error.code(), std::make_error_code(std::errc::invalid_argument));
}

TEST(Error, DecoderUsesDocumentedCodes)
{
  // Arrange
  const std::vector<std::uint8_t> cbor { 0x01 };

  // Act & Assert
  try
  {
    const AssetName value = AssetName::fromCbor(cbor);
    FAIL() << "Expected a byte-string decoding error";
  }
  catch (const Error& error)
  {
    EXPECT_EQ(error.code(), makeErrorCode(ErrorCode::Decoding));
  }
}

TEST(ErrorCode, MatchesCardanoStatuses)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(ErrorCode::Success), CARDANO_SUCCESS);
  EXPECT_EQ(static_cast<int>(ErrorCode::Generic), CARDANO_ERROR_GENERIC);
  EXPECT_EQ(static_cast<int>(ErrorCode::InsufficientBufferSize), CARDANO_ERROR_INSUFFICIENT_BUFFER_SIZE);
  EXPECT_EQ(static_cast<int>(ErrorCode::PointerIsNull), CARDANO_ERROR_POINTER_IS_NULL);
  EXPECT_EQ(static_cast<int>(ErrorCode::MemoryAllocationFailed), CARDANO_ERROR_MEMORY_ALLOCATION_FAILED);
  EXPECT_EQ(static_cast<int>(ErrorCode::OutOfBoundsMemoryRead), CARDANO_ERROR_OUT_OF_BOUNDS_MEMORY_READ);
  EXPECT_EQ(static_cast<int>(ErrorCode::OutOfBoundsMemoryWrite), CARDANO_ERROR_OUT_OF_BOUNDS_MEMORY_WRITE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidArgument), CARDANO_ERROR_INVALID_ARGUMENT);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidUrl), CARDANO_ERROR_INVALID_URL);
  EXPECT_EQ(static_cast<int>(ErrorCode::ElementNotFound), CARDANO_ERROR_ELEMENT_NOT_FOUND);
  EXPECT_EQ(static_cast<int>(ErrorCode::Encoding), CARDANO_ERROR_ENCODING);
  EXPECT_EQ(static_cast<int>(ErrorCode::Decoding), CARDANO_ERROR_DECODING);
  EXPECT_EQ(static_cast<int>(ErrorCode::ChecksumMismatch), CARDANO_ERROR_CHECKSUM_MISMATCH);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidJson), CARDANO_ERROR_INVALID_JSON);
  EXPECT_EQ(static_cast<int>(ErrorCode::IntegerOverflow), CARDANO_ERROR_INTEGER_OVERFLOW);
  EXPECT_EQ(static_cast<int>(ErrorCode::IntegerUnderflow), CARDANO_ERROR_INTEGER_UNDERFLOW);
  EXPECT_EQ(static_cast<int>(ErrorCode::ConversionFailed), CARDANO_ERROR_CONVERSION_FAILED);
  EXPECT_EQ(static_cast<int>(ErrorCode::IndexOutOfBounds), CARDANO_ERROR_INDEX_OUT_OF_BOUNDS);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidCertificateType), CARDANO_ERROR_INVALID_CERTIFICATE_TYPE);
  EXPECT_EQ(static_cast<int>(ErrorCode::NotImplemented), CARDANO_ERROR_NOT_IMPLEMENTED);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidPassphrase), CARDANO_ERROR_INVALID_PASSPHRASE);
  EXPECT_EQ(static_cast<int>(ErrorCode::IllegalState), CARDANO_ERROR_ILLEGAL_STATE);
  EXPECT_EQ(static_cast<int>(ErrorCode::DuplicatedKey), CARDANO_ERROR_DUPLICATED_KEY);
  EXPECT_EQ(static_cast<int>(ErrorCode::JsonTypeMismatch), CARDANO_ERROR_JSON_TYPE_MISMATCH);
  EXPECT_EQ(static_cast<int>(ErrorCode::LossOfPrecision), CARDANO_ERROR_LOSS_OF_PRECISION);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidMagic), CARDANO_ERROR_INVALID_MAGIC);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidChecksum), CARDANO_ERROR_INVALID_CHECKSUM);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidBlake2bHashSize), CARDANO_ERROR_INVALID_BLAKE2B_HASH_SIZE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidEd25519SignatureSize), CARDANO_ERROR_INVALID_ED25519_SIGNATURE_SIZE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidEd25519PublicKeySize), CARDANO_ERROR_INVALID_ED25519_PUBLIC_KEY_SIZE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidEd25519PrivateKeySize), CARDANO_ERROR_INVALID_ED25519_PRIVATE_KEY_SIZE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidBip32PublicKeySize), CARDANO_ERROR_INVALID_BIP32_PUBLIC_KEY_SIZE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidBip32PrivateKeySize), CARDANO_ERROR_INVALID_BIP32_PRIVATE_KEY_SIZE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidBip32DerivationIndex), CARDANO_ERROR_INVALID_BIP32_DERIVATION_INDEX);
  EXPECT_EQ(static_cast<int>(ErrorCode::UnexpectedCborType), CARDANO_ERROR_UNEXPECTED_CBOR_TYPE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidCborValue), CARDANO_ERROR_INVALID_CBOR_VALUE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidCborArraySize), CARDANO_ERROR_INVALID_CBOR_ARRAY_SIZE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidCborMapSize), CARDANO_ERROR_INVALID_CBOR_MAP_SIZE);
  EXPECT_EQ(static_cast<int>(ErrorCode::DuplicatedCborMapKey), CARDANO_ERROR_DUPLICATED_CBOR_MAP_KEY);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidCborMapKey), CARDANO_ERROR_INVALID_CBOR_MAP_KEY);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidAddressType), CARDANO_ERROR_INVALID_ADDRESS_TYPE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidAddressFormat), CARDANO_ERROR_INVALID_ADDRESS_FORMAT);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidCredentialType), CARDANO_ERROR_INVALID_CREDENTIAL_TYPE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidPlutusDataConversion), CARDANO_ERROR_INVALID_PLUTUS_DATA_CONVERSION);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidDatumType), CARDANO_ERROR_INVALID_DATUM_TYPE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidScriptLanguage), CARDANO_ERROR_INVALID_SCRIPT_LANGUAGE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidNativeScriptType), CARDANO_ERROR_INVALID_NATIVE_SCRIPT_TYPE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidPlutusCostModel), CARDANO_ERROR_INVALID_PLUTUS_COST_MODEL);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidProcedureProposalType), CARDANO_ERROR_INVALID_PROCEDURE_PROPOSAL_TYPE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidMetadatumConversion), CARDANO_ERROR_INVALID_METADATUM_CONVERSION);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidMetadatumTextStringSize), CARDANO_ERROR_INVALID_METADATUM_TEXT_STRING_SIZE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidMetadatumBoundedBytesSize), CARDANO_ERROR_INVALID_METADATUM_BOUNDED_BYTES_SIZE);
  EXPECT_EQ(static_cast<int>(ErrorCode::InvalidHttpRequest), CARDANO_ERROR_INVALID_HTTP_REQUEST);
  EXPECT_EQ(static_cast<int>(ErrorCode::BalanceInsufficient), CARDANO_ERROR_BALANCE_INSUFFICIENT);
  EXPECT_EQ(static_cast<int>(ErrorCode::UtxoNotFragmentedEnough), CARDANO_ERROR_UTXO_NOT_FRAGMENTED_ENOUGH);
  EXPECT_EQ(static_cast<int>(ErrorCode::UtxoFullyDepleted), CARDANO_ERROR_UTXO_FULLY_DEPLETED);
  EXPECT_EQ(static_cast<int>(ErrorCode::MaximumInputCountExceeded), CARDANO_ERROR_MAXIMUM_INPUT_COUNT_EXCEEDED);
  EXPECT_EQ(static_cast<int>(ErrorCode::UnbalancedSubTransactions), CARDANO_ERROR_UNBALANCED_SUB_TRANSACTIONS);
  EXPECT_EQ(static_cast<int>(ErrorCode::NotAllowedSupplementalDatums), CARDANO_ERROR_NOT_ALLOWED_SUPPLEMENTAL_DATUMS);
  EXPECT_EQ(static_cast<int>(ErrorCode::MissingRedeemer), CARDANO_ERROR_MISSING_REDEEMER);
  EXPECT_EQ(static_cast<int>(ErrorCode::ScriptEvaluationFailure), CARDANO_ERROR_SCRIPT_EVALUATION_FAILURE);
}
