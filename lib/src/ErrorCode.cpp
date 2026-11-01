/**
 * @file ErrorCode.cpp
 *
 * @brief Error category and status conversion.
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

#include <Cometa/ErrorCode.h>

#include <cardano/error.h>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
class CometaCategory final : public std::error_category
{
  public:

    const char*
    name() const noexcept override
    {
      return "Cometa";
    }

    std::string
    message(int value) const override
    {
      return cardano_error_to_string(static_cast<cardano_error_t>(value));
    }
};

static_assert(static_cast<int>(ErrorCode::Success) == CARDANO_SUCCESS);
static_assert(static_cast<int>(ErrorCode::Generic) == CARDANO_ERROR_GENERIC);
static_assert(static_cast<int>(ErrorCode::InsufficientBufferSize) == CARDANO_ERROR_INSUFFICIENT_BUFFER_SIZE);
static_assert(static_cast<int>(ErrorCode::PointerIsNull) == CARDANO_ERROR_POINTER_IS_NULL);
static_assert(static_cast<int>(ErrorCode::MemoryAllocationFailed) == CARDANO_ERROR_MEMORY_ALLOCATION_FAILED);
static_assert(static_cast<int>(ErrorCode::OutOfBoundsMemoryRead) == CARDANO_ERROR_OUT_OF_BOUNDS_MEMORY_READ);
static_assert(static_cast<int>(ErrorCode::OutOfBoundsMemoryWrite) == CARDANO_ERROR_OUT_OF_BOUNDS_MEMORY_WRITE);
static_assert(static_cast<int>(ErrorCode::InvalidArgument) == CARDANO_ERROR_INVALID_ARGUMENT);
static_assert(static_cast<int>(ErrorCode::InvalidUrl) == CARDANO_ERROR_INVALID_URL);
static_assert(static_cast<int>(ErrorCode::ElementNotFound) == CARDANO_ERROR_ELEMENT_NOT_FOUND);
static_assert(static_cast<int>(ErrorCode::Encoding) == CARDANO_ERROR_ENCODING);
static_assert(static_cast<int>(ErrorCode::Decoding) == CARDANO_ERROR_DECODING);
static_assert(static_cast<int>(ErrorCode::ChecksumMismatch) == CARDANO_ERROR_CHECKSUM_MISMATCH);
static_assert(static_cast<int>(ErrorCode::InvalidJson) == CARDANO_ERROR_INVALID_JSON);
static_assert(static_cast<int>(ErrorCode::IntegerOverflow) == CARDANO_ERROR_INTEGER_OVERFLOW);
static_assert(static_cast<int>(ErrorCode::IntegerUnderflow) == CARDANO_ERROR_INTEGER_UNDERFLOW);
static_assert(static_cast<int>(ErrorCode::ConversionFailed) == CARDANO_ERROR_CONVERSION_FAILED);
static_assert(static_cast<int>(ErrorCode::IndexOutOfBounds) == CARDANO_ERROR_INDEX_OUT_OF_BOUNDS);
static_assert(static_cast<int>(ErrorCode::InvalidCertificateType) == CARDANO_ERROR_INVALID_CERTIFICATE_TYPE);
static_assert(static_cast<int>(ErrorCode::NotImplemented) == CARDANO_ERROR_NOT_IMPLEMENTED);
static_assert(static_cast<int>(ErrorCode::InvalidPassphrase) == CARDANO_ERROR_INVALID_PASSPHRASE);
static_assert(static_cast<int>(ErrorCode::IllegalState) == CARDANO_ERROR_ILLEGAL_STATE);
static_assert(static_cast<int>(ErrorCode::DuplicatedKey) == CARDANO_ERROR_DUPLICATED_KEY);
static_assert(static_cast<int>(ErrorCode::JsonTypeMismatch) == CARDANO_ERROR_JSON_TYPE_MISMATCH);
static_assert(static_cast<int>(ErrorCode::LossOfPrecision) == CARDANO_ERROR_LOSS_OF_PRECISION);
static_assert(static_cast<int>(ErrorCode::InvalidMagic) == CARDANO_ERROR_INVALID_MAGIC);
static_assert(static_cast<int>(ErrorCode::InvalidChecksum) == CARDANO_ERROR_INVALID_CHECKSUM);
static_assert(static_cast<int>(ErrorCode::InvalidBlake2bHashSize) == CARDANO_ERROR_INVALID_BLAKE2B_HASH_SIZE);
static_assert(static_cast<int>(ErrorCode::InvalidEd25519SignatureSize) == CARDANO_ERROR_INVALID_ED25519_SIGNATURE_SIZE);
static_assert(static_cast<int>(ErrorCode::InvalidEd25519PublicKeySize) == CARDANO_ERROR_INVALID_ED25519_PUBLIC_KEY_SIZE);
static_assert(static_cast<int>(ErrorCode::InvalidEd25519PrivateKeySize) == CARDANO_ERROR_INVALID_ED25519_PRIVATE_KEY_SIZE);
static_assert(static_cast<int>(ErrorCode::InvalidBip32PublicKeySize) == CARDANO_ERROR_INVALID_BIP32_PUBLIC_KEY_SIZE);
static_assert(static_cast<int>(ErrorCode::InvalidBip32PrivateKeySize) == CARDANO_ERROR_INVALID_BIP32_PRIVATE_KEY_SIZE);
static_assert(static_cast<int>(ErrorCode::InvalidBip32DerivationIndex) == CARDANO_ERROR_INVALID_BIP32_DERIVATION_INDEX);
static_assert(static_cast<int>(ErrorCode::UnexpectedCborType) == CARDANO_ERROR_UNEXPECTED_CBOR_TYPE);
static_assert(static_cast<int>(ErrorCode::InvalidCborValue) == CARDANO_ERROR_INVALID_CBOR_VALUE);
static_assert(static_cast<int>(ErrorCode::InvalidCborArraySize) == CARDANO_ERROR_INVALID_CBOR_ARRAY_SIZE);
static_assert(static_cast<int>(ErrorCode::InvalidCborMapSize) == CARDANO_ERROR_INVALID_CBOR_MAP_SIZE);
static_assert(static_cast<int>(ErrorCode::DuplicatedCborMapKey) == CARDANO_ERROR_DUPLICATED_CBOR_MAP_KEY);
static_assert(static_cast<int>(ErrorCode::InvalidCborMapKey) == CARDANO_ERROR_INVALID_CBOR_MAP_KEY);
static_assert(static_cast<int>(ErrorCode::InvalidAddressType) == CARDANO_ERROR_INVALID_ADDRESS_TYPE);
static_assert(static_cast<int>(ErrorCode::InvalidAddressFormat) == CARDANO_ERROR_INVALID_ADDRESS_FORMAT);
static_assert(static_cast<int>(ErrorCode::InvalidCredentialType) == CARDANO_ERROR_INVALID_CREDENTIAL_TYPE);
static_assert(static_cast<int>(ErrorCode::InvalidPlutusDataConversion) == CARDANO_ERROR_INVALID_PLUTUS_DATA_CONVERSION);
static_assert(static_cast<int>(ErrorCode::InvalidDatumType) == CARDANO_ERROR_INVALID_DATUM_TYPE);
static_assert(static_cast<int>(ErrorCode::InvalidScriptLanguage) == CARDANO_ERROR_INVALID_SCRIPT_LANGUAGE);
static_assert(static_cast<int>(ErrorCode::InvalidNativeScriptType) == CARDANO_ERROR_INVALID_NATIVE_SCRIPT_TYPE);
static_assert(static_cast<int>(ErrorCode::InvalidPlutusCostModel) == CARDANO_ERROR_INVALID_PLUTUS_COST_MODEL);
static_assert(static_cast<int>(ErrorCode::InvalidProcedureProposalType) == CARDANO_ERROR_INVALID_PROCEDURE_PROPOSAL_TYPE);
static_assert(static_cast<int>(ErrorCode::InvalidMetadatumConversion) == CARDANO_ERROR_INVALID_METADATUM_CONVERSION);
static_assert(static_cast<int>(ErrorCode::InvalidMetadatumTextStringSize) == CARDANO_ERROR_INVALID_METADATUM_TEXT_STRING_SIZE);
static_assert(static_cast<int>(ErrorCode::InvalidMetadatumBoundedBytesSize) == CARDANO_ERROR_INVALID_METADATUM_BOUNDED_BYTES_SIZE);
static_assert(static_cast<int>(ErrorCode::InvalidHttpRequest) == CARDANO_ERROR_INVALID_HTTP_REQUEST);
static_assert(static_cast<int>(ErrorCode::BalanceInsufficient) == CARDANO_ERROR_BALANCE_INSUFFICIENT);
static_assert(static_cast<int>(ErrorCode::UtxoNotFragmentedEnough) == CARDANO_ERROR_UTXO_NOT_FRAGMENTED_ENOUGH);
static_assert(static_cast<int>(ErrorCode::UtxoFullyDepleted) == CARDANO_ERROR_UTXO_FULLY_DEPLETED);
static_assert(static_cast<int>(ErrorCode::MaximumInputCountExceeded) == CARDANO_ERROR_MAXIMUM_INPUT_COUNT_EXCEEDED);
static_assert(static_cast<int>(ErrorCode::UnbalancedSubTransactions) == CARDANO_ERROR_UNBALANCED_SUB_TRANSACTIONS);
static_assert(static_cast<int>(ErrorCode::NotAllowedSupplementalDatums) == CARDANO_ERROR_NOT_ALLOWED_SUPPLEMENTAL_DATUMS);
static_assert(static_cast<int>(ErrorCode::MissingRedeemer) == CARDANO_ERROR_MISSING_REDEEMER);
static_assert(static_cast<int>(ErrorCode::ScriptEvaluationFailure) == CARDANO_ERROR_SCRIPT_EVALUATION_FAILURE);
} // namespace

/* FUNCTIONS IMPLEMENTATION **************************************************/

std::error_code
makeErrorCode(ErrorCode code) noexcept
{
  static const CometaCategory CATEGORY;
  return { static_cast<int>(code), CATEGORY };
}
} // namespace Cometa
