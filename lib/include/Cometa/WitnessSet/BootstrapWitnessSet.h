/**
 * @file BootstrapWitnessSet.h
 *
 * @brief Ledger bootstrap witness collections.
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

#include <Cometa/WitnessSet/BootstrapWitness.h>

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

struct cardano_bootstrap_witness_set_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Byron spending witnesses in insertion order.
 *
 * The collection supplies signatures and address reconstruction data for legacy inputs. Use it
 * alongside ordinary verification-key witnesses when a transaction spends both address families.
 */
class BootstrapWitnessSet
{
  public:

    /** @brief The element type. */
    using value_type = BootstrapWitness;

    /** @brief The lookup value type. */
    using key_type = BootstrapWitness;

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
    BootstrapWitnessSet(std::initializer_list<value_type> values)
    : BootstrapWitnessSet(values.begin(), values.end())
    {
    }

    /**
     * @brief Constructs a collection from a range.
     * @tparam InputIterator The input iterator type.
     * @param first The beginning of the input range.
     * @param last The end of the input range.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    BootstrapWitnessSet(InputIterator first, InputIterator last)
    : BootstrapWitnessSet()
    {
      insert(first, last);
    }

  public:

    /** @brief Creates an empty bootstrap witness collection. */
    BootstrapWitnessSet();

    /**
     * @brief Copies a collection.
     * @param other The collection to copy.
     */
    BootstrapWitnessSet(const BootstrapWitnessSet& other);

    /**
     * @brief Moves a collection.
     * @param other The collection to move.
     */
    BootstrapWitnessSet(BootstrapWitnessSet&& other) noexcept;

    /** @brief Destroys the collection. */
    ~BootstrapWitnessSet();

  public:

    /**
     * @brief Decodes one CBOR collection.
     * @param cbor The encoded collection.
     * @return The decoded collection.
     */
    static BootstrapWitnessSet fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded collection.
     */
    static BootstrapWitnessSet fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of bootstrap witnesses.
     * @return The number of elements.
     */
    std::size_t size() const;

    /**
     * @brief Adds a witness, replacing the signature for the same address root.
     * @param value The bootstrap witness to copy.
     */
    void add(const BootstrapWitness& value);

    /**
     * @brief Gets a bootstrap witness by index.
     * @param index The zero-based index.
     * @return An independent bootstrap witness.
     */
    BootstrapWitness at(std::size_t index) const;

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
    void swap(BootstrapWitnessSet& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The collection to copy.
     * @return This collection.
     */
    BootstrapWitnessSet& operator=(const BootstrapWitnessSet& other);

    /**
     * @brief Assigns a moved collection.
     * @param other The collection to move.
     * @return This collection.
     */
    BootstrapWitnessSet& operator=(BootstrapWitnessSet&& other) noexcept;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when equal.
     */
    bool operator==(const BootstrapWitnessSet& other) const;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when different.
     */
    bool operator!=(const BootstrapWitnessSet& other) const;

  public:

    /**
     * @brief Returns an iterator to the first element.
     * @return The iterator.
     */
    const_iterator begin() const;

    /**
     * @brief Returns an iterator to the past-the-end position.
     * @return The iterator.
     */
    const_iterator end() const;

    /**
     * @brief Returns an iterator to the first element.
     * @return The iterator.
     */
    const_iterator cbegin() const;

    /**
     * @brief Returns an iterator to the past-the-end position.
     * @return The iterator.
     */
    const_iterator cend() const;

    /**
     * @brief Returns an iterator to the last element in reverse traversal.
     * @return The iterator.
     */
    const_reverse_iterator rbegin() const;

    /**
     * @brief Returns an iterator to the position before the first element in reverse traversal.
     * @return The iterator.
     */
    const_reverse_iterator rend() const;

    /**
     * @brief Returns an iterator to the last element in reverse traversal.
     * @return The iterator.
     */
    const_reverse_iterator crbegin() const;

    /**
     * @brief Returns an iterator to the position before the first element in reverse traversal.
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
    explicit BootstrapWitnessSet(cardano_bootstrap_witness_set_t* value) noexcept;

    /**
     * @brief Gets the stored collection.
     * @return The borrowed collection.
     */
    cardano_bootstrap_witness_set_t* checked() const;

  private:

    /** @brief The owned collection. */
    cardano_bootstrap_witness_set_t* m_value;

    /** @brief Stable element storage for read-only iteration. */
    mutable std::vector<value_type> m_elements;

    /** @brief Whether the element storage matches the collection. */
    mutable bool m_materialized = false;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges bootstrap witness collections through argument-dependent lookup.
 * @param lhs The first collection.
 * @param rhs The second collection.
 */
void swap(BootstrapWitnessSet& lhs, BootstrapWitnessSet& rhs) noexcept;
} // namespace Cometa
