/**
 * @file Anchor.cpp
 *
 * @brief A URL and the hash of its governance metadata.
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

#include <Cometa/Common/Anchor.h>

#include "../CardanoInterop.h"

#include <cardano/common/anchor.h>

#include <cstring>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Anchor::Anchor(std::string_view url, const Blake2bHash& hash)
: m_value(nullptr)
{
  if (url.empty() || url.size() > 128 || url.find('\0') != std::string_view::npos)
  {
    throw std::invalid_argument("Anchor URL must contain 1 to 128 bytes without embedded NULs");
  }

  const std::vector<std::uint8_t> bytes = hash.toBytes();

  if (bytes.size() != 32)
  {
    throw std::invalid_argument("Anchor hash must be 32 bytes");
  }

  cardano_anchor_t*     value  = nullptr;
  const cardano_error_t result = cardano_anchor_from_hash_bytes(url.data(), url.size(), bytes.data(), bytes.size(), &value);

  CardanoObject<cardano_anchor_t, cardano_anchor_unref> owner(value);

  checkCardanoError(result, "Anchor");

  m_value = owner.release();
}

Anchor::Anchor(const Anchor& other)
: m_value(nullptr)
{
  cardano_anchor_t*     value  = nullptr;
  const cardano_error_t result = cardano_anchor_deep_clone(other.checked(), &value);

  CardanoObject<cardano_anchor_t, cardano_anchor_unref> owner(value);

  checkCardanoError(result, "Anchor copy");

  m_value = owner.release();
}

Anchor::Anchor(Anchor&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Anchor::~Anchor()
{
  cardano_anchor_unref(&m_value);
}

Anchor
Anchor::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return Anchor(decodeCbor<cardano_anchor_t, cardano_anchor_unref>(cbor, cardano_anchor_from_cbor));
}

Anchor
Anchor::fromCbor(const std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Blake2bHash
Anchor::hash() const
{
  const cardano_anchor_t* value = checked();
  const std::uint8_t*     bytes = cardano_anchor_get_hash_bytes(value);

  return Blake2bHash(std::vector<std::uint8_t>(bytes, bytes + cardano_anchor_get_hash_bytes_size(value)));
}

void
Anchor::setHash(const Blake2bHash& hash)
{
  Anchor replacement(url(), hash);

  swap(replacement);
}

std::string
Anchor::url() const
{
  return cardano_anchor_get_url(checked());
}

void
Anchor::setUrl(std::string_view url)
{
  Anchor replacement(url, hash());

  swap(replacement);
}

std::vector<std::uint8_t>
Anchor::toCbor() const
{
  return encodeCbor(checked(), cardano_anchor_to_cbor);
}

std::string
Anchor::toJson() const
{
  return encodeJson(checked(), cardano_anchor_to_cip116_json);
}

void
Anchor::swap(Anchor& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Anchor&
Anchor::operator=(const Anchor& other)
{
  if (this != &other)
  {
    Anchor copy(other);

    swap(copy);
  }

  return *this;
}

Anchor&
Anchor::operator=(Anchor&& other) noexcept
{
  if (this != &other)
  {
    cardano_anchor_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
Anchor::operator==(const Anchor& other) const
{
  const cardano_anchor_t* lhs = checked();
  const cardano_anchor_t* rhs = other.checked();

  return std::strcmp(cardano_anchor_get_url(lhs), cardano_anchor_get_url(rhs)) == 0 &&
    std::memcmp(cardano_anchor_get_hash_bytes(lhs), cardano_anchor_get_hash_bytes(rhs), 32) == 0;
}

bool
Anchor::operator!=(const Anchor& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

Anchor::Anchor(cardano_anchor_t* value) noexcept
: m_value(value)
{
}

cardano_anchor_t*
Anchor::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Anchor");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Anchor& lhs, Anchor& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
