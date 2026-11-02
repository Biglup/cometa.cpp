/**
 * @file Address.h
 *
 * @brief Address operations.
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

#include <Cometa/Address/AddressType.h>
#include <Cometa/Address/BaseAddress.h>
#include <Cometa/Address/ByronAddress.h>
#include <Cometa/Address/EnterpriseAddress.h>
#include <Cometa/Address/PointerAddress.h>
#include <Cometa/Address/RewardAddress.h>
#include <Cometa/Common/NetworkId.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_address_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A Cardano address.
 */
class Address
{
  public:

    /**
     * @brief Parses address bytes.
     *
     * @param bytes One complete address of the expected type.
     *
     * @throws Error The address is invalid or has the wrong type.
     * @throws std::invalid_argument A Shelley encoding is noncanonical or contains trailing bytes.
     */
    explicit Address(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    Address(const Address& other);

    /**
     * @brief Moves another address.
     *
     * @param other The source value.
     */
    Address(Address&& other) noexcept;

    /**
     * @brief Destroys the address.
     */
    ~Address();

  public:

    /**
     * @brief Parses an encoded address.
     *
     * @param text The complete address text.
     *
     * @return The decoded address.
     *
     * @throws Error The address is invalid or has the wrong type.
     * @throws std::invalid_argument The encoding or network prefix is invalid.
     */
    static Address fromString(std::string_view text);

    /**
     * @brief Checks whether text encodes a valid address.
     *
     * @param text The text to validate.
     *
     * @return True if parsing succeeds for the expected format.
     */
    static bool isValid(std::string_view text);

    /**
     * @brief Checks whether text encodes a valid Shelley address.
     *
     * @param text The text to validate.
     *
     * @return True if parsing succeeds for the expected format.
     */
    static bool isValidBech32(std::string_view text);

    /**
     * @brief Checks whether text encodes a valid Byron address.
     *
     * @param text The text to validate.
     *
     * @return True if parsing succeeds for the expected format.
     */
    static bool isValidByron(std::string_view text);

  public:

    /**
     * @brief Returns the encoded address format.
     *
     * @return An independent value.
     */
    AddressType type() const;

    /**
     * @brief Returns the address network.
     *
     * @return The network identifier; Shelley addresses preserve the four-bit value.
     */
    NetworkId networkId() const;

    /**
     * @brief Copies the encoded address bytes.
     *
     * @return The address bytes.
     */
    std::vector<std::uint8_t> toBytes() const;

    /**
     * @brief Formats the address as text.
     *
     * @return Base58 for Byron, or lowercase Bech32 for Shelley addresses.
     */
    std::string toString() const;

    /**
     * @brief Converts to BaseAddress.
     *
     * @return An independent value, or nullopt for a different address format.
     */
    std::optional<BaseAddress> asBase() const;

    /**
     * @brief Converts to EnterpriseAddress.
     *
     * @return An independent value, or nullopt for a different address format.
     */
    std::optional<EnterpriseAddress> asEnterprise() const;

    /**
     * @brief Converts to RewardAddress.
     *
     * @return An independent value, or nullopt for a different address format.
     */
    std::optional<RewardAddress> asReward() const;

    /**
     * @brief Converts to PointerAddress.
     *
     * @return An independent value, or nullopt for a different address format.
     */
    std::optional<PointerAddress> asPointer() const;

    /**
     * @brief Converts to ByronAddress.
     *
     * @return An independent value, or nullopt for a different address format.
     */
    std::optional<ByronAddress> asByron() const;

    /**
     * @brief Exchanges two addresses.
     *
     * @param other The value to exchange.
     */
    void swap(Address& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The value to copy.
     *
     * @return This value; unchanged if copying fails.
     */
    Address& operator=(const Address& other);

    /**
     * @brief Moves another address into this one.
     *
     * @param other The source; self-move is safe.
     *
     * @return This value.
     */
    Address& operator=(Address&& other) noexcept;

    /**
     * @brief Compares encoded address bytes.
     *
     * @param other The address to compare.
     *
     * @return True if the addresses match.
     */
    bool operator==(const Address& other) const;

    /**
     * @brief Compares encoded address bytes.
     *
     * @param other The address to compare.
     *
     * @return True if the addresses differ.
     */
    bool operator!=(const Address& other) const;

  private:

    /** @brief Allows TransactionOutput to copy components. */
    friend class TransactionOutput;

    /**
     * @brief Allows BaseAddress conversions to inspect this address.
     */
    friend class BaseAddress;

    /**
     * @brief Allows EnterpriseAddress conversions to inspect this address.
     */
    friend class EnterpriseAddress;

    /**
     * @brief Allows RewardAddress conversions to inspect this address.
     */
    friend class RewardAddress;

    /**
     * @brief Allows PointerAddress conversions to inspect this address.
     */
    friend class PointerAddress;

    /**
     * @brief Allows ByronAddress conversions to inspect this address.
     */
    friend class ByronAddress;

    /**
     * @brief Adopts an owned native address.
     *
     * @param value The non-null owned reference to transfer.
     */
    explicit Address(cardano_address_t& value) noexcept;

  private:

    /**
     * @brief Checks the lifetime and gets the stored reference.
     *
     * @return The borrowed native reference.
     *
     * @throws std::logic_error This value has been moved from.
     */
    cardano_address_t* checked() const;

  private:

    /**
     * @brief The owned native reference, or null after a move.
     */
    cardano_address_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges addresses through argument-dependent lookup.
 *
 * @param lhs The first address.
 * @param rhs The second address.
 */
void swap(Address& lhs, Address& rhs) noexcept;
} // namespace Cometa
