/**
 * @file PlutusV4ScriptList.h
 *
 * @brief An ordered sequence of auxiliary Plutus scripts.
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

#include <Cometa/Scripts/PlutusScripts/PlutusV4Script.h>

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_plutus_v4_script_list_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief An ordered sequence of auxiliary Plutus scripts. */
class PlutusV4ScriptList
{
  public:

    /**
     * @brief Initializes an empty script list.
     */
    PlutusV4ScriptList();

    /**
     * @brief Initializes an ordered list of copied scripts.
     *
     * @param scripts The child scripts to copy.
     */
    PlutusV4ScriptList(std::initializer_list<PlutusV4Script> scripts);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    PlutusV4ScriptList(const PlutusV4ScriptList& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    PlutusV4ScriptList(PlutusV4ScriptList&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~PlutusV4ScriptList();

  public:

    /**
     * @brief Decodes exactly one CBOR script list.
     *
     * @param cbor The encoded script; empty input and trailing bytes are rejected.
     *
     * @return The decoded script.
     */
    static PlutusV4ScriptList fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded script.
     */
    static PlutusV4ScriptList fromCbor(std::string_view cborHex);

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
    PlutusV4Script at(std::size_t index) const;

    /**
     * @brief Appends a copied child script.
     *
     * @param script The child to append.
     */
    void pushBack(const PlutusV4Script& script);

    /**
     * @brief Gets independent children in order.
     *
     * @return The copied child scripts.
     */
    std::vector<PlutusV4Script> values() const;

    /**
     * @brief Serializes the script list as CBOR.
     *
     * @return A definite-length array of CBOR byte strings.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the script list as CIP-116 inspection JSON.
     *
     * @return The compact inspection object.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(PlutusV4ScriptList& other) noexcept;

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
    PlutusV4ScriptList& operator=(const PlutusV4ScriptList& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    PlutusV4ScriptList& operator=(PlutusV4ScriptList&& other) noexcept;

    /**
     * @brief Compares script values.
     *
     * @param other The script list to compare.
     *
     * @return True when the values are equal.
     */
    bool operator==(const PlutusV4ScriptList& other) const;

    /**
     * @brief Compares script values for inequality.
     *
     * @param other The script list to compare.
     *
     * @return True when the values differ.
     */
    bool operator!=(const PlutusV4ScriptList& other) const;

  private:

    /** @brief Allows AuxiliaryData to copy attached values. */
    friend class AuxiliaryData;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit PlutusV4ScriptList(cardano_plutus_v4_script_list_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_plutus_v4_script_list_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_plutus_v4_script_list_t* m_value;
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
void swap(PlutusV4ScriptList& lhs, PlutusV4ScriptList& rhs) noexcept;
} // namespace Cometa
