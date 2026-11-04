/**
 * @file TransactionInputSet.h
 *
 * @brief Unique input references ordered by transaction ID and output index.
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

#include <Cometa/TransactionBody/TransactionInput.h>

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

struct cardano_transaction_input_set_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Unique input references ordered by transaction ID and output index.
 *
 * Ledger input order determines spending redeemer indices. Insertion order therefore does not
 * determine a spending redeemer's index.
 */
class TransactionInputSet
{
  public:

    /** @brief The element type. */
    using value_type = TransactionInput;

    /** @brief The lookup value type. */
    using key_type = TransactionInput;

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
     * @brief Constructs a collection from a range.
     * @tparam InputIterator The input iterator type.
     * @param first The beginning of the input range.
     * @param last The end of the input range.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    TransactionInputSet(InputIterator first, InputIterator last)
    : TransactionInputSet()
    {
      insert(first, last);
    }

  public:

    /**
     * @brief Creates an empty set.
     */
    TransactionInputSet();

    /**
     * @brief Copies values into a set, ignoring duplicates.
     *
     * @param values The elements to copy.
     */
    TransactionInputSet(std::initializer_list<TransactionInput> values);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The set to copy.
     */
    TransactionInputSet(const TransactionInputSet& other);

    /**
     * @brief Moves another set.
     *
     * @param other The source set.
     */
    TransactionInputSet(TransactionInputSet&& other) noexcept;

    /**
     * @brief Destroys the set.
     */
    ~TransactionInputSet();

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
    static TransactionInputSet fromCbor(const std::vector<std::uint8_t>& cbor);

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
    static TransactionInputSet fromCbor(std::string_view cborHex);

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
     * @brief Reports whether the decoded input included set tag 258.
     *
     * @return True for newly constructed sets and sets decoded with tag 258.
     */
    bool wasTagged() const;

    /**
     * @brief Checks whether an element is present.
     *
     * @param value The element to find.
     *
     * @return True if an equal element exists.
     */
    bool contains(const TransactionInput& value) const;

    /**
     * @brief Copies an element by its sorted position.
     *
     * @param index The zero-based sorted position.
     *
     * @return An independent element.
     *
     * @throws std::out_of_range The index is outside the set.
     */
    TransactionInput at(std::size_t index) const;

    /**
     * @brief Copies the sorted elements.
     *
     * @return Independent elements in sorted order.
     */
    std::vector<TransactionInput> values() const;

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
    void swap(TransactionInputSet& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source set.
     *
     * @return This set; unchanged on failure.
     */
    TransactionInputSet& operator=(const TransactionInputSet& other);

    /**
     * @brief Moves another set into this one.
     *
     * @param other The source set; self-move is safe.
     *
     * @return This set.
     */
    TransactionInputSet& operator=(TransactionInputSet&& other) noexcept;

    /**
     * @brief Checks equality of elements.
     *
     * @param other The set to compare.
     *
     * @return The comparison result.
     */
    bool operator==(const TransactionInputSet& other) const;

    /**
     * @brief Checks inequality of elements.
     *
     * @param other The set to compare.
     *
     * @return The comparison result.
     */
    bool operator!=(const TransactionInputSet& other) const;

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

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static TransactionInputSet adopt(cardano_transaction_input_set_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     * @remark Invalidates all collection iterators and element references. Finish native access before using the collection again.
     */
    cardano_transaction_input_set_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_transaction_input_set_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     * @remark Invalidates all collection iterators and element references. Finish native access before using the collection again.
     */
    cardano_transaction_input_set_t* release() noexcept;

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

    /**
     * @brief Inserts a native value.
     * @param value The value to copy.
     * @return True if inserted.
     */
    bool insertValue(const TransactionInput& value);

  private:

    /**
     * @brief Adopts one owned native set.
     *
     * @param value The owned reference to transfer.
     */
    explicit TransactionInputSet(cardano_transaction_input_set_t& value) noexcept;

  private:

    /**
     * @brief Checks lifetime and gets the native set.
     *
     * @return The borrowed native reference.
     *
     * @throws std::logic_error The set has been moved from.
     */
    cardano_transaction_input_set_t* checked() const;

  private:

    /**
     * @brief The owned native set, or null after a move.
     */
    cardano_transaction_input_set_t* m_value;

    /** @brief Stable element storage for read-only iteration. */
    mutable std::vector<value_type> m_elements;

    /** @brief Whether the element storage matches the collection. */
    mutable bool m_materialized = false;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges sets through argument-dependent lookup.
 *
 * @param lhs The first set.
 * @param rhs The second set.
 */
void swap(TransactionInputSet& lhs, TransactionInputSet& rhs) noexcept;
} // namespace Cometa
