/**
 * @file BaseAddress.h
 *
 * @brief BaseAddress operations.
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

#include <Cometa/Common/Credential.h>
#include <Cometa/Common/NetworkId.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_base_address_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
class Address;

/**
 * @brief A base address.
 */
class BaseAddress
{
  public:

    /**
     * @brief Constructs an address from its components.
     *
     * @param network The four-bit network ID.
     * @param payment The payment credential to copy.
     * @param stake The stake credential to copy.
     *
     * @throws std::invalid_argument A component is outside its supported range.
     * @throws Error Construction fails.
     */
    BaseAddress(NetworkId network, const Credential& payment, const Credential& stake);

    /**
     * @brief Parses address bytes.
     *
     * @param bytes One complete address of the expected type.
     *
     * @throws Error The address is invalid or has the wrong type.
     * @throws std::invalid_argument A Shelley encoding is noncanonical or contains trailing bytes.
     */
    explicit BaseAddress(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    BaseAddress(const BaseAddress& other);

    /**
     * @brief Moves another address.
     *
     * @param other The source value.
     */
    BaseAddress(BaseAddress&& other) noexcept;

    /**
     * @brief Destroys the address.
     */
    ~BaseAddress();

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
    static BaseAddress fromBech32(std::string_view text);

    /**
     * @brief Converts a general address to this address format.
     *
     * @param address The address to copy.
     *
     * @return The converted address.
     *
     * @throws Error The address has the wrong type.
     */
    static BaseAddress fromAddress(const Address& address);

  public:

    /**
     * @brief Returns the payment credential.
     *
     * @return An independent value.
     */
    Credential paymentCredential() const;

    /**
     * @brief Returns the stake credential.
     *
     * @return An independent value.
     */
    Credential stakeCredential() const;

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
     * @brief Converts to a general address.
     *
     * @return An independent address value.
     */
    Address toAddress() const;

    /**
     * @brief Exchanges two addresses.
     *
     * @param other The value to exchange.
     */
    void swap(BaseAddress& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The value to copy.
     *
     * @return This value; unchanged if copying fails.
     */
    BaseAddress& operator=(const BaseAddress& other);

    /**
     * @brief Moves another address into this one.
     *
     * @param other The source; self-move is safe.
     *
     * @return This value.
     */
    BaseAddress& operator=(BaseAddress&& other) noexcept;

    /**
     * @brief Compares encoded address bytes.
     *
     * @param other The address to compare.
     *
     * @return True if the addresses match.
     */
    bool operator==(const BaseAddress& other) const;

    /**
     * @brief Compares encoded address bytes.
     *
     * @param other The address to compare.
     *
     * @return True if the addresses differ.
     */
    bool operator!=(const BaseAddress& other) const;

  private:

    /**
     * @brief Adopts an owned native address.
     *
     * @param value The non-null owned reference to transfer.
     */
    explicit BaseAddress(cardano_base_address_t& value) noexcept;

  private:

    /**
     * @brief Checks the lifetime and gets the stored reference.
     *
     * @return The borrowed native reference.
     *
     * @throws std::logic_error This value has been moved from.
     */
    cardano_base_address_t* checked() const;

  private:

    /**
     * @brief The owned native reference, or null after a move.
     */
    cardano_base_address_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges addresses through argument-dependent lookup.
 *
 * @param lhs The first address.
 * @param rhs The second address.
 */
void swap(BaseAddress& lhs, BaseAddress& rhs) noexcept;
} // namespace Cometa
