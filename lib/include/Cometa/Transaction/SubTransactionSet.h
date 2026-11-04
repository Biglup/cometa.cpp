/**
 * @file SubTransactionSet.h
 *
 * @brief Sub-transactions in batch order, unique by body hash.
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

#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/Transaction/SubTransaction.h>

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_sub_transaction_set_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Sub-transactions in batch order, unique by body hash.
 *
 * The enclosing transaction commits to this sequence. Preserve order when resolving batch indices or
 * evaluating an individual sub-transaction.
 */
class SubTransactionSet
{
  public:

    /** @brief The element type. */
    using value_type = SubTransaction;

    /** @brief The lookup value type. */
    using key_type = SubTransaction;

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
    SubTransactionSet(std::initializer_list<value_type> values)
    : SubTransactionSet(values.begin(), values.end())
    {
    }

    /**
     * @brief Constructs a collection from a range.
     * @tparam InputIterator The input iterator type.
     * @param first The beginning of the input range.
     * @param last The end of the input range.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    SubTransactionSet(InputIterator first, InputIterator last)
    : SubTransactionSet()
    {
      insert(first, last);
    }

  public:

    /** @brief Creates an empty set. */
    SubTransactionSet();

    /**
     * @brief Copies a set.
     * @param other The set to copy.
     */
    SubTransactionSet(const SubTransactionSet& other);

    /**
     * @brief Moves a set.
     * @param other The set to move.
     */
    SubTransactionSet(SubTransactionSet&& other) noexcept;

    /** @brief Destroys the set. */
    ~SubTransactionSet();

  public:

    /**
     * @brief Decodes one CBOR set.
     * @param cbor The encoded set.
     * @return The decoded set.
     */
    static SubTransactionSet fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded set.
     */
    static SubTransactionSet fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the entry count.
     * @return The number of sub-transactions.
     */
    std::size_t size() const;

    /**
     * @brief Reports whether the set is empty.
     * @return True when empty.
     */
    bool empty() const;

    /**
     * @brief Adds a sub-transaction in order.
     * @param value The sub-transaction to copy.
     * @throws Error Another sub-transaction has the same identifier.
     */
    void add(const SubTransaction& value);

    /**
     * @brief Copies a sub-transaction by position.
     * @param index The zero-based index.
     * @return An independent sub-transaction.
     * @throws std::out_of_range The index is outside the set.
     */
    SubTransaction at(std::size_t index) const;

    /**
     * @brief Finds a sub-transaction by body hash.
     * @param id The sub-transaction identifier.
     * @return An independent sub-transaction, when found.
     */
    std::optional<SubTransaction> findById(const Blake2bHash& id) const;

    /**
     * @brief Reports whether the CBOR set tag is included.
     * @return True when tagged.
     */
    bool useTag() const;

    /**
     * @brief Selects whether to include the CBOR set tag.
     * @param value True to include the tag.
     */
    void setUseTag(bool value);

    /**
     * @brief Encodes the set as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the set as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges sets.
     * @param other The set to exchange.
     */
    void swap(SubTransactionSet& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The set to copy.
     * @return This set.
     */
    SubTransactionSet& operator=(const SubTransactionSet& other);

    /**
     * @brief Assigns a moved set.
     * @param other The set to move.
     * @return This set.
     */
    SubTransactionSet& operator=(SubTransactionSet&& other) noexcept;

    /**
     * @brief Compares sets.
     * @param other The set to compare.
     * @return True when equal.
     */
    bool operator==(const SubTransactionSet& other) const;

    /**
     * @brief Compares sets.
     * @param other The set to compare.
     * @return True when different.
     */
    bool operator!=(const SubTransactionSet& other) const;

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

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static SubTransactionSet adopt(cardano_sub_transaction_set_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     * @remark Invalidates all collection iterators and element references. Finish native access before using the collection again.
     */
    cardano_sub_transaction_set_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_sub_transaction_set_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     * @remark Invalidates all collection iterators and element references. Finish native access before using the collection again.
     */
    cardano_sub_transaction_set_t* release() noexcept;

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

    /**
     * @brief Adopts an owned set.
     * @param value The owned set.
     */
    explicit SubTransactionSet(cardano_sub_transaction_set_t* value) noexcept;

    /**
     * @brief Gets the stored set.
     * @return The borrowed set.
     */
    cardano_sub_transaction_set_t* checked() const;

  private:

    /** @brief The owned set. */
    cardano_sub_transaction_set_t* m_value;

    /** @brief Stable element storage for read-only iteration. */
    mutable std::vector<value_type> m_elements;

    /** @brief Whether the element storage matches the collection. */
    mutable bool m_materialized = false;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges sets through argument-dependent lookup.
 * @param lhs The first set.
 * @param rhs The second set.
 */
void swap(SubTransactionSet& lhs, SubTransactionSet& rhs) noexcept;
} // namespace Cometa
