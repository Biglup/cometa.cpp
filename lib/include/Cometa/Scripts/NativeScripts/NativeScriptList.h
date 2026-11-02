/**
 * @file NativeScriptList.h
 *
 * @brief An ordered sequence of native scripts.
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

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_native_script_list_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief An ordered sequence of native scripts. */
class NativeScriptList
{
  public:

    /**
     * @brief Initializes an empty script list.
     */
    NativeScriptList();

    /**
     * @brief Initializes an ordered list of copied scripts.
     *
     * @param scripts The child scripts to copy.
     */
    NativeScriptList(std::initializer_list<NativeScript> scripts);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    NativeScriptList(const NativeScriptList& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    NativeScriptList(NativeScriptList&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~NativeScriptList();

  public:

    /**
     * @brief Decodes exactly one CBOR script list.
     *
     * @param cbor The encoded script; empty input and trailing bytes are rejected.
     *
     * @return The decoded script.
     */
    static NativeScriptList fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded script.
     */
    static NativeScriptList fromCbor(std::string_view cborHex);

    /**
     * @brief Decodes cardano-cli native-script JSON.
     *
     * @param json An object containing a scripts array.
     *
     * @return The decoded value.
     *
     * @remark The input schema differs from CIP-116 inspection output.
     */
    static NativeScriptList fromJson(std::string_view json);

  public:

    /**
     * @brief Gets the number of child scripts.
     *
     * @return The number of children.
     */
    std::size_t size() const;

    /**
     * @brief Tests whether the list is empty.
     *
     * @return True when there are no scripts.
     */
    bool empty() const;

    /**
     * @brief Gets an independent child script.
     *
     * @param index The zero-based child index.
     *
     * @return The copied script.
     *
     * @throws std::out_of_range The index is not less than size().
     */
    NativeScript at(std::size_t index) const;

    /**
     * @brief Appends a copied child script.
     *
     * @param script The child to append.
     */
    void pushBack(const NativeScript& script);

    /**
     * @brief Gets independent children in order.
     *
     * @return The copied child scripts.
     */
    std::vector<NativeScript> values() const;

    /**
     * @brief Serializes the script list as CBOR.
     *
     * @return The encoded bytes; child encodings and array length form are retained; nonminimal array headers are normalized.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the script list as CIP-116 inspection JSON.
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
    void swap(NativeScriptList& other) noexcept;

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
    NativeScriptList& operator=(const NativeScriptList& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    NativeScriptList& operator=(NativeScriptList&& other) noexcept;

    /**
     * @brief Compares script requirements.
     *
     * @param other The script to compare.
     *
     * @return True when the requirements are equal.
     */
    bool operator==(const NativeScriptList& other) const;

    /**
     * @brief Compares script requirements for inequality.
     *
     * @param other The script to compare.
     *
     * @return True when the requirements differ.
     */
    bool operator!=(const NativeScriptList& other) const;

  private:

    /** @brief Allows NativeScript to copy this script variant. */
    friend class NativeScript;

    /** @brief Allows ScriptAll to copy and validate child scripts. */
    friend class ScriptAll;

    /** @brief Allows ScriptAny to copy and validate child scripts. */
    friend class ScriptAny;

    /** @brief Allows ScriptNOfK to copy and validate child scripts. */
    friend class ScriptNOfK;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit NativeScriptList(cardano_native_script_list_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_native_script_list_t* checked() const;

  private:

    /**
     * @brief Checks nested requirements before exposing decoded values.
     *
     * @throws std::invalid_argument A key hash is not 28 bytes.
     */
    void validate() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_native_script_list_t* m_value;
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
void swap(NativeScriptList& lhs, NativeScriptList& rhs) noexcept;
} // namespace Cometa
