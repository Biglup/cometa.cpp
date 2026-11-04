/**
 * @file CostModels.h
 *
 * @brief Execution-cost models indexed by Plutus language identifier.
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

#include <Cometa/ProtocolParams/CostModel.h>

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

struct cardano_costmdls_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Execution-cost models indexed by Plutus language identifier.
 *
 * A transaction can use several Plutus versions, each with its own model. languageViews() provides the
 * encoding used when constructing the script-data hash.
 */
class CostModels
{
  public:

    /** @brief The key type. */
    using key_type = std::uint8_t;

    /** @brief The mapped value type. */
    using mapped_type = CostModel;

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
    CostModels(std::initializer_list<value_type> values)
    : CostModels(values.begin(), values.end())
    {
    }

    /**
     * @brief Constructs a collection from a range.
     * @tparam InputIterator The input iterator type.
     * @param first The beginning of the input range.
     * @param last The end of the input range.
     */
    template<class InputIterator, class = std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>::value>>
    CostModels(InputIterator first, InputIterator last)
    : CostModels()
    {
      insert(first, last);
    }

  public:

    /** @brief Creates an empty collection. */
    CostModels();

    /**
     * @brief Copies a collection.
     * @param other The collection to copy.
     */
    CostModels(const CostModels& other);

    /**
     * @brief Moves a collection.
     * @param other The collection to move.
     */
    CostModels(CostModels&& other) noexcept;

    /** @brief Destroys the collection. */
    ~CostModels();

  public:

    /**
     * @brief Decodes CBOR.
     * @param cbor Encoded bytes.
     * @return The decoded collection.
     */
    static CostModels fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex Hexadecimal bytes.
     * @return The decoded collection.
     */
    static CostModels fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Inserts or replaces a model.
     * @param model The model to copy.
     */
    void insert(const CostModel& model);

    /**
     * @brief Checks for a known language.
     * @param language The language.
     * @return True when present.
     */
    bool contains(PlutusLanguageVersion language) const;

    /**
     * @brief Tests whether a raw language ID is present.
     * @param languageId The language ID, including future IDs.
     * @return Whether a model exists for the language ID.
     */
    bool contains(std::uint8_t languageId) const;

    /**
     * @brief Gets a model by raw language ID.
     * @param languageId The ID.
     * @return An independent model, or empty.
     */
    std::optional<CostModel> get(std::uint8_t languageId) const;

    /**
     * @brief Gets a model by known language.
     * @param language The language.
     * @return An independent model, or empty.
     */
    std::optional<CostModel> get(PlutusLanguageVersion language) const;

    /**
     * @brief Gets the language views for script data hashes.
     * @return Encoded bytes.
     */
    std::vector<std::uint8_t> languageViews() const;

    /**
     * @brief Encodes the collection as CBOR.
     * @return Encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the collection as CIP-116 JSON.
     * @return Compact JSON.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges collections.
     * @param other The other collection.
     */
    void swap(CostModels& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The source.
     * @return This collection.
     */
    CostModels& operator=(const CostModels& other);

    /**
     * @brief Assigns a moved collection.
     * @param other The source.
     * @return This collection.
     */
    CostModels& operator=(CostModels&& other) noexcept;

    /**
     * @brief Compares collections.
     * @param other The other collection.
     * @return True when equal.
     */
    bool operator==(const CostModels& other) const;

    /**
     * @brief Compares collections.
     * @param other The other collection.
     * @return True when different.
     */
    bool operator!=(const CostModels& other) const;

  public:

    /**
     * @brief Gets the number of entries.
     * @return The entry count.
     */
    std::size_t size() const;

    /**
     * @brief Copies all entries.
     * @return Independent entries.
     */
    std::vector<std::pair<std::uint8_t, CostModel>> entries() const;

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
    static CostModels adopt(cardano_costmdls_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     * @remark Invalidates all collection iterators and element references. Finish native access before using the collection again.
     * @throws std::bad_alloc Synchronizing edited elements cannot allocate memory.
     * @throws Error Edited elements cannot be represented by the native collection.
     */
    cardano_costmdls_t* nativeHandle();

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     * @throws std::bad_alloc Synchronizing edited elements cannot allocate memory.
     * @throws Error Edited elements cannot be represented by the native collection.
     */
    const cardano_costmdls_t* nativeHandle() const;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     * @remark Invalidates all collection iterators and element references. Finish native access before using the collection again.
     * @throws std::bad_alloc Synchronizing edited elements cannot allocate memory.
     * @throws Error Edited elements cannot be represented by the native collection.
     */
    cardano_costmdls_t* release();

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
     * @brief Inserts an entry through the native interface.
     * @param key The entry key.
     * @param value The entry value.
     */
    void putEntry(const std::uint8_t& key, const CostModel& value);

  private:

    /**
     * @brief Adopts an owned collection.
     * @param value The owned collection.
     */
    explicit CostModels(cardano_costmdls_t* value) noexcept;

    /**
     * @brief Gets the stored collection.
     * @return The borrowed collection.
     */
    cardano_costmdls_t* checked() const;

  private:

    /** @brief The owned collection. */
    mutable cardano_costmdls_t* m_value;

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
 * @brief Exchanges collections.
 * @param lhs The first collection.
 * @param rhs The second collection.
 */
void swap(CostModels& lhs, CostModels& rhs) noexcept;
} // namespace Cometa
