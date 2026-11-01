/**
 * @file CardanoInterop.h
 *
 * @brief Internal native ownership and serialization helpers.
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

#include <Cometa/Error.h>

#include <cardano/cbor/cbor_reader.h>
#include <cardano/cbor/cbor_writer.h>
#include <cardano/json/json_writer.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa::Detail
{
/**
 * @brief Translates a native failure into a C++ exception.
 *
 * @param error The native status; success returns normally.
 * @param operation The operation name included in the exception.
 */
inline void
check(cardano_error_t error, const char* operation)
{
  if (error == CARDANO_ERROR_MEMORY_ALLOCATION_FAILED)
  {
    throw std::bad_alloc();
  }
  if (error != CARDANO_SUCCESS)
  {
    throw Error(static_cast<ErrorCode>(error), operation);
  }
}

/**
 * @brief Releases one owned native reference using its typed unref function.
 */
template<class T, void (*Release)(T**)>
struct Deleter
{
    void
    operator()(T* value) const noexcept
    {
      Release(&value);
    }
};
/**
 * @brief Guards an operation-scoped native reference.
 */
template<class T, void (*Release)(T**)>
using Owned = std::unique_ptr<T, Deleter<T, Release>>;

/**
 * @brief Acquires a native object and releases partial outputs on status errors.
 *
 * @param function The native constructor adapter; it must not throw after publishing an owned output.
 * @param operation The operation name included in a failure.
 *
 * @return One owned native reference, transferred to the caller.
 */
template<class T, void (*Release)(T**), class Function>
T*
create(Function function, const char* operation)
{
  T*                value  = nullptr;
  const auto        result = function(&value);
  Owned<T, Release> owner(value);
  check(result, operation);
  if (!owner)
  {
    throw std::bad_alloc();
  }
  return owner.release();
}

/**
 * @brief Creates an independent owned native copy.
 *
 * @param source The borrowed source value.
 * @param copy The native deep-clone operation.
 *
 * @return One owned reference to the copy.
 */
template<class T, void (*Release)(T**)>
T*
clone(const T* source, cardano_error_t (*copy)(const T*, T**))
{
  return create<T, Release>(
    [&](T** out)
    {
      return copy(source, out);
    },
    "copy");
}

/**
 * @brief Gets a non-null borrowed byte pointer, including for empty input.
 *
 * @param bytes The input bytes; their owner must remain alive during use.
 *
 * @return The byte storage, or a sentinel for a zero-length input.
 */
inline const std::uint8_t*
data(const std::vector<std::uint8_t>& bytes)
{
  static const std::uint8_t EMPTY_BYTE = 0;
  return bytes.empty() ? &EMPTY_BYTE : bytes.data();
}

/**
 * @brief Rejects odd-length or non-hexadecimal input.
 *
 * @param hex The hexadecimal bytes to validate.
 */
inline void
validateHex(std::string_view hex)
{
  if (hex.size() % 2 != 0)
  {
    throw std::invalid_argument("Hex must contain pairs of digits");
  }
  const auto invalid = [](char digit)
  {
    return !((digit >= '0' && digit <= '9') || (digit >= 'a' && digit <= 'f') || (digit >= 'A' && digit <= 'F'));
  };
  if (std::any_of(hex.begin(), hex.end(), invalid))
  {
    throw std::invalid_argument("Invalid hexadecimal digit");
  }
}

/**
 * @brief Converts validated hexadecimal pairs to bytes.
 *
 * @param hex Hexadecimal bytes without a prefix.
 *
 * @return The decoded bytes.
 *
 * @throws std::invalid_argument The input contains an incomplete pair or invalid digit.
 */
inline std::vector<std::uint8_t>
decodeHex(std::string_view hex)
{
  validateHex(hex);
  const auto nibble = [](char digit) -> std::uint8_t
  {
    if (digit >= '0' && digit <= '9')
    {
      return static_cast<std::uint8_t>(digit - '0');
    }
    if (digit >= 'a' && digit <= 'f')
    {
      return static_cast<std::uint8_t>(digit - 'a' + 10);
    }
    return static_cast<std::uint8_t>(digit - 'A' + 10);
  };
  std::vector<std::uint8_t> bytes(hex.size() / 2);
  for (std::size_t i = 0; i < bytes.size(); ++i)
  {
    bytes[i] = static_cast<std::uint8_t>((nibble(hex[i * 2]) << 4) | nibble(hex[i * 2 + 1]));
  }
  return bytes;
}

/**
 * @brief Decodes one CBOR value with scoped reader and result ownership.
 *
 * @param bytes The encoded input; trailing data is rejected.
 * @param decoder The native decoder.
 *
 * @return One owned native reference to the decoded value.
 */
template<class T, void (*Release)(T**), class Decoder>
T*
decode(const std::vector<std::uint8_t>& bytes, Decoder decoder)
{
  if (bytes.empty())
  {
    throw std::invalid_argument("CBOR must not be empty");
  }
  Owned<cardano_cbor_reader_t, cardano_cbor_reader_unref> reader(cardano_cbor_reader_new(data(bytes), bytes.size()));
  if (!reader)
  {
    throw std::bad_alloc();
  }
  Owned<T, Release> result(create<T, Release>(
    [&](T** out)
    {
      return decoder(reader.get(), out);
    },
    "fromCbor"));
  std::size_t       remaining = 0;
  check(cardano_cbor_reader_get_bytes_remaining(reader.get(), &remaining), "fromCbor");
  if (remaining != 0)
  {
    throw std::invalid_argument("Trailing CBOR bytes");
  }
  return result.release();
}

/**
 * @brief Serializes a value with a scoped native CBOR writer.
 *
 * @param value The borrowed source value.
 * @param encoder The native CBOR encoder.
 *
 * @return An owned byte sequence.
 */
template<class T, class Encoder>
std::vector<std::uint8_t>
encode(const T* value, Encoder encoder)
{
  Owned<cardano_cbor_writer_t, cardano_cbor_writer_unref> writer(cardano_cbor_writer_new());
  if (!writer)
  {
    throw std::bad_alloc();
  }
  check(encoder(value, writer.get()), "toCbor");
  std::vector<std::uint8_t> bytes(cardano_cbor_writer_get_encode_size(writer.get()));
  check(cardano_cbor_writer_encode(writer.get(), bytes.data(), bytes.size()), "toCbor");
  return bytes;
}

/**
 * @brief Serializes a value with a scoped native JSON writer.
 *
 * @param value The borrowed source value.
 * @param encoder The native CIP-116 encoder.
 *
 * @return An owned compact JSON string without its terminating NUL.
 */
template<class T, class Encoder>
std::string
json(const T* value, Encoder encoder)
{
  Owned<cardano_json_writer_t, cardano_json_writer_unref> writer(cardano_json_writer_new(CARDANO_JSON_FORMAT_COMPACT));
  if (!writer)
  {
    throw std::bad_alloc();
  }
  check(encoder(value, writer.get()), "toJson");
  std::string text(cardano_json_writer_get_encoded_size(writer.get()), '\0');
  check(cardano_json_writer_encode(writer.get(), text.data(), text.size()), "toJson");
  text.pop_back();
  return text;
}
} // namespace Cometa::Detail
