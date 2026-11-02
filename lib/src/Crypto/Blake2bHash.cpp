/**
 * @file Blake2bHash.cpp
 *
 * @brief Implementation of the Blake2bHash value type.
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

/* INCLUDES ******************************************************************/

#include <Cometa/Crypto/Blake2bHash.h>

#include "../CardanoInterop.h"

#include <cardano/crypto/blake2b_hash.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Blake2bHash::Blake2bHash(const std::vector<std::uint8_t>& bytes)
: m_value(nullptr)
{
  cardano_blake2b_hash_t* value  = nullptr;
  const cardano_error_t   result = cardano_blake2b_hash_from_bytes(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  checkCardanoError(result, "Blake2bHash");

  m_value = owner.release();
}

Blake2bHash::Blake2bHash(const Blake2bHash& other)
: m_value(nullptr)
{
  cardano_blake2b_hash_t* value  = nullptr;
  const cardano_error_t   result = cardano_blake2b_hash_deep_clone(other.checked(), &value);

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  checkCardanoError(result, "Blake2bHash copy");

  m_value = owner.release();
}

Blake2bHash::Blake2bHash(Blake2bHash&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Blake2bHash::~Blake2bHash()
{
  cardano_blake2b_hash_unref(&m_value);
}

Blake2bHash
Blake2bHash::fromHex(std::string_view hex)
{
  validateHex(hex);

  cardano_blake2b_hash_t* value  = nullptr;
  const cardano_error_t   result = cardano_blake2b_hash_from_hex(hex.empty() ? "" : hex.data(), hex.size(), &value);

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  checkCardanoError(result, "Blake2bHash fromHex");

  return Blake2bHash(owner.release());
}

Blake2bHash
Blake2bHash::compute(const std::vector<std::uint8_t>& data, std::size_t digestSize)
{
  if (data.empty())
  {
    throw std::invalid_argument("Hash input must not be empty");
  }

  if (digestSize == 0 || digestSize > 64)
  {
    throw std::invalid_argument("Digest size must be between 1 and 64");
  }

  cardano_blake2b_hash_t* value  = nullptr;
  const cardano_error_t   result = cardano_blake2b_compute_hash(byteData(data), data.size(), digestSize, &value);

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  checkCardanoError(result, "Blake2bHash compute");

  return Blake2bHash(owner.release());
}

Blake2bHash
Blake2bHash::compute(const std::vector<std::uint8_t>& data, Blake2bHashSize digestSize)
{
  switch (digestSize)
  {
    case Blake2bHashSize::Bits224:
    case Blake2bHashSize::Bits256:
    case Blake2bHashSize::Bits512:
      return compute(data, static_cast<std::size_t>(digestSize));
    default:
      throw std::invalid_argument("Unknown BLAKE2b digest length");
  }
}

Blake2bHash
Blake2bHash::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return Blake2bHash(decodeCbor<cardano_blake2b_hash_t, cardano_blake2b_hash_unref>(cbor, cardano_blake2b_hash_from_cbor));
}

Blake2bHash
Blake2bHash::fromCbor(const std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::vector<std::uint8_t>
Blake2bHash::toBytes() const
{
  const cardano_blake2b_hash_t* value = checked();
  const std::uint8_t*           bytes = cardano_blake2b_hash_get_data(value);

  return { bytes, bytes + cardano_blake2b_hash_get_bytes_size(value) };
}

std::string
Blake2bHash::toHex() const
{
  const cardano_blake2b_hash_t* value = checked();
  std::string                   hex(cardano_blake2b_hash_get_hex_size(value), '\0');
  checkCardanoError(cardano_blake2b_hash_to_hex(value, hex.data(), hex.size()), "Blake2bHash toHex");
  hex.pop_back();

  return hex;
}

std::vector<std::uint8_t>
Blake2bHash::toCbor() const
{
  return encodeCbor(checked(), cardano_blake2b_hash_to_cbor);
}

std::string
Blake2bHash::toJson() const
{
  return encodeJson(checked(), cardano_blake2b_hash_to_cip116_json);
}

void
Blake2bHash::swap(Blake2bHash& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Blake2bHash&
Blake2bHash::operator=(const Blake2bHash& other)
{
  if (this != &other)
  {
    Blake2bHash copy(other);

    swap(copy);
  }

  return *this;
}

Blake2bHash&
Blake2bHash::operator=(Blake2bHash&& other) noexcept
{
  if (this != &other)
  {
    cardano_blake2b_hash_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
Blake2bHash::operator==(const Blake2bHash& other) const
{
  return cardano_blake2b_hash_equals(checked(), other.checked());
}

bool
Blake2bHash::operator!=(const Blake2bHash& other) const
{
  return !(*this == other);
}

bool
Blake2bHash::operator<(const Blake2bHash& other) const
{
  return cardano_blake2b_hash_compare(checked(), other.checked()) < 0;
}

/* PRIVATE FUNCTIONS *********************************************************/

Blake2bHash::Blake2bHash(cardano_blake2b_hash_t* value) noexcept
: m_value(value)
{
}

cardano_blake2b_hash_t*
Blake2bHash::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Blake2bHash");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Blake2bHash& lhs, Blake2bHash& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
