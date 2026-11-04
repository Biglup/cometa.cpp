/**
 * @file DRepVotingThresholds.h
 *
 * @brief Stake-weighted DRep approval thresholds for governance action categories.
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

struct cardano_drep_voting_thresholds_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Stake-weighted DRep approval thresholds for governance action categories.
 *
 * Each rational value describes the required approval fraction for its category. Committee and pool
 * voting thresholds are separate governance requirements.
 */
class DRepVotingThresholds
{
  public:

    /**
     * @brief Creates voting thresholds in ledger order.
     * @param motionNoConfidence The approval fraction required for a motion of no confidence in the committee.
     * @param committeeNormal The approval fraction required for a committee update while the committee is in a normal state.
     * @param committeeNoConfidence The approval fraction required for a committee update following a no-confidence state.
     * @param updateConstitution The approval fraction required for a new constitution.
     * @param hardForkInitiation The approval fraction required for a protocol-version upgrade.
     * @param ppNetworkGroup The approval fraction required for changes to network protocol parameters.
     * @param ppEconomicGroup The approval fraction required for changes to economic protocol parameters.
     * @param ppTechnicalGroup The approval fraction required for changes to technical protocol parameters.
     * @param ppGovernanceGroup The approval fraction required for changes to governance protocol parameters.
     * @param treasuryWithdrawal The approval fraction required for treasury withdrawals.
     */
    DRepVotingThresholds(
      const UnitInterval& motionNoConfidence,
      const UnitInterval& committeeNormal,
      const UnitInterval& committeeNoConfidence,
      const UnitInterval& updateConstitution,
      const UnitInterval& hardForkInitiation,
      const UnitInterval& ppNetworkGroup,
      const UnitInterval& ppEconomicGroup,
      const UnitInterval& ppTechnicalGroup,
      const UnitInterval& ppGovernanceGroup,
      const UnitInterval& treasuryWithdrawal);

    /**
     * @brief Copies voting thresholds.
     * @param other The thresholds to copy.
     */
    DRepVotingThresholds(const DRepVotingThresholds& other);

    /**
     * @brief Moves voting thresholds.
     * @param other The thresholds to move.
     */
    DRepVotingThresholds(DRepVotingThresholds&& other) noexcept;

    /** @brief Destroys the thresholds. */
    ~DRepVotingThresholds();

  public:

    /**
     * @brief Decodes voting thresholds from CBOR.
     * @param cbor The encoded thresholds.
     * @return The decoded thresholds.
     */
    static DRepVotingThresholds fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded thresholds.
     */
    static DRepVotingThresholds fromCbor(std::string_view cborHex);

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
     * @brief Gets the approval threshold for a new constitution.
     * @return The required stake-weighted approval fraction.
     */
    UnitInterval updateConstitution() const;

    /**
     * @brief Sets the approval threshold for a new constitution.
     * @param value The required stake-weighted approval fraction.
     */
    void setUpdateConstitution(const UnitInterval& value);

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
     * @brief Gets the approval threshold for changes to network protocol parameters.
     * @return The required stake-weighted approval fraction.
     */
    UnitInterval ppNetworkGroup() const;

    /**
     * @brief Sets the approval threshold for changes to network protocol parameters.
     * @param value The required stake-weighted approval fraction.
     */
    void setPpNetworkGroup(const UnitInterval& value);

    /**
     * @brief Gets the approval threshold for changes to economic protocol parameters.
     * @return The required stake-weighted approval fraction.
     */
    UnitInterval ppEconomicGroup() const;

    /**
     * @brief Sets the approval threshold for changes to economic protocol parameters.
     * @param value The required stake-weighted approval fraction.
     */
    void setPpEconomicGroup(const UnitInterval& value);

    /**
     * @brief Gets the approval threshold for changes to technical protocol parameters.
     * @return The required stake-weighted approval fraction.
     */
    UnitInterval ppTechnicalGroup() const;

    /**
     * @brief Sets the approval threshold for changes to technical protocol parameters.
     * @param value The required stake-weighted approval fraction.
     */
    void setPpTechnicalGroup(const UnitInterval& value);

    /**
     * @brief Gets the approval threshold for changes to governance protocol parameters.
     * @return The required stake-weighted approval fraction.
     */
    UnitInterval ppGovernanceGroup() const;

    /**
     * @brief Sets the approval threshold for changes to governance protocol parameters.
     * @param value The required stake-weighted approval fraction.
     */
    void setPpGovernanceGroup(const UnitInterval& value);

    /**
     * @brief Gets the approval threshold for treasury withdrawals.
     * @return The required stake-weighted approval fraction.
     */
    UnitInterval treasuryWithdrawal() const;

    /**
     * @brief Sets the approval threshold for treasury withdrawals.
     * @param value The required stake-weighted approval fraction.
     */
    void setTreasuryWithdrawal(const UnitInterval& value);

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
    void swap(DRepVotingThresholds& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The thresholds to copy.
     * @return This object.
     */
    DRepVotingThresholds& operator=(const DRepVotingThresholds& other);

    /**
     * @brief Assigns moved thresholds.
     * @param other The thresholds to move.
     * @return This object.
     */
    DRepVotingThresholds& operator=(DRepVotingThresholds&& other) noexcept;

    /**
     * @brief Compares thresholds.
     * @param other The thresholds to compare.
     * @return True when equal.
     */
    bool operator==(const DRepVotingThresholds& other) const;

    /**
     * @brief Compares thresholds.
     * @param other The thresholds to compare.
     * @return True when different.
     */
    bool operator!=(const DRepVotingThresholds& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static DRepVotingThresholds adopt(cardano_drep_voting_thresholds_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_drep_voting_thresholds_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_drep_voting_thresholds_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_drep_voting_thresholds_t* release() noexcept;

  private:

    /**
     * @brief Adopts owned thresholds.
     * @param value The owned thresholds.
     */
    explicit DRepVotingThresholds(cardano_drep_voting_thresholds_t* value) noexcept;

    /**
     * @brief Gets the stored thresholds.
     * @return The borrowed thresholds.
     */
    cardano_drep_voting_thresholds_t* checked() const;

  private:

    /** @brief The owned thresholds. */
    cardano_drep_voting_thresholds_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges voting thresholds through argument-dependent lookup.
 * @param lhs The first thresholds.
 * @param rhs The second thresholds.
 */
void swap(DRepVotingThresholds& lhs, DRepVotingThresholds& rhs) noexcept;
} // namespace Cometa
