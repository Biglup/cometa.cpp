/**
 * @file RequiredGuardsMap.h
 *
 * @brief Guard credentials and their optional datums.
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
#include <Cometa/PlutusData/PlutusData.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_required_guards_map_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Guard credentials and optional data supplied by a sub-transaction. */
class RequiredGuardsMap
{
  public:

    /** @brief Creates an empty map. */
    RequiredGuardsMap();

    /**
     * @brief Copies a map.
     * @param other The map to copy.
     */
    RequiredGuardsMap(const RequiredGuardsMap& other);

    /**
     * @brief Moves a map.
     * @param other The map to move.
     */
    RequiredGuardsMap(RequiredGuardsMap&& other) noexcept;

    /** @brief Destroys the map. */
    ~RequiredGuardsMap();

  public:

    /**
     * @brief Decodes one CBOR map.
     * @param cbor The encoded map.
     * @return The decoded map.
     */
    static RequiredGuardsMap fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded map.
     */
    static RequiredGuardsMap fromCbor(std::string_view cborHex);

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
     * @brief Checks whether a credential is present.
     * @param credential The credential to find.
     * @return True when present, including entries without a datum.
     */
    bool contains(const Credential& credential) const;

    /**
     * @brief Gets the datum associated with a credential.
     * @param credential The credential to find.
     * @return No outer value when absent; an empty inner value when present without a datum.
     */
    std::optional<std::optional<PlutusData>> get(const Credential& credential) const;

    /**
     * @brief Gets the datum for a required credential.
     * @param credential The credential to find.
     * @return The datum, when supplied.
     * @throws std::out_of_range The credential is absent.
     */
    std::optional<PlutusData> at(const Credential& credential) const;

    /**
     * @brief Inserts a credential and optional datum.
     * @param credential The credential to copy.
     * @param datum The datum to copy, when present.
     * @throws Error The credential is already present.
     */
    void insert(const Credential& credential, const std::optional<PlutusData>& datum = std::nullopt);

    /**
     * @brief Copies an entry in canonical CBOR order.
     * @param index The zero-based index.
     * @return The credential and its optional datum.
     * @throws std::out_of_range The index is outside the map.
     */
    std::pair<Credential, std::optional<PlutusData>> entryAt(std::size_t index) const;

    /**
     * @brief Copies the credentials in canonical CBOR order.
     * @return Independent credentials.
     */
    std::vector<Credential> keys() const;

    /**
     * @brief Copies all entries in canonical CBOR order.
     * @return Credential and optional datum pairs.
     */
    std::vector<std::pair<Credential, std::optional<PlutusData>>> entries() const;

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
    void swap(RequiredGuardsMap& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The map to copy.
     * @return This map.
     */
    RequiredGuardsMap& operator=(const RequiredGuardsMap& other);

    /**
     * @brief Assigns a moved map.
     * @param other The map to move.
     * @return This map.
     */
    RequiredGuardsMap& operator=(RequiredGuardsMap&& other) noexcept;

    /**
     * @brief Compares maps.
     * @param other The map to compare.
     * @return True when equal.
     */
    bool operator==(const RequiredGuardsMap& other) const;

    /**
     * @brief Compares maps.
     * @param other The map to compare.
     * @return True when different.
     */
    bool operator!=(const RequiredGuardsMap& other) const;

  private:

    /** @brief Allows sub-transaction bodies to copy this value. */
    friend class SubTransactionBody;

    /** @brief Allows transaction bodies to copy this value. */
    friend class TransactionBody;

    /**
     * @brief Adopts an owned map.
     * @param value The owned map.
     */
    explicit RequiredGuardsMap(cardano_required_guards_map_t* value) noexcept;

    /**
     * @brief Gets the stored map.
     * @return The borrowed map.
     */
    cardano_required_guards_map_t* checked() const;

  private:

    /** @brief The owned map. */
    cardano_required_guards_map_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges maps through argument-dependent lookup.
 * @param lhs The first map.
 * @param rhs The second map.
 */
void swap(RequiredGuardsMap& lhs, RequiredGuardsMap& rhs) noexcept;
} // namespace Cometa
