/**
 * @file CommitteeMembersMap.h
 *
 * @brief Committee members and their term expiration epochs.
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
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_committee_members_map_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Committee member credentials mapped to term expiration epochs. */
class CommitteeMembersMap
{
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
    cardano_committee_members_map_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges committee member maps.
 * @param lhs The first map.
 * @param rhs The second map.
 */
void swap(CommitteeMembersMap& lhs, CommitteeMembersMap& rhs) noexcept;
} // namespace Cometa
