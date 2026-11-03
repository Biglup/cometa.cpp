/**
 * @file Relay.cpp
 *
 * @brief A pool relay address or DNS-name variant.
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

#include <Cometa/PoolParams/Relay.h>

#include "../CardanoInterop.h"

#include <cardano/pool_params/multi_host_name_relay.h>
#include <cardano/pool_params/relay.h>
#include <cardano/pool_params/single_host_addr_relay.h>
#include <cardano/pool_params/single_host_name_relay.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Relay::Relay(const SingleHostAddrRelay& relay)
: m_value(nullptr)
{
  SingleHostAddrRelay copy(relay);

  cardano_relay_t*      value  = nullptr;
  const cardano_error_t result = cardano_relay_new_single_host_addr(copy.checked(), &value);

  CardanoObject<cardano_relay_t, cardano_relay_unref> owner(value);

  checkCardanoError(result, "Relay");

  m_value = owner.release();
}

Relay::Relay(SingleHostAddrRelay&& relay)
: m_value(nullptr)
{
  cardano_relay_t*      value  = nullptr;
  const cardano_error_t result = cardano_relay_new_single_host_addr(relay.checked(), &value);

  CardanoObject<cardano_relay_t, cardano_relay_unref> owner(value);

  checkCardanoError(result, "Relay");

  m_value = owner.release();

  SingleHostAddrRelay consumed(std::move(relay));
}

Relay::Relay(const SingleHostNameRelay& relay)
: m_value(nullptr)
{
  SingleHostNameRelay copy(relay);

  cardano_relay_t*      value  = nullptr;
  const cardano_error_t result = cardano_relay_new_single_host_name(copy.checked(), &value);

  CardanoObject<cardano_relay_t, cardano_relay_unref> owner(value);

  checkCardanoError(result, "Relay");

  m_value = owner.release();
}

Relay::Relay(SingleHostNameRelay&& relay)
: m_value(nullptr)
{
  cardano_relay_t*      value  = nullptr;
  const cardano_error_t result = cardano_relay_new_single_host_name(relay.checked(), &value);

  CardanoObject<cardano_relay_t, cardano_relay_unref> owner(value);

  checkCardanoError(result, "Relay");

  m_value = owner.release();

  SingleHostNameRelay consumed(std::move(relay));
}

Relay::Relay(const MultiHostNameRelay& relay)
: m_value(nullptr)
{
  MultiHostNameRelay copy(relay);

  cardano_relay_t*      value  = nullptr;
  const cardano_error_t result = cardano_relay_new_multi_host_name(copy.checked(), &value);

  CardanoObject<cardano_relay_t, cardano_relay_unref> owner(value);

  checkCardanoError(result, "Relay");

  m_value = owner.release();
}

Relay::Relay(MultiHostNameRelay&& relay)
: m_value(nullptr)
{
  cardano_relay_t*      value  = nullptr;
  const cardano_error_t result = cardano_relay_new_multi_host_name(relay.checked(), &value);

  CardanoObject<cardano_relay_t, cardano_relay_unref> owner(value);

  checkCardanoError(result, "Relay");

  m_value = owner.release();

  MultiHostNameRelay consumed(std::move(relay));
}

Relay::Relay(const Relay& other)
: m_value(nullptr)
{
  cardano_relay_t*      value  = nullptr;
  const cardano_error_t result = cardano_relay_deep_clone(other.checked(), &value);

  CardanoObject<cardano_relay_t, cardano_relay_unref> owner(value);

  checkCardanoError(result, "Relay");

  m_value = owner.release();
}

Relay::Relay(Relay&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Relay::~Relay()
{
  cardano_relay_unref(&m_value);
}

Relay
Relay::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  Relay value(*decodeCbor<cardano_relay_t, cardano_relay_unref>(cbor, cardano_relay_from_cbor));

  return value;
}

Relay
Relay::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

RelayType
Relay::type() const
{
  cardano_relay_type_t value = CARDANO_RELAY_TYPE_SINGLE_HOST_ADDRESS;

  checkCardanoError(cardano_relay_get_type(checked(), &value), "Relay type");

  return static_cast<RelayType>(value);
}

SingleHostAddrRelay
Relay::asSingleHostAddr() const
{
  cardano_single_host_addr_relay_t* value  = nullptr;
  const cardano_error_t             result = cardano_relay_to_single_host_addr(checked(), &value);

  CardanoObject<cardano_single_host_addr_relay_t, cardano_single_host_addr_relay_unref> owner(value);

  checkCardanoError(result, "Relay asSingleHostAddr");

  const SingleHostAddrRelay relay(*owner.release());

  return SingleHostAddrRelay(relay);
}

SingleHostNameRelay
Relay::asSingleHostName() const
{
  cardano_single_host_name_relay_t* value  = nullptr;
  const cardano_error_t             result = cardano_relay_to_single_host_name(checked(), &value);

  CardanoObject<cardano_single_host_name_relay_t, cardano_single_host_name_relay_unref> owner(value);

  checkCardanoError(result, "Relay asSingleHostName");

  const SingleHostNameRelay relay(*owner.release());

  return SingleHostNameRelay(relay);
}

MultiHostNameRelay
Relay::asMultiHostName() const
{
  cardano_multi_host_name_relay_t* value  = nullptr;
  const cardano_error_t            result = cardano_relay_to_multi_host_name(checked(), &value);

  CardanoObject<cardano_multi_host_name_relay_t, cardano_multi_host_name_relay_unref> owner(value);

  checkCardanoError(result, "Relay asMultiHostName");

  const MultiHostNameRelay relay(*owner.release());

  return MultiHostNameRelay(relay);
}

std::vector<std::uint8_t>
Relay::toCbor() const
{
  return encodeCbor(checked(), cardano_relay_to_cbor);
}

std::string
Relay::toJson() const
{
  return encodeJson(checked(), cardano_relay_to_cip116_json);
}

void
Relay::swap(Relay& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Relay&
Relay::operator=(const Relay& other)
{
  if (this != &other)
  {
    Relay copy(other);

    swap(copy);
  }

  return *this;
}

Relay&
Relay::operator=(Relay&& other) noexcept
{
  if (this != &other)
  {
    cardano_relay_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
Relay::operator==(const Relay& other) const
{
  return toCbor() == other.toCbor();
}

bool
Relay::operator!=(const Relay& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

Relay::Relay(cardano_relay_t& value) noexcept
: m_value(&value)
{
}

cardano_relay_t*
Relay::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Relay");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Relay& lhs, Relay& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
