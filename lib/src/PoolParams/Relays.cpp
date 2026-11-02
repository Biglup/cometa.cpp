/**
 * @file Relays.cpp
 *
 * @brief An ordered sequence of pool relays.
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

#include <Cometa/PoolParams/Relays.h>

#include <Cometa/PoolParams/Relay.h>

#include "../CardanoInterop.h"

#include <cardano/pool_params/relay.h>
#include <cardano/pool_params/relays.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Relays::Relays()
: m_value(nullptr)
{
  cardano_relays_t*     value  = nullptr;
  const cardano_error_t result = cardano_relays_new(&value);

  CardanoObject<cardano_relays_t, cardano_relays_unref> owner(value);

  checkCardanoError(result, "Relays");

  m_value = owner.release();
}

Relays::Relays(std::initializer_list<Relay> relays)
: Relays()
{
  for (const Relay& relay: relays)
  {
    pushBack(relay);
  }
}

Relays::Relays(const Relays& other)
: m_value(nullptr)
{
  cardano_relays_t*     value  = nullptr;
  const cardano_error_t result = cardano_relays_deep_clone(other.checked(), &value);

  CardanoObject<cardano_relays_t, cardano_relays_unref> owner(value);

  checkCardanoError(result, "Relays");

  m_value = owner.release();
}

Relays::Relays(Relays&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Relays::~Relays()
{
  cardano_relays_unref(&m_value);
}

Relays
Relays::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  Relays value(*decodeCbor<cardano_relays_t, cardano_relays_unref>(cbor, cardano_relays_from_cbor));

  return value;
}

Relays
Relays::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
Relays::size() const
{
  return cardano_relays_get_length(checked());
}

bool
Relays::empty() const
{
  return size() == 0U;
}

Relay
Relays::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Relays index");
  }

  cardano_relay_t*      value  = nullptr;
  const cardano_error_t result = cardano_relays_get(checked(), index, &value);

  CardanoObject<cardano_relay_t, cardano_relay_unref> owner(value);

  checkCardanoError(result, "Relays at");

  const Relay relay(*owner.release());

  return Relay(relay);
}

void
Relays::pushBack(const Relay& relay)
{
  Relay copy(relay);

  checkCardanoError(cardano_relays_add(checked(), copy.checked()), "Relays pushBack");
}

std::vector<Relay>
Relays::values() const
{
  std::vector<Relay> result;
  const std::size_t  count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
Relays::toCbor() const
{
  return encodeCbor(checked(), cardano_relays_to_cbor);
}

std::string
Relays::toJson() const
{
  return encodeJson(checked(), cardano_relays_to_cip116_json);
}

void
Relays::swap(Relays& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Relays&
Relays::operator=(const Relays& other)
{
  if (this != &other)
  {
    Relays copy(other);

    swap(copy);
  }

  return *this;
}

Relays&
Relays::operator=(Relays&& other) noexcept
{
  if (this != &other)
  {
    cardano_relays_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
Relays::operator==(const Relays& other) const
{
  return values() == other.values();
}

bool
Relays::operator!=(const Relays& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

Relays::Relays(cardano_relays_t& value) noexcept
: m_value(&value)
{
}

cardano_relays_t*
Relays::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Relays");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Relays& lhs, Relays& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
