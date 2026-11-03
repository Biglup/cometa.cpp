/**
 * @file TransactionOutputList.h
 *
 * @brief An ordered sequence of transaction outputs.
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

#include <Cometa/TransactionBody/TransactionOutput.h>

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

struct cardano_transaction_output_list_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief An ordered sequence of transaction outputs. */
class TransactionOutputList
{
  public:

    /** @brief The element type. */
    using value_type = TransactionOutput;

    /** @brief The element count type. */
    using size_type = std::size_t;

    /** @brief The iterator distance type. */
    using difference_type = std::ptrdiff_t;

    /** @brief A mutable element reference. */
    using reference = value_type&;

    /** @brief A read-only element reference. */
    using const_reference = const value_type&;

    /** @brief A mutable random-access iterator. */
    using iterator = std::vector<value_type>::iterator;

    /** @brief A read-only random-access iterator. */
    using const_iterator = std::vector<value_type>::const_iterator;

    /** @brief A mutable reverse iterator. */
    using reverse_iterator = std::vector<value_type>::reverse_iterator;

    /** @brief A read-only reverse iterator. */
    using const_reverse_iterator = std::vector<value_type>::const_reverse_iterator;

  public:

    /**
     * @brief Constructs a collection from a range.
     * @tparam InputIterator The input iterator type.
     * @param first The beginning of the input range.
     * @param last The end of the input range.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    TransactionOutputList(InputIterator first, InputIterator last)
    : TransactionOutputList()
    {
      assign(first, last);
    }

  public:

    /**
     * @brief Initializes an empty output list.
     */
    TransactionOutputList();

    /**
     * @brief Initializes an ordered list of copied outputs.
     *
     * @param outputs The child outputs to copy.
     */
    TransactionOutputList(std::initializer_list<TransactionOutput> outputs);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    TransactionOutputList(const TransactionOutputList& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    TransactionOutputList(TransactionOutputList&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~TransactionOutputList();

  public:

    /**
     * @brief Decodes exactly one CBOR output list.
     *
     * @param cbor The encoded output; empty input and trailing bytes are rejected.
     *
     * @return The decoded output.
     */
    static TransactionOutputList fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded output.
     */
    static TransactionOutputList fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of child outputs.
     *
     * @return The number of children.
     */
    std::size_t size() const;

    /**
     * @brief Tests whether the list is empty.
     *
     * @return True when there are no outputs.
     */
    bool empty() const;

    /**
     * @brief Gets an element by position.
     * @param index The zero-based position.
     * @return A read-only element reference.
     * @throws std::out_of_range The position is outside the sequence.
     */
    const_reference at(std::size_t index) const;

    /**
     * @brief Appends a copied child output.
     *
     * @param output The child to append.
     */
    void pushBack(const TransactionOutput& output);

    /**
     * @brief Gets independent children in order.
     *
     * @return The copied child outputs.
     */
    std::vector<TransactionOutput> values() const;

    /**
     * @brief Serializes the output list as CBOR.
     *
     * @return A definite-length output array with normalized output forms.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the output list as CIP-116 inspection JSON.
     *
     * @return The compact inspection object.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(TransactionOutputList& other) noexcept;

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
    TransactionOutputList& operator=(const TransactionOutputList& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    TransactionOutputList& operator=(TransactionOutputList&& other) noexcept;

    /**
     * @brief Compares output values.
     *
     * @param other The output list to compare.
     *
     * @return True when the values are equal.
     */
    bool operator==(const TransactionOutputList& other) const;

    /**
     * @brief Compares output values for inequality.
     *
     * @param other The output list to compare.
     *
     * @return True when the values differ.
     */
    bool operator!=(const TransactionOutputList& other) const;

  public:

    /** @brief Gets the begin iterator. @return The iterator. */
    iterator begin();

    /** @brief Gets the end iterator. @return The iterator. */
    iterator end();

    /** @brief Gets the begin iterator. @return The iterator. */
    const_iterator begin() const;

    /** @brief Gets the end iterator. @return The iterator. */
    const_iterator end() const;

    /** @brief Gets the cbegin iterator. @return The iterator. */
    const_iterator cbegin() const;

    /** @brief Gets the cend iterator. @return The iterator. */
    const_iterator cend() const;

    /** @brief Gets the rbegin iterator. @return The iterator. */
    reverse_iterator rbegin();

    /** @brief Gets the rend iterator. @return The iterator. */
    reverse_iterator rend();

    /** @brief Gets the rbegin iterator. @return The iterator. */
    const_reverse_iterator rbegin() const;

    /** @brief Gets the rend iterator. @return The iterator. */
    const_reverse_iterator rend() const;

    /** @brief Gets the crbegin iterator. @return The iterator. */
    const_reverse_iterator crbegin() const;

    /** @brief Gets the crend iterator. @return The iterator. */
    const_reverse_iterator crend() const;

    /**
     * @brief Gets an element.
     * @param index The zero-based position.
     * @return The element reference.
     * @throws std::out_of_range The position is outside the sequence.
     */
    reference at(size_type index);

    /**
     * @brief Gets an element.
     * @param index The zero-based position.
     * @return The element reference.
     * @pre The position is less than size().
     */
    reference operator[](size_type index);

    /**
     * @brief Gets an element.
     * @param index The zero-based position.
     * @return The element reference.
     * @pre The position is less than size().
     */
    const_reference operator[](size_type index) const;

    /**
     * @brief Gets an element.
     * @return The element reference.
     * @pre The sequence is not empty.
     */
    reference front();

    /**
     * @brief Gets an element.
     * @return The element reference.
     * @pre The sequence is not empty.
     */
    const_reference front() const;

    /**
     * @brief Gets an element.
     * @return The element reference.
     * @pre The sequence is not empty.
     */
    reference back();

    /**
     * @brief Gets an element.
     * @return The element reference.
     * @pre The sequence is not empty.
     */
    const_reference back() const;

    /**
     * @brief Appends an element.
     *
     * @param value The element to append.
     */
    void push_back(const value_type& value);

    /**
     * @brief Appends an element.
     *
     * @param value The element to append.
     */
    void push_back(value_type&& value);

    /**
     * @brief Removes the last element.
     *
     * @remark The sequence must not be empty.
     */
    void pop_back();

    /**
     * @brief Removes all elements.
     */
    void clear();

    /**
     * @brief Reserves element capacity.
     *
     * @param count The requested capacity.
     */
    void reserve(size_type count);

    /**
     * @brief Gets the allocated element capacity.
     *
     * @return The capacity.
     */
    size_type capacity() const;

    /**
     * @brief Requests release of unused capacity.
     */
    void shrink_to_fit();

    /**
     * @brief Inserts an element before a position.
     *
     * @param position The insertion position.
     * @param value The element to copy.
     *
     * @return An iterator to the inserted element.
     */
    iterator insert(const_iterator position, const value_type& value);

    /**
     * @brief Inserts an element before a position.
     *
     * @param position The insertion position.
     * @param value The element to move.
     *
     * @return An iterator to the inserted element.
     */
    iterator insert(const_iterator position, value_type&& value);

    /**
     * @brief Erases elements.
     *
     * @param position The element to erase.
     *
     * @return An iterator following the erased elements.
     */
    iterator erase(const_iterator position);

    /**
     * @brief Erases elements.
     *
     * @param first The start of the range.
     * @param last The end of the range.
     *
     * @return An iterator following the erased elements.
     */
    iterator erase(const_iterator first, const_iterator last);

    /**
     * @brief Constructs and appends an element.
     * @tparam Args The constructor argument types.
     * @param args The constructor arguments.
     * @return The appended element.
     */
    template<class... Args>
    reference
    emplace_back(Args&&... args)
    {
      value_type value(std::forward<Args>(args)...);
      push_back(std::move(value));

      return back();
    }

    /**
     * @brief Inserts a range before a position.
     * @tparam InputIterator The input iterator type.
     * @param position The insertion position.
     * @param first The start of the input range.
     * @param last The end of the input range.
     * @return An iterator to the first inserted element.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    iterator
    insert(const_iterator position, InputIterator first, InputIterator last)
    {
      materialize();
      m_mutableAccess = true;

      return m_elements.insert(position, first, last);
    }

    /**
     * @brief Replaces the elements with a range.
     * @tparam InputIterator The input iterator type.
     * @param first The start of the input range.
     * @param last The end of the input range.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    void
    assign(InputIterator first, InputIterator last)
    {
      materialize();
      std::vector<value_type> replacement(first, last);
      m_elements.swap(replacement);
      m_mutableAccess = true;
    }

  private:

    /** @brief Copies native elements into the C++ sequence on first access. */
    void materialize() const;

    /** @brief Updates the native representation after element changes. */
    void synchronize() const;

    /**
     * @brief Copies an element from the native representation.
     * @param index The native position.
     * @return An independent element.
     */
    value_type copyElement(std::size_t index) const;

  private:

    /** @brief Allows sub-transaction bodies to copy this value. */
    friend class SubTransactionBody;

    /** @brief Allows transaction bodies to copy this value. */
    friend class TransactionBody;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit TransactionOutputList(cardano_transaction_output_list_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_transaction_output_list_t* checked() const;

    /**
     * @brief Checks nested output values before exposing decoded values.
     *
     * @throws std::invalid_argument An output violates the value constraints.
     */
    void validate() const;

  private:

    /** @brief The owned reference, or null after a move. */
    mutable cardano_transaction_output_list_t* m_value;

    /** @brief The authoritative elements after materialization. */
    mutable std::vector<value_type> m_elements;

    /** @brief Element encodings associated with the native representation. */
    mutable std::vector<std::vector<std::uint8_t>> m_encoding;

    /** @brief Whether C++ elements have been initialized. */
    mutable bool m_materialized = false;

    /** @brief Whether element references or mutations can change the sequence. */
    mutable bool m_mutableAccess = false;
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
void swap(TransactionOutputList& lhs, TransactionOutputList& rhs) noexcept;
} // namespace Cometa
