/**
 * @file Voter.h
 *
 * @brief A governance voter identified by its role and credential.
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
#include <Cometa/VotingProcedures/VoterType.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_voter_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A governance voter identified by its role and credential.
 *
 * Committee, DRep, and stake pool roles have different authorization and eligibility rules. The same
 * hash in a different role does not identify the same voter.
 */
class Voter
{
  public:

    /**
     * @brief Creates a voter.
     * @param type The voter kind.
     * @param credential The voter credential.
     */
    Voter(VoterType type, const Credential& credential);

    /**
     * @brief Copies a voter.
     * @param other The voter to copy.
     */
    Voter(const Voter& other);

    /**
     * @brief Moves a voter.
     * @param other The voter to move.
     */
    Voter(Voter&& other) noexcept;

    /** @brief Destroys the voter. */
    ~Voter();

  public:

    /**
     * @brief Decodes a voter from CBOR.
     * @param cbor The encoded bytes.
     * @return The decoded voter.
     */
    static Voter fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal bytes.
     * @return The decoded voter.
     */
    static Voter fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the voter kind.
     * @return The voter kind.
     */
    VoterType type() const;

    /**
     * @brief Sets the voter kind.
     * @param value The new voter kind.
     */
    void setType(VoterType value);

    /**
     * @brief Gets the voter credential.
     * @return An independent credential.
     */
    Credential credential() const;

    /**
     * @brief Sets the voter credential.
     * @param value The new credential.
     */
    void setCredential(const Credential& value);

    /**
     * @brief Encodes the voter as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the voter as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges voters.
     * @param other The voter to exchange.
     */
    void swap(Voter& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The voter to copy.
     * @return This voter.
     */
    Voter& operator=(const Voter& other);

    /**
     * @brief Assigns a moved voter.
     * @param other The voter to move.
     * @return This voter.
     */
    Voter& operator=(Voter&& other) noexcept;

    /**
     * @brief Compares voters.
     * @param other The voter to compare.
     * @return True when equal.
     */
    bool operator==(const Voter& other) const;

    /**
     * @brief Compares voters.
     * @param other The voter to compare.
     * @return True when different.
     */
    bool operator!=(const Voter& other) const;

    /**
     * @brief Orders voters for standard containers.
     * @param other The voter to compare.
     * @return True when this voter precedes the other.
     */
    bool operator<(const Voter& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static Voter adopt(cardano_voter_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_voter_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_voter_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_voter_t* release() noexcept;

  private:

    /**
     * @brief Adopts an owned voter.
     * @param value The owned voter.
     */
    explicit Voter(cardano_voter_t* value) noexcept;

    /**
     * @brief Gets the stored voter.
     * @return The borrowed voter.
     */
    cardano_voter_t* checked() const;

  private:

    /** @brief The owned voter. */
    cardano_voter_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges voters.
 * @param lhs The first voter.
 * @param rhs The second voter.
 */
void swap(Voter& lhs, Voter& rhs) noexcept;
} // namespace Cometa
