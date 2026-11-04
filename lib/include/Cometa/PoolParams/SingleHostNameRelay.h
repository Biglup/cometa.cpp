/**
 * @file SingleHostNameRelay.h
 *
 * @brief A stake pool relay identified by a DNS hostname and optional port.
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

struct cardano_single_host_name_relay_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A stake pool relay identified by a DNS hostname and optional port.
 *
 * Use a hostname rather than a URL. DNS resolution and connectivity are outside this value's
 * responsibilities.
 */
class SingleHostNameRelay
{
  public:

    /**
     * @brief Initializes a DNS-name relay.
     *
     * @param dns Between 1 and 64 bytes without embedded NULs.
     * @param port The port number, or nullopt when absent.
     */
    explicit SingleHostNameRelay(std::string_view dns, std::optional<std::uint16_t> port = std::nullopt);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The source value.
     */
    SingleHostNameRelay(const SingleHostNameRelay& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    SingleHostNameRelay(SingleHostNameRelay&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~SingleHostNameRelay();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; empty input and trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static SingleHostNameRelay fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static SingleHostNameRelay fromCbor(std::string_view cborHex);

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
    void swap(SingleHostNameRelay& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This instance; unchanged if copying fails.
     */
    SingleHostNameRelay& operator=(const SingleHostNameRelay& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source; self-move assignment is safe.
     *
     * @return This instance.
     */
    SingleHostNameRelay& operator=(SingleHostNameRelay&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when equal.
     */
    bool operator==(const SingleHostNameRelay& other) const;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when different.
     */
    bool operator!=(const SingleHostNameRelay& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static SingleHostNameRelay adopt(cardano_single_host_name_relay_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_single_host_name_relay_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_single_host_name_relay_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_single_host_name_relay_t* release() noexcept;

  private:

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference transferred to this instance.
     */
    explicit SingleHostNameRelay(cardano_single_host_name_relay_t& value) noexcept;

  private:

    /**
     * @brief Checks and gets the stored reference.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_single_host_name_relay_t* checked() const;

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
    cardano_single_host_name_relay_t* m_value;
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
void swap(SingleHostNameRelay& lhs, SingleHostNameRelay& rhs) noexcept;
} // namespace Cometa
