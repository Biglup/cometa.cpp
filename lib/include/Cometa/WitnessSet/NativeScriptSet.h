/**
 * @file NativeScriptSet.h
 *
 * @brief Ledger native script collections.
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

#include <Cometa/Scripts/NativeScripts/NativeScript.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_native_script_set_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A collection of ledger native scripts in insertion order.
 */
class NativeScriptSet
{
  public:

    /** @brief Creates an empty native script collection. */
    NativeScriptSet();

    /**
     * @brief Copies a collection.
     * @param other The collection to copy.
     */
    NativeScriptSet(const NativeScriptSet& other);

    /**
     * @brief Moves a collection.
     * @param other The collection to move.
     */
    NativeScriptSet(NativeScriptSet&& other) noexcept;

    /** @brief Destroys the collection. */
    ~NativeScriptSet();

  public:

    /**
     * @brief Decodes one CBOR collection.
     * @param cbor The encoded collection.
     * @return The decoded collection.
     */
    static NativeScriptSet fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded collection.
     */
    static NativeScriptSet fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of native scripts.
     * @return The number of elements.
     */
    std::size_t size() const;

    /**
     * @brief Adds a native script if its hash is not already present.
     * @param value The native script to copy.
     */
    void add(const NativeScript& value);

    /**
     * @brief Gets a native script by index.
     * @param index The zero-based index.
     * @return An independent native script.
     */
    NativeScript at(std::size_t index) const;

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
    void swap(NativeScriptSet& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The collection to copy.
     * @return This collection.
     */
    NativeScriptSet& operator=(const NativeScriptSet& other);

    /**
     * @brief Assigns a moved collection.
     * @param other The collection to move.
     * @return This collection.
     */
    NativeScriptSet& operator=(NativeScriptSet&& other) noexcept;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when equal.
     */
    bool operator==(const NativeScriptSet& other) const;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when different.
     */
    bool operator!=(const NativeScriptSet& other) const;

  private:

    /** @brief Allows a witness set to copy this field. */
    friend class WitnessSet;

    /**
     * @brief Adopts an owned collection.
     * @param value The owned collection.
     */
    explicit NativeScriptSet(cardano_native_script_set_t* value) noexcept;

    /**
     * @brief Gets the stored collection.
     * @return The borrowed collection.
     */
    cardano_native_script_set_t* checked() const;

  private:

    /** @brief The owned collection. */
    cardano_native_script_set_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges native script collections through argument-dependent lookup.
 * @param lhs The first collection.
 * @param rhs The second collection.
 */
void swap(NativeScriptSet& lhs, NativeScriptSet& rhs) noexcept;
} // namespace Cometa
