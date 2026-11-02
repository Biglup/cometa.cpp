/**
 * @file ExUnitPrices.h
 *
 * @brief Prices for Plutus execution units.
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

#include <Cometa/Common/UnitInterval.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_ex_unit_prices_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Rational prices for Plutus memory units and CPU steps. */
class ExUnitPrices
{
  public:

    /**
     * @brief Creates execution unit prices.
     * @param memoryPrice Price per memory unit.
     * @param stepsPrice Price per CPU step.
     */
    ExUnitPrices(const UnitInterval& memoryPrice, const UnitInterval& stepsPrice);

    /**
     * @brief Copies execution unit prices.
     * @param other The prices to copy.
     */
    ExUnitPrices(const ExUnitPrices& other);

    /**
     * @brief Moves execution unit prices.
     * @param other The prices to move.
     */
    ExUnitPrices(ExUnitPrices&& other) noexcept;

    /** @brief Destroys the prices. */
    ~ExUnitPrices();

  public:

    /**
     * @brief Decodes execution unit prices from CBOR.
     * @param cbor The encoded prices.
     * @return The decoded prices.
     */
    static ExUnitPrices fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded prices.
     */
    static ExUnitPrices fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the price per memory unit.
     * @return The memory price.
     */
    UnitInterval memoryPrice() const;

    /**
     * @brief Gets the price per CPU step.
     * @return The step price.
     */
    UnitInterval stepsPrice() const;

    /**
     * @brief Sets the price per memory unit.
     * @param value The new memory price.
     */
    void setMemoryPrice(const UnitInterval& value);

    /**
     * @brief Sets the price per CPU step.
     * @param value The new step price.
     */
    void setStepsPrice(const UnitInterval& value);

    /**
     * @brief Encodes the prices as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the prices as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges prices.
     * @param other The prices to exchange.
     */
    void swap(ExUnitPrices& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The prices to copy.
     * @return This object.
     */
    ExUnitPrices& operator=(const ExUnitPrices& other);

    /**
     * @brief Assigns moved prices.
     * @param other The prices to move.
     * @return This object.
     */
    ExUnitPrices& operator=(ExUnitPrices&& other) noexcept;

    /**
     * @brief Compares prices.
     * @param other The prices to compare.
     * @return True when equal.
     */
    bool operator==(const ExUnitPrices& other) const;

    /**
     * @brief Compares prices.
     * @param other The prices to compare.
     * @return True when different.
     */
    bool operator!=(const ExUnitPrices& other) const;

    /** @brief Allows protocol updates to copy native values. */
    friend class ProtocolParamUpdate;

    /** @brief Allows protocol parameters to copy native values. */
    friend class ProtocolParameters;

  private:

    /**
     * @brief Adopts owned prices.
     * @param value The owned prices.
     */
    explicit ExUnitPrices(cardano_ex_unit_prices_t* value) noexcept;

    /**
     * @brief Gets the stored prices.
     * @return The borrowed prices.
     */
    cardano_ex_unit_prices_t* checked() const;

  private:

    /** @brief The owned prices. */
    cardano_ex_unit_prices_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges execution unit prices through argument-dependent lookup.
 * @param lhs The first price pair.
 * @param rhs The second price pair.
 */
void swap(ExUnitPrices& lhs, ExUnitPrices& rhs) noexcept;
} // namespace Cometa
