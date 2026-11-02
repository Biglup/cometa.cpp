/**
 * @file Redeemer.h
 *
 * @brief Script redeemer data and execution budget.
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

#include <Cometa/Common/ExUnits.h>
#include <Cometa/PlutusData/PlutusData.h>
#include <Cometa/WitnessSet/RedeemerTag.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_redeemer_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief A script purpose, index, data value, and execution budget. */
class Redeemer
{
  public:

    /**
     * @brief Creates a redeemer.
     * @param tag The script purpose.
     * @param index The index within that purpose.
     * @param data The script data.
     * @param exUnits The execution budget.
     */
    Redeemer(RedeemerTag tag, std::uint64_t index, const PlutusData& data, const ExUnits& exUnits);

    /**
     * @brief Copies a redeemer.
     * @param other The redeemer to copy.
     */
    Redeemer(const Redeemer& other);

    /**
     * @brief Moves a redeemer.
     * @param other The redeemer to move.
     */
    Redeemer(Redeemer&& other) noexcept;

    /** @brief Destroys the redeemer. */
    ~Redeemer();

  public:

    /**
     * @brief Decodes a redeemer from CBOR.
     * @param cbor The encoded bytes.
     * @return The decoded redeemer.
     */
    static Redeemer fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal bytes.
     * @return The decoded redeemer.
     */
    static Redeemer fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the script purpose.
     * @return The script purpose.
     */
    RedeemerTag tag() const;

    /**
     * @brief Sets the script purpose.
     * @param value The new script purpose.
     */
    void setTag(RedeemerTag value);

    /**
     * @brief Gets the index within the script purpose.
     * @return The index.
     */
    std::uint64_t index() const;

    /**
     * @brief Sets the index within the script purpose.
     * @param value The new index.
     */
    void setIndex(std::uint64_t value);

    /**
     * @brief Gets the script data.
     * @return An independent data value.
     */
    PlutusData data() const;

    /**
     * @brief Sets the script data.
     * @param value The new data value.
     */
    void setData(const PlutusData& value);

    /**
     * @brief Gets the execution budget.
     * @return An independent execution budget.
     */
    ExUnits exUnits() const;

    /**
     * @brief Sets the execution budget.
     * @param value The new execution budget.
     */
    void setExUnits(const ExUnits& value);

    /**
     * @brief Encodes the redeemer as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the redeemer as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges redeemers.
     * @param other The redeemer to exchange.
     */
    void swap(Redeemer& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The redeemer to copy.
     * @return This redeemer.
     */
    Redeemer& operator=(const Redeemer& other);

    /**
     * @brief Assigns a moved redeemer.
     * @param other The redeemer to move.
     * @return This redeemer.
     */
    Redeemer& operator=(Redeemer&& other) noexcept;

    /**
     * @brief Compares redeemers.
     * @param other The redeemer to compare.
     * @return True when equal.
     */
    bool operator==(const Redeemer& other) const;

    /**
     * @brief Compares redeemers.
     * @param other The redeemer to compare.
     * @return True when different.
     */
    bool operator!=(const Redeemer& other) const;

  private:

    /** @brief Allows redeemer lists to copy their elements. */
    friend class RedeemerList;

    /**
     * @brief Adopts an owned redeemer.
     * @param value The owned redeemer.
     */
    explicit Redeemer(cardano_redeemer_t* value) noexcept;

    /**
     * @brief Gets the stored redeemer.
     * @return The borrowed redeemer.
     */
    cardano_redeemer_t* checked() const;

  private:

    /** @brief The owned redeemer. */
    cardano_redeemer_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges redeemers.
 * @param lhs The first redeemer.
 * @param rhs The second redeemer.
 */
void swap(Redeemer& lhs, Redeemer& rhs) noexcept;
} // namespace Cometa
