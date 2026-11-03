/**
 * @file SingleHostAddrRelay.h
 *
 * @brief A stake pool relay identified by an IP address and optional port.
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

#include <Cometa/PoolParams/Ipv4.h>
#include <Cometa/PoolParams/Ipv6.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_single_host_addr_relay_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A stake pool relay identified by an IP address and optional port.
 *
 * IPv4 and IPv6 are independently optional. The relay advertises a network endpoint; constructing this
 * value does not connect to it.
 */
class SingleHostAddrRelay
{
  public:

    /**
     * @brief Initializes an IP-address relay.
     *
     * @param port The port number, or nullopt when absent.
     * @param ipv4 The optional IPv4 address to copy.
     * @param ipv6 The optional IPv6 address to copy.
     */
    explicit SingleHostAddrRelay(std::optional<std::uint16_t> port = std::nullopt, const std::optional<Ipv4>& ipv4 = std::nullopt, const std::optional<Ipv6>& ipv6 = std::nullopt);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The source value.
     */
    SingleHostAddrRelay(const SingleHostAddrRelay& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    SingleHostAddrRelay(SingleHostAddrRelay&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~SingleHostAddrRelay();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; empty input and trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static SingleHostAddrRelay fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static SingleHostAddrRelay fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the optional port number.
     *
     * @return The port number, or nullopt when absent.
     */
    std::optional<std::uint16_t> port() const;

    /**
     * @brief Replaces or removes the port number.
     *
     * @param port The new port number, or nullopt to remove it.
     */
    void setPort(std::optional<std::uint16_t> port);

    /**
     * @brief Gets the optional IP address.
     *
     * @return An independent address, or nullopt when absent.
     */
    std::optional<Ipv4> ipv4() const;

    /**
     * @brief Replaces or removes the IP address.
     *
     * @param address The address to copy, or nullopt to remove it.
     */
    void setIpv4(const std::optional<Ipv4>& address);

    /**
     * @brief Gets the optional IP address.
     *
     * @return An independent address, or nullopt when absent.
     */
    std::optional<Ipv6> ipv6() const;

    /**
     * @brief Replaces or removes the IP address.
     *
     * @param address The address to copy, or nullopt to remove it.
     */
    void setIpv6(const std::optional<Ipv6>& address);

    /**
     * @brief Serializes this value as CBOR.
     *
     * @return The encoded value.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes this value as CIP-116 JSON.
     *
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges values without allocation.
     *
     * @param other The value to exchange.
     */
    void swap(SingleHostAddrRelay& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This instance; unchanged if copying fails.
     */
    SingleHostAddrRelay& operator=(const SingleHostAddrRelay& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source; self-move assignment is safe.
     *
     * @return This instance.
     */
    SingleHostAddrRelay& operator=(SingleHostAddrRelay&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when equal.
     */
    bool operator==(const SingleHostAddrRelay& other) const;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when different.
     */
    bool operator!=(const SingleHostAddrRelay& other) const;

  private:

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference transferred to this instance.
     */
    explicit SingleHostAddrRelay(cardano_single_host_addr_relay_t& value) noexcept;

  private:

    /**
     * @brief Checks and gets the stored reference.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_single_host_addr_relay_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_single_host_addr_relay_t* m_value;

    /** @brief Allows Relay to exchange component values. */
    friend class Relay;
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
void swap(SingleHostAddrRelay& lhs, SingleHostAddrRelay& rhs) noexcept;
} // namespace Cometa
