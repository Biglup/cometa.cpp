/**
 * @file VotingProcedure.h
 *
 * @brief Governance vote and optional metadata anchor.
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

#include <Cometa/Common/Anchor.h>
#include <Cometa/VotingProcedures/Vote.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_voting_procedure_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief A governance vote with optional metadata. */
class VotingProcedure
{
  public:

    /**
     * @brief Creates a voting procedure.
     * @param vote The voting choice.
     * @param anchor The optional metadata anchor.
     */
    explicit VotingProcedure(Vote vote, const std::optional<Anchor>& anchor = std::nullopt);

    /**
     * @brief Copies a voting procedure.
     * @param other The voting procedure to copy.
     */
    VotingProcedure(const VotingProcedure& other);

    /**
     * @brief Moves a voting procedure.
     * @param other The voting procedure to move.
     */
    VotingProcedure(VotingProcedure&& other) noexcept;

    /** @brief Destroys the voting procedure. */
    ~VotingProcedure();

  public:

    /**
     * @brief Decodes a voting procedure from CBOR.
     * @param cbor The encoded bytes.
     * @return The decoded voting procedure.
     */
    static VotingProcedure fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal bytes.
     * @return The decoded voting procedure.
     */
    static VotingProcedure fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the voting choice.
     * @return The voting choice.
     */
    Vote vote() const;

    /**
     * @brief Sets the voting choice.
     * @param value The new choice.
     */
    void setVote(Vote value);

    /**
     * @brief Gets the metadata anchor.
     * @return An independent anchor, or empty when absent.
     */
    std::optional<Anchor> anchor() const;

    /**
     * @brief Sets the metadata anchor.
     * @param value The new anchor.
     */
    void setAnchor(const Anchor& value);

    /**
     * @brief Encodes the vote as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the vote as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges voting procedures.
     * @param other The voting procedure to exchange.
     */
    void swap(VotingProcedure& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The voting procedure to copy.
     * @return This voting procedure.
     */
    VotingProcedure& operator=(const VotingProcedure& other);

    /**
     * @brief Assigns a moved voting procedure.
     * @param other The voting procedure to move.
     * @return This voting procedure.
     */
    VotingProcedure& operator=(VotingProcedure&& other) noexcept;

    /**
     * @brief Compares voting procedures.
     * @param other The voting procedure to compare.
     * @return True when equal.
     */
    bool operator==(const VotingProcedure& other) const;

    /**
     * @brief Compares voting procedures.
     * @param other The voting procedure to compare.
     * @return True when different.
     */
    bool operator!=(const VotingProcedure& other) const;

  private:

    /**
     * @brief Adopts an owned voting procedure.
     * @param value The owned voting procedure.
     */
    explicit VotingProcedure(cardano_voting_procedure_t* value) noexcept;

    /**
     * @brief Gets the stored voting procedure.
     * @return The borrowed voting procedure.
     */
    cardano_voting_procedure_t* checked() const;

  private:

    /** @brief The owned voting procedure. */
    cardano_voting_procedure_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges voting procedures.
 * @param lhs The first voting procedure.
 * @param rhs The second voting procedure.
 */
void swap(VotingProcedure& lhs, VotingProcedure& rhs) noexcept;
} // namespace Cometa
