/**
 * @file ProtocolVersion.h
 *
 * @brief Major and minor Cardano protocol version.
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

struct cardano_protocol_version_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Major and minor Cardano protocol version.
 */
class ProtocolVersion
{
  public:

    /**
     * @brief Initializes the value.
     *
     * @param major Major protocol version.
     * @param minor Minor protocol version, at most UINT32_MAX.
     *
     * @throws std::invalid_argument The minor version exceeds UINT32_MAX.
     */
    ProtocolVersion(std::uint64_t major, std::uint64_t minor);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     *
     * @throws std::logic_error The source has been moved from.
     */
    ProtocolVersion(const ProtocolVersion& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    ProtocolVersion(ProtocolVersion&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~ProtocolVersion();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded bytes.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument Input is empty, hex is invalid, or trailing bytes remain.
     * @throws Error The CBOR input is invalid.
     */
    static ProtocolVersion fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cborHex Hexadecimal CBOR without a 0x prefix.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument Input is empty, hex is invalid, or trailing bytes remain.
     * @throws Error The CBOR input is invalid.
     */
    static ProtocolVersion fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the major protocol version.
     *
     * @return Major protocol version.
     */
    std::uint64_t major() const;

    /**
     * @brief Replaces the major protocol version.
     *
     * @param major Major protocol version.
     */
    void setMajor(std::uint64_t major);

    /**
     * @brief Gets the minor protocol version.
     *
     * @return Minor protocol version.
     */
    std::uint64_t minor() const;

    /**
     * @brief Replaces the minor protocol version.
     *
     * @param minor Minor protocol version, at most UINT32_MAX.
     *
     * @throws std::invalid_argument The minor version exceeds UINT32_MAX.
     */
    void setMinor(std::uint64_t minor);

    /**
     * @brief Encodes the value as CBOR.
     *
     * @return The CBOR bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Encodes the value as CIP-116 JSON.
     *
     * @return The compact JSON string.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges two values.
     *
     * @param other The value to exchange with this instance.
     */
    void swap(ProtocolVersion& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The value to copy.
     *
     * @return This instance. If copying fails, its value is unchanged.
     */
    ProtocolVersion& operator=(const ProtocolVersion& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source; self-move assignment is safe.
     *
     * @return This instance.
     */
    ProtocolVersion& operator=(ProtocolVersion&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const ProtocolVersion& other) const;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True if the values are different.
     */
    bool operator!=(const ProtocolVersion& other) const;

    /** @brief Allows protocol updates to copy native values. */
    friend class ProtocolParamUpdate;

    /** @brief Allows protocol parameters to copy native values. */
    friend class ProtocolParameters;

    /** @brief Allows hard fork actions to copy protocol versions. */
    friend class HardForkInitiationAction;

  private:

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference transferred to this instance.
     */
    explicit ProtocolVersion(cardano_protocol_version_t* value) noexcept;

  private:

    /**
     * @brief Gets the stored value after checking its lifetime.
     *
     * @return The reference, without transferring ownership.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_protocol_version_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_protocol_version_t* m_value;
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
void swap(ProtocolVersion& lhs, ProtocolVersion& rhs) noexcept;
} // namespace Cometa
