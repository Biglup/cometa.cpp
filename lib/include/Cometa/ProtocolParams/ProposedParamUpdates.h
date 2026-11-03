/**
 * @file ProposedParamUpdates.h
 *
 * @brief Historical protocol parameter proposals indexed by genesis delegate key hash.
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
#include <Cometa/ProtocolParams/ProtocolParamUpdate.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_proposed_param_updates_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Historical protocol parameter proposals indexed by genesis delegate key hash.
 *
 * Used by the legacy Update mechanism. Contemporary governance proposals use ParameterChangeAction and
 * ProposalProcedure.
 */
class ProposedParamUpdates
{
  public:

    /** @brief Creates an empty collection. */
    ProposedParamUpdates();

    /**
     * @brief Copies a collection.
     * @param other The collection to copy.
     */
    ProposedParamUpdates(const ProposedParamUpdates& other);

    /**
     * @brief Moves a collection.
     * @param other The collection to move.
     */
    ProposedParamUpdates(ProposedParamUpdates&& other) noexcept;

    /** @brief Destroys the collection. */
    ~ProposedParamUpdates();

  public:

    /**
     * @brief Decodes the collection from CBOR.
     * @param cbor The encoded collection.
     * @return The decoded collection.
     */
    static ProposedParamUpdates fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded collection.
     */
    static ProposedParamUpdates fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of proposals.
     * @return The number of proposals.
     */
    std::size_t size() const;

    /**
     * @brief Adds a proposal and sorts entries by key.
     * @param key The genesis delegate key hash.
     * @param update The proposed changes.
     */
    void add(const Blake2bHash& key, const ProtocolParamUpdate& update);

    /**
     * @brief Gets the first proposal for a key.
     * @param key The genesis delegate key hash.
     * @return An independent proposal, or empty when absent.
     */
    std::optional<ProtocolParamUpdate> get(const Blake2bHash& key) const;

    /**
     * @brief Gets a key by sorted index.
     * @param index The zero-based index.
     * @return An independent hash.
     */
    Blake2bHash keyAt(std::size_t index) const;

    /**
     * @brief Gets a proposal by sorted index.
     * @param index The zero-based index.
     * @return An independent proposal.
     */
    ProtocolParamUpdate valueAt(std::size_t index) const;

    /**
     * @brief Gets a key and proposal by sorted index.
     * @param index The zero-based index.
     * @return An independent key and proposal.
     */
    std::pair<Blake2bHash, ProtocolParamUpdate> at(std::size_t index) const;

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
    void swap(ProposedParamUpdates& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The collection to copy.
     * @return This collection.
     */
    ProposedParamUpdates& operator=(const ProposedParamUpdates& other);

    /**
     * @brief Assigns a moved collection.
     * @param other The collection to move.
     * @return This collection.
     */
    ProposedParamUpdates& operator=(ProposedParamUpdates&& other) noexcept;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when equal.
     */
    bool operator==(const ProposedParamUpdates& other) const;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when different.
     */
    bool operator!=(const ProposedParamUpdates& other) const;

  private:

    friend class Update;

    /**
     * @brief Adopts an owned collection.
     * @param value The owned collection.
     */
    explicit ProposedParamUpdates(cardano_proposed_param_updates_t* value) noexcept;

    /**
     * @brief Gets the stored collection.
     * @return The borrowed collection.
     */
    cardano_proposed_param_updates_t* checked() const;

  private:

    /** @brief The owned collection. */
    cardano_proposed_param_updates_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges proposed parameter collections.
 * @param lhs The first collection.
 * @param rhs The second collection.
 */
void swap(ProposedParamUpdates& lhs, ProposedParamUpdates& rhs) noexcept;
} // namespace Cometa
