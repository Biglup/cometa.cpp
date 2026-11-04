/**
 * @file Address.h
 *
 * @brief A Cardano payment or reward address, including Shelley and Byron formats.
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
 * @brief A Cardano payment or reward address, including Shelley and Byron formats.
 *
 * Use fromString() for Bech32 or Base58 text. Inspect type() before extracting a specific address
 * form. A reward address identifies a staking account rather than a destination for transaction
 * outputs.
 */
class Address
{
  public:

    /**
     * @brief Initializes an independent copy of a BaseAddress.
     * @param address The address to copy.
     */
    Address(const BaseAddress& address);

    /**
     * @brief Initializes an independent copy of an EnterpriseAddress.
     * @param address The address to copy.
     */
    Address(const EnterpriseAddress& address);

    /**
     * @brief Initializes an independent copy of a PointerAddress.
     * @param address The address to copy.
     */
    Address(const PointerAddress& address);

    /**
     * @brief Initializes an independent copy of a RewardAddress.
     * @param address The address to copy.
     */
    Address(const RewardAddress& address);

    /**
     * @brief Initializes an independent copy of a ByronAddress.
     * @param address The address to copy.
     */
    Address(const ByronAddress& address);

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

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static Address adopt(cardano_address_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_address_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_address_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_address_t* release() noexcept;

  private:

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
