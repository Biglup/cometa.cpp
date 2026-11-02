/**
 * @file Bech32.cpp
 *
 * @brief Bech32 encoding and decoding for Cardano values.
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

#include <Cometa/Encoding/Bech32.h>

#include "../CardanoInterop.h"

#include <cardano/encoding/bech32.h>

#include <algorithm>
#include <limits>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
/**
 * @brief Tests whether a character lies outside the printable ASCII range.
 *
 * @param character The character to inspect.
 *
 * @return True for characters disallowed in Bech32 text.
 */
bool
isInvalidBech32Character(char character)
{
  return character < 33 || character > 126;
}

/**
 * @brief Validates and lowercases a human-readable prefix.
 *
 * @param prefix The input; invalid length or characters throw std::invalid_argument.
 *
 * @return A lowercase copy of the prefix.
 */
std::string
normalizePrefix(const std::string_view& prefix)
{
  if (prefix.empty() || prefix.size() > 83)
  {
    throw std::invalid_argument("Bech32 prefix must contain 1 to 83 characters");
  }

  std::string normalized(prefix);

  for (char& character: normalized)
  {
    if (character < 33 || character > 126)
    {
      throw std::invalid_argument("Bech32 prefix must contain printable ASCII characters");
    }

    if (character >= 'A' && character <= 'Z')
    {
      character = static_cast<char>(character - 'A' + 'a');
    }
  }

  return normalized;
}
} // namespace

/* FUNCTIONS IMPLEMENTATION **************************************************/

std::string
encodeBech32(const std::string_view& prefix, const std::vector<std::uint8_t>& bytes)
{
  const std::string normalized = normalizePrefix(prefix);

  if (bytes.size() > static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()) / 8)
  {
    throw std::length_error("Bech32 payload exceeds the supported capacity");
  }

  const std::size_t capacity = cardano_encoding_bech32_get_encoded_length(normalized.c_str(), normalized.size(), byteData(bytes), bytes.size());
  std::string       encoded(capacity, '\0');

  checkCardanoError(cardano_encoding_bech32_encode(normalized.c_str(), normalized.size(), byteData(bytes), bytes.size(), encoded.data(), encoded.size()), "encodeBech32");

  encoded.pop_back();

  return encoded;
}

Bech32Data
decodeBech32(std::string_view text)
{
  if (text.size() > static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()) / 8)
  {
    throw std::length_error("Bech32 text exceeds the supported capacity");
  }

  const std::size_t separator = text.rfind('1');

  if (separator == std::string_view::npos || separator == 0 || separator > 83 || text.size() - separator < 7)
  {
    throw std::invalid_argument("Invalid Bech32 prefix or checksum length");
  }

  if (std::any_of(text.begin(), text.end(), isInvalidBech32Character))
  {
    throw std::invalid_argument("Bech32 text must contain printable ASCII characters");
  }

  std::size_t               prefixSize = 0;
  const std::size_t         size       = cardano_encoding_bech32_get_decoded_length(text.data(), text.size(), &prefixSize);
  std::string               prefix(prefixSize, '\0');
  std::vector<std::uint8_t> bytes(size);
  std::uint8_t              emptyPayload = 0;

  checkCardanoError(cardano_encoding_bech32_decode(text.data(), text.size(), prefix.data(), prefix.size(), bytes.empty() ? &emptyPayload : bytes.data(), bytes.size()), "decodeBech32");

  prefix.pop_back();

  return { std::move(prefix), std::move(bytes) };
}
} // namespace Cometa
