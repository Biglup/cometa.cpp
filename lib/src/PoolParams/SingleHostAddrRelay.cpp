/**
 * @file SingleHostAddrRelay.cpp
 *
 * @brief A pool relay with optional IP addresses and port.
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

#include <Cometa/PoolParams/SingleHostAddrRelay.h>

#include "../CardanoInterop.h"

#include <cardano/pool_params/single_host_addr_relay.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
SingleHostAddrRelay::SingleHostAddrRelay(std::optional<std::uint16_t> port, const std::optional<Ipv4>& ipv4, const std::optional<Ipv6>& ipv6)
: m_value(nullptr)
{
  const std::optional<Ipv4> v4(ipv4);
  const std::optional<Ipv6> v6(ipv6);

  cardano_single_host_addr_relay_t* value  = nullptr;
  const cardano_error_t             result = cardano_single_host_addr_relay_new(port ? &*port : nullptr, v4 ? v4->checked() : nullptr, v6 ? v6->checked() : nullptr, &value);

  CardanoObject<cardano_single_host_addr_relay_t, cardano_single_host_addr_relay_unref> owner(value);

  checkCardanoError(result, "SingleHostAddrRelay");

  m_value = owner.release();
}

SingleHostAddrRelay::SingleHostAddrRelay(const SingleHostAddrRelay& other)
: m_value(nullptr)
{
  cardano_single_host_addr_relay_t* value  = nullptr;
  const cardano_error_t             result = cardano_single_host_addr_relay_deep_clone(other.checked(), &value);

  CardanoObject<cardano_single_host_addr_relay_t, cardano_single_host_addr_relay_unref> owner(value);

  checkCardanoError(result, "SingleHostAddrRelay");

  m_value = owner.release();
}

SingleHostAddrRelay::SingleHostAddrRelay(SingleHostAddrRelay&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

SingleHostAddrRelay::~SingleHostAddrRelay()
{
  cardano_single_host_addr_relay_unref(&m_value);
}

SingleHostAddrRelay
SingleHostAddrRelay::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  SingleHostAddrRelay value(*decodeCbor<cardano_single_host_addr_relay_t, cardano_single_host_addr_relay_unref>(cbor, cardano_single_host_addr_relay_from_cbor));

  return value;
}

SingleHostAddrRelay
SingleHostAddrRelay::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::optional<std::uint16_t>
SingleHostAddrRelay::port() const
{
  const std::uint16_t* value = cardano_single_host_addr_relay_get_port(checked());

  return value ? std::optional<std::uint16_t>(*value) : std::nullopt;
}

void
SingleHostAddrRelay::setPort(std::optional<std::uint16_t> port)
{
  checkCardanoError(cardano_single_host_addr_relay_set_port(checked(), port ? &*port : nullptr), "SingleHostAddrRelay setPort");
}

std::optional<Ipv4>
SingleHostAddrRelay::ipv4() const
{
  cardano_ipv4_t*       value  = nullptr;
  const cardano_error_t result = cardano_single_host_addr_relay_get_ipv4(checked(), &value);

  CardanoObject<cardano_ipv4_t, cardano_ipv4_unref> owner(value);

  checkCardanoError(result, "SingleHostAddrRelay ipv4");

  if (!value)
  {
    return std::nullopt;
  }

  const Ipv4 address(*owner.release());

  return Ipv4(address);
}

void
SingleHostAddrRelay::setIpv4(const std::optional<Ipv4>& address)
{
  SingleHostAddrRelay replacement(port(), address, ipv6());

  swap(replacement);
}

std::optional<Ipv6>
SingleHostAddrRelay::ipv6() const
{
  cardano_ipv6_t*       value  = nullptr;
  const cardano_error_t result = cardano_single_host_addr_relay_get_ipv6(checked(), &value);

  CardanoObject<cardano_ipv6_t, cardano_ipv6_unref> owner(value);

  checkCardanoError(result, "SingleHostAddrRelay ipv6");

  if (!value)
  {
    return std::nullopt;
  }

  const Ipv6 address(*owner.release());

  return Ipv6(address);
}

void
SingleHostAddrRelay::setIpv6(const std::optional<Ipv6>& address)
{
  SingleHostAddrRelay replacement(port(), ipv4(), address);

  swap(replacement);
}

std::vector<std::uint8_t>
SingleHostAddrRelay::toCbor() const
{
  return encodeCbor(checked(), cardano_single_host_addr_relay_to_cbor);
}

std::string
SingleHostAddrRelay::toJson() const
{
  return encodeJson(checked(), cardano_single_host_addr_relay_to_cip116_json);
}

void
SingleHostAddrRelay::swap(SingleHostAddrRelay& other) noexcept
{
  std::swap(m_value, other.m_value);
}

SingleHostAddrRelay&
SingleHostAddrRelay::operator=(const SingleHostAddrRelay& other)
{
  if (this != &other)
  {
    SingleHostAddrRelay copy(other);

    swap(copy);
  }

  return *this;
}

SingleHostAddrRelay&
SingleHostAddrRelay::operator=(SingleHostAddrRelay&& other) noexcept
{
  if (this != &other)
  {
    cardano_single_host_addr_relay_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
SingleHostAddrRelay::operator==(const SingleHostAddrRelay& other) const
{
  return toCbor() == other.toCbor();
}

bool
SingleHostAddrRelay::operator!=(const SingleHostAddrRelay& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

SingleHostAddrRelay::SingleHostAddrRelay(cardano_single_host_addr_relay_t& value) noexcept
: m_value(&value)
{
}

cardano_single_host_addr_relay_t*
SingleHostAddrRelay::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from SingleHostAddrRelay");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(SingleHostAddrRelay& lhs, SingleHostAddrRelay& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
