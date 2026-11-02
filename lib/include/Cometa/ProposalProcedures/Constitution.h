/**
 * @file Constitution.h
 *
 * @brief Constitution content.
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

#include <Cometa/Common/Anchor.h>
#include <Cometa/Crypto/Blake2bHash.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_constitution_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Constitution anchor and optional script hash. */
class Constitution
{
  public:

    /**
     * @brief Creates a constitution.
     * @param anchor The constitution anchor.
     * @param scriptHash The optional constitution script hash.
     */
    explicit Constitution(const Anchor& anchor, const std::optional<Blake2bHash>& scriptHash = std::nullopt);

    /**
     * @brief Copies a constitution.
     * @param other The constitution to copy.
     */
    Constitution(const Constitution& other);

    /**
     * @brief Moves a constitution.
     * @param other The constitution to move.
     */
    Constitution(Constitution&& other) noexcept;

    /** @brief Destroys the constitution. */
    ~Constitution();

  public:

    /**
     * @brief Gets the constitution anchor.
     * @return An independent anchor.
     */
    Anchor anchor() const;

    /**
     * @brief Sets the constitution anchor.
     * @param value The anchor to set.
     */
    void setAnchor(const Anchor& value);

    /**
     * @brief Decodes CBOR.
     * @param cbor Encoded bytes.
     * @return The decoded constitution.
     */
    static Constitution fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex Hexadecimal bytes.
     * @return The decoded constitution.
     */
    static Constitution fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the constitution script hash.
     * @return An independent hash, or empty when absent.
     */
    std::optional<Blake2bHash> scriptHash() const;

    /**
     * @brief Sets the constitution script hash.
     * @param value The hash, or empty to remove it.
     */
    void setScriptHash(const std::optional<Blake2bHash>& value);

    /**
     * @brief Encodes CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the constitution as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges constitutions.
     * @param other The constitution to exchange.
     */
    void swap(Constitution& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The constitution to copy.
     * @return This constitution.
     */
    Constitution& operator=(const Constitution& other);

    /**
     * @brief Assigns a moved constitution.
     * @param other The constitution to move.
     * @return This constitution.
     */
    Constitution& operator=(Constitution&& other) noexcept;

    /**
     * @brief Compares constitutions.
     * @param other The constitution to compare.
     * @return True when equal.
     */
    bool operator==(const Constitution& other) const;

    /**
     * @brief Compares constitutions.
     * @param other The constitution to compare.
     * @return True when different.
     */
    bool operator!=(const Constitution& other) const;

  private:

    /**
     * @brief Adopts an owned constitution.
     * @param value The owned constitution.
     */
    explicit Constitution(cardano_constitution_t* value) noexcept;

    /**
     * @brief Gets the stored constitution.
     * @return The borrowed constitution.
     */
    cardano_constitution_t* checked() const;

  private:

    /** @brief The owned constitution. */
    cardano_constitution_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges constitutions.
 * @param lhs The first constitution.
 * @param rhs The second constitution.
 */
void swap(Constitution& lhs, Constitution& rhs) noexcept;
} // namespace Cometa
