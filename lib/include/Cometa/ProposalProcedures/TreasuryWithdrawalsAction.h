/**
 * @file TreasuryWithdrawalsAction.h
 *
 * @brief A governance action proposing payments from the treasury to reward accounts.
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

#include <Cometa/Common/WithdrawalMap.h>
#include <Cometa/Crypto/Blake2bHash.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_treasury_withdrawals_action_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A governance action proposing payments from the treasury to reward accounts.
 *
 * The withdrawal map supplies recipients and lovelace amounts. These payments depend on enactment,
 * unlike ordinary transaction reward withdrawals.
 */
class TreasuryWithdrawalsAction
{
  public:

    /**
     * @brief Creates a treasury withdrawal action.
     * @param withdrawals The withdrawals to propose.
     * @param policyHash The optional policy script hash.
     */
    explicit TreasuryWithdrawalsAction(const WithdrawalMap& withdrawals, const std::optional<Blake2bHash>& policyHash = std::nullopt);

    /**
     * @brief Copies a treasury withdrawal action.
     * @param other The treasury withdrawal action to copy.
     */
    TreasuryWithdrawalsAction(const TreasuryWithdrawalsAction& other);

    /**
     * @brief Moves a treasury withdrawal action.
     * @param other The treasury withdrawal action to move.
     */
    TreasuryWithdrawalsAction(TreasuryWithdrawalsAction&& other) noexcept;

    /** @brief Destroys the treasury withdrawal action. */
    ~TreasuryWithdrawalsAction();

  public:

    /**
     * @brief Gets the proposed withdrawals.
     * @return An independent withdrawal map.
     */
    WithdrawalMap withdrawals() const;

    /**
     * @brief Sets the proposed withdrawals.
     * @param value The withdrawals to set.
     */
    void setWithdrawals(const WithdrawalMap& value);

    /**
     * @brief Decodes CBOR.
     * @param cbor Encoded bytes.
     * @return The decoded treasury withdrawal action.
     */
    static TreasuryWithdrawalsAction fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex Hexadecimal bytes.
     * @return The decoded treasury withdrawal action.
     */
    static TreasuryWithdrawalsAction fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the policy script hash.
     * @return An independent hash, or empty when absent.
     */
    std::optional<Blake2bHash> policyHash() const;

    /**
     * @brief Sets the policy script hash.
     * @param value The hash, or empty to remove it.
     */
    void setPolicyHash(const std::optional<Blake2bHash>& value);

    /**
     * @brief Encodes CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the treasury withdrawal action as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges treasury withdrawal actions.
     * @param other The treasury withdrawal action to exchange.
     */
    void swap(TreasuryWithdrawalsAction& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The treasury withdrawal action to copy.
     * @return This treasury withdrawal action.
     */
    TreasuryWithdrawalsAction& operator=(const TreasuryWithdrawalsAction& other);

    /**
     * @brief Assigns a moved treasury withdrawal action.
     * @param other The treasury withdrawal action to move.
     * @return This treasury withdrawal action.
     */
    TreasuryWithdrawalsAction& operator=(TreasuryWithdrawalsAction&& other) noexcept;

    /**
     * @brief Compares treasury withdrawal actions.
     * @param other The treasury withdrawal action to compare.
     * @return True when equal.
     */
    bool operator==(const TreasuryWithdrawalsAction& other) const;

    /**
     * @brief Compares treasury withdrawal actions.
     * @param other The treasury withdrawal action to compare.
     * @return True when different.
     */
    bool operator!=(const TreasuryWithdrawalsAction& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static TreasuryWithdrawalsAction adopt(cardano_treasury_withdrawals_action_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_treasury_withdrawals_action_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_treasury_withdrawals_action_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_treasury_withdrawals_action_t* release() noexcept;

  private:

    /** @brief Allows governance actions to copy treasury withdrawal actions. */

    /**
     * @brief Adopts an owned treasury withdrawal action.
     * @param value The owned treasury withdrawal action.
     */
    explicit TreasuryWithdrawalsAction(cardano_treasury_withdrawals_action_t* value) noexcept;

    /**
     * @brief Gets the stored treasury withdrawal action.
     * @return The borrowed treasury withdrawal action.
     */
    cardano_treasury_withdrawals_action_t* checked() const;

  private:

    /** @brief The owned treasury withdrawal action. */
    cardano_treasury_withdrawals_action_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges treasury withdrawal actions.
 * @param lhs The first treasury withdrawal action.
 * @param rhs The second treasury withdrawal action.
 */
void swap(TreasuryWithdrawalsAction& lhs, TreasuryWithdrawalsAction& rhs) noexcept;
} // namespace Cometa
