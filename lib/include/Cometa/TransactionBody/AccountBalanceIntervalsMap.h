/**
 * @file AccountBalanceIntervalsMap.h
 *
 * @brief Account balance requirements by reward address.
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

#include <Cometa/Common/RewardAddressList.h>
#include <Cometa/TransactionBody/AccountBalanceInterval.h>

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

struct cardano_account_balance_intervals_map_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Reward addresses and their required account balances. */
class AccountBalanceIntervalsMap
{
  public:

    /** @brief The key type. */
    using key_type = RewardAddress;

    /** @brief The mapped value type. */
    using mapped_type = AccountBalanceInterval;

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
    AccountBalanceIntervalsMap(std::initializer_list<value_type> values)
    : AccountBalanceIntervalsMap(values.begin(), values.end())
    {
    }

    /**
     * @brief Constructs a collection from a range.
     * @tparam InputIterator The input iterator type.
     * @param first The beginning of the input range.
     * @param last The end of the input range.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    AccountBalanceIntervalsMap(InputIterator first, InputIterator last)
    : AccountBalanceIntervalsMap()
    {
      insert(first, last);
    }

  public:

    /** @brief Creates an empty map. */
    AccountBalanceIntervalsMap();

    /**
     * @brief Copies a map.
     * @param other The map to copy.
     */
    AccountBalanceIntervalsMap(const AccountBalanceIntervalsMap& other);

    /**
     * @brief Moves a map.
     * @param other The map to move.
     */
    AccountBalanceIntervalsMap(AccountBalanceIntervalsMap&& other) noexcept;

    /** @brief Destroys the map. */
    ~AccountBalanceIntervalsMap();

  public:

    /**
     * @brief Decodes one CBOR map.
     * @param cbor The encoded map.
     * @return The decoded map.
     */
    static AccountBalanceIntervalsMap fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded map.
     */
    static AccountBalanceIntervalsMap fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the entry count.
     * @return The number of entries.
     */
    std::size_t size() const;

    /**
     * @brief Reports whether the map is empty.
     * @return True when empty.
     */
    bool empty() const;

    /**
     * @brief Finds an account balance requirement.
     * @param address The reward address to search for.
     * @return An independent interval, when found.
     */
    std::optional<AccountBalanceInterval> get(const RewardAddress& address) const;

    /**
     * @brief Inserts an account balance requirement.
     * @param address The reward address to copy.
     * @param interval The interval to copy.
     * @throws Error The address is already present.
     */
    void insert(const RewardAddress& address, const AccountBalanceInterval& interval);

    /**
     * @brief Inserts an account balance requirement by encoded address.
     * @param address The Bech32 reward address.
     * @param interval The interval to copy.
     * @throws Error The address is already present.
     */
    void insert(std::string_view address, const AccountBalanceInterval& interval);

    /**
     * @brief Copies an entry in canonical CBOR order.
     * @param index The zero-based index.
     * @return The address and interval.
     * @throws std::out_of_range The index is outside the map.
     */
    std::pair<RewardAddress, AccountBalanceInterval> entryAt(std::size_t index) const;

    /**
     * @brief Copies the reward addresses in canonical CBOR order.
     * @return Independent reward addresses.
     */
    RewardAddressList keys() const;

    /**
     * @brief Copies all entries in canonical CBOR order.
     * @return The address and interval pairs.
     */
    std::vector<std::pair<RewardAddress, AccountBalanceInterval>> entries() const;

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
    void swap(AccountBalanceIntervalsMap& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The map to copy.
     * @return This map.
     */
    AccountBalanceIntervalsMap& operator=(const AccountBalanceIntervalsMap& other);

    /**
     * @brief Assigns a moved map.
     * @param other The map to move.
     * @return This map.
     */
    AccountBalanceIntervalsMap& operator=(AccountBalanceIntervalsMap&& other) noexcept;

    /**
     * @brief Compares maps.
     * @param other The map to compare.
     * @return True when equal.
     */
    bool operator==(const AccountBalanceIntervalsMap& other) const;

    /**
     * @brief Compares maps.
     * @param other The map to compare.
     * @return True when different.
     */
    bool operator!=(const AccountBalanceIntervalsMap& other) const;

  public:

    /**
     * @brief Gets the begin iterator.
     * @return The iterator.
     */
    iterator begin();

    /**
     * @brief Gets the begin iterator.
     * @return The iterator.
     */
    const_iterator begin() const;

    /**
     * @brief Gets the end iterator.
     * @return The iterator.
     */
    iterator end();

    /**
     * @brief Gets the end iterator.
     * @return The iterator.
     */
    const_iterator end() const;

    /**
     * @brief Gets the rbegin iterator.
     * @return The iterator.
     */
    reverse_iterator rbegin();

    /**
     * @brief Gets the rbegin iterator.
     * @return The iterator.
     */
    const_reverse_iterator rbegin() const;

    /**
     * @brief Gets the rend iterator.
     * @return The iterator.
     */
    reverse_iterator rend();

    /**
     * @brief Gets the rend iterator.
     * @return The iterator.
     */
    const_reverse_iterator rend() const;

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

    /** @brief Allows sub-transaction bodies to copy this value. */
    friend class SubTransactionBody;

    /** @brief Allows transaction bodies to copy this value. */
    friend class TransactionBody;

    /**
     * @brief Adopts an owned map.
     * @param value The owned map.
     */
    explicit AccountBalanceIntervalsMap(cardano_account_balance_intervals_map_t* value) noexcept;

    /**
     * @brief Gets the stored map.
     * @return The borrowed map.
     */
    cardano_account_balance_intervals_map_t* checked() const;

  private:

    /** @brief The owned map. */
    mutable cardano_account_balance_intervals_map_t* m_value;

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
 * @brief Exchanges maps through argument-dependent lookup.
 * @param lhs The first map.
 * @param rhs The second map.
 */
void swap(AccountBalanceIntervalsMap& lhs, AccountBalanceIntervalsMap& rhs) noexcept;
} // namespace Cometa
