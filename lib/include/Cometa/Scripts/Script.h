/**
 * @file Script.h
 *
 * @brief A native or Plutus script with its ledger language.
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

#include <Cometa/Scripts/NativeScripts/NativeScript.h>
#include <Cometa/Scripts/PlutusScripts/PlutusV1Script.h>
#include <Cometa/Scripts/PlutusScripts/PlutusV2Script.h>
#include <Cometa/Scripts/PlutusScripts/PlutusV3Script.h>
#include <Cometa/Scripts/PlutusScripts/PlutusV4Script.h>
#include <Cometa/Scripts/ScriptLanguage.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_script_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief A native or Plutus script with its ledger language. */
class Script
{
  public:

    /**
     * @brief Initializes a copied native script.
     *
     * @param script The script to copy.
     */
    explicit Script(const NativeScript& script);

    /**
     * @brief Initializes a copied Plutus V1 script.
     *
     * @param script The script to copy.
     */
    explicit Script(const PlutusV1Script& script);

    /**
     * @brief Initializes a copied Plutus V2 script.
     *
     * @param script The script to copy.
     */
    explicit Script(const PlutusV2Script& script);

    /**
     * @brief Initializes a copied Plutus V3 script.
     *
     * @param script The script to copy.
     */
    explicit Script(const PlutusV3Script& script);

    /**
     * @brief Initializes a copied Plutus V4 script.
     *
     * @param script The script to copy.
     */
    explicit Script(const PlutusV4Script& script);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    Script(const Script& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    Script(Script&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~Script();

  public:

    /**
     * @brief Decodes exactly one CBOR script.
     *
     * @param cbor The encoded script; empty input and trailing bytes are rejected.
     *
     * @return The decoded script.
     */
    static Script fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded script.
     */
    static Script fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the ledger script language.
     *
     * @return The native or Plutus language discriminator.
     */
    ScriptLanguage language() const;

    /**
     * @brief Gets an independent native script.
     *
     * @return The requested script.
     *
     * @throws Error This value has a different language.
     */
    NativeScript asNative() const;

    /**
     * @brief Gets an independent Plutus V1 script.
     *
     * @return The requested script.
     *
     * @throws Error This value has a different language.
     */
    PlutusV1Script asPlutusV1() const;

    /**
     * @brief Gets an independent Plutus V2 script.
     *
     * @return The requested script.
     *
     * @throws Error This value has a different language.
     */
    PlutusV2Script asPlutusV2() const;

    /**
     * @brief Gets an independent Plutus V3 script.
     *
     * @return The requested script.
     *
     * @throws Error This value has a different language.
     */
    PlutusV3Script asPlutusV3() const;

    /**
     * @brief Gets an independent Plutus V4 script.
     *
     * @return The requested script.
     *
     * @throws Error This value has a different language.
     */
    PlutusV4Script asPlutusV4() const;

    /**
     * @brief Computes the script hash.
     *
     * @return The 28-byte hash computed for this language.
     */
    Blake2bHash hash() const;

    /**
     * @brief Serializes the script as CBOR.
     *
     * @return The language tag and script payload encoded as a two-element array.
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
    void swap(Script& other) noexcept;

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
    Script& operator=(const Script& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    Script& operator=(Script&& other) noexcept;

    /**
     * @brief Compares script requirements.
     *
     * @param other The script to compare.
     *
     * @return True when the requirements are equal.
     */
    bool operator==(const Script& other) const;

    /**
     * @brief Compares script requirements for inequality.
     *
     * @param other The script to compare.
     *
     * @return True when the requirements differ.
     */
    bool operator!=(const Script& other) const;

  private:

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit Script(cardano_script_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_script_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_script_t* m_value;
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
void swap(Script& lhs, Script& rhs) noexcept;
} // namespace Cometa
