/**
 * @file Ipv6.cpp
 *
 * @brief An IPv6 address in network byte order.
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

#include <Cometa/PoolParams/Ipv6.h>

#include "../CardanoInterop.h"

#include <cardano/pool_params/ipv6.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Ipv6::Ipv6(const std::vector<std::uint8_t>& bytes)
: m_value(nullptr)
{
  cardano_ipv6_t*       value  = nullptr;
  const cardano_error_t result = cardano_ipv6_new(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_ipv6_t, cardano_ipv6_unref> owner(value);

  checkCardanoError(result, "Ipv6");

  m_value = owner.release();
}

Ipv6::Ipv6(const Ipv6& other)
: m_value(nullptr)
{
  cardano_ipv6_t*       value  = nullptr;
  const cardano_error_t result = cardano_ipv6_deep_clone(other.checked(), &value);

  CardanoObject<cardano_ipv6_t, cardano_ipv6_unref> owner(value);

  checkCardanoError(result, "Ipv6");

  m_value = owner.release();
}

Ipv6::Ipv6(Ipv6&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Ipv6::~Ipv6()
{
  cardano_ipv6_unref(&m_value);
}

Ipv6
Ipv6::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return Ipv6(*decodeCbor<cardano_ipv6_t, cardano_ipv6_unref>(cbor, cardano_ipv6_from_cbor));
}

Ipv6
Ipv6::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Ipv6
Ipv6::fromString(std::string_view text)
{
  cardano_ipv6_t*       value  = nullptr;
  const cardano_error_t result = cardano_ipv6_from_string(text.empty() ? "" : text.data(), text.size(), &value);

  CardanoObject<cardano_ipv6_t, cardano_ipv6_unref> owner(value);

  checkCardanoError(result, "Ipv6 fromString");

  return Ipv6(*owner.release());
}

std::vector<std::uint8_t>
Ipv6::toBytes() const
{
  const cardano_ipv6_t* value = checked();
  const std::uint8_t*   bytes = cardano_ipv6_get_bytes(value);

  return std::vector<std::uint8_t>(bytes, bytes + cardano_ipv6_get_bytes_size(value));
}

std::string
Ipv6::toString() const
{
  return cardano_ipv6_get_string(checked());
}

std::vector<std::uint8_t>
Ipv6::toCbor() const
{
  return encodeCbor(checked(), cardano_ipv6_to_cbor);
}

void
Ipv6::swap(Ipv6& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Ipv6&
Ipv6::operator=(const Ipv6& other)
{
  if (this != &other)
  {
    Ipv6 copy(other);

    swap(copy);
  }

  return *this;
}

Ipv6&
Ipv6::operator=(Ipv6&& other) noexcept
{
  if (this != &other)
  {
    cardano_ipv6_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
Ipv6::operator==(const Ipv6& other) const
{
  return toBytes() == other.toBytes();
}

bool
Ipv6::operator!=(const Ipv6& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

Ipv6::Ipv6(cardano_ipv6_t& value) noexcept
: m_value(&value)
{
}

cardano_ipv6_t*
Ipv6::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Ipv6");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Ipv6& lhs, Ipv6& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
