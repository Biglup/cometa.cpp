/**
 * @file CborReader.h
 *
 * @brief Reads CBOR values sequentially while tracking nested containers.
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
 * @brief Reads CBOR values sequentially while tracking nested containers.
 *
 * Use peekState() to inspect the next item and typed read operations to consume it. Container
 * boundaries and trailing bytes must be handled when parsing a complete application value.
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
     * @return The next item kind, container boundary, or finished state.
     */
    CborReaderState peekState() const;

    /**
     * @brief Gets the number of unread bytes.
     * @return The byte count from the current position to the end of the input.
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
     * @brief Reads an array header and enters its elements.
     * @return The element count, or std::nullopt for an indefinite-length array.
     */
    std::optional<std::uint64_t> readStartArray();

    /**
     * @brief Reads a map header and enters its key-value pairs.
     * @return The pair count, or std::nullopt for an indefinite-length map.
     */
    std::optional<std::uint64_t> readStartMap();

    /**
     * @brief Copies the unread bytes without advancing.
     * @return The remaining encoded CBOR bytes.
     */
    std::vector<std::uint8_t> remainderBytes() const;

    /**
     * @brief Reads one complete value as encoded CBOR.
     * @return The encoded item, including its header and any nested values.
     */
    std::vector<std::uint8_t> readEncodedValue();

    /**
     * @brief Reads a definite or indefinite byte string.
     * @return The decoded byte-string payload, without CBOR headers.
     */
    std::vector<std::uint8_t> readBytes();

    /**
     * @brief Reads a definite or indefinite text string.
     * @return The decoded text bytes, preserving embedded NUL characters.
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

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static CborReader adopt(cardano_cbor_reader_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_cbor_reader_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_cbor_reader_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_cbor_reader_t* release() noexcept;

  private:

    /**
     * @brief Takes ownership of a validated native reference.
     * @param value The owned non-null reference.
     */
    explicit CborReader(cardano_cbor_reader_t* value) noexcept;

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
