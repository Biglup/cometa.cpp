/**
 * @file JsonWriter.h
 *
 * @brief JSON writer.
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

#include <Cometa/Common/BigInt.h>
#include <Cometa/Json/JsonContext.h>
#include <Cometa/Json/JsonFormat.h>
#include <Cometa/Json/JsonType.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_json_writer_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
class JsonValue;

/** @brief Builds one JSON document with checked write operations. */
class JsonWriter
{
  public:

    /**
     * @brief Initializes an empty document.
     * @param format Output whitespace style.
     * @throws std::invalid_argument The format is unknown.
     */
    explicit JsonWriter(JsonFormat format = JsonFormat::Compact);

    /**
     * @brief Initializes an independent copy.
     * @param other The source instance.
     */
    JsonWriter(const JsonWriter& other);

    /**
     * @brief Moves another instance.
     * @param other The source instance.
     */
    JsonWriter(JsonWriter&& other) noexcept;

    /**
     * @brief Destroys this writer.
     */
    ~JsonWriter();

  public:

    /**
     * @brief Starts an array.
     */
    void writeStartArray();

    /**
     * @brief Ends the current array.
     */
    void writeEndArray();

    /**
     * @brief Starts an object.
     */
    void writeStartObject();

    /**
     * @brief Ends the current object.
     */
    void writeEndObject();

    /**
     * @brief Writes null.
     */
    void writeNull();

    /**
     * @brief Writes a boolean.
     * @param value The value to write.
     */
    void writeBool(bool value);

    /**
     * @brief Writes an unsigned integer.
     * @param value The value to write.
     */
    void writeUInt(std::uint64_t value);

    /**
     * @brief Writes a signed integer.
     * @param value The value to write.
     */
    void writeInt(std::int64_t value);

    /**
     * @brief Writes an unsigned integer as a JSON string.
     * @param value The value to write.
     */
    void writeUIntAsString(std::uint64_t value);

    /**
     * @brief Writes a signed integer as a JSON string.
     * @param value The value to write.
     */
    void writeIntAsString(std::int64_t value);

    /**
     * @brief Writes a finite floating-point number.
     * @param value The value to write.
     */
    void writeDouble(double value);

    /**
     * @brief Writes a finite floating-point number as a JSON string.
     * @param value The value to write.
     */
    void writeDoubleAsString(double value);

    /**
     * @brief Writes a string, escaping special characters.
     * @param text UTF-8 text; may be empty or contain null characters.
     */
    void writeString(std::string_view text);

    /**
     * @brief Writes the name of the next object property.
     * @param text UTF-8 text; may be empty or contain null characters.
     */
    void writePropertyName(std::string_view text);

    /**
     * @brief Appends one validated JSON value, preserving its whitespace and number tokens.
     * @param json One complete JSON value.
     */
    void writeRaw(std::string_view json);

    /**
     * @brief Writes a parsed JSON value.
     * @param value The value to serialize.
     */
    void writeValue(const JsonValue& value);

    /**
     * @brief Writes an arbitrary-precision integer as a JSON number.
     * @param value The integer to serialize.
     */
    void writeBigInt(const BigInt& value);

    /**
     * @brief Writes an arbitrary-precision integer as a JSON string.
     * @param value The integer to serialize.
     */
    void writeBigIntAsString(const BigInt& value);

    /**
     * @brief Writes lowercase hexadecimal bytes as a JSON string.
     * @param bytes The bytes; may be empty.
     */
    void writeBytesAsHex(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Writes a Bech32-encoded JSON string.
     * @param prefix The human-readable prefix.
     * @param bytes The binary payload.
     */
    void writeBytesAsBech32(const std::string_view& prefix, const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Gets the active document container.
     * @return Root, object, or array context.
     */
    JsonContext context() const;

    /**
     * @brief Copies the completed JSON document.
     * @return The JSON document.
     * @throws std::logic_error No complete root value has been written.
     */
    std::string toString() const;

    /**
     * @brief Clears the document and any write error, preserving the output format.
     */
    void reset();

    /**
     * @brief Exchanges two instances.
     * @param other The instance to exchange.
     */
    void swap(JsonWriter& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The source.
     * @return This instance; unchanged if copying fails.
     */
    JsonWriter& operator=(const JsonWriter& other);

    /**
     * @brief Moves another instance into this one.
     * @param other The source; self-move is safe.
     * @return This instance.
     */
    JsonWriter& operator=(JsonWriter&& other) noexcept;

  private:

    /**
     * @brief Checks the sticky writer error after an operation.
     * @throws Error Writing failed; reset clears the error.
     */
    void checkWrite() const;

    /**
     * @brief Gets the stored reference after checking its lifetime.
     * @return The borrowed reference.
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_json_writer_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_json_writer_t* m_value;

    /**
     * @brief The output whitespace format.
     */
    JsonFormat m_format;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges instances through argument-dependent lookup.
 * @param lhs The first instance.
 * @param rhs The second instance.
 */
void swap(JsonWriter& lhs, JsonWriter& rhs) noexcept;
} // namespace Cometa
