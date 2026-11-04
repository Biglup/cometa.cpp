/**
 * @file ByronAddress.h
 *
 * @brief A legacy payment address encoded as Base58 text.
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

#include <Cometa/Address/ByronAddressAttributes.h>
#include <Cometa/Address/ByronAddressType.h>
#include <Cometa/Common/NetworkId.h>
#include <Cometa/Crypto/Blake2bHash.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_byron_address_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
class Address;

/**
 * @brief A legacy payment address encoded as Base58 text.
 *
 * The address root commits to the spending data and attributes. Spending a Byron output requires a
 * BootstrapWitness rather than a Shelley verification-key witness alone.
 */
class ByronAddress
{
  public:

    /**
     * @brief Constructs an address from its components.
     *
     * @param root Exactly 28 bytes.
     * @param attributes The metadata to copy.
     * @param type The spending credential variant.
     *
     * @throws std::invalid_argument A component is outside its supported range.
     * @throws Error Construction fails.
     */
    ByronAddress(const Blake2bHash& root, const ByronAddressAttributes& attributes, ByronAddressType type);

    /**
     * @brief Parses address bytes.
     *
     * @param bytes One complete address of the expected type.
     *
     * @throws Error The address is invalid or has the wrong type.
     */
    explicit ByronAddress(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    ByronAddress(const ByronAddress& other);

    /**
     * @brief Moves another address.
     *
     * @param other The source value.
     */
    ByronAddress(ByronAddress&& other) noexcept;

    /**
     * @brief Destroys the address.
     */
    ~ByronAddress();

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
    static ByronAddress fromBase58(std::string_view text);

    /**
     * @brief Converts a general address to this address format.
     *
     * @param address The address to copy.
     *
     * @return The converted address.
     *
     * @throws Error The address has the wrong type.
     */
    static ByronAddress fromAddress(const Address& address);

  public:

    /**
     * @brief Returns the 28-byte address root.
     *
     * @return An independent value.
     */
    Blake2bHash root() const;

    /**
     * @brief Returns the Byron metadata.
     *
     * @return An independent value.
     */
    ByronAddressAttributes attributes() const;

    /**
     * @brief Returns the Byron spending variant.
     *
     * @return An independent value.
     */
    ByronAddressType type() const;

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
    void swap(ByronAddress& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The value to copy.
     *
     * @return This value; unchanged if copying fails.
     */
    ByronAddress& operator=(const ByronAddress& other);

    /**
     * @brief Moves another address into this one.
     *
     * @param other The source; self-move is safe.
     *
     * @return This value.
     */
    ByronAddress& operator=(ByronAddress&& other) noexcept;

    /**
     * @brief Compares encoded address bytes.
     *
     * @param other The address to compare.
     *
     * @return True if the addresses match.
     */
    bool operator==(const ByronAddress& other) const;

    /**
     * @brief Compares encoded address bytes.
     *
     * @param other The address to compare.
     *
     * @return True if the addresses differ.
     */
    bool operator!=(const ByronAddress& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static ByronAddress adopt(cardano_byron_address_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_byron_address_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_byron_address_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_byron_address_t* release() noexcept;

  private:

    /**
     * @brief Adopts an owned native address.
     *
     * @param value The non-null owned reference to transfer.
     */
    explicit ByronAddress(cardano_byron_address_t& value) noexcept;

  private:

    /**
     * @brief Checks the lifetime and gets the stored reference.
     *
     * @return The borrowed native reference.
     *
     * @throws std::logic_error This value has been moved from.
     */
    cardano_byron_address_t* checked() const;

  private:

    /**
     * @brief The owned native reference, or null after a move.
     */
    cardano_byron_address_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges addresses through argument-dependent lookup.
 *
 * @param lhs The first address.
 * @param rhs The second address.
 */
void swap(ByronAddress& lhs, ByronAddress& rhs) noexcept;
} // namespace Cometa
