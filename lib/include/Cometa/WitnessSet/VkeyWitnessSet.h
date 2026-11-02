/**
 * @file VkeyWitnessSet.h
 *
 * @brief Ledger verification-key witness collections.
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

#include <Cometa/WitnessSet/VkeyWitness.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_vkey_witness_set_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A collection of ledger verification-key witnesses in insertion order.
 */
class VkeyWitnessSet
{
  public:

    /** @brief Creates an empty verification-key witness collection. */
    VkeyWitnessSet();

    /**
     * @brief Copies a collection.
     * @param other The collection to copy.
     */
    VkeyWitnessSet(const VkeyWitnessSet& other);

    /**
     * @brief Moves a collection.
     * @param other The collection to move.
     */
    VkeyWitnessSet(VkeyWitnessSet&& other) noexcept;

    /** @brief Destroys the collection. */
    ~VkeyWitnessSet();

  public:

    /**
     * @brief Decodes one CBOR collection.
     * @param cbor The encoded collection.
     * @return The decoded collection.
     */
    static VkeyWitnessSet fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded collection.
     */
    static VkeyWitnessSet fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of verification-key witnesses.
     * @return The number of elements.
     */
    std::size_t size() const;

    /**
     * @brief Adds or updates a witness by verification key.
     * @param value The verification-key witness to copy.
     */
    void add(const VkeyWitness& value);

    /**
     * @brief Gets a verification-key witness by index.
     * @param index The zero-based index.
     * @return An independent verification-key witness.
     */
    VkeyWitness at(std::size_t index) const;

    /**
     * @brief Applies another set, replacing signatures for matching keys.
     * @param other The witnesses to apply.
     */
    void apply(const VkeyWitnessSet& other);

    /**
     * @brief Reports whether the CBOR set tag is included.
     * @return True when the set tag is included.
     */
    bool useTag() const;

    /**
     * @brief Selects whether to include the CBOR set tag.
     * @param value True to include the tag.
     */
    void setUseTag(bool value);

    /**
     * @brief Encodes the collection as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the collection as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges collections.
     * @param other The collection to exchange.
     */
    void swap(VkeyWitnessSet& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The collection to copy.
     * @return This collection.
     */
    VkeyWitnessSet& operator=(const VkeyWitnessSet& other);

    /**
     * @brief Assigns a moved collection.
     * @param other The collection to move.
     * @return This collection.
     */
    VkeyWitnessSet& operator=(VkeyWitnessSet&& other) noexcept;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when equal.
     */
    bool operator==(const VkeyWitnessSet& other) const;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when different.
     */
    bool operator!=(const VkeyWitnessSet& other) const;

  private:

    /** @brief Allows sub-transactions to copy this value. */
    friend class SubTransaction;

    /** @brief Allows a witness set to copy this field. */
    friend class WitnessSet;

    /**
     * @brief Adopts an owned collection.
     * @param value The owned collection.
     */
    explicit VkeyWitnessSet(cardano_vkey_witness_set_t* value) noexcept;

    /**
     * @brief Gets the stored collection.
     * @return The borrowed collection.
     */
    cardano_vkey_witness_set_t* checked() const;

  private:

    /** @brief The owned collection. */
    cardano_vkey_witness_set_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges verification-key witness collections through argument-dependent lookup.
 * @param lhs The first collection.
 * @param rhs The second collection.
 */
void swap(VkeyWitnessSet& lhs, VkeyWitnessSet& rhs) noexcept;
} // namespace Cometa
