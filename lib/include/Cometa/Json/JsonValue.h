/**
 * @file JsonValue.h
 *
 * @brief JSON value.
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

struct cardano_json_object_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief A parsed JSON value with exact decimal number tokens. */
class JsonValue
{
  public:

    /**
     * @brief Initializes an independent copy.
     * @param other The source instance.
     */
    JsonValue(const JsonValue& other);

    /**
     * @brief Moves another instance.
     * @param other The source instance.
     */
    JsonValue(JsonValue&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~JsonValue();

  public:

    /**
     * @brief Parses exactly one JSON value, preserving decimal number tokens.
     * @param json UTF-8 JSON text; trailing whitespace is allowed.
     * @return The parsed value.
     * @throws Error The JSON syntax or UTF-8 is invalid.
     */
    static JsonValue parse(std::string_view json);

  public:

    /**
     * @brief Gets the JSON type.
     * @return The value type.
     */
    JsonType type() const;

    /**
     * @brief Serializes a complete JSON value without changing number tokens.
     * @param format Output whitespace style.
     * @return The JSON document.
     */
    std::string toJson(JsonFormat format = JsonFormat::Compact) const;

    /**
     * @brief Gets an array length or object property count.
     * @return The element or property count.
     * @throws std::invalid_argument This is neither an array nor an object.
     */
    std::size_t size() const;

    /**
     * @brief Checks whether an object contains a property.
     * @param key Property name, including empty or embedded-null names.
     * @return True if the property exists.
     */
    bool contains(std::string_view key) const;

    /**
     * @brief Copies an object property value.
     * @param key The property name.
     * @return An independent value.
     * @throws std::out_of_range The property does not exist.
     */
    JsonValue at(std::string_view key) const;

    /**
     * @brief Copies an array element.
     * @param index Zero-based element index.
     * @return An independent value.
     * @throws std::out_of_range The index is out of range.
     */
    JsonValue at(std::size_t index) const;

    /**
     * @brief Copies an object property name in parsed order.
     * @param index Zero-based property index.
     * @return The property name, preserving embedded nulls.
     * @throws std::out_of_range The index is out of range.
     */
    std::string keyAt(std::size_t index) const;

    /**
     * @brief Copies an object property value in parsed order.
     * @param index Zero-based property index.
     * @return An independent property value.
     * @throws std::out_of_range The index is out of range.
     */
    JsonValue valueAt(std::size_t index) const;

    /**
     * @brief Copies a JSON string, preserving embedded nulls.
     * @return The string contents.
     */
    std::string asString() const;

    /**
     * @brief Converts a number or numeric string to an unsigned 64-bit integer.
     * @return The converted value.
     * @throws Error The type or numeric representation cannot be converted.
     */
    std::uint64_t asUInt() const;

    /**
     * @brief Converts a number or numeric string to a signed 64-bit integer.
     * @return The converted value.
     * @throws Error The type or numeric representation cannot be converted.
     */
    std::int64_t asInt() const;

    /**
     * @brief Converts a number or numeric string to double precision.
     * @return The converted value.
     * @throws Error The type or numeric representation cannot be converted.
     */
    double asDouble() const;

    /**
     * @brief Reads a JSON boolean.
     * @return The converted value.
     * @throws Error The type or numeric representation cannot be converted.
     */
    bool asBool() const;

    /**
     * @brief Checks whether a number token has a minus sign.
     * @return True if the token has the specified form.
     */
    bool isNegative() const;

    /**
     * @brief Checks whether a number token contains a fraction or exponent.
     * @return True if the token has the specified form.
     */
    bool isReal() const;

    /**
     * @brief Exchanges two instances.
     * @param other The instance to exchange.
     */
    void swap(JsonValue& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The source.
     * @return This instance; unchanged if copying fails.
     */
    JsonValue& operator=(const JsonValue& other);

    /**
     * @brief Moves another instance into this one.
     * @param other The source; self-move is safe.
     * @return This instance.
     */
    JsonValue& operator=(JsonValue&& other) noexcept;

  private:

    /**
     * @brief Allows the JSON writer to inspect the parsed value.
     */
    friend class JsonWriter;

    /**
     * @brief Adopts an owned native value.
     * @param value The owned reference transferred to this instance.
     */
    explicit JsonValue(cardano_json_object_t& value) noexcept;

  private:

    /**
     * @brief Clones a borrowed native value.
     * @param value The value to clone.
     * @return The independent clone.
     */
    static JsonValue copyOf(const cardano_json_object_t& value);

    /**
     * @brief Checks the JSON type.
     * @param expected The required type.
     * @throws std::invalid_argument The type does not match.
     */
    void requireType(JsonType expected) const;

    /**
     * @brief Gets the stored reference after checking its lifetime.
     * @return The borrowed reference.
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_json_object_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_json_object_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges instances through argument-dependent lookup.
 * @param lhs The first instance.
 * @param rhs The second instance.
 */
void swap(JsonValue& lhs, JsonValue& rhs) noexcept;
} // namespace Cometa
