/**
 * @file Committee.h
 *
 * @brief Constitutional committee membership and quorum.
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
#include <Cometa/Common/UnitInterval.h>

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_committee_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief A constitutional committee and its voting quorum. */
class Committee
{
  public:

    /**
     * @brief Creates an empty committee.
     * @param quorumThreshold The voting quorum.
     */
    explicit Committee(const UnitInterval& quorumThreshold);

    /**
     * @brief Copies a committee.
     * @param other The committee to copy.
     */
    Committee(const Committee& other);

    /**
     * @brief Moves a committee.
     * @param other The committee to move.
     */
    Committee(Committee&& other) noexcept;

    /** @brief Destroys the committee. */
    ~Committee();

  public:

    /**
     * @brief Decodes a committee from CBOR.
     * @param cbor The encoded bytes.
     * @return The decoded committee.
     */
    static Committee fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal bytes.
     * @return The decoded committee.
     */
    static Committee fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the quorum threshold.
     * @return An independent interval.
     */
    UnitInterval quorumThreshold() const;

    /**
     * @brief Sets the quorum threshold.
     * @param value The new threshold.
     */
    void setQuorumThreshold(const UnitInterval& value);

    /**
     * @brief Adds a member with an expiration epoch.
     * @param credential The member credential.
     * @param epoch The expiration epoch.
     */
    void addMember(const Credential& credential, std::uint64_t epoch);

    /**
     * @brief Gets member credentials.
     * @return The member credentials.
     */
    CredentialSet keys() const;

    /**
     * @brief Gets a member's expiration epoch.
     * @param credential The member credential.
     * @return The epoch, or zero when absent.
     */
    std::uint64_t memberEpoch(const Credential& credential) const;

    /**
     * @brief Gets a member credential by index.
     * @param index The zero-based index.
     * @return An independent credential.
     */
    Credential keyAt(std::size_t index) const;

    /**
     * @brief Gets an expiration epoch by index.
     * @param index The zero-based index.
     * @return The expiration epoch.
     */
    std::uint64_t valueAt(std::size_t index) const;

    /**
     * @brief Gets a member credential and expiration epoch by index.
     * @param index The zero-based index.
     * @return The credential and epoch.
     */
    std::pair<Credential, std::uint64_t> at(std::size_t index) const;

    /**
     * @brief Encodes the committee as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Exchanges committees.
     * @param other The committee to exchange.
     */
    void swap(Committee& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The committee to copy.
     * @return This committee.
     */
    Committee& operator=(const Committee& other);

    /**
     * @brief Assigns a moved committee.
     * @param other The committee to move.
     * @return This committee.
     */
    Committee& operator=(Committee&& other) noexcept;

    /**
     * @brief Compares committees.
     * @param other The committee to compare.
     * @return True when equal.
     */
    bool operator==(const Committee& other) const;

    /**
     * @brief Compares committees.
     * @param other The committee to compare.
     * @return True when different.
     */
    bool operator!=(const Committee& other) const;

  private:

    /**
     * @brief Adopts an owned committee.
     * @param value The owned committee.
     */
    explicit Committee(cardano_committee_t* value) noexcept;

    /**
     * @brief Gets the stored committee.
     * @return The borrowed committee.
     */
    cardano_committee_t* checked() const;

  private:

    /** @brief The owned committee. */
    cardano_committee_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges committees.
 * @param lhs The first committee.
 * @param rhs The second committee.
 */
void swap(Committee& lhs, Committee& rhs) noexcept;
} // namespace Cometa
