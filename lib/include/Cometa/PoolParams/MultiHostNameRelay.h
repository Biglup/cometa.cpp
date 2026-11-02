/**
 * @file MultiHostNameRelay.h
 *
 * @brief A pool relay resolved through a DNS SRV name.
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
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_multi_host_name_relay_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief A pool relay resolved through a DNS SRV name. */
class MultiHostNameRelay
{
  public:

    /**
     * @brief Initializes a DNS-name relay.
     *
     * @param dns Between 1 and 64 bytes without embedded NULs.
     */
    explicit MultiHostNameRelay(std::string_view dns);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The source value.
     */
    MultiHostNameRelay(const MultiHostNameRelay& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    MultiHostNameRelay(MultiHostNameRelay&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~MultiHostNameRelay();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; empty input and trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static MultiHostNameRelay fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static MultiHostNameRelay fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the DNS name.
     *
     * @return The DNS name.
     */
    std::string dns() const;

    /**
     * @brief Replaces the DNS name.
     *
     * @param dns Between 1 and 64 bytes without embedded NULs.
     */
    void setDns(std::string_view dns);

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
    void swap(MultiHostNameRelay& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This instance; unchanged if copying fails.
     */
    MultiHostNameRelay& operator=(const MultiHostNameRelay& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source; self-move assignment is safe.
     *
     * @return This instance.
     */
    MultiHostNameRelay& operator=(MultiHostNameRelay&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when equal.
     */
    bool operator==(const MultiHostNameRelay& other) const;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when different.
     */
    bool operator!=(const MultiHostNameRelay& other) const;

  private:

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference transferred to this instance.
     */
    explicit MultiHostNameRelay(cardano_multi_host_name_relay_t& value) noexcept;

  private:

    /**
     * @brief Checks and gets the stored reference.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_multi_host_name_relay_t* checked() const;

    /**
     * @brief Checks the DNS text representation.
     *
     * @param dns The proposed DNS name.
     */
    static void validateDns(std::string_view dns);

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_multi_host_name_relay_t* m_value;

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
void swap(MultiHostNameRelay& lhs, MultiHostNameRelay& rhs) noexcept;
} // namespace Cometa
