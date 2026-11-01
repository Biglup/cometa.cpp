/**
 * \file native.hpp
 *
 * \author angel.castillo
 * \date   Nov 01, 2026
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
#include <algorithm>
#include <cardano/cbor/cbor_reader.h>
#include <cardano/cbor/cbor_writer.h>
#include <cardano/json/json_writer.h>
#include <cometa/error.hpp>
#include <memory>
#include <new>
#include <string_view>
#include <utility>
#include <vector>

namespace Biglup::Cometa::Detail
{
inline void
check(cardano_error_t error, const char* operation)
{
  if (error == CARDANO_ERROR_MEMORY_ALLOCATION_FAILED)
  {
    throw std::bad_alloc();
  }
  if (error != CARDANO_SUCCESS)
  {
    throw Error(static_cast<int>(error), operation);
  }
}

template<class T, void (*Release)(T**)>
struct Deleter
{
    void
    operator()(T* value) const noexcept
    {
      Release(&value);
    }
};
template<class T, void (*Release)(T**)>
using Owned = std::unique_ptr<T, Deleter<T, Release>>;

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

template<class T, void (*Release)(T**)>
T*
clone(const T* source, cardano_error_t (*copy)(const T*, T**))
{
  return create<T, Release>([&](T** out)
                            { return copy(source, out); },
                            "copy");
}

inline const std::uint8_t*
data(const std::vector<std::uint8_t>& bytes)
{
  static const std::uint8_t empty = 0;
  return bytes.empty() ? &empty : bytes.data();
}

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

template<class T, void (*Release)(T**), class Decoder>
T*
decode(const std::vector<std::uint8_t>& bytes, Decoder decoder)
{
  Owned<cardano_cbor_reader_t, cardano_cbor_reader_unref> reader(cardano_cbor_reader_new(data(bytes), bytes.size()));
  if (!reader)
  {
    throw std::bad_alloc();
  }
  Owned<T, Release> result(create<T, Release>([&](T** out)
                                              { return decoder(reader.get(), out); },
                                              "fromCbor"));
  std::size_t       remaining = 0;
  check(cardano_cbor_reader_get_bytes_remaining(reader.get(), &remaining), "fromCbor");
  if (remaining != 0)
  {
    throw std::invalid_argument("Trailing CBOR bytes");
  }
  return result.release();
}

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
} // namespace Biglup::Cometa::Detail
