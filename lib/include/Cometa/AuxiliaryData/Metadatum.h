/**
 * @file Metadatum.h
 *
 * @brief Declaration of the Metadatum value type.
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

#include <Cometa/AuxiliaryData/MetadatumKind.h>
#include <Cometa/Common/BigInt.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_metadatum_t;

namespace Cometa
{
class MetadatumList;
class MetadatumMap;
class JsonValue;
class JsonWriter;

/* CLASS DECLARATION *********************************************************/

/** @brief A metadata map, list, integer, byte string, or text string. */
class Metadatum
{
  public:

    /**
     * @brief Initializes an integer.
     *
     * @param integer The signed value.
     */
    explicit Metadatum(std::int64_t integer);

    /**
     * @brief Initializes an arbitrary-precision integer.
     *
     * @param integer The integer to copy.
     */
    explicit Metadatum(const BigInt& integer);

    /**
     * @brief Initializes a byte string.
     *
     * @param bytes The bytes to copy; may be empty. CBOR encoding permits at most 64 bytes.
     */
    explicit Metadatum(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Initializes a list value.
     *
     * @param data The value to copy.
     */
    // cppcheck-suppress noExplicitConstructor
    Metadatum(const MetadatumList& data);

    /**
     * @brief Initializes a value by moving its MetadatumList alternative.
     * @param data The alternative to move.
     */
    // cppcheck-suppress noExplicitConstructor
    Metadatum(MetadatumList&& data);

    /**
     * @brief Initializes a map value.
     *
     * @param data The value to copy.
     */
    // cppcheck-suppress noExplicitConstructor
    Metadatum(const MetadatumMap& data);

    /**
     * @brief Initializes a value by moving its MetadatumMap alternative.
     * @param data The alternative to move.
     */
    // cppcheck-suppress noExplicitConstructor
    Metadatum(MetadatumMap&& data);

    /**
     * @brief Initializes a text string.
     *
     * @param text The UTF-8 text to copy; embedded nulls are preserved. CBOR encoding permits at most 64 bytes.
     */
    explicit Metadatum(std::string_view text);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    Metadatum(const Metadatum& other);

    /**
     * @brief Moves a value into this instance.
     *
     * @param other The value to move.
     */
    Metadatum(Metadatum&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~Metadatum();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static Metadatum fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static Metadatum fromCbor(std::string_view cborHex);

    /**
     * @brief Creates an unsigned integer.
     *
     * @param integer The unsigned value.
     *
     * @return The integer data.
     */
    static Metadatum fromUnsigned(std::uint64_t integer);

    /**
     * @brief Creates a byte string from hexadecimal.
     *
     * @param hex Pairs of hexadecimal digits without a prefix; may be empty.
     *
     * @return The byte-string data.
     */
    static Metadatum fromHex(std::string_view hex);

    /**
     * @brief Converts plain JSON metadata without changing integer precision.
     *
     * @param json A JSON integer, string, array, or object.
     *
     * @return The decoded metadata.
     *
     * @throws std::invalid_argument The JSON contains null, booleans, fractions, or exponents.
     */
    static Metadatum fromPlainJson(std::string_view json);

  public:

    /**
     * @brief Gets the data discriminator.
     *
     * @return The data kind.
     */
    MetadatumKind kind() const;

    /**
     * @brief Gets an independent list value.
     *
     * @return The requested value.
     *
     * @throws Error The data has a different kind.
     */
    MetadatumList asList() const;

    /**
     * @brief Gets an independent map value.
     *
     * @return The requested value.
     *
     * @throws Error The data has a different kind.
     */
    MetadatumMap asMap() const;

    /**
     * @brief Gets the text string.
     *
     * @return The copied UTF-8 text, including embedded nulls.
     *
     * @throws Error The data is not text.
     */
    std::string asText() const;

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
     * @return The encoded bytes; collection length forms are retained and scalar encodings are normalized.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the value as CIP-116 JSON.
     *
     * @return The compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Serializes metadata as plain JSON.
     *
     * @return Compact JSON with exact integer values.
     *
     * @throws std::invalid_argument A value is bytes or a map key is not text.
     */
    std::string toPlainJson() const;

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(Metadatum& other) noexcept;

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
    Metadatum& operator=(const Metadatum& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    Metadatum& operator=(Metadatum&& other) noexcept;

    /**
     * @brief Compares data values for equality.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const Metadatum& other) const;

    /**
     * @brief Compares data values for inequality.
     *
     * @param other The value to compare.
     *
     * @return True if the values differ.
     */
    bool operator!=(const Metadatum& other) const;

  private:

    /** @brief Allows MetadatumList to copy components. */
    friend class MetadatumList;

    /** @brief Allows MetadatumMap to copy components. */
    friend class MetadatumMap;

    /** @brief Allows TransactionMetadata to copy components. */
    friend class TransactionMetadata;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The non-null owned reference.
     */
    explicit Metadatum(cardano_metadatum_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_metadatum_t* checked() const;

    /**
     * @brief Converts a parsed plain JSON subtree.
     *
     * @param value The parsed subtree.
     *
     * @return The converted metadata.
     */
    static Metadatum fromJsonValue(const JsonValue& value);

    /**
     * @brief Writes a plain JSON subtree.
     *
     * @param writer The writer receiving the subtree.
     */
    void writePlainJson(JsonWriter& writer) const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_metadatum_t* m_value;
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
void swap(Metadatum& lhs, Metadatum& rhs) noexcept;
} // namespace Cometa
