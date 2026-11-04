/**
 * @file AccountBalanceInterval.h
 *
 * @brief An exact reward-account balance or a range with inclusive lower and exclusive upper bounds.
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

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_account_balance_interval_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief An exact reward-account balance or a range with inclusive lower and exclusive upper bounds.
 *
 * Amounts are in lovelace. An absent range endpoint leaves that side unbounded. Dijkstra transactions
 * use these constraints to state required account balances.
 */
class AccountBalanceInterval
{
  public:

    /**
     * @brief Creates an account balance range.
     * @param lower Inclusive lower bound, when present.
     * @param upper Exclusive upper bound, when present.
     * @throws Error Neither bound is present.
     */
    AccountBalanceInterval(std::optional<std::uint64_t> lower, std::optional<std::uint64_t> upper);

    /**
     * @brief Copies an interval.
     * @param other The interval to copy.
     */
    AccountBalanceInterval(const AccountBalanceInterval& other);

    /**
     * @brief Moves an interval.
     * @param other The interval to move.
     */
    AccountBalanceInterval(AccountBalanceInterval&& other) noexcept;

    /** @brief Destroys the interval. */
    ~AccountBalanceInterval();

  public:

    /**
     * @brief Requires an exact account balance.
     * @param balance The required lovelace balance.
     * @return The exact balance requirement.
     */
    static AccountBalanceInterval exact(std::uint64_t balance);

    /**
     * @brief Decodes one CBOR interval.
     * @param cbor The encoded interval.
     * @return The decoded interval.
     */
    static AccountBalanceInterval fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded interval.
     */
    static AccountBalanceInterval fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the inclusive lower bound.
     * @return The bound, when present.
     */
    std::optional<std::uint64_t> inclusiveLowerBound() const;

    /**
     * @brief Gets the exclusive upper bound.
     * @return The bound, when present.
     */
    std::optional<std::uint64_t> exclusiveUpperBound() const;

    /**
     * @brief Gets the exact balance.
     * @return The balance, for an exact interval.
     */
    std::optional<std::uint64_t> exactBalance() const;

    /**
     * @brief Reports whether this interval requires one exact balance.
     * @return True for an exact balance.
     */
    bool isExact() const;

    /**
     * @brief Encodes the interval as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the interval as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges intervals.
     * @param other The interval to exchange.
     */
    void swap(AccountBalanceInterval& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The interval to copy.
     * @return This interval.
     */
    AccountBalanceInterval& operator=(const AccountBalanceInterval& other);

    /**
     * @brief Assigns a moved interval.
     * @param other The interval to move.
     * @return This interval.
     */
    AccountBalanceInterval& operator=(AccountBalanceInterval&& other) noexcept;

    /**
     * @brief Compares intervals.
     * @param other The interval to compare.
     * @return True when equal.
     */
    bool operator==(const AccountBalanceInterval& other) const;

    /**
     * @brief Compares intervals.
     * @param other The interval to compare.
     * @return True when different.
     */
    bool operator!=(const AccountBalanceInterval& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static AccountBalanceInterval adopt(cardano_account_balance_interval_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_account_balance_interval_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_account_balance_interval_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_account_balance_interval_t* release() noexcept;

  private:

    /**
     * @brief Adopts an owned interval.
     * @param value The owned interval.
     */
    explicit AccountBalanceInterval(cardano_account_balance_interval_t* value) noexcept;

    /**
     * @brief Gets the stored interval.
     * @return The borrowed interval.
     */
    cardano_account_balance_interval_t* checked() const;

  private:

    /** @brief The owned interval. */
    cardano_account_balance_interval_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges intervals through argument-dependent lookup.
 * @param lhs The first interval.
 * @param rhs The second interval.
 */
void swap(AccountBalanceInterval& lhs, AccountBalanceInterval& rhs) noexcept;
} // namespace Cometa
