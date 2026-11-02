/**
 * @file UnitInterval.h
 *
 * @brief An unsigned rational number encoded with CBOR tag 30.
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

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_unit_interval_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief An unsigned rational number encoded with CBOR tag 30.
 *
 * The stored numerator and denominator are preserved without reduction. Equality
 * compares both components. Zero denominators can be represented but cannot be
 * converted to floating point. Values are not restricted to the range [0, 1].
 */
class UnitInterval
{
  public:

    /**
     * @brief Initializes the value.
     *
     * @param numerator Numerator.
     * @param denominator Denominator.
     */
    UnitInterval(std::uint64_t numerator, std::uint64_t denominator);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     *
     * @throws std::logic_error The source has been moved from.
     */
    UnitInterval(const UnitInterval& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    UnitInterval(UnitInterval&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~UnitInterval();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded bytes.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument Input is empty, hex is invalid, or trailing bytes remain.
     * @throws Error The CBOR input is invalid.
     */
    static UnitInterval fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cborHex Hexadecimal CBOR without a 0x prefix.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument Input is empty, hex is invalid, or trailing bytes remain.
     * @throws Error The CBOR input is invalid.
     */
    static UnitInterval fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Approximates a nonnegative number as an unsigned rational.
     *
     * @param value A finite, nonnegative number. The fractional part is truncated to 15 decimal places.
     *
     * @return The rational approximation, with a nonzero denominator.
     *
     * @throws std::invalid_argument The number is negative or not finite.
     * @throws std::overflow_error The numerator cannot fit in 64 bits.
     */
    static UnitInterval fromDouble(double value);

  public:

    /**
     * @brief Converts the ratio to floating point.
     *
     * @return The numerator divided by the denominator, rounded to double precision.
     *
     * @throws std::domain_error The denominator is zero.
     */
    double toDouble() const;

    /**
     * @brief Gets the numerator.
     *
     * @return Numerator.
     */
    std::uint64_t numerator() const;

    /**
     * @brief Replaces the numerator.
     *
     * @param numerator Numerator.
     */
    void setNumerator(std::uint64_t numerator);

    /**
     * @brief Gets the denominator.
     *
     * @return Denominator.
     */
    std::uint64_t denominator() const;

    /**
     * @brief Replaces the denominator.
     *
     * @param denominator Denominator.
     */
    void setDenominator(std::uint64_t denominator);

    /**
     * @brief Encodes the value as CBOR.
     *
     * @return The CBOR bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Encodes the value as CIP-116 JSON.
     *
     * @return The compact JSON string.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges two values.
     *
     * @param other The value to exchange with this instance.
     */
    void swap(UnitInterval& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The value to copy.
     *
     * @return This instance. If copying fails, its value is unchanged.
     */
    UnitInterval& operator=(const UnitInterval& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source; self-move assignment is safe.
     *
     * @return This instance.
     */
    UnitInterval& operator=(UnitInterval&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const UnitInterval& other) const;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True if the values are different.
     */
    bool operator!=(const UnitInterval& other) const;

    /** @brief Allows PoolParams to copy pool components. */
    friend class PoolParams;

    /** @brief Allows ExUnitPrices to copy price components. */
    friend class ExUnitPrices;

    /** @brief Allows voting thresholds to copy their components. */
    friend class DRepVotingThresholds;

    /** @brief Allows voting thresholds to copy their components. */
    friend class PoolVotingThresholds;

    /** @brief Allows protocol updates to copy native values. */
    friend class ProtocolParamUpdate;

    /** @brief Allows protocol parameters to copy native values. */
    friend class ProtocolParameters;

    /** @brief Allows committees to copy quorum thresholds. */
    friend class Committee;

  private:

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference transferred to this instance.
     */
    explicit UnitInterval(cardano_unit_interval_t* value) noexcept;

  private:

    /**
     * @brief Gets the stored value after checking its lifetime.
     *
     * @return The reference, without transferring ownership.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_unit_interval_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_unit_interval_t* m_value;
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
void swap(UnitInterval& lhs, UnitInterval& rhs) noexcept;
} // namespace Cometa
