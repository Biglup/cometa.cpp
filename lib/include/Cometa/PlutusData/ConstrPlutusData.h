/**
 * @file ConstrPlutusData.h
 *
 * @brief A constructor alternative and its ordered Plutus data fields.
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

#include <Cometa/PlutusData/PlutusList.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_constr_plutus_data_t;

namespace Cometa
{

/* CLASS DECLARATION *********************************************************/

/**
 * @brief A constructor alternative and its ordered Plutus data fields.
 *
 * The alternative selects a case in a validator's data schema. Field order and types must match that
 * schema; this type does not interpret their application meaning.
 */
class ConstrPlutusData
{
  public:

    /**
     * @brief Initializes a constructor.
     *
     * @param alternative The constructor index.
     * @param fields The ordered fields to copy.
     */
    ConstrPlutusData(std::uint64_t alternative, const PlutusList& fields);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    ConstrPlutusData(const ConstrPlutusData& other);

    /**
     * @brief Moves a value into this instance.
     *
     * @param other The value to move.
     */
    ConstrPlutusData(ConstrPlutusData&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~ConstrPlutusData();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static ConstrPlutusData fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static ConstrPlutusData fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the constructor index.
     *
     * @return The constructor index.
     */
    std::uint64_t alternative() const;

    /**
     * @brief Replaces the constructor index.
     *
     * @param alternative The new constructor index.
     */
    void setAlternative(std::uint64_t alternative);

    /**
     * @brief Gets independent constructor fields.
     *
     * @return The copied fields.
     */
    PlutusList fields() const;

    /**
     * @brief Replaces the constructor fields.
     *
     * @param fields The ordered fields to copy.
     */
    void setFields(const PlutusList& fields);

    /**
     * @brief Serializes the value as CBOR.
     *
     * @return The encoded bytes; decoded Plutus data retains its original encoding until changed.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the value as CIP-116 JSON.
     *
     * @return The compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Rebuilds subsequent CBOR encodings from the current fields.
     *
     * @remark Recursively discards original encodings; datum hashes may change.
     */
    void normalizeEncoding();

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(ConstrPlutusData& other) noexcept;

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
    ConstrPlutusData& operator=(const ConstrPlutusData& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    ConstrPlutusData& operator=(ConstrPlutusData&& other) noexcept;

    /**
     * @brief Compares data values for equality.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const ConstrPlutusData& other) const;

    /**
     * @brief Compares data values for inequality.
     *
     * @param other The value to compare.
     *
     * @return True if the values differ.
     */
    bool operator!=(const ConstrPlutusData& other) const;

  private:

    /** @brief Allows PlutusData to copy components. */
    friend class PlutusData;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The non-null owned reference.
     */
    explicit ConstrPlutusData(cardano_constr_plutus_data_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_constr_plutus_data_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_constr_plutus_data_t* m_value;
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
void swap(ConstrPlutusData& lhs, ConstrPlutusData& rhs) noexcept;
} // namespace Cometa
