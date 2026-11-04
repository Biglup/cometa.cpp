/**
 * @file MultiAsset.h
 *
 * @brief Native-token quantities grouped by minting policy and asset name.
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

#include <Cometa/Assets/AssetId.h>
#include <Cometa/Assets/AssetNameMap.h>
#include <Cometa/Assets/PolicyIdList.h>

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

struct cardano_multi_asset_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Native-token quantities grouped by minting policy and asset name.
 *
 * Policy IDs are 28-byte script hashes. Signed quantities support minting and burning; transaction
 * output values must satisfy the ledger's nonnegative quantity rules. Lovelace is stored separately in
 * Value.
 */
class MultiAsset
{
  public:

    /** @brief The key type. */
    using key_type = Blake2bHash;

    /** @brief The mapped value type. */
    using mapped_type = AssetNameMap;

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
    MultiAsset(std::initializer_list<value_type> values)
    : MultiAsset(values.begin(), values.end())
    {
    }

    /**
     * @brief Constructs a collection from a range.
     * @tparam InputIterator The input iterator type.
     * @param first The beginning of the input range.
     * @param last The end of the input range.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    MultiAsset(InputIterator first, InputIterator last)
    : MultiAsset()
    {
      insert(first, last);
    }

  public:

    /**
     * @brief Creates an empty collection.
     */
    MultiAsset();

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The collection to copy.
     */
    MultiAsset(const MultiAsset& other);

    /**
     * @brief Moves another collection.
     *
     * @param other The source collection.
     */
    MultiAsset(MultiAsset&& other) noexcept;

    /**
     * @brief Destroys the collection.
     */
    ~MultiAsset();

  public:

    /**
     * @brief Parses one policy-to-assets CBOR map.
     *
     * @param cbor CBOR bytes or hexadecimal without a prefix.
     *
     * @return The parsed quantities.
     *
     * @throws std::invalid_argument The input has duplicate policies, invalid policy lengths, or trailing data.
     * @throws Error The CBOR or amount range is invalid.
     */
    static MultiAsset fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Parses one policy-to-assets CBOR map.
     *
     * @param cborHex CBOR bytes or hexadecimal without a prefix.
     *
     * @return The parsed quantities.
     *
     * @throws std::invalid_argument The input has duplicate policies, invalid policy lengths, or trailing data.
     * @throws Error The CBOR or amount range is invalid.
     */
    static MultiAsset fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Counts policies.
     *
     * @return The number of policy entries, including empty ones.
     */
    std::size_t policyCount() const;

    /**
     * @brief Looks up the assets under one policy.
     *
     * @param policyId The 28-byte policy hash.
     *
     * @return An independent map, or nullopt if absent.
     */
    std::optional<AssetNameMap> assets(const Blake2bHash& policyId) const;

    /**
     * @brief Replaces all assets under one policy.
     *
     * @param policyId The 28-byte policy hash to copy.
     * @param assets The asset quantities to copy.
     */
    void setAssets(const Blake2bHash& policyId, const AssetNameMap& assets);

    /**
     * @brief Looks up an asset quantity.
     *
     * @param policyId The 28-byte policy hash.
     * @param name The asset name.
     *
     * @return The amount, or nullopt if absent.
     */
    std::optional<std::int64_t> get(const Blake2bHash& policyId, const AssetName& name) const;

    /**
     * @brief Looks up an asset quantity by identifier.
     *
     * @param id A native-token identifier.
     *
     * @return The amount, or nullopt if absent.
     *
     * @throws std::invalid_argument The identifier denotes lovelace.
     */
    std::optional<std::int64_t> get(const AssetId& id) const;

    /**
     * @brief Inserts or replaces one asset quantity.
     *
     * @param policyId The 28-byte policy hash to copy.
     * @param name The asset name to copy.
     * @param amount The signed quantity; zero is retained.
     */
    void set(const Blake2bHash& policyId, const AssetName& name, std::int64_t amount);

    /**
     * @brief Inserts or replaces a quantity by identifier.
     *
     * @param id A native-token identifier.
     * @param amount The signed quantity.
     *
     * @throws std::invalid_argument The identifier denotes lovelace.
     */
    void set(const AssetId& id, std::int64_t amount);

    /**
     * @brief Copies the policy IDs.
     *
     * @return Policy hashes in serialization order.
     */
    PolicyIdList policies() const;

    /**
     * @brief Copies all policy maps.
     *
     * @return Independent policy/map pairs in serialization order.
     */
    std::vector<std::pair<Blake2bHash, AssetNameMap>> entries() const;

    /**
     * @brief Selects strictly positive quantities.
     *
     * @return The selected quantities with their signs preserved.
     */
    MultiAsset positive() const;

    /**
     * @brief Selects strictly negative quantities.
     *
     * @return The selected quantities with their signs preserved.
     */
    MultiAsset negative() const;

    /**
     * @brief Encodes policy maps as CBOR.
     *
     * @return Encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats policy and asset-name keyed JSON objects.
     *
     * @return Compact JSON with amounts as decimal strings.
     */
    std::string toJson() const;

    /**
     * @brief Formats policy entries as a JSON array.
     *
     * @return Objects containing script_hash and assets; amounts are decimal strings.
     */
    std::string toJsonEntries() const;

    /**
     * @brief Exchanges collections.
     *
     * @param other The collection to exchange.
     */
    void swap(MultiAsset& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source collection.
     *
     * @return This collection; unchanged on failure.
     */
    MultiAsset& operator=(const MultiAsset& other);

    /**
     * @brief Moves another collection into this one.
     *
     * @param other The source collection; self-move is safe.
     *
     * @return This collection.
     */
    MultiAsset& operator=(MultiAsset&& other) noexcept;

    /**
     * @brief Adds corresponding quantities, removing zero results.
     *
     * @param other The right-hand quantities.
     *
     * @return The resulting quantities.
     *
     * @throws std::overflow_error A quantity exceeds the signed 64-bit range.
     */
    MultiAsset operator+(const MultiAsset& other) const;

    /**
     * @brief Adds quantities into this collection.
     *
     * @param other The right-hand quantities.
     *
     * @return This collection; unchanged on failure.
     *
     * @throws std::overflow_error A quantity exceeds the signed 64-bit range.
     */
    MultiAsset& operator+=(const MultiAsset& other);

    /**
     * @brief Subtracts corresponding quantities, removing zero results.
     *
     * @param other The right-hand quantities.
     *
     * @return The resulting quantities.
     *
     * @throws std::overflow_error A quantity exceeds the signed 64-bit range.
     */
    MultiAsset operator-(const MultiAsset& other) const;

    /**
     * @brief Subtracts quantities into this collection.
     *
     * @param other The right-hand quantities.
     *
     * @return This collection; unchanged on failure.
     *
     * @throws std::overflow_error A quantity exceeds the signed 64-bit range.
     */
    MultiAsset& operator-=(const MultiAsset& other);

    /**
     * @brief Checks equality of policy maps.
     *
     * @param other The collection to compare.
     *
     * @return The comparison result.
     */
    bool operator==(const MultiAsset& other) const;

    /**
     * @brief Checks inequality of policy maps.
     *
     * @param other The collection to compare.
     *
     * @return The comparison result.
     */
    bool operator!=(const MultiAsset& other) const;

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
     * @brief Gets the number of policies.
     * @return The policy count.
     */
    size_type size() const;

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

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static MultiAsset adopt(cardano_multi_asset_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     * @remark Invalidates all collection iterators and element references. Finish native access before using the collection again.
     * @throws std::bad_alloc Synchronizing edited elements cannot allocate memory.
     * @throws Error Edited elements cannot be represented by the native collection.
     */
    cardano_multi_asset_t* nativeHandle();

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     * @throws std::bad_alloc Synchronizing edited elements cannot allocate memory.
     * @throws Error Edited elements cannot be represented by the native collection.
     */
    const cardano_multi_asset_t* nativeHandle() const;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     * @remark Invalidates all collection iterators and element references. Finish native access before using the collection again.
     * @throws std::bad_alloc Synchronizing edited elements cannot allocate memory.
     * @throws Error Edited elements cannot be represented by the native collection.
     */
    cardano_multi_asset_t* release();

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
     * @brief Adopts one owned native collection.
     *
     * @param value The owned reference to transfer.
     */
    explicit MultiAsset(cardano_multi_asset_t* value) noexcept;

  private:

    /**
     * @brief Checks lifetime and gets the native collection.
     *
     * @return The borrowed native reference.
     *
     * @throws std::logic_error The collection has been moved from.
     */
    cardano_multi_asset_t* checked() const;

  private:

    /**
     * @brief The owned native collection, or null after a move.
     */
    mutable cardano_multi_asset_t* m_value;

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
 * @brief Exchanges collections through argument-dependent lookup.
 *
 * @param lhs The first collection.
 * @param rhs The second collection.
 */
void swap(MultiAsset& lhs, MultiAsset& rhs) noexcept;
} // namespace Cometa
