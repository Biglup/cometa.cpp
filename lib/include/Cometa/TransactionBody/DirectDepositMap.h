/**
 * @file DirectDepositMap.h
 *
 * @brief Direct deposits to reward accounts.
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

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_direct_deposit_map_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Direct deposit amounts by reward address. */
class DirectDepositMap
{
  public:

    /** @brief Creates an empty direct deposit map. */
    DirectDepositMap();

    /**
     * @brief Copies a map.
     * @param other The map to copy.
     */
    DirectDepositMap(const DirectDepositMap& other);

    /**
     * @brief Moves a map.
     * @param other The map to move.
     */
    DirectDepositMap(DirectDepositMap&& other) noexcept;

    /** @brief Destroys the map. */
    ~DirectDepositMap();

  public:

    /**
     * @brief Decodes one CBOR map.
     * @param cbor The encoded map.
     * @return The decoded map.
     */
    static DirectDepositMap fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded map.
     */
    static DirectDepositMap fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of deposits.
     * @return The entry count.
     */
    std::size_t size() const;

    /**
     * @brief Reports whether the map is empty.
     * @return True when empty.
     */
    bool empty() const;

    /**
     * @brief Finds a direct deposit.
     * @param address The reward address to search for.
     * @return The amount in lovelace, when found.
     */
    std::optional<std::uint64_t> get(const RewardAddress& address) const;

    /**
     * @brief Inserts a direct deposit.
     * @param address The reward address to copy.
     * @param amount The lovelace amount.
     * @throws Error The address is already present.
     */
    void insert(const RewardAddress& address, std::uint64_t amount);

    /**
     * @brief Inserts a direct deposit by encoded address.
     * @param address The Bech32 reward address.
     * @param amount The lovelace amount.
     * @throws Error The address is already present.
     */
    void insert(std::string_view address, std::uint64_t amount);

    /**
     * @brief Copies an entry in canonical CBOR order.
     * @param index The zero-based index.
     * @return The address and amount.
     * @throws std::out_of_range The index is outside the map.
     */
    std::pair<RewardAddress, std::uint64_t> entryAt(std::size_t index) const;

    /**
     * @brief Copies the addresses in canonical CBOR order.
     * @return Independent reward addresses.
     */
    RewardAddressList keys() const;

    /**
     * @brief Copies all entries in canonical CBOR order.
     * @return The address and amount pairs.
     */
    std::vector<std::pair<RewardAddress, std::uint64_t>> entries() const;

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
    void swap(DirectDepositMap& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The map to copy.
     * @return This map.
     */
    DirectDepositMap& operator=(const DirectDepositMap& other);

    /**
     * @brief Assigns a moved map.
     * @param other The map to move.
     * @return This map.
     */
    DirectDepositMap& operator=(DirectDepositMap&& other) noexcept;

    /**
     * @brief Compares maps.
     * @param other The map to compare.
     * @return True when equal.
     */
    bool operator==(const DirectDepositMap& other) const;

    /**
     * @brief Compares maps.
     * @param other The map to compare.
     * @return True when different.
     */
    bool operator!=(const DirectDepositMap& other) const;

  private:

    /** @brief Allows sub-transaction bodies to copy this value. */
    friend class SubTransactionBody;

    /** @brief Allows transaction bodies to copy this value. */
    friend class TransactionBody;

    /**
     * @brief Adopts an owned map.
     * @param value The owned map.
     */
    explicit DirectDepositMap(cardano_direct_deposit_map_t* value) noexcept;

    /**
     * @brief Gets the stored map.
     * @return The borrowed map.
     */
    cardano_direct_deposit_map_t* checked() const;

  private:

    /** @brief The owned map. */
    cardano_direct_deposit_map_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges maps through argument-dependent lookup.
 * @param lhs The first map.
 * @param rhs The second map.
 */
void swap(DirectDepositMap& lhs, DirectDepositMap& rhs) noexcept;
} // namespace Cometa
