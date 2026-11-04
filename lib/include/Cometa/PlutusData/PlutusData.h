/**
 * @file PlutusData.h
 *
 * @brief A Plutus integer, byte string, list, map, or constructor value.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
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

#include <Cometa/Common/BigInt.h>
#include <Cometa/PlutusData/PlutusDataKind.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_plutus_data_t;

namespace Cometa
{
class PlutusList;
class PlutusMap;
class ConstrPlutusData;

/* CLASS DECLARATION *********************************************************/

/**
 * @brief A Plutus integer, byte string, list, map, or constructor value.
 *
 * Use it for validator datums, redeemers, and script parameters. The validator defines the expected
 * structure; a valid PlutusData value does not by itself satisfy that schema.
 */
class PlutusData
{
  public:

    /**
     * @brief Initializes an integer.
     *
     * @param integer The signed value.
     */
    explicit PlutusData(std::int64_t integer);

    /**
     * @brief Initializes an arbitrary-precision integer.
     *
     * @param integer The integer to copy.
     */
    explicit PlutusData(const BigInt& integer);

    /**
     * @brief Initializes a byte string.
     *
     * @param bytes The bytes to copy; may be empty.
     */
    explicit PlutusData(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Initializes a list value.
     *
     * @param data The value to copy.
     */
    PlutusData(const PlutusList& data);

    /**
     * @brief Initializes a value by moving its PlutusList alternative.
     * @param data The alternative to move.
     */
    PlutusData(PlutusList&& data);

    /**
     * @brief Initializes a map value.
     *
     * @param data The value to copy.
     */
    PlutusData(const PlutusMap& data);

    /**
     * @brief Initializes a value by moving its PlutusMap alternative.
     * @param data The alternative to move.
     */
    PlutusData(PlutusMap&& data);

    /**
     * @brief Initializes a constructor value.
     *
     * @param data The value to copy.
     */
    PlutusData(const ConstrPlutusData& data);

    /**
     * @brief Initializes a value by moving its ConstrPlutusData alternative.
     * @param data The alternative to move.
     */
    PlutusData(ConstrPlutusData&& data);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    PlutusData(const PlutusData& other);

    /**
     * @brief Moves a value into this instance.
     *
     * @param other The value to move.
     */
    PlutusData(PlutusData&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~PlutusData();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static PlutusData fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static PlutusData fromCbor(std::string_view cborHex);

    /**
     * @brief Creates an unsigned integer.
     *
     * @param integer The unsigned value.
     *
     * @return The integer data.
     */
    static PlutusData fromUnsigned(std::uint64_t integer);

    /**
     * @brief Creates a byte string from hexadecimal.
     *
     * @param hex Pairs of hexadecimal digits without a prefix; may be empty.
     *
     * @return The byte-string data.
     */
    static PlutusData fromHex(std::string_view hex);

  public:

    /**
     * @brief Gets the data discriminator.
     *
     * @return The data kind.
     */
    PlutusDataKind kind() const;

    /**
     * @brief Gets an independent list value.
     *
     * @return The requested value.
     *
     * @throws Error The data has a different kind.
     */
    PlutusList asList() const;

    /**
     * @brief Gets an independent map value.
     *
     * @return The requested value.
     *
     * @throws Error The data has a different kind.
     */
    PlutusMap asMap() const;

    /**
     * @brief Gets an independent constructor value.
     *
     * @return The requested value.
     *
     * @throws Error The data has a different kind.
     */
    ConstrPlutusData asConstructor() const;

    /**
     * @brief Gets an independent integer value.
     *
     * @return The requested value.
     *
     * @throws Error The data has a different kind.
     */
    BigInt asInteger() const;

    /**
     * @brief Gets a byte-string value.
     *
     * @return The copied bytes.
     *
     * @throws Error The data is not a byte string.
     */
    std::vector<std::uint8_t> asBytes() const;

    /**
     * @brief Serializes the value as CBOR.
     *
     * @return The encoded bytes; decoded Plutus data retains its original encoding until changed.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the value as CIP-116 JSON.
     *
     * @return The compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Rebuilds subsequent CBOR encodings from the current fields.
     *
     * @remark Recursively discards original encodings; datum hashes may change.
     */
    void normalizeEncoding();

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(PlutusData& other) noexcept;

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
    PlutusData& operator=(const PlutusData& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    PlutusData& operator=(PlutusData&& other) noexcept;

    /**
     * @brief Compares data values for equality.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const PlutusData& other) const;

    /**
     * @brief Compares data values for inequality.
     *
     * @param other The value to compare.
     *
     * @return True if the values differ.
     */
    bool operator!=(const PlutusData& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static PlutusData adopt(cardano_plutus_data_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_plutus_data_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_plutus_data_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_plutus_data_t* release() noexcept;

  private:

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The non-null owned reference.
     */
    explicit PlutusData(cardano_plutus_data_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_plutus_data_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_plutus_data_t* m_value;
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
void swap(PlutusData& lhs, PlutusData& rhs) noexcept;
} // namespace Cometa
