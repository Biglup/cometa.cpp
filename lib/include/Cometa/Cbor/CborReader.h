/**
 * @file CborReader.h
 *
 * @brief Streaming CBOR reader.
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

#include <Cometa/Cbor/CborReaderState.h>
#include <Cometa/Cbor/CborSimpleValue.h>
#include <Cometa/Cbor/CborTag.h>
#include <Cometa/Common/BigInt.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_cbor_reader_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Streaming CBOR reader.
 *
 * Operations report malformed input or encoding failures through Error.
 */
class CborReader
{
  public:

    /**
     * @brief Copies an encoded CBOR stream.
     * @param bytes Nonempty CBOR bytes.
     * @throws std::invalid_argument The input is empty.
     */
    explicit CborReader(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Decodes a hexadecimal CBOR stream.
     * @param hex Nonempty hexadecimal text without a 0x prefix.
     * @throws std::invalid_argument The input is empty or invalid hex.
     */
    explicit CborReader(std::string_view hex);

    /**
     * @brief Copies the current position and nested parsing state.
     * @param other The instance to copy.
     */
    CborReader(const CborReader& other);

    /**
     * @brief Moves another instance.
     * @param other The source instance.
     */
    CborReader(CborReader&& other) noexcept;

    /**
     * @brief Destroys this reader.
     */
    ~CborReader();

  public:

    /**
     * @brief Inspects the next item without consuming it.
     * @return The current state or remaining byte count.
     */
    CborReaderState peekState() const;

    /**
     * @brief Gets the number of unread bytes.
     * @return The current state or remaining byte count.
     */
    std::size_t bytesRemaining() const;

    /**
     * @brief Reads a signed 64-bit integer.
     * @throws std::out_of_range The encoded integer exceeds the signed range; the item is consumed.
     * @return The decoded value.
     */
    std::int64_t readInt();

    /**
     * @brief Reads an unsigned 64-bit integer.
     * @return The decoded value.
     */
    std::uint64_t readUInt();

    /**
     * @brief Reads the argument n of a negative integer with value -1-n.
     * @return The decoded value.
     */
    std::uint64_t readNegativeInt();

    /**
     * @brief Reads a half-, single-, or double-precision floating-point value.
     * @return The decoded value.
     */
    double readDouble();

    /**
     * @brief Reads a boolean.
     * @return The decoded value.
     */
    bool readBool();

    /**
     * @brief Reads a simple value.
     * @return The decoded value.
     */
    CborSimpleValue readSimpleValue();

    /**
     * @brief Skips one complete value.
     */
    void skipValue();

    /**
     * @brief Finishes a definite or indefinite array after reading all its items.
     */
    void readEndArray();

    /**
     * @brief Finishes a definite or indefinite map after reading all its pairs.
     */
    void readEndMap();

    /**
     * @brief Reads a null value.
     */
    void readNull();

    /**
     * @brief Starts reading a container.
     * @return The item or pair count; std::nullopt for an indefinite container.
     */
    std::optional<std::uint64_t> readStartArray();

    /**
     * @brief Starts reading a container.
     * @return The item or pair count; std::nullopt for an indefinite container.
     */
    std::optional<std::uint64_t> readStartMap();

    /**
     * @brief Copies the unread bytes without advancing.
     * @return An independent copy of the bytes or text.
     */
    std::vector<std::uint8_t> remainderBytes() const;

    /**
     * @brief Reads one complete value as encoded CBOR.
     * @return An independent copy of the bytes or text.
     */
    std::vector<std::uint8_t> readEncodedValue();

    /**
     * @brief Reads a definite or indefinite byte string.
     * @return An independent copy of the bytes or text.
     */
    std::vector<std::uint8_t> readBytes();

    /**
     * @brief Reads a definite or indefinite text string, preserving embedded nulls.
     * @return An independent copy of the bytes or text.
     */
    std::string readText();

    /**
     * @brief Reads a tag 2 or 3 arbitrary-precision integer.
     * @return The decoded integer.
     */
    BigInt readBigInt();

    /**
     * @brief Inspects a semantic tag without advancing.
     * @return The tag.
     * @throws std::out_of_range The tag exceeds INT32_MAX.
     */
    CborTag peekTag() const;

    /**
     * @brief Reads a semantic tag.
     * @return The tag.
     * @throws std::out_of_range The tag exceeds INT32_MAX.
     */
    CborTag readTag();

    /**
     * @brief Exchanges two instances.
     * @param other The instance to exchange.
     */
    void swap(CborReader& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The source instance.
     * @return This instance; unchanged if copying fails.
     */
    CborReader& operator=(const CborReader& other);

    /**
     * @brief Moves another instance into this one.
     * @param other The source; self-move is safe.
     * @return This instance.
     */
    CborReader& operator=(CborReader&& other) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     * @return The borrowed reference.
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_cbor_reader_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_cbor_reader_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges instances through argument-dependent lookup.
 * @param lhs The first instance.
 * @param rhs The second instance.
 */
void swap(CborReader& lhs, CborReader& rhs) noexcept;
} // namespace Cometa
