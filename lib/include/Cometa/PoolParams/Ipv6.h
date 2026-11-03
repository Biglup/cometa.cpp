/**
 * @file Ipv6.h
 *
 * @brief An IPv6 address used to describe a stake pool relay.
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
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_ipv6_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief An IPv6 address used to describe a stake pool relay.
 *
 * The sixteen bytes are in network order. Use fromString() with the supported textual form, or
 * construct from bytes when handling externally parsed addresses.
 */
class Ipv6
{
  public:

    /**
     * @brief Initializes an address from bytes.
     *
     * @param bytes Exactly 16 bytes in network order.
     */
    explicit Ipv6(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The source value.
     */
    Ipv6(const Ipv6& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    Ipv6(Ipv6&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~Ipv6();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; empty input and trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static Ipv6 fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static Ipv6 fromCbor(std::string_view cborHex);

    /**
     * @brief Parses an address.
     *
     * @param text Eight groups of four hexadecimal digits separated by colons; compressed forms are not accepted.
     *
     * @return The parsed address.
     */
    static Ipv6 fromString(std::string_view text);

  public:

    /**
     * @brief Gets the address bytes.
     *
     * @return The copied bytes in network order.
     */
    std::vector<std::uint8_t> toBytes() const;

    /**
     * @brief Gets the address text.
     *
     * @return The expanded hexadecimal address.
     */
    std::string toString() const;

    /**
     * @brief Serializes this value as CBOR.
     *
     * @return The encoded value.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Exchanges values without allocation.
     *
     * @param other The value to exchange.
     */
    void swap(Ipv6& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This instance; unchanged if copying fails.
     */
    Ipv6& operator=(const Ipv6& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source; self-move assignment is safe.
     *
     * @return This instance.
     */
    Ipv6& operator=(Ipv6&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when equal.
     */
    bool operator==(const Ipv6& other) const;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when different.
     */
    bool operator!=(const Ipv6& other) const;

  private:

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference transferred to this instance.
     */
    explicit Ipv6(cardano_ipv6_t& value) noexcept;

  private:

    /**
     * @brief Checks and gets the stored reference.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_ipv6_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_ipv6_t* m_value;

    /** @brief Allows SingleHostAddrRelay to exchange component values. */
    friend class SingleHostAddrRelay;
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
void swap(Ipv6& lhs, Ipv6& rhs) noexcept;
} // namespace Cometa
