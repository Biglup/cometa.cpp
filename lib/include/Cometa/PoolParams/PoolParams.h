/**
 * @file PoolParams.h
 *
 * @brief Stake pool registration parameters.
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

#include <Cometa/Address/RewardAddress.h>
#include <Cometa/Common/UnitInterval.h>
#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/PoolParams/BlsKey.h>
#include <Cometa/PoolParams/PoolMetadata.h>
#include <Cometa/PoolParams/PoolOwners.h>
#include <Cometa/PoolParams/Relays.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_pool_params_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Stake pool registration parameters.
 */
class PoolParams
{
  public:

    /**
     * @brief Initializes a pool registration.
     *
     * @param operatorKeyHash The 28-byte pool operator key hash.
     * @param vrfKeyHash The 32-byte VRF key hash.
     * @param pledge The pledge in lovelace.
     * @param cost The fixed cost in lovelace.
     * @param margin The pool margin.
     * @param rewardAccount The pool reward address.
     * @param owners The pool owner key hashes.
     * @param relays The pool relay list.
     * @param metadata Optional pool metadata.
     */
    PoolParams(const Blake2bHash& operatorKeyHash, const Blake2bHash& vrfKeyHash, std::uint64_t pledge, std::uint64_t cost, const UnitInterval& margin, const RewardAddress& rewardAccount, const PoolOwners& owners, const Relays& relays, const std::optional<PoolMetadata>& metadata = std::nullopt);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    PoolParams(const PoolParams& other);

    /**
     * @brief Moves a value.
     *
     * @param other The value to move.
     */
    PoolParams(PoolParams&& other) noexcept;

    /**
     * @brief Destroys the value.
     */
    ~PoolParams();

  public:

    /**
     * @brief Decodes one pool parameter CBOR sequence.
     *
     * @param cbor The complete encoded sequence.
     *
     * @return The decoded value.
     */
    static PoolParams fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes a hexadecimal CBOR sequence.
     *
     * @param cborHex Hexadecimal CBOR without a prefix.
     *
     * @return The decoded value.
     */
    static PoolParams fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the operator key hash.
     *
     * @return An independent value.
     */
    Blake2bHash operatorKeyHash() const;

    /**
     * @brief Replaces the operator key hash.
     *
     * @param value The new 28-byte operator key hash to copy.
     */
    void setOperatorKeyHash(const Blake2bHash& value);

    /**
     * @brief Gets the VRF key hash.
     *
     * @return An independent value.
     */
    Blake2bHash vrfKeyHash() const;

    /**
     * @brief Replaces the VRF key hash.
     *
     * @param value The new 32-byte VRF key hash to copy.
     */
    void setVrfKeyHash(const Blake2bHash& value);

    /**
     * @brief Gets the pledge amount.
     *
     * @return The amount.
     */
    std::uint64_t pledge() const;

    /**
     * @brief Replaces the pledge amount.
     *
     * @param value The new pledge amount to copy.
     */
    void setPledge(std::uint64_t value);

    /**
     * @brief Gets the pool cost.
     *
     * @return The amount.
     */
    std::uint64_t cost() const;

    /**
     * @brief Replaces the pool cost.
     *
     * @param value The new pool cost to copy.
     */
    void setCost(std::uint64_t value);

    /**
     * @brief Gets the pool margin.
     *
     * @return An independent value.
     */
    UnitInterval margin() const;

    /**
     * @brief Replaces the pool margin.
     *
     * @param value The new pool margin to copy.
     */
    void setMargin(const UnitInterval& value);

    /**
     * @brief Gets the reward address.
     *
     * @return An independent value.
     */
    RewardAddress rewardAccount() const;

    /**
     * @brief Replaces the reward address.
     *
     * @param value The new reward address to copy.
     */
    void setRewardAccount(const RewardAddress& value);

    /**
     * @brief Gets the owner key hashes.
     *
     * @return An independent value.
     */
    PoolOwners owners() const;

    /**
     * @brief Replaces the owner key hashes.
     *
     * @param value The new owner key hashes to copy.
     */
    void setOwners(const PoolOwners& value);

    /**
     * @brief Gets the relay list.
     *
     * @return An independent value.
     */
    Relays relays() const;

    /**
     * @brief Replaces the relay list.
     *
     * @param value The new relay list to copy.
     */
    void setRelays(const Relays& value);

    /**
     * @brief Gets the optional BLS key.
     *
     * @return An independent value when present.
     */
    std::optional<BlsKey> blsKey() const;

    /**
     * @brief Replaces or removes the BLS key.
     *
     * @param value The new value, or std::nullopt to remove it.
     */
    void setBlsKey(const std::optional<BlsKey>& value);

    /**
     * @brief Gets the optional pool metadata.
     *
     * @return An independent value when present.
     */
    std::optional<PoolMetadata> metadata() const;

    /**
     * @brief Replaces or removes the pool metadata.
     *
     * @param value The new value, or std::nullopt to remove it.
     */
    void setMetadata(const std::optional<PoolMetadata>& value);

    /**
     * @brief Encodes the pool parameters as a CBOR sequence.
     *
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the parameters as CIP-116 JSON.
     *
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges values.
     *
     * @param other The value to exchange.
     */
    void swap(PoolParams& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This value.
     */
    PoolParams& operator=(const PoolParams& other);

    /**
     * @brief Moves a value into this one.
     *
     * @param other The source value.
     *
     * @return This value.
     */
    PoolParams& operator=(PoolParams&& other) noexcept;

    /**
     * @brief Compares all fields.
     *
     * @param other The value to compare.
     *
     * @return True when equal.
     */
    bool operator==(const PoolParams& other) const;

    /**
     * @brief Compares all fields.
     *
     * @param other The value to compare.
     *
     * @return True when different.
     */
    bool operator!=(const PoolParams& other) const;

  private:

    /** @brief Allows pool registration certificates to copy their parameters. */
    friend class PoolRegistrationCert;

    /**
     * @brief Adopts an owned native value.
     *
     * @param value The owned reference.
     */
    explicit PoolParams(cardano_pool_params_t* value) noexcept;

    /**
     * @brief Gets the stored native reference.
     *
     * @return The borrowed reference.
     */
    cardano_pool_params_t* checked() const;

    /**
     * @brief Checks a hash length.
     *
     * @param value The hash to inspect.
     *
     * @param size The required byte count.
     */
    static void validateHash(const Blake2bHash& value, std::size_t size);

  private:

    /**
     * @brief The owned native reference, or null after a move.
     */
    cardano_pool_params_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges values through argument-dependent lookup.
 *
 * @param lhs The first value.
 *
 * @param rhs The second value.
 */
void swap(PoolParams& lhs, PoolParams& rhs) noexcept;
} // namespace Cometa
