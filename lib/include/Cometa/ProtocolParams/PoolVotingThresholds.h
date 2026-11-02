/**
 * @file PoolVotingThresholds.h
 *
 * @brief Governance voting thresholds.
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
/** @brief Rational voting thresholds for stake pools. */
class PoolVotingThresholds
{
  public:

    /**
     * @brief Creates voting thresholds in ledger order.
     * @param motionNoConfidence The motionNoConfidence threshold.
     * @param committeeNormal The committeeNormal threshold.
     * @param committeeNoConfidence The committeeNoConfidence threshold.
     * @param hardForkInitiation The hardForkInitiation threshold.
     * @param securityRelevantParam The securityRelevantParam threshold.
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
     * @brief Gets the motionNoConfidence threshold.
     * @return The threshold.
     */
    UnitInterval motionNoConfidence() const;

    /**
     * @brief Sets the motionNoConfidence threshold.
     * @param value The new threshold.
     */
    void setMotionNoConfidence(const UnitInterval& value);

    /**
     * @brief Gets the committeeNormal threshold.
     * @return The threshold.
     */
    UnitInterval committeeNormal() const;

    /**
     * @brief Sets the committeeNormal threshold.
     * @param value The new threshold.
     */
    void setCommitteeNormal(const UnitInterval& value);

    /**
     * @brief Gets the committeeNoConfidence threshold.
     * @return The threshold.
     */
    UnitInterval committeeNoConfidence() const;

    /**
     * @brief Sets the committeeNoConfidence threshold.
     * @param value The new threshold.
     */
    void setCommitteeNoConfidence(const UnitInterval& value);

    /**
     * @brief Gets the hardForkInitiation threshold.
     * @return The threshold.
     */
    UnitInterval hardForkInitiation() const;

    /**
     * @brief Sets the hardForkInitiation threshold.
     * @param value The new threshold.
     */
    void setHardForkInitiation(const UnitInterval& value);

    /**
     * @brief Gets the securityRelevantParam threshold.
     * @return The threshold.
     */
    UnitInterval securityRelevantParam() const;

    /**
     * @brief Sets the securityRelevantParam threshold.
     * @param value The new threshold.
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

    /** @brief Allows protocol updates to copy native values. */
    friend class ProtocolParamUpdate;

    /** @brief Allows protocol parameters to copy native values. */
    friend class ProtocolParameters;

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
