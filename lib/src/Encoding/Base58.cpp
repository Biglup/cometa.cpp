/**
 * @file Base58.cpp
 *
 * @brief Base58 encoding and decoding.
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

/* INCLUDES ******************************************************************/

#include <Cometa/Encoding/Base58.h>

#include "../CardanoInterop.h"

#include <cardano/encoding/base58.h>

#include <algorithm>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
/**
 * @brief Tests whether a character is outside the Base58 alphabet.
 *
 * @param character The character to inspect.
 *
 * @return True for an invalid Base58 digit.
 */
bool
isInvalidBase58Character(char character)
{
  const std::string_view alphabet = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

  return alphabet.find(character) == std::string_view::npos;
}
} // namespace

/* FUNCTIONS IMPLEMENTATION **************************************************/
std::string
encodeBase58(const std::vector<std::uint8_t>& bytes)
{
  if (bytes.empty())
  {
    return {};
  }

  const std::size_t capacity = cardano_encoding_base58_get_encoded_length(bytes.data(), bytes.size());

  if (capacity == 0)
  {
    throw std::length_error("Base58 input exceeds the supported capacity");
  }

  std::string encoded(capacity, '\0');

  checkCardanoError(cardano_encoding_base58_encode(bytes.data(), bytes.size(), encoded.data(), encoded.size()), "encodeBase58");

  encoded.resize(encoded.find('\0'));

  return encoded;
}

std::vector<std::uint8_t>
decodeBase58(std::string_view text)
{
  if (text.empty())
  {
    return {};
  }

  if (std::any_of(text.begin(), text.end(), isInvalidBase58Character))
  {
    throw std::invalid_argument("Invalid Base58 digit");
  }

  const std::size_t size = cardano_encoding_base58_get_decoded_length(text.data(), text.size());

  if (size == 0)
  {
    // A valid nonempty Base58 string has at least one decoded byte.
    throw std::bad_alloc();
  }

  std::vector<std::uint8_t> bytes(size);

  checkCardanoError(cardano_encoding_base58_decode(text.data(), text.size(), bytes.data(), bytes.size()), "decodeBase58");

  return bytes;
}
} // namespace Cometa
