/**
 * @file Relay.h
 *
 * @brief A stake pool relay using an IP address, hostname, or DNS SRV record.
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

#include <Cometa/PoolParams/MultiHostNameRelay.h>
#include <Cometa/PoolParams/RelayType.h>
#include <Cometa/PoolParams/SingleHostAddrRelay.h>
#include <Cometa/PoolParams/SingleHostNameRelay.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_relay_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A stake pool relay using an IP address, hostname, or DNS SRV record.
 *
 * Inspect type() before extracting a particular relay form. PoolParams uses Relays to advertise the
 * pool's relay endpoints.
 */
class Relay
{
  public:

    /**
     * @brief Initializes a copied IP-address variant.
     *
     * @param relay The relay to copy.
     */
    Relay(const SingleHostAddrRelay& relay);

    /**
     * @brief Initializes a value by moving its SingleHostAddrRelay alternative.
     * @param relay The alternative to move.
     */
    Relay(SingleHostAddrRelay&& relay);

    /**
     * @brief Initializes a copied DNS-name variant.
     *
     * @param relay The relay to copy.
     */
    Relay(const SingleHostNameRelay& relay);

    /**
     * @brief Initializes a value by moving its SingleHostNameRelay alternative.
     * @param relay The alternative to move.
     */
    Relay(SingleHostNameRelay&& relay);

    /**
     * @brief Initializes a copied DNS-name variant.
     *
     * @param relay The relay to copy.
     */
    Relay(const MultiHostNameRelay& relay);

    /**
     * @brief Initializes a value by moving its MultiHostNameRelay alternative.
     * @param relay The alternative to move.
     */
    Relay(MultiHostNameRelay&& relay);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The source value.
     */
    Relay(const Relay& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    Relay(Relay&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~Relay();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; empty input and trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static Relay fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static Relay fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the relay discriminator.
     *
     * @return The relay type.
     */
    RelayType type() const;

    /**
     * @brief Gets an independent SingleHostAddr variant.
     *
     * @return The copied relay.
     *
     * @throws Error The relay has a different type.
     */
    SingleHostAddrRelay asSingleHostAddr() const;

    /**
     * @brief Gets an independent SingleHostName variant.
     *
     * @return The copied relay.
     *
     * @throws Error The relay has a different type.
     */
    SingleHostNameRelay asSingleHostName() const;

    /**
     * @brief Gets an independent MultiHostName variant.
     *
     * @return The copied relay.
     *
     * @throws Error The relay has a different type.
     */
    MultiHostNameRelay asMultiHostName() const;

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
    void swap(Relay& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This instance; unchanged if copying fails.
     */
    Relay& operator=(const Relay& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source; self-move assignment is safe.
     *
     * @return This instance.
     */
    Relay& operator=(Relay&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when equal.
     */
    bool operator==(const Relay& other) const;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when different.
     */
    bool operator!=(const Relay& other) const;

  private:

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference transferred to this instance.
     */
    explicit Relay(cardano_relay_t& value) noexcept;

  private:

    /**
     * @brief Checks and gets the stored reference.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_relay_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_relay_t* m_value;

    /** @brief Allows Relays to exchange component values. */
    friend class Relays;
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
void swap(Relay& lhs, Relay& rhs) noexcept;
} // namespace Cometa
