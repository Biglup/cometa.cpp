/**
 * @file CommitteeMembersMap.h
 *
 * @brief Committee cold credentials mapped to their membership expiry epochs.
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
#include <Cometa/Common/CredentialSet.h>

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_committee_members_map_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Committee cold credentials mapped to their membership expiry epochs.
 *
 * The epoch is a ledger epoch number, not a slot or Unix timestamp. Used when representing committee
 * state or proposing member additions.
 */
class CommitteeMembersMap
{
  public:

    /** @brief The key type. */
    using key_type = Credential;

    /** @brief The mapped value type. */
    using mapped_type = std::uint64_t;

    /** @brief The key ordering function. */
    using key_compare = bool (*)(const key_type&, const key_type&);

    /** @brief An entry with an immutable key. */
    using value_type = std::pair<const key_type, mapped_type>;

    /** @brief The element count type. */
    using size_type = std::size_t;

    /** @brief The iterator distance type. */
    using difference_type = std::ptrdiff_t;

    /** @brief A mutable entry reference. */
    using reference = value_type&;

    /** @brief A read-only entry reference. */
    using const_reference = const value_type&;

    /** @brief A mutable bidirectional iterator. */
    using iterator = std::map<key_type, mapped_type, key_compare>::iterator;

    /** @brief A read-only bidirectional iterator. */
    using const_iterator = std::map<key_type, mapped_type, key_compare>::const_iterator;

    /** @brief A mutable reverse iterator. */
    using reverse_iterator = std::map<key_type, mapped_type, key_compare>::reverse_iterator;

    /** @brief A read-only reverse iterator. */
    using const_reverse_iterator = std::map<key_type, mapped_type, key_compare>::const_reverse_iterator;

  public:

    /**
     * @brief Constructs a collection from its elements.
     * @param values The elements to copy, in insertion order.
     */
    CommitteeMembersMap(std::initializer_list<value_type> values)
    : CommitteeMembersMap(values.begin(), values.end())
    {
    }

    /**
     * @brief Constructs a collection from a range.
     * @tparam InputIterator The input iterator type.
     * @param first The beginning of the input range.
     * @param last The end of the input range.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    CommitteeMembersMap(InputIterator first, InputIterator last)
    : CommitteeMembersMap()
    {
      insert(first, last);
    }

  public:

    /** @brief Creates an empty member map. */
    CommitteeMembersMap();

    /**
     * @brief Copies a member map.
     * @param other The map to copy.
     */
    CommitteeMembersMap(const CommitteeMembersMap& other);

    /**
     * @brief Moves a member map.
     * @param other The map to move.
     */
    CommitteeMembersMap(CommitteeMembersMap&& other) noexcept;

    /** @brief Destroys the member map. */
    ~CommitteeMembersMap();

  public:

    /**
     * @brief Decodes a member map from CBOR.
     * @param cbor The encoded bytes.
     * @return The decoded map.
     */
    static CommitteeMembersMap fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal bytes.
     * @return The decoded map.
     */
    static CommitteeMembersMap fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of members.
     * @return The member count.
     */
    std::size_t size() const;

    /**
     * @brief Adds a member with its expiration epoch.
     * @param credential The committee member credential.
     * @param epoch The expiration epoch.
     */
    void insert(const Credential& credential, std::uint64_t epoch);

    /**
     * @brief Gets the first expiration epoch for a credential.
     * @param credential The member credential.
     * @return The epoch, or empty when absent.
     */
    std::optional<std::uint64_t> get(const Credential& credential) const;

    /**
     * @brief Gets the member credentials.
     * @return The credentials in a set.
     */
    CredentialSet keys() const;

    /**
     * @brief Gets a credential by sorted index.
     * @param index The zero-based index.
     * @return An independent credential.
     */
    Credential keyAt(std::size_t index) const;

    /**
     * @brief Gets an expiration epoch by sorted index.
     * @param index The zero-based index.
     * @return The expiration epoch.
     */
    std::uint64_t valueAt(std::size_t index) const;

    /**
     * @brief Gets a credential and epoch by sorted index.
     * @param index The zero-based index.
     * @return An independent credential and its expiration epoch.
     */
    std::pair<Credential, std::uint64_t> at(std::size_t index) const;

    /**
     * @brief Encodes the map as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the map as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges maps.
     * @param other The map to exchange.
     */
    void swap(CommitteeMembersMap& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The map to copy.
     * @return This map.
     */
    CommitteeMembersMap& operator=(const CommitteeMembersMap& other);

    /**
     * @brief Assigns a moved map.
     * @param other The map to move.
     * @return This map.
     */
    CommitteeMembersMap& operator=(CommitteeMembersMap&& other) noexcept;

    /**
     * @brief Compares maps.
     * @param other The map to compare.
     * @return True when equal.
     */
    bool operator==(const CommitteeMembersMap& other) const;

    /**
     * @brief Compares maps.
     * @param other The map to compare.
     * @return True when different.
     */
    bool operator!=(const CommitteeMembersMap& other) const;

  public:

    /**
     * @brief Returns an iterator to the first element.
     * @return The iterator.
     */
    iterator begin();

    /**
     * @brief Returns an iterator to the first element.
     * @return The iterator.
     */
    const_iterator begin() const;

    /**
     * @brief Returns an iterator to the past-the-end position.
     * @return The iterator.
     */
    iterator end();

    /**
     * @brief Returns an iterator to the past-the-end position.
     * @return The iterator.
     */
    const_iterator end() const;

    /**
     * @brief Returns an iterator to the last element in reverse traversal.
     * @return The iterator.
     */
    reverse_iterator rbegin();

    /**
     * @brief Returns an iterator to the last element in reverse traversal.
     * @return The iterator.
     */
    const_reverse_iterator rbegin() const;

    /**
     * @brief Returns an iterator to the position before the first element in reverse traversal.
     * @return The iterator.
     */
    reverse_iterator rend();

    /**
     * @brief Returns an iterator to the position before the first element in reverse traversal.
     * @return The iterator.
     */
    const_reverse_iterator rend() const;

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
    const_reverse_iterator crbegin() const;

    /**
     * @brief Returns an iterator to the position before the first element in reverse traversal.
     * @return The iterator.
     */
    const_reverse_iterator crend() const;

    /**
     * @brief Finds an entry.
     * @param key The key to find.
     * @return The matching iterator, or end().
     */
    iterator find(const key_type& key);

    /**
     * @brief Finds an entry.
     * @param key The key to find.
     * @return The matching iterator, or end().
     */
    const_iterator find(const key_type& key) const;

    /**
     * @brief Counts entries matching a key.
     * @param key The key to find.
     * @return Zero or one.
     */
    size_type count(const key_type& key) const;

    /**
     * @brief Tests whether a key is present.
     * @param key The key to find.
     * @return Whether the key is present.
     */
    bool contains(const key_type& key) const;

    /**
     * @brief Gets a mapped value; throws std::out_of_range for an absent key.
     * @param key The key to find.
     * @return The mapped value reference.
     */
    mapped_type& at(const key_type& key);

    /**
     * @brief Gets a mapped value; throws std::out_of_range for an absent key.
     * @param key The key to find.
     * @return The mapped value reference.
     */
    const mapped_type& at(const key_type& key) const;

    /**
     * @brief Inserts an entry without replacing an existing key.
     * @param value The entry to insert.
     * @return The entry iterator and whether insertion occurred.
     */
    std::pair<iterator, bool> insert(const value_type& value);

    /**
     * @brief Inserts an entry with an optional position hint.
     * @param position The insertion hint.
     * @param value The entry to insert.
     * @return The entry iterator.
     */
    iterator insert(const_iterator position, const value_type& value);

    /**
     * @brief Inserts or replaces a mapped value.
     * @param key The entry key.
     * @param value The mapped value.
     * @return The entry iterator and whether insertion occurred.
     */
    std::pair<iterator, bool> insert_or_assign(const key_type& key, const mapped_type& value);

    /**
     * @brief Gets a mapped value, inserting a default value when absent.
     * @param key The entry key.
     * @return The mapped value reference.
     */
    mapped_type& operator[](const key_type& key);

    /**
     * @brief Erases entries.
     * @param position The entry to erase.
     * @return The following iterator or number of erased entries.
     */
    iterator erase(const_iterator position);

    /**
     * @brief Erases entries.
     * @param first The beginning of the range.
     * @param last The end of the range.
     * @return The following iterator or number of erased entries.
     */
    iterator erase(const_iterator first, const_iterator last);

    /**
     * @brief Erases entries.
     * @param key The key to erase.
     * @return The following iterator or number of erased entries.
     */
    size_type erase(const key_type& key);

    /**
     * @brief Removes all entries.
     */
    void clear();

    /**
     * @brief Tests whether the map has no entries.
     * @return Whether the map is empty.
     */
    bool empty() const;

    /**
     * @brief Constructs an entry without replacing an existing key.
     * @tparam Args The entry constructor argument types.
     * @param args The entry constructor arguments.
     * @return The entry iterator and whether insertion occurred.
     */
    template<class... Args>
    std::pair<iterator, bool>
    emplace(Args&&... args)
    {
      return insert(value_type(std::forward<Args>(args)...));
    }

    /**
     * @brief Constructs a mapped value only when its key is absent.
     * @tparam Args The mapped value constructor argument types.
     * @param key The entry key.
     * @param args The mapped value constructor arguments.
     * @return The entry iterator and whether insertion occurred.
     */
    template<class... Args>
    std::pair<iterator, bool>
    try_emplace(const key_type& key, Args&&... args)
    {
      const iterator existing = find(key);

      if (existing != end())
      {
        return { existing, false };
      }

      return insert(value_type(key, mapped_type(std::forward<Args>(args)...)));
    }

    /**
     * @brief Inserts entries from a range without replacing existing keys.
     * @tparam InputIterator The input iterator type.
     * @param first The beginning of the range.
     * @param last The end of the range.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    void
    insert(InputIterator first, InputIterator last)
    {
      for (; first != last; ++first)
      {
        insert(value_type(*first));
      }
    }

  private:

    /** @brief Initializes the C++ entries on first access. */
    void materialize() const;

    /** @brief Synchronizes changed entries before native operations. */
    void synchronize() const;

    /** @brief Invalidates entries after native mutation. */
    void invalidateElements();

    /**
     * @brief Orders entry keys.
     * @param left The left key.
     * @param right The right key.
     * @return Whether the left key precedes the right key.
     */
    static bool compareKeys(const key_type& left, const key_type& right);

    /**
     * @brief Captures the current entry encodings for change detection.
     * @return The key and value encodings.
     */
    std::vector<std::vector<std::uint8_t>> encodeElements() const;

  private:

    /** @brief Allows update actions to copy member terms. */
    friend class UpdateCommitteeAction;

    /**
     * @brief Adopts an owned member map.
     * @param value The owned map.
     */
    explicit CommitteeMembersMap(cardano_committee_members_map_t* value) noexcept;

    /**
     * @brief Gets the stored map.
     * @return The borrowed map.
     */
    cardano_committee_members_map_t* checked() const;

  private:

    /** @brief The owned map. */
    mutable cardano_committee_members_map_t* m_value;

    /** @brief C++ entries with immutable keys. */
    mutable std::map<key_type, mapped_type, key_compare> m_elements { compareKeys };

    /** @brief Encodings corresponding to the native representation. */
    mutable std::vector<std::vector<std::uint8_t>> m_encoding;

    /** @brief Whether C++ entries are initialized. */
    mutable bool m_materialized = false;

    /** @brief Whether mapped references or mutations may change entries. */
    mutable bool m_mutableAccess = false;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges committee member maps.
 * @param lhs The first map.
 * @param rhs The second map.
 */
void swap(CommitteeMembersMap& lhs, CommitteeMembersMap& rhs) noexcept;
} // namespace Cometa
