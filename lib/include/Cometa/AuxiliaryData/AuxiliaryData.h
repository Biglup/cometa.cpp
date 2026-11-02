/**
 * @file AuxiliaryData.h
 *
 * @brief Metadata and scripts attached to a transaction.
 *
 * @author angel.castillo
 * @date   Nov 02, 2026
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

#include <Cometa/AuxiliaryData/PlutusV1ScriptList.h>
#include <Cometa/AuxiliaryData/PlutusV2ScriptList.h>
#include <Cometa/AuxiliaryData/PlutusV3ScriptList.h>
#include <Cometa/AuxiliaryData/PlutusV4ScriptList.h>
#include <Cometa/AuxiliaryData/TransactionMetadata.h>
#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/Scripts/NativeScripts/NativeScriptList.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_auxiliary_data_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Metadata and scripts attached to a transaction. */
class AuxiliaryData
{
  public:

    /**
     * @brief Initializes empty auxiliary data.
     */
    AuxiliaryData();

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    AuxiliaryData(const AuxiliaryData& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    AuxiliaryData(AuxiliaryData&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~AuxiliaryData();

  public:

    /**
     * @brief Decodes exactly one auxiliary-data value.
     *
     * @param cbor The encoded value; tagged and legacy forms are accepted, and trailing bytes are rejected.
     *
     * @return The decoded auxiliary data.
     */
    static AuxiliaryData fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded auxiliary data.
     */
    static AuxiliaryData fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets an independent metadata value.
     *
     * @return The copied value, or nullopt when absent.
     */
    std::optional<TransactionMetadata> metadata() const;

    /**
     * @brief Replaces or removes transaction metadata.
     *
     * @param value The value to copy, or nullopt to remove it.
     */
    void setMetadata(const std::optional<TransactionMetadata>& value);

    /**
     * @brief Gets an independent script collection.
     *
     * @return The copied value, or nullopt when absent.
     */
    std::optional<NativeScriptList> nativeScripts() const;

    /**
     * @brief Replaces or removes the script collection.
     *
     * @param value The value to copy, or nullopt to remove it.
     */
    void setNativeScripts(const std::optional<NativeScriptList>& value);

    /**
     * @brief Gets an independent script collection.
     *
     * @return The copied value, or nullopt when absent.
     */
    std::optional<PlutusV1ScriptList> plutusV1Scripts() const;

    /**
     * @brief Replaces or removes the script collection.
     *
     * @param value The value to copy, or nullopt to remove it.
     */
    void setPlutusV1Scripts(const std::optional<PlutusV1ScriptList>& value);

    /**
     * @brief Gets an independent script collection.
     *
     * @return The copied value, or nullopt when absent.
     */
    std::optional<PlutusV2ScriptList> plutusV2Scripts() const;

    /**
     * @brief Replaces or removes the script collection.
     *
     * @param value The value to copy, or nullopt to remove it.
     */
    void setPlutusV2Scripts(const std::optional<PlutusV2ScriptList>& value);

    /**
     * @brief Gets an independent script collection.
     *
     * @return The copied value, or nullopt when absent.
     */
    std::optional<PlutusV3ScriptList> plutusV3Scripts() const;

    /**
     * @brief Replaces or removes the script collection.
     *
     * @param value The value to copy, or nullopt to remove it.
     */
    void setPlutusV3Scripts(const std::optional<PlutusV3ScriptList>& value);

    /**
     * @brief Gets an independent script collection.
     *
     * @return The copied value, or nullopt when absent.
     */
    std::optional<PlutusV4ScriptList> plutusV4Scripts() const;

    /**
     * @brief Replaces or removes the script collection.
     *
     * @param value The value to copy, or nullopt to remove it.
     */
    void setPlutusV4Scripts(const std::optional<PlutusV4ScriptList>& value);

    /**
     * @brief Hashes the current CBOR representation.
     *
     * @return The 32-byte BLAKE2b hash.
     */
    Blake2bHash hash() const;

    /**
     * @brief Uses the current tagged auxiliary-data form for subsequent CBOR serialization.
     *
     * @remark Discards the original outer encoding; nested script encodings are retained.
     */
    void normalizeEncoding();

    /**
     * @brief Serializes the auxiliary data as CBOR.
     *
     * @return The original decoded bytes, or the current tagged form after editing or normalization.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the auxiliary data as CIP-116 inspection JSON.
     *
     * @return The compact inspection object.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(AuxiliaryData& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This instance.
     *
     * @remark If copying fails, this instance is unchanged.
     */
    AuxiliaryData& operator=(const AuxiliaryData& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    AuxiliaryData& operator=(AuxiliaryData&& other) noexcept;

    /**
     * @brief Compares metadata and attached scripts.
     *
     * @param other The auxiliary data to compare.
     *
     * @return True when the fields are equal, regardless of their original encoding.
     */
    bool operator==(const AuxiliaryData& other) const;

    /**
     * @brief Compares metadata and attached scripts for inequality.
     *
     * @param other The auxiliary data to compare.
     *
     * @return True when the fields differ.
     */
    bool operator!=(const AuxiliaryData& other) const;

  private:

    /** @brief Allows sub-transactions to copy this value. */
    friend class SubTransaction;

    /** @brief Allows transactions to copy this value. */
    friend class Transaction;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit AuxiliaryData(cardano_auxiliary_data_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_auxiliary_data_t* checked() const;

    /**
     * @brief Validates decoded metadata labels and native scripts.
     *
     * @throws std::invalid_argument A nested value is invalid.
     */
    void validate() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_auxiliary_data_t* m_value;
};
} // namespace Cometa

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Exchanges values through argument-dependent lookup.
 *
 * @param lhs The first value.
 * @param rhs The second value.
 */
void swap(AuxiliaryData& lhs, AuxiliaryData& rhs) noexcept;
} // namespace Cometa
