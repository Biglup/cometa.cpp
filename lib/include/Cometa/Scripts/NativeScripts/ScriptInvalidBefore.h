/**
 * @file ScriptInvalidBefore.h
 *
 * @brief A native script requiring the transaction's validity interval to start at or after a slot.
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

struct cardano_script_invalid_before_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A native script requiring the transaction's validity interval to start at or after a slot.
 *
 * The slot is an inclusive lower bound. This rule constrains the interval declared by the transaction;
 * it does not read a wall-clock timestamp.
 */
class ScriptInvalidBefore
{
  public:

    /**
     * @brief Initializes the script requirement.
     *
     * @param slot The absolute slot number.
     */
    explicit ScriptInvalidBefore(std::uint64_t slot);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    ScriptInvalidBefore(const ScriptInvalidBefore& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    ScriptInvalidBefore(ScriptInvalidBefore&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~ScriptInvalidBefore();

  public:

    /**
     * @brief Decodes exactly one CBOR script.
     *
     * @param cbor The encoded script; empty input and trailing bytes are rejected.
     *
     * @return The decoded script.
     */
    static ScriptInvalidBefore fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded script.
     */
    static ScriptInvalidBefore fromCbor(std::string_view cborHex);

    /**
     * @brief Decodes cardano-cli native-script JSON.
     *
     * @param json The object with type "after" and its slot.
     *
     * @return The decoded script.
     *
     * @remark This input schema differs from the CIP-116 output of toJson().
     */
    static ScriptInvalidBefore fromJson(std::string_view json);

  public:

    /**
     * @brief Gets the slot bound.
     *
     * @return The slot value.
     */
    std::uint64_t slot() const;

    /**
     * @brief Replaces the slot bound.
     *
     * @param slot The replacement slot.
     */
    void setSlot(std::uint64_t slot);

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
    void swap(ScriptInvalidBefore& other) noexcept;

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
    ScriptInvalidBefore& operator=(const ScriptInvalidBefore& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    ScriptInvalidBefore& operator=(ScriptInvalidBefore&& other) noexcept;

    /**
     * @brief Compares script requirements.
     *
     * @param other The script to compare.
     *
     * @return True when the requirements are equal.
     */
    bool operator==(const ScriptInvalidBefore& other) const;

    /**
     * @brief Compares script requirements for inequality.
     *
     * @param other The script to compare.
     *
     * @return True when the requirements differ.
     */
    bool operator!=(const ScriptInvalidBefore& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static ScriptInvalidBefore adopt(cardano_script_invalid_before_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_script_invalid_before_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_script_invalid_before_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_script_invalid_before_t* release() noexcept;

  private:

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit ScriptInvalidBefore(cardano_script_invalid_before_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_script_invalid_before_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_script_invalid_before_t* m_value;
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
void swap(ScriptInvalidBefore& lhs, ScriptInvalidBefore& rhs) noexcept;
} // namespace Cometa
