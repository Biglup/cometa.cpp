/**
 * @file NativeScript.h
 *
 * @brief A signature, validity-interval, guard, or composite authorization rule.
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

/**
 * @brief A signature, validity-interval, guard, or composite authorization rule.
 *
 * Native scripts express ledger-checked conditions without Plutus redeemers or execution budgets. The
 * script hash can identify a spending credential or minting policy.
 */
class NativeScript
{
  public:

    /**
     * @brief Initializes a copied script requirement.
     *
     * @param script The requirement to copy.
     */
    NativeScript(const ScriptPubkey& script);

    /**
     * @brief Initializes a value by moving its ScriptPubkey alternative.
     * @param script The alternative to move.
     */
    NativeScript(ScriptPubkey&& script);

    /**
     * @brief Initializes a copied script requirement.
     *
     * @param script The requirement to copy.
     */
    NativeScript(const ScriptAll& script);

    /**
     * @brief Initializes a value by moving its ScriptAll alternative.
     * @param script The alternative to move.
     */
    NativeScript(ScriptAll&& script);

    /**
     * @brief Initializes a copied script requirement.
     *
     * @param script The requirement to copy.
     */
    NativeScript(const ScriptAny& script);

    /**
     * @brief Initializes a value by moving its ScriptAny alternative.
     * @param script The alternative to move.
     */
    NativeScript(ScriptAny&& script);

    /**
     * @brief Initializes a copied script requirement.
     *
     * @param script The requirement to copy.
     */
    NativeScript(const ScriptNOfK& script);

    /**
     * @brief Initializes a value by moving its ScriptNOfK alternative.
     * @param script The alternative to move.
     */
    NativeScript(ScriptNOfK&& script);

    /**
     * @brief Initializes a copied script requirement.
     *
     * @param script The requirement to copy.
     */
    NativeScript(const ScriptInvalidBefore& script);

    /**
     * @brief Initializes a value by moving its ScriptInvalidBefore alternative.
     * @param script The alternative to move.
     */
    NativeScript(ScriptInvalidBefore&& script);

    /**
     * @brief Initializes a copied script requirement.
     *
     * @param script The requirement to copy.
     */
    NativeScript(const ScriptInvalidAfter& script);

    /**
     * @brief Initializes a value by moving its ScriptInvalidAfter alternative.
     * @param script The alternative to move.
     */
    NativeScript(ScriptInvalidAfter&& script);

    /**
     * @brief Initializes a copied script requirement.
     *
     * @param script The requirement to copy.
     */
    NativeScript(const ScriptRequireGuard& script);

    /**
     * @brief Initializes a value by moving its ScriptRequireGuard alternative.
     * @param script The alternative to move.
     */
    NativeScript(ScriptRequireGuard&& script);

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

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static NativeScript adopt(cardano_native_script_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_native_script_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_native_script_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_native_script_t* release() noexcept;

  private:

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
