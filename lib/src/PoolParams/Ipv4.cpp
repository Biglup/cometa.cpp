/**
 * @file Ipv4.cpp
 *
 * @brief An IPv4 address in network byte order.
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

#include <Cometa/PoolParams/Ipv4.h>

#include "../CardanoInterop.h"

#include <cardano/pool_params/ipv4.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Ipv4::Ipv4(const std::vector<std::uint8_t>& bytes)
: m_value(nullptr)
{
  cardano_ipv4_t*       value  = nullptr;
  const cardano_error_t result = cardano_ipv4_new(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_ipv4_t, cardano_ipv4_unref> owner(value);

  checkCardanoError(result, "Ipv4");

  m_value = owner.release();
}

Ipv4::Ipv4(const Ipv4& other)
: m_value(nullptr)
{
  cardano_ipv4_t*       value  = nullptr;
  const cardano_error_t result = cardano_ipv4_deep_clone(other.checked(), &value);

  CardanoObject<cardano_ipv4_t, cardano_ipv4_unref> owner(value);

  checkCardanoError(result, "Ipv4");

  m_value = owner.release();
}

Ipv4::Ipv4(Ipv4&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Ipv4::~Ipv4()
{
  cardano_ipv4_unref(&m_value);
}

Ipv4
Ipv4::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return Ipv4(*decodeCbor<cardano_ipv4_t, cardano_ipv4_unref>(cbor, cardano_ipv4_from_cbor));
}

Ipv4
Ipv4::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Ipv4
Ipv4::fromString(std::string_view text)
{
  cardano_ipv4_t*       value  = nullptr;
  const cardano_error_t result = cardano_ipv4_from_string(text.empty() ? "" : text.data(), text.size(), &value);

  CardanoObject<cardano_ipv4_t, cardano_ipv4_unref> owner(value);

  checkCardanoError(result, "Ipv4 fromString");

  return Ipv4(*owner.release());
}

std::vector<std::uint8_t>
Ipv4::toBytes() const
{
  const cardano_ipv4_t* value = checked();
  const std::uint8_t*   bytes = cardano_ipv4_get_bytes(value);

  return std::vector<std::uint8_t>(bytes, bytes + cardano_ipv4_get_bytes_size(value));
}

std::string
Ipv4::toString() const
{
  return cardano_ipv4_get_string(checked());
}

std::vector<std::uint8_t>
Ipv4::toCbor() const
{
  return encodeCbor(checked(), cardano_ipv4_to_cbor);
}

void
Ipv4::swap(Ipv4& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Ipv4&
Ipv4::operator=(const Ipv4& other)
{
  if (this != &other)
  {
    Ipv4 copy(other);

    swap(copy);
  }

  return *this;
}

Ipv4&
Ipv4::operator=(Ipv4&& other) noexcept
{
  if (this != &other)
  {
    cardano_ipv4_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
Ipv4::operator==(const Ipv4& other) const
{
  return toBytes() == other.toBytes();
}

bool
Ipv4::operator!=(const Ipv4& other) const
{
  return !(*this == other);
}

Ipv4
Ipv4::adopt(cardano_ipv4_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null Ipv4");
  }

  return Ipv4(*value);
}

cardano_ipv4_t*
Ipv4::nativeHandle() noexcept
{
  return m_value;
}

const cardano_ipv4_t*
Ipv4::nativeHandle() const noexcept
{
  return m_value;
}

cardano_ipv4_t*
Ipv4::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

Ipv4::Ipv4(cardano_ipv4_t& value) noexcept
: m_value(&value)
{
}

cardano_ipv4_t*
Ipv4::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Ipv4");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Ipv4& lhs, Ipv4& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
