/**
 * @file CredentialSet.h
 *
 * @brief Sorted unique Credential collections.
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

#include <Cometa/Common/Credential.h>

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_credential_set_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A sorted collection of unique Credential values.
 */
class CredentialSet
{
  public:

    /**
     * @brief Creates an empty set.
     */
    CredentialSet();

    /**
     * @brief Copies values into a set, ignoring duplicates.
     *
     * @param values The elements to copy.
     */
    CredentialSet(std::initializer_list<Credential> values);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The set to copy.
     */
    CredentialSet(const CredentialSet& other);

    /**
     * @brief Moves another set.
     *
     * @param other The source set.
     */
    CredentialSet(CredentialSet&& other) noexcept;

    /**
     * @brief Destroys the set.
     */
    ~CredentialSet();

  public:

    /**
     * @brief Parses a CBOR array, optionally tagged with set tag 258.
     *
     * @param cbor CBOR bytes or hexadecimal without a prefix.
     *
     * @return The sorted set.
     *
     * @throws Error The CBOR or element representation is invalid.
     * @throws std::out_of_range A CBOR tag exceeds the supported range.
     * @throws std::invalid_argument The input contains duplicates, malformed hex, or trailing data.
     */
    static CredentialSet fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Parses a CBOR array, optionally tagged with set tag 258.
     *
     * @param cborHex CBOR bytes or hexadecimal without a prefix.
     *
     * @return The sorted set.
     *
     * @throws Error The CBOR or element representation is invalid.
     * @throws std::out_of_range A CBOR tag exceeds the supported range.
     * @throws std::invalid_argument The input contains duplicates, malformed hex, or trailing data.
     */
    static CredentialSet fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Counts the set elements.
     *
     * @return The element count.
     */
    std::size_t size() const;

    /**
     * @brief Checks whether the set has no elements.
     *
     * @return True for an empty set.
     */
    bool empty() const;

    /**
     * @brief Checks whether an element is present.
     *
     * @param value The element to find.
     *
     * @return True if an equal element exists.
     */
    bool contains(const Credential& value) const;

    /**
     * @brief Copies an element into the set.
     *
     * @param value The element to copy.
     *
     * @return True if inserted, or false if already present; unchanged on failure.
     */
    bool insert(const Credential& value);

    /**
     * @brief Copies an element by its sorted position.
     *
     * @param index The zero-based sorted position.
     *
     * @return An independent element.
     *
     * @throws std::out_of_range The index is outside the set.
     */
    Credential at(std::size_t index) const;

    /**
     * @brief Copies the sorted elements.
     *
     * @return Independent elements in sorted order.
     */
    std::vector<Credential> values() const;

    /**
     * @brief Encodes the sorted set with CBOR tag 258.
     *
     * @return A tagged definite-length array, including when input was untagged or indefinite.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the set as a JSON array.
     *
     * @return Compact JSON in sorted order.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges sets.
     *
     * @param other The set to exchange.
     */
    void swap(CredentialSet& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source set.
     *
     * @return This set; unchanged on failure.
     */
    CredentialSet& operator=(const CredentialSet& other);

    /**
     * @brief Moves another set into this one.
     *
     * @param other The source set; self-move is safe.
     *
     * @return This set.
     */
    CredentialSet& operator=(CredentialSet&& other) noexcept;

    /**
     * @brief Checks equality of elements.
     *
     * @param other The set to compare.
     *
     * @return The comparison result.
     */
    bool operator==(const CredentialSet& other) const;

    /**
     * @brief Checks inequality of elements.
     *
     * @param other The set to compare.
     *
     * @return The comparison result.
     */
    bool operator!=(const CredentialSet& other) const;

  private:

    /**
     * @brief Adopts one owned native set.
     *
     * @param value The owned reference to transfer.
     */
    explicit CredentialSet(cardano_credential_set_t* value) noexcept;

  private:

    /**
     * @brief Checks lifetime and gets the native set.
     *
     * @return The borrowed native reference.
     *
     * @throws std::logic_error The set has been moved from.
     */
    cardano_credential_set_t* checked() const;

  private:

    /**
     * @brief The owned native set, or null after a move.
     */
    cardano_credential_set_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges sets through argument-dependent lookup.
 *
 * @param lhs The first set.
 * @param rhs The second set.
 */
void swap(CredentialSet& lhs, CredentialSet& rhs) noexcept;
} // namespace Cometa
