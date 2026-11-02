/**
 * @file NativeScript.h
 *
 * @brief A signature, timelock, guard, or composite native script.
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

#include <Cometa/Scripts/NativeScripts/NativeScriptType.h>
#include <Cometa/Scripts/NativeScripts/ScriptInvalidAfter.h>
#include <Cometa/Scripts/NativeScripts/ScriptInvalidBefore.h>
#include <Cometa/Scripts/NativeScripts/ScriptPubkey.h>
#include <Cometa/Scripts/NativeScripts/ScriptRequireGuard.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_native_script_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
class ScriptAll;
class ScriptAny;
class ScriptNOfK;

/** @brief A signature, timelock, guard, or composite native script. */
class NativeScript
{
  public:

    /**
     * @brief Initializes a copied script requirement.
     *
     * @param script The requirement to copy.
     */
    explicit NativeScript(const ScriptPubkey& script);

    /**
     * @brief Initializes a copied script requirement.
     *
     * @param script The requirement to copy.
     */
    explicit NativeScript(const ScriptAll& script);

    /**
     * @brief Initializes a copied script requirement.
     *
     * @param script The requirement to copy.
     */
    explicit NativeScript(const ScriptAny& script);

    /**
     * @brief Initializes a copied script requirement.
     *
     * @param script The requirement to copy.
     */
    explicit NativeScript(const ScriptNOfK& script);

    /**
     * @brief Initializes a copied script requirement.
     *
     * @param script The requirement to copy.
     */
    explicit NativeScript(const ScriptInvalidBefore& script);

    /**
     * @brief Initializes a copied script requirement.
     *
     * @param script The requirement to copy.
     */
    explicit NativeScript(const ScriptInvalidAfter& script);

    /**
     * @brief Initializes a copied script requirement.
     *
     * @param script The requirement to copy.
     */
    explicit NativeScript(const ScriptRequireGuard& script);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    NativeScript(const NativeScript& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    NativeScript(NativeScript&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~NativeScript();

  public:

    /**
     * @brief Decodes exactly one CBOR script.
     *
     * @param cbor The encoded script; empty input and trailing bytes are rejected.
     *
     * @return The decoded script.
     */
    static NativeScript fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded script.
     */
    static NativeScript fromCbor(std::string_view cborHex);

    /**
     * @brief Decodes cardano-cli native-script JSON.
     *
     * @param json The script object.
     *
     * @return The decoded value.
     *
     * @remark The input schema differs from CIP-116 inspection output.
     */
    static NativeScript fromJson(std::string_view json);

  public:

    /**
     * @brief Gets the script discriminator.
     *
     * @return The script variant.
     */
    NativeScriptType type() const;

    /**
     * @brief Gets an independent ScriptPubkey requirement.
     *
     * @return The copied requirement.
     *
     * @throws Error The script has a different type.
     */
    ScriptPubkey asPubkey() const;

    /**
     * @brief Gets an independent ScriptAll requirement.
     *
     * @return The copied requirement.
     *
     * @throws Error The script has a different type.
     */
    ScriptAll asAll() const;

    /**
     * @brief Gets an independent ScriptAny requirement.
     *
     * @return The copied requirement.
     *
     * @throws Error The script has a different type.
     */
    ScriptAny asAny() const;

    /**
     * @brief Gets an independent ScriptNOfK requirement.
     *
     * @return The copied requirement.
     *
     * @throws Error The script has a different type.
     */
    ScriptNOfK asNOfK() const;

    /**
     * @brief Gets an independent ScriptInvalidBefore requirement.
     *
     * @return The copied requirement.
     *
     * @throws Error The script has a different type.
     */
    ScriptInvalidBefore asInvalidBefore() const;

    /**
     * @brief Gets an independent ScriptInvalidAfter requirement.
     *
     * @return The copied requirement.
     *
     * @throws Error The script has a different type.
     */
    ScriptInvalidAfter asInvalidAfter() const;

    /**
     * @brief Gets an independent ScriptRequireGuard requirement.
     *
     * @return The copied requirement.
     *
     * @throws Error The script has a different type.
     */
    ScriptRequireGuard asRequireGuard() const;

    /**
     * @brief Computes the native script hash.
     *
     * @return The Blake2b-224 hash of the native prefix and preserved script CBOR.
     */
    Blake2bHash hash() const;

    /**
     * @brief Serializes the script as CBOR.
     *
     * @return The encoded bytes; original encodings are preserved until mutation.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the script as CIP-116 inspection JSON.
     *
     * @return The compact inspection object.
     */
    std::string toJson() const;

    /**
     * @brief Rebuilds subsequent CBOR from the current fields.
     *
     * @remark Normalization may change a script hash without changing the requirement.
     */
    void normalizeEncoding();

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(NativeScript& other) noexcept;

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
    NativeScript& operator=(const NativeScript& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    NativeScript& operator=(NativeScript&& other) noexcept;

    /**
     * @brief Compares script requirements.
     *
     * @param other The script to compare.
     *
     * @return True when the requirements are equal.
     */
    bool operator==(const NativeScript& other) const;

    /**
     * @brief Compares script requirements for inequality.
     *
     * @param other The script to compare.
     *
     * @return True when the requirements differ.
     */
    bool operator!=(const NativeScript& other) const;

  private:

    /** @brief Allows Script to copy and validate native script payloads. */
    friend class Script;

    /** @brief Allows NativeScriptList to copy script elements. */
    friend class NativeScriptList;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit NativeScript(cardano_native_script_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_native_script_t* checked() const;

  private:

    /**
     * @brief Checks nested requirements before exposing decoded values.
     *
     * @throws std::invalid_argument A key hash is not 28 bytes.
     */
    void validate() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_native_script_t* m_value;
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
void swap(NativeScript& lhs, NativeScript& rhs) noexcept;
} // namespace Cometa
