/**
 * @file Datum.h
 *
 * @brief Declaration of the Datum value type.
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

#include <Cometa/Common/DatumType.h>
#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/PlutusData/PlutusData.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_datum_t;

namespace Cometa
{

/* CLASS DECLARATION *********************************************************/

/** @brief An output datum hash or inline Plutus data. */
class Datum
{
  public:

    /**
     * @brief Initializes a datum hash.
     *
     * @param hash The 32-byte hash to copy.
     */
    explicit Datum(const Blake2bHash& hash);

    /**
     * @brief Initializes inline datum data.
     *
     * @param data The data to copy.
     */
    explicit Datum(const PlutusData& data);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    Datum(const Datum& other);

    /**
     * @brief Moves a value into this instance.
     *
     * @param other The value to move.
     */
    Datum(Datum&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~Datum();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static Datum fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static Datum fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the datum discriminator.
     *
     * @return Hash or inline data.
     */
    DatumType type() const;

    /**
     * @brief Gets independent inline data.
     *
     * @return The copied inline data, or nullopt for a datum hash.
     */
    std::optional<PlutusData> inlineData() const;

    /**
     * @brief Gets the datum hash.
     *
     * @return The stored hash, or the Blake2b-256 hash of the inline data CBOR.
     */
    Blake2bHash hash() const;

    /**
     * @brief Replaces a hash datum with another hash.
     *
     * @param hash The 32-byte replacement hash.
     *
     * @throws std::logic_error This datum contains inline data.
     */
    void setHash(const Blake2bHash& hash);

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
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(Datum& other) noexcept;

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
    Datum& operator=(const Datum& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    Datum& operator=(Datum&& other) noexcept;

    /**
     * @brief Compares data values for equality.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const Datum& other) const;

    /**
     * @brief Compares data values for inequality.
     *
     * @param other The value to compare.
     *
     * @return True if the values differ.
     */
    bool operator!=(const Datum& other) const;

  private:

    /** @brief Allows TransactionOutput to copy components. */
    friend class TransactionOutput;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The non-null owned reference.
     */
    explicit Datum(cardano_datum_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_datum_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_datum_t* m_value;
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
void swap(Datum& lhs, Datum& rhs) noexcept;
} // namespace Cometa
