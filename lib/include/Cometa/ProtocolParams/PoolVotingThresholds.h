/**
 * @file PoolVotingThresholds.h
 *
 * @brief Stake-weighted pool approval thresholds for governance action categories.
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

#include <Cometa/Common/UnitInterval.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_pool_voting_thresholds_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Stake-weighted pool approval thresholds for governance action categories.
 *
 * These thresholds apply to stake pool operator votes, not DRep votes. The applicable category depends
 * on the governance action.
 */
class PoolVotingThresholds
{
  public:

    /**
     * @brief Creates voting thresholds in ledger order.
     * @param motionNoConfidence The approval fraction required for a motion of no confidence in the committee.
     * @param committeeNormal The approval fraction required for a committee update while the committee is in a normal state.
     * @param committeeNoConfidence The approval fraction required for a committee update following a no-confidence state.
     * @param hardForkInitiation The approval fraction required for a protocol-version upgrade.
     * @param securityRelevantParam The approval fraction required for changes to security-relevant protocol parameters.
     */
    PoolVotingThresholds(
      const UnitInterval& motionNoConfidence,
      const UnitInterval& committeeNormal,
      const UnitInterval& committeeNoConfidence,
      const UnitInterval& hardForkInitiation,
      const UnitInterval& securityRelevantParam);

    /**
     * @brief Copies voting thresholds.
     * @param other The thresholds to copy.
     */
    PoolVotingThresholds(const PoolVotingThresholds& other);

    /**
     * @brief Moves voting thresholds.
     * @param other The thresholds to move.
     */
    PoolVotingThresholds(PoolVotingThresholds&& other) noexcept;

    /** @brief Destroys the thresholds. */
    ~PoolVotingThresholds();

  public:

    /**
     * @brief Decodes voting thresholds from CBOR.
     * @param cbor The encoded thresholds.
     * @return The decoded thresholds.
     */
    static PoolVotingThresholds fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded thresholds.
     */
    static PoolVotingThresholds fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the approval threshold for a motion of no confidence in the committee.
     * @return The required stake-weighted approval fraction.
     */
    UnitInterval motionNoConfidence() const;

    /**
     * @brief Sets the approval threshold for a motion of no confidence in the committee.
     * @param value The required stake-weighted approval fraction.
     */
    void setMotionNoConfidence(const UnitInterval& value);

    /**
     * @brief Gets the approval threshold for a committee update while the committee is in a normal state.
     * @return The required stake-weighted approval fraction.
     */
    UnitInterval committeeNormal() const;

    /**
     * @brief Sets the approval threshold for a committee update while the committee is in a normal state.
     * @param value The required stake-weighted approval fraction.
     */
    void setCommitteeNormal(const UnitInterval& value);

    /**
     * @brief Gets the approval threshold for a committee update following a no-confidence state.
     * @return The required stake-weighted approval fraction.
     */
    UnitInterval committeeNoConfidence() const;

    /**
     * @brief Sets the approval threshold for a committee update following a no-confidence state.
     * @param value The required stake-weighted approval fraction.
     */
    void setCommitteeNoConfidence(const UnitInterval& value);

    /**
     * @brief Gets the approval threshold for a protocol-version upgrade.
     * @return The required stake-weighted approval fraction.
     */
    UnitInterval hardForkInitiation() const;

    /**
     * @brief Sets the approval threshold for a protocol-version upgrade.
     * @param value The required stake-weighted approval fraction.
     */
    void setHardForkInitiation(const UnitInterval& value);

    /**
     * @brief Gets the approval threshold for changes to security-relevant protocol parameters.
     * @return The required stake-weighted approval fraction.
     */
    UnitInterval securityRelevantParam() const;

    /**
     * @brief Sets the approval threshold for changes to security-relevant protocol parameters.
     * @param value The required stake-weighted approval fraction.
     */
    void setSecurityRelevantParam(const UnitInterval& value);

    /**
     * @brief Encodes the thresholds as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the thresholds as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges thresholds.
     * @param other The thresholds to exchange.
     */
    void swap(PoolVotingThresholds& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The thresholds to copy.
     * @return This object.
     */
    PoolVotingThresholds& operator=(const PoolVotingThresholds& other);

    /**
     * @brief Assigns moved thresholds.
     * @param other The thresholds to move.
     * @return This object.
     */
    PoolVotingThresholds& operator=(PoolVotingThresholds&& other) noexcept;

    /**
     * @brief Compares thresholds.
     * @param other The thresholds to compare.
     * @return True when equal.
     */
    bool operator==(const PoolVotingThresholds& other) const;

    /**
     * @brief Compares thresholds.
     * @param other The thresholds to compare.
     * @return True when different.
     */
    bool operator!=(const PoolVotingThresholds& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static PoolVotingThresholds adopt(cardano_pool_voting_thresholds_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_pool_voting_thresholds_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_pool_voting_thresholds_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_pool_voting_thresholds_t* release() noexcept;

  private:

    /**
     * @brief Adopts owned thresholds.
     * @param value The owned thresholds.
     */
    explicit PoolVotingThresholds(cardano_pool_voting_thresholds_t* value) noexcept;

    /**
     * @brief Gets the stored thresholds.
     * @return The borrowed thresholds.
     */
    cardano_pool_voting_thresholds_t* checked() const;

  private:

    /** @brief The owned thresholds. */
    cardano_pool_voting_thresholds_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges voting thresholds through argument-dependent lookup.
 * @param lhs The first thresholds.
 * @param rhs The second thresholds.
 */
void swap(PoolVotingThresholds& lhs, PoolVotingThresholds& rhs) noexcept;
} // namespace Cometa
