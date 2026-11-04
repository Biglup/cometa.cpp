/**
 * @file CborWriter.h
 *
 * @brief Encodes CBOR values and container boundaries into a byte sequence.
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

struct cardano_cbor_writer_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Encodes CBOR values and container boundaries into a byte sequence.
 *
 * Use matching array or map start/end operations. writeEncoded() appends already encoded CBOR,
 * whereas byte-string operations encode their input as bytes.
 */
class CborWriter
{
  public:

    /**
     * @brief Initializes an empty CBOR stream.
     */
    CborWriter();

    /**
     * @brief Copies the current encoded bytes.
     * @param other The instance to copy.
     */
    CborWriter(const CborWriter& other);

    /**
     * @brief Moves another instance.
     * @param other The source instance.
     */
    CborWriter(CborWriter&& other) noexcept;

    /**
     * @brief Destroys this writer.
     */
    ~CborWriter();

  public:

    /**
     * @brief Writes an unsigned 64-bit integer.
     * @param value The value to encode.
     */
    void writeUInt(std::uint64_t value);

    /**
     * @brief Writes a signed 64-bit integer.
     * @param value The value to encode.
     */
    void writeInt(std::int64_t value);

    /**
     * @brief Writes the negative integer -1-value.
     * @param value The value to encode.
     */
    void writeNegativeInt(std::uint64_t value);

    /**
     * @brief Writes a boolean.
     * @param value The value to encode.
     */
    void writeBool(bool value);

    /**
     * @brief Writes null.
     */
    void writeNull();

    /**
     * @brief Writes undefined.
     */
    void writeUndefined();

    /**
     * @brief Writes the break ending an indefinite array; do not use for definite arrays.
     */
    void writeEndArray();

    /**
     * @brief Writes the break ending an indefinite map; do not use for definite maps.
     */
    void writeEndMap();

    /**
     * @brief Writes a container header.
     * @param size Item or pair count, or std::nullopt for indefinite length.
     * @throws std::out_of_range The count exceeds INT64_MAX.
     */
    void writeStartArray(std::optional<std::uint64_t> size);

    /**
     * @brief Writes a container header.
     * @param size Item or pair count, or std::nullopt for indefinite length.
     * @throws std::out_of_range The count exceeds INT64_MAX.
     */
    void writeStartMap(std::optional<std::uint64_t> size);

    /**
     * @brief Writes a byte string.
     * @param bytes The bytes to copy; may be empty.
     */
    void writeBytes(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Appends encoded bytes without validating their CBOR structure.
     * @param bytes The bytes to copy; may be empty.
     */
    void writeEncoded(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Writes a text string, preserving embedded nulls.
     * @param text The UTF-8 bytes to copy; may be empty.
     */
    void writeText(std::string_view text);

    /**
     * @brief Writes a tag 2 or 3 bignum, including for integers that fit in 64 bits.
     * @param value The integer to encode.
     */
    void writeBigInt(const BigInt& value);

    /**
     * @brief Writes a semantic tag preceding its value.
     * @param tag The tag to encode.
     * @throws std::out_of_range The tag exceeds INT32_MAX.
     */
    void writeTag(CborTag tag);

    /**
     * @brief Copies the encoded bytes.
     * @return The encoded stream; empty if nothing was written.
     */
    std::vector<std::uint8_t> toBytes() const;

    /**
     * @brief Formats the encoded bytes as lowercase hexadecimal.
     * @return The hexadecimal stream without a prefix.
     */
    std::string toHex() const;

    /**
     * @brief Clears all encoded bytes.
     */
    void reset();

    /**
     * @brief Exchanges two instances.
     * @param other The instance to exchange.
     */
    void swap(CborWriter& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The source instance.
     * @return This instance; unchanged if copying fails.
     */
    CborWriter& operator=(const CborWriter& other);

    /**
     * @brief Moves another instance into this one.
     * @param other The source; self-move is safe.
     * @return This instance.
     */
    CborWriter& operator=(CborWriter&& other) noexcept;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static CborWriter adopt(cardano_cbor_writer_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_cbor_writer_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_cbor_writer_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_cbor_writer_t* release() noexcept;

  private:

    /**
     * @brief Takes ownership of a validated native reference.
     * @param value The owned non-null reference.
     */
    explicit CborWriter(cardano_cbor_writer_t* value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     * @return The borrowed reference.
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_cbor_writer_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_cbor_writer_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges instances through argument-dependent lookup.
 * @param lhs The first instance.
 * @param rhs The second instance.
 */
void swap(CborWriter& lhs, CborWriter& rhs) noexcept;
} // namespace Cometa
