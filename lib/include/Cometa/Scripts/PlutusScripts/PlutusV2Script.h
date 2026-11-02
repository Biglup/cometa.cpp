/**
 * @file PlutusV2Script.h
 *
 * @brief A compiled Plutus V2 script.
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

#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/PlutusData/PlutusList.h>
#include <Cometa/Scripts/PlutusScripts/PlutusLanguageVersion.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_plutus_v2_script_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief A compiled Plutus V2 script. */
class PlutusV2Script
{
  public:

    /**
     * @brief Initializes a compiled script.
     *
     * @param bytes The nonempty compiled script bytes to copy.
     *
     * @throws std::invalid_argument The input is empty.
     */
    explicit PlutusV2Script(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    PlutusV2Script(const PlutusV2Script& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    PlutusV2Script(PlutusV2Script&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~PlutusV2Script();

  public:

    /**
     * @brief Decodes exactly one CBOR script.
     *
     * @param cbor The encoded script; empty input and trailing bytes are rejected.
     *
     * @return The decoded script.
     */
    static PlutusV2Script fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded script.
     */
    static PlutusV2Script fromCbor(std::string_view cborHex);

    /**
     * @brief Decodes hexadecimal compiled script bytes.
     *
     * @param hex Nonempty pairs of hexadecimal digits without a prefix.
     *
     * @return The script.
     */
    static PlutusV2Script fromHex(std::string_view hex);

  public:

    /**
     * @brief Gets the compiled script bytes.
     *
     * @return A copy of the bytes, without the outer serialization bytestring.
     */
    std::vector<std::uint8_t> toBytes() const;

    /**
     * @brief Encodes the compiled script bytes as hexadecimal.
     *
     * @return Lowercase hexadecimal without a prefix.
     */
    std::string toHex() const;

    /**
     * @brief Gets the ledger language version.
     *
     * @return The Plutus language version.
     */
    PlutusLanguageVersion languageVersion() const;

    /**
     * @brief Computes the versioned script hash.
     *
     * @return The 28-byte script hash.
     */
    Blake2bHash hash() const;

    /**
     * @brief Applies parameters to the compiled program in list order.
     *
     * @param parameters The data arguments; an empty list canonicalizes the program.
     *
     * @return A new script with the parameters applied.
     *
     * @throws Error The bytes are not a valid CBOR-wrapped Flat program.
     */
    PlutusV2Script applyParameters(const PlutusList& parameters) const;

    /**
     * @brief Serializes the script as CBOR.
     *
     * @return The compiled bytes wrapped in a canonical CBOR bytestring.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the script as CIP-116 inspection JSON.
     *
     * @return The compact inspection object.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(PlutusV2Script& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This instance.
     *
     * @remark If copying fails, this instance is unchanged.
     */
    PlutusV2Script& operator=(const PlutusV2Script& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    PlutusV2Script& operator=(PlutusV2Script&& other) noexcept;

    /**
     * @brief Compares script requirements.
     *
     * @param other The script to compare.
     *
     * @return True when the requirements are equal.
     */
    bool operator==(const PlutusV2Script& other) const;

    /**
     * @brief Compares script requirements for inequality.
     *
     * @param other The script to compare.
     *
     * @return True when the requirements differ.
     */
    bool operator!=(const PlutusV2Script& other) const;

  private:

    /** @brief Allows Script to copy this script variant. */
    friend class Script;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit PlutusV2Script(cardano_plutus_v2_script_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_plutus_v2_script_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_plutus_v2_script_t* m_value;
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
void swap(PlutusV2Script& lhs, PlutusV2Script& rhs) noexcept;
} // namespace Cometa
