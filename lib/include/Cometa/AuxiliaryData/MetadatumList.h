/**
 * @file MetadatumList.h
 *
 * @brief Declaration of the MetadatumList value type.
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

#include <Cometa/AuxiliaryData/Metadatum.h>

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_metadatum_list_t;

namespace Cometa
{

/* CLASS DECLARATION *********************************************************/

/** @brief An ordered sequence of metadata values. */
class MetadatumList
{
  public:

    /**
     * @brief Initializes an empty collection.
     */
    MetadatumList();

    /**
     * @brief Initializes an ordered list of copied metadata values.
     *
     * @param values The values to copy.
     */
    MetadatumList(std::initializer_list<Metadatum> values);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    MetadatumList(const MetadatumList& other);

    /**
     * @brief Moves a value into this instance.
     *
     * @param other The value to move.
     */
    MetadatumList(MetadatumList&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~MetadatumList();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static MetadatumList fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static MetadatumList fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of entries.
     *
     * @return The entry count, including duplicates.
     */
    std::size_t size() const;

    /**
     * @brief Tests whether the collection is empty.
     *
     * @return True when there are no entries.
     */
    bool empty() const;

    /**
     * @brief Gets an independent element.
     *
     * @param index The zero-based element index.
     *
     * @return The copied element.
     *
     * @throws std::out_of_range The index is not less than size().
     */
    Metadatum at(std::size_t index) const;

    /**
     * @brief Appends a copied element.
     *
     * @param data The element to append.
     */
    void pushBack(const Metadatum& data);

    /**
     * @brief Gets independent elements in order.
     *
     * @return The copied elements.
     */
    std::vector<Metadatum> values() const;

    /**
     * @brief Serializes the value as CBOR.
     *
     * @return The encoded bytes; collection length forms are retained and scalar encodings are normalized.
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
    void swap(MetadatumList& other) noexcept;

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
    MetadatumList& operator=(const MetadatumList& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    MetadatumList& operator=(MetadatumList&& other) noexcept;

    /**
     * @brief Compares data values for equality.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const MetadatumList& other) const;

    /**
     * @brief Compares data values for inequality.
     *
     * @param other The value to compare.
     *
     * @return True if the values differ.
     */
    bool operator!=(const MetadatumList& other) const;

  private:

    /** @brief Allows Metadatum to copy components. */
    friend class Metadatum;

    /** @brief Allows MetadatumMap to copy components. */
    friend class MetadatumMap;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The non-null owned reference.
     */
    explicit MetadatumList(cardano_metadatum_list_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_metadatum_list_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_metadatum_list_t* m_value;
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
void swap(MetadatumList& lhs, MetadatumList& rhs) noexcept;
} // namespace Cometa
