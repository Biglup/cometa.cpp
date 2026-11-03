/**
 * @file CertificateSet.h
 *
 * @brief Ledger certificate collections.
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

#include <Cometa/Certificates/Certificate.h>

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

struct cardano_certificate_set_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A collection of ledger certificates in insertion order.
 */
class CertificateSet
{
  public:

    /** @brief The element type. */
    using value_type = Certificate;

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
     * @brief Constructs a collection from its elements.
     * @param values The elements to copy, in insertion order.
     */
    CertificateSet(std::initializer_list<value_type> values)
    : CertificateSet(values.begin(), values.end())
    {
    }

    /**
     * @brief Constructs a collection from a range.
     * @tparam InputIterator The input iterator type.
     * @param first The beginning of the input range.
     * @param last The end of the input range.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    CertificateSet(InputIterator first, InputIterator last)
    : CertificateSet()
    {
      assign(first, last);
    }

  public:

    /** @brief Creates an empty certificate collection. */
    CertificateSet();

    /**
     * @brief Copies a collection.
     * @param other The collection to copy.
     */
    CertificateSet(const CertificateSet& other);

    /**
     * @brief Moves a collection.
     * @param other The collection to move.
     */
    CertificateSet(CertificateSet&& other) noexcept;

    /** @brief Destroys the collection. */
    ~CertificateSet();

  public:

    /**
     * @brief Decodes one CBOR collection.
     * @param cbor The encoded collection.
     * @return The decoded collection.
     */
    static CertificateSet fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded collection.
     */
    static CertificateSet fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of certificates.
     * @return The number of elements.
     */
    std::size_t size() const;

    /** @brief Tests whether there are no elements. @return Whether the collection is empty. */
    bool empty() const;

    /**
     * @brief Appends a certificate.
     * @param value The certificate to copy.
     */
    void add(const Certificate& value);

    /**
     * @brief Gets an element by position.
     * @param index The zero-based position.
     * @return A read-only element reference.
     * @throws std::out_of_range The position is outside the sequence.
     */
    const_reference at(std::size_t index) const;

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
    void swap(CertificateSet& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The collection to copy.
     * @return This collection.
     */
    CertificateSet& operator=(const CertificateSet& other);

    /**
     * @brief Assigns a moved collection.
     * @param other The collection to move.
     * @return This collection.
     */
    CertificateSet& operator=(CertificateSet&& other) noexcept;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when equal.
     */
    bool operator==(const CertificateSet& other) const;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when different.
     */
    bool operator!=(const CertificateSet& other) const;

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
     * @brief Adopts an owned collection.
     * @param value The owned collection.
     */
    explicit CertificateSet(cardano_certificate_set_t* value) noexcept;

    /**
     * @brief Gets the stored collection.
     * @return The borrowed collection.
     */
    cardano_certificate_set_t* checked() const;

  private:

    /** @brief The owned collection. */
    mutable cardano_certificate_set_t* m_value;

    /** @brief The authoritative elements after materialization. */
    mutable std::vector<value_type> m_elements;

    /** @brief Element encodings associated with the native representation. */
    mutable std::vector<std::vector<std::uint8_t>> m_encoding;

    /** @brief Whether C++ elements have been initialized. */
    mutable bool m_materialized = false;

    /** @brief Whether element references or mutations can change the sequence. */
    mutable bool m_mutableAccess = false;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges certificate collections through argument-dependent lookup.
 * @param lhs The first collection.
 * @param rhs The second collection.
 */
void swap(CertificateSet& lhs, CertificateSet& rhs) noexcept;
} // namespace Cometa
