/**
 * @file AccountBalanceIntervalsMap.h
 *
 * @brief Account balance requirements by reward address.
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

#include <Cometa/Common/RewardAddressList.h>
#include <Cometa/TransactionBody/AccountBalanceInterval.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_account_balance_intervals_map_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Reward addresses and their required account balances. */
class AccountBalanceIntervalsMap
{
  public:

    /** @brief Creates an empty map. */
    AccountBalanceIntervalsMap();

    /**
     * @brief Copies a map.
     * @param other The map to copy.
     */
    AccountBalanceIntervalsMap(const AccountBalanceIntervalsMap& other);

    /**
     * @brief Moves a map.
     * @param other The map to move.
     */
    AccountBalanceIntervalsMap(AccountBalanceIntervalsMap&& other) noexcept;

    /** @brief Destroys the map. */
    ~AccountBalanceIntervalsMap();

  public:

    /**
     * @brief Decodes one CBOR map.
     * @param cbor The encoded map.
     * @return The decoded map.
     */
    static AccountBalanceIntervalsMap fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded map.
     */
    static AccountBalanceIntervalsMap fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the entry count.
     * @return The number of entries.
     */
    std::size_t size() const;

    /**
     * @brief Reports whether the map is empty.
     * @return True when empty.
     */
    bool empty() const;

    /**
     * @brief Finds an account balance requirement.
     * @param address The reward address to search for.
     * @return An independent interval, when found.
     */
    std::optional<AccountBalanceInterval> get(const RewardAddress& address) const;

    /**
     * @brief Inserts an account balance requirement.
     * @param address The reward address to copy.
     * @param interval The interval to copy.
     * @throws Error The address is already present.
     */
    void insert(const RewardAddress& address, const AccountBalanceInterval& interval);

    /**
     * @brief Inserts an account balance requirement by encoded address.
     * @param address The Bech32 reward address.
     * @param interval The interval to copy.
     * @throws Error The address is already present.
     */
    void insert(std::string_view address, const AccountBalanceInterval& interval);

    /**
     * @brief Copies an entry in canonical CBOR order.
     * @param index The zero-based index.
     * @return The address and interval.
     * @throws std::out_of_range The index is outside the map.
     */
    std::pair<RewardAddress, AccountBalanceInterval> entryAt(std::size_t index) const;

    /**
     * @brief Copies the reward addresses in canonical CBOR order.
     * @return Independent reward addresses.
     */
    RewardAddressList keys() const;

    /**
     * @brief Copies all entries in canonical CBOR order.
     * @return The address and interval pairs.
     */
    std::vector<std::pair<RewardAddress, AccountBalanceInterval>> entries() const;

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
    void swap(AccountBalanceIntervalsMap& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The map to copy.
     * @return This map.
     */
    AccountBalanceIntervalsMap& operator=(const AccountBalanceIntervalsMap& other);

    /**
     * @brief Assigns a moved map.
     * @param other The map to move.
     * @return This map.
     */
    AccountBalanceIntervalsMap& operator=(AccountBalanceIntervalsMap&& other) noexcept;

    /**
     * @brief Compares maps.
     * @param other The map to compare.
     * @return True when equal.
     */
    bool operator==(const AccountBalanceIntervalsMap& other) const;

    /**
     * @brief Compares maps.
     * @param other The map to compare.
     * @return True when different.
     */
    bool operator!=(const AccountBalanceIntervalsMap& other) const;

  private:

    /** @brief Allows sub-transaction bodies to copy this value. */
    friend class SubTransactionBody;

    /**
     * @brief Adopts an owned map.
     * @param value The owned map.
     */
    explicit AccountBalanceIntervalsMap(cardano_account_balance_intervals_map_t* value) noexcept;

    /**
     * @brief Gets the stored map.
     * @return The borrowed map.
     */
    cardano_account_balance_intervals_map_t* checked() const;

  private:

    /** @brief The owned map. */
    cardano_account_balance_intervals_map_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges maps through argument-dependent lookup.
 * @param lhs The first map.
 * @param rhs The second map.
 */
void swap(AccountBalanceIntervalsMap& lhs, AccountBalanceIntervalsMap& rhs) noexcept;
} // namespace Cometa
