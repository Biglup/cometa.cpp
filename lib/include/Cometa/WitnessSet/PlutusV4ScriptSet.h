/**
 * @file PlutusV4ScriptSet.h
 *
 * @brief Ledger Plutus V4 script collections.
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

#include <Cometa/Scripts/PlutusScripts/PlutusV4Script.h>

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_plutus_v4_script_set_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A collection of ledger Plutus V4 scripts in insertion order.
 */
class PlutusV4ScriptSet
{
  public:

    /** @brief The element type. */
    using value_type = PlutusV4Script;

    /** @brief The lookup value type. */
    using key_type = PlutusV4Script;

    /** @brief The element count type. */
    using size_type = std::size_t;

    /** @brief The iterator distance type. */
    using difference_type = std::ptrdiff_t;

    /** @brief A read-only element reference. */
    using reference = const value_type&;

    /** @brief A read-only element reference. */
    using const_reference = const value_type&;

    /** @brief A read-only random-access iterator. */
    using iterator = std::vector<value_type>::const_iterator;

    /** @brief A read-only random-access iterator. */
    using const_iterator = iterator;

    /** @brief A read-only reverse iterator. */
    using reverse_iterator = std::vector<value_type>::const_reverse_iterator;

    /** @brief A read-only reverse iterator. */
    using const_reverse_iterator = reverse_iterator;

  public:

    /**
     * @brief Constructs a collection from its elements.
     * @param values The elements to copy, in insertion order.
     */
    PlutusV4ScriptSet(std::initializer_list<value_type> values)
    : PlutusV4ScriptSet(values.begin(), values.end())
    {
    }

    /**
     * @brief Constructs a collection from a range.
     * @tparam InputIterator The input iterator type.
     * @param first The beginning of the input range.
     * @param last The end of the input range.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    PlutusV4ScriptSet(InputIterator first, InputIterator last)
    : PlutusV4ScriptSet()
    {
      insert(first, last);
    }

  public:

    /** @brief Creates an empty Plutus V4 script collection. */
    PlutusV4ScriptSet();

    /**
     * @brief Copies a collection.
     * @param other The collection to copy.
     */
    PlutusV4ScriptSet(const PlutusV4ScriptSet& other);

    /**
     * @brief Moves a collection.
     * @param other The collection to move.
     */
    PlutusV4ScriptSet(PlutusV4ScriptSet&& other) noexcept;

    /** @brief Destroys the collection. */
    ~PlutusV4ScriptSet();

  public:

    /**
     * @brief Decodes one CBOR collection.
     * @param cbor The encoded collection.
     * @return The decoded collection.
     */
    static PlutusV4ScriptSet fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded collection.
     */
    static PlutusV4ScriptSet fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of Plutus V4 scripts.
     * @return The number of elements.
     */
    std::size_t size() const;

    /**
     * @brief Adds a Plutus V4 script if its hash is not already present.
     * @param value The Plutus V4 script to copy.
     */
    void add(const PlutusV4Script& value);

    /**
     * @brief Gets a Plutus V4 script by index.
     * @param index The zero-based index.
     * @return An independent Plutus V4 script.
     */
    PlutusV4Script at(std::size_t index) const;

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
    void swap(PlutusV4ScriptSet& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The collection to copy.
     * @return This collection.
     */
    PlutusV4ScriptSet& operator=(const PlutusV4ScriptSet& other);

    /**
     * @brief Assigns a moved collection.
     * @param other The collection to move.
     * @return This collection.
     */
    PlutusV4ScriptSet& operator=(PlutusV4ScriptSet&& other) noexcept;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when equal.
     */
    bool operator==(const PlutusV4ScriptSet& other) const;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when different.
     */
    bool operator!=(const PlutusV4ScriptSet& other) const;

  public:

    /**
     * @brief Gets the begin iterator.
     * @return The iterator.
     */
    const_iterator begin() const;

    /**
     * @brief Gets the end iterator.
     * @return The iterator.
     */
    const_iterator end() const;

    /**
     * @brief Gets the cbegin iterator.
     * @return The iterator.
     */
    const_iterator cbegin() const;

    /**
     * @brief Gets the cend iterator.
     * @return The iterator.
     */
    const_iterator cend() const;

    /**
     * @brief Gets the rbegin iterator.
     * @return The iterator.
     */
    const_reverse_iterator rbegin() const;

    /**
     * @brief Gets the rend iterator.
     * @return The iterator.
     */
    const_reverse_iterator rend() const;

    /**
     * @brief Gets the crbegin iterator.
     * @return The iterator.
     */
    const_reverse_iterator crbegin() const;

    /**
     * @brief Gets the crend iterator.
     * @return The iterator.
     */
    const_reverse_iterator crend() const;

    /**
     * @brief Finds an element by its collection identity.
     * @param value The lookup value.
     * @return An iterator to the element, or end().
     */
    iterator find(const value_type& value) const;

    /**
     * @brief Counts elements with the given identity.
     * @param value The lookup value.
     * @return Zero or one.
     */
    size_type count(const value_type& value) const;

    /**
     * @brief Checks whether the collection is empty.
     * @return True when empty.
     */
    bool empty() const;

    /**
     * @brief Checks whether an identity is present.
     * @param value The lookup value.
     * @return True when present.
     */
    bool contains(const value_type& value) const;

    /**
     * @brief Inserts an element unless its identity is already present.
     * @param value The element to copy.
     * @return The element iterator and whether insertion occurred.
     */
    std::pair<iterator, bool> insert(const value_type& value);

    /**
     * @brief Inserts an element, preserving collection order.
     * @param hint An optional position hint; the collection determines the order.
     * @param value The element to copy.
     * @return The element iterator.
     */
    iterator insert(const_iterator hint, const value_type& value);

    /**
     * @brief Removes all elements.
     */
    void clear();

    /**
     * @brief Erases an element.
     * @param position The iterator to erase.
     * @return An iterator following the removed element.
     */
    iterator erase(const_iterator position);

    /**
     * @brief Erases a range of elements.
     * @param first The first element to erase.
     * @param last The end of the erased range.
     * @return An iterator following the removed range.
     */
    iterator erase(const_iterator first, const_iterator last);

    /**
     * @brief Erases an element by identity.
     * @param value The lookup value.
     * @return The number of erased elements.
     */
    size_type erase(const value_type& value);

    /**
     * @brief Constructs and inserts an element.
     * @tparam Args The constructor argument types.
     * @param args The constructor arguments.
     * @return The element iterator and whether insertion occurred.
     */
    template<class... Args>
    std::pair<iterator, bool>
    emplace(Args&&... args)
    {
      const value_type value(std::forward<Args>(args)...);

      return insert(value);
    }

    /**
     * @brief Inserts a range of elements.
     * @tparam InputIterator The input iterator type.
     * @param first The start of the input range.
     * @param last The end of the input range.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    void
    insert(InputIterator first, InputIterator last)
    {
      for (; first != last; ++first)
      {
        insert(*first);
      }
    }

  private:

    /** @brief Prepares stable read-only element references for iteration. */
    void materialize() const;

    /** @brief Invalidates the element references after a mutation. */
    void invalidateElements() noexcept;

    /**
     * @brief Compares element identities.
     * @param left The first value.
     * @param right The second value.
     * @return True when the values identify the same entry.
     */
    static bool sameKey(const value_type& left, const value_type& right);

  private:

    /** @brief Allows a witness set to copy this field. */
    friend class WitnessSet;

    /**
     * @brief Adopts an owned collection.
     * @param value The owned collection.
     */
    explicit PlutusV4ScriptSet(cardano_plutus_v4_script_set_t* value) noexcept;

    /**
     * @brief Gets the stored collection.
     * @return The borrowed collection.
     */
    cardano_plutus_v4_script_set_t* checked() const;

  private:

    /** @brief The owned collection. */
    cardano_plutus_v4_script_set_t* m_value;

    /** @brief Stable element storage for read-only iteration. */
    mutable std::vector<value_type> m_elements;

    /** @brief Whether the element storage matches the collection. */
    mutable bool m_materialized = false;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges Plutus V4 script collections through argument-dependent lookup.
 * @param lhs The first collection.
 * @param rhs The second collection.
 */
void swap(PlutusV4ScriptSet& lhs, PlutusV4ScriptSet& rhs) noexcept;
} // namespace Cometa
