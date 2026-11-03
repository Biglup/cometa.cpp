/**
 * @file AssetIdMap.h
 *
 * @brief Signed asset quantities keyed by AssetId.
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

#include <Cometa/Assets/AssetIdList.h>

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

struct cardano_asset_id_map_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A map of AssetId keys to signed 64-bit quantities.
 */
class AssetIdMap
{
  public:

    /** @brief The key type. */
    using key_type = AssetId;

    /** @brief The mapped value type. */
    using mapped_type = std::int64_t;

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
     * @brief Constructs a collection from a range.
     * @tparam InputIterator The input iterator type.
     * @param first The beginning of the input range.
     * @param last The end of the input range.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    AssetIdMap(InputIterator first, InputIterator last)
    : AssetIdMap()
    {
      insert(first, last);
    }

  public:

    /**
     * @brief Creates an empty map.
     */
    AssetIdMap();

    /**
     * @brief Creates a map from entries; later duplicate keys replace earlier amounts.
     *
     * @param entries The keys and quantities to copy.
     */
    AssetIdMap(std::initializer_list<std::pair<AssetId, std::int64_t>> entries);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The map to copy.
     */
    AssetIdMap(const AssetIdMap& other);

    /**
     * @brief Moves another map.
     *
     * @param other The source map.
     */
    AssetIdMap(AssetIdMap&& other) noexcept;

    /**
     * @brief Destroys the map.
     */
    ~AssetIdMap();

  public:

    /**
     * @brief Gets the number of entries.
     *
     * @return The entry count.
     */
    std::size_t size() const;

    /**
     * @brief Checks whether the map has no entries.
     *
     * @return True when no keys are present.
     */
    bool empty() const;

    /**
     * @brief Looks up a quantity.
     *
     * @param key The key to look up.
     *
     * @return The stored amount, or nullopt if absent.
     */
    std::optional<std::int64_t> get(const AssetId& key) const;

    /**
     * @brief Inserts or replaces a quantity.
     *
     * @param key The key to copy.
     * @param amount The signed quantity; zero is retained.
     */
    void set(const AssetId& key, std::int64_t amount);

    /**
     * @brief Copies the map keys.
     *
     * @return Independent keys in serialization order.
     */
    AssetIdList keys() const;

    /**
     * @brief Copies the map entries.
     *
     * @return Independent key/quantity pairs in serialization order.
     */
    std::vector<std::pair<AssetId, std::int64_t>> entries() const;

    /**
     * @brief Exchanges maps.
     *
     * @param other The map to exchange.
     */
    void swap(AssetIdMap& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source map.
     *
     * @return This map; unchanged if copying fails.
     */
    AssetIdMap& operator=(const AssetIdMap& other);

    /**
     * @brief Moves another map into this one.
     *
     * @param other The source map; self-move is safe.
     *
     * @return This map.
     */
    AssetIdMap& operator=(AssetIdMap&& other) noexcept;

    /**
     * @brief Adds corresponding quantities, removing zero results.
     *
     * @param other The right-hand map.
     *
     * @return The resulting map.
     *
     * @throws std::overflow_error A resulting quantity exceeds the signed 64-bit range.
     */
    AssetIdMap operator+(const AssetIdMap& other) const;

    /**
     * @brief Adds quantities into this map.
     *
     * @param other The right-hand map.
     *
     * @return This map; unchanged on failure.
     *
     * @throws std::overflow_error A resulting quantity exceeds the signed 64-bit range.
     */
    AssetIdMap& operator+=(const AssetIdMap& other);

    /**
     * @brief Subtracts corresponding quantities, removing zero results.
     *
     * @param other The right-hand map.
     *
     * @return The resulting map.
     *
     * @throws std::overflow_error A resulting quantity exceeds the signed 64-bit range.
     */
    AssetIdMap operator-(const AssetIdMap& other) const;

    /**
     * @brief Subtracts quantities into this map.
     *
     * @param other The right-hand map.
     *
     * @return This map; unchanged on failure.
     *
     * @throws std::overflow_error A resulting quantity exceeds the signed 64-bit range.
     */
    AssetIdMap& operator-=(const AssetIdMap& other);

    /**
     * @brief Checks equality of keys and quantities.
     *
     * @param other The map to compare.
     *
     * @return The comparison result.
     */
    bool operator==(const AssetIdMap& other) const;

    /**
     * @brief Checks inequality of keys and quantities.
     *
     * @param other The map to compare.
     *
     * @return The comparison result.
     */
    bool operator!=(const AssetIdMap& other) const;

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

    /**
     * @brief Adopts one owned native map.
     *
     * @param value The owned reference to transfer.
     */
    explicit AssetIdMap(cardano_asset_id_map_t* value) noexcept;

  private:

    /**
     * @brief Checks lifetime and gets the native map.
     *
     * @return The borrowed native reference.
     *
     * @throws std::logic_error The map has been moved from.
     */
    cardano_asset_id_map_t* checked() const;

  private:

    /**
     * @brief The owned native map, or null after a move.
     */
    mutable cardano_asset_id_map_t* m_value;

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
 *
 * @param lhs The first map.
 * @param rhs The second map.
 */
void swap(AssetIdMap& lhs, AssetIdMap& rhs) noexcept;
} // namespace Cometa
