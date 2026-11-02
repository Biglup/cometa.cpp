/**
 * @file WithdrawalMap.h
 *
 * @brief Reward addresses and withdrawal amounts in lovelace.
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
#include <optional>
#include <utility>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_withdrawal_map_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Reward addresses and withdrawal amounts in lovelace. */
class WithdrawalMap
{
  public:

    /**
     * @brief Initializes an empty withdrawal map.
     */
    WithdrawalMap();

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    WithdrawalMap(const WithdrawalMap& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    WithdrawalMap(WithdrawalMap&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~WithdrawalMap();

  public:

    /**
     * @brief Decodes exactly one CBOR map.
     *
     * @param cbor The encoded map; empty input and trailing bytes are rejected.
     *
     * @return The decoded map.
     */
    static WithdrawalMap fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded map.
     */
    static WithdrawalMap fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of withdrawals.
     *
     * @return The entry count.
     */
    std::size_t size() const;

    /**
     * @brief Checks whether there are no withdrawals.
     *
     * @return True when the map is empty.
     */
    bool empty() const;

    /**
     * @brief Looks up a withdrawal.
     *
     * @param key The reward address.
     *
     * @return The amount in lovelace, or nullopt if absent.
     */
    std::optional<std::uint64_t> get(const RewardAddress& key) const;

    /**
     * @brief Looks up a required withdrawal.
     *
     * @param key The reward address.
     *
     * @return The amount in lovelace.
     *
     * @throws std::out_of_range The key is absent.
     */
    std::uint64_t at(const RewardAddress& key) const;

    /**
     * @brief Inserts or replaces a withdrawal.
     *
     * @param key The reward address to copy.
     * @param amount The lovelace amount; zero is retained.
     */
    void set(const RewardAddress& key, std::uint64_t amount);

    /**
     * @brief Inserts or replaces a withdrawal from an encoded address.
     *
     * @param rewardAddress The Bech32 reward address.
     * @param amount The lovelace amount.
     */
    void set(std::string_view rewardAddress, std::uint64_t amount);

    /**
     * @brief Copies an entry in serialization order.
     *
     * @param index The zero-based entry index.
     *
     * @return An independent reward address and its amount.
     *
     * @throws std::out_of_range The index is outside the map.
     */
    std::pair<RewardAddress, std::uint64_t> entryAt(std::size_t index) const;

    /**
     * @brief Copies the reward addresses.
     *
     * @return Independent addresses in serialization order.
     */
    RewardAddressList keys() const;

    /**
     * @brief Copies all withdrawal entries.
     *
     * @return Independent address/amount pairs in serialization order.
     */
    std::vector<std::pair<RewardAddress, std::uint64_t>> entries() const;

    /**
     * @brief Serializes the map as CBOR.
     *
     * @return A sorted, definite-length map.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the map as CIP-116 inspection JSON.
     *
     * @return The compact inspection object.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(WithdrawalMap& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This instance.
     *
     * @remark If copying fails, this instance is unchanged.
     */
    WithdrawalMap& operator=(const WithdrawalMap& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    WithdrawalMap& operator=(WithdrawalMap&& other) noexcept;

    /**
     * @brief Compares map entries.
     *
     * @param other The map to compare.
     *
     * @return True when the entries are equal.
     */
    bool operator==(const WithdrawalMap& other) const;

    /**
     * @brief Compares map entries for inequality.
     *
     * @param other The map to compare.
     *
     * @return True when the entries differ.
     */
    bool operator!=(const WithdrawalMap& other) const;

  private:

    /** @brief Allows sub-transaction bodies to copy this value. */
    friend class SubTransactionBody;

    /** @brief Allows treasury actions to copy withdrawals. */
    friend class TreasuryWithdrawalsAction;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit WithdrawalMap(cardano_withdrawal_map_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_withdrawal_map_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_withdrawal_map_t* m_value;
};
} // namespace Cometa

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Exchanges values through argument-dependent lookup.
 *
 * @param lhs The first value.
 * @param rhs The second value.
 */
void swap(WithdrawalMap& lhs, WithdrawalMap& rhs) noexcept;
} // namespace Cometa
