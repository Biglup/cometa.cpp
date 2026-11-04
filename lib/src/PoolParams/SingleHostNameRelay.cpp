/**
 * @file SingleHostNameRelay.cpp
 *
 * @brief A pool relay identified by a DNS name and optional port.
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

#include <Cometa/PoolParams/SingleHostNameRelay.h>

#include "../CardanoInterop.h"

#include <cardano/pool_params/single_host_name_relay.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
SingleHostNameRelay::SingleHostNameRelay(std::string_view dns, std::optional<std::uint16_t> port)
: m_value(nullptr)
{
  validateDns(dns);

  cardano_single_host_name_relay_t* value  = nullptr;
  const cardano_error_t             result = cardano_single_host_name_relay_new(port ? &*port : nullptr, dns.data(), dns.size(), &value);

  CardanoObject<cardano_single_host_name_relay_t, cardano_single_host_name_relay_unref> owner(value);

  checkCardanoError(result, "SingleHostNameRelay");

  m_value = owner.release();
}

SingleHostNameRelay::SingleHostNameRelay(const SingleHostNameRelay& other)
: m_value(nullptr)
{
  cardano_single_host_name_relay_t* value  = nullptr;
  const cardano_error_t             result = cardano_single_host_name_relay_deep_clone(other.checked(), &value);

  CardanoObject<cardano_single_host_name_relay_t, cardano_single_host_name_relay_unref> owner(value);

  checkCardanoError(result, "SingleHostNameRelay");

  m_value = owner.release();
}

SingleHostNameRelay::SingleHostNameRelay(SingleHostNameRelay&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

SingleHostNameRelay::~SingleHostNameRelay()
{
  cardano_single_host_name_relay_unref(&m_value);
}

SingleHostNameRelay
SingleHostNameRelay::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  SingleHostNameRelay value(*decodeCbor<cardano_single_host_name_relay_t, cardano_single_host_name_relay_unref>(cbor, cardano_single_host_name_relay_from_cbor));

  return value;
}

SingleHostNameRelay
SingleHostNameRelay::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::string
SingleHostNameRelay::dns() const
{
  const cardano_single_host_name_relay_t* value = checked();

  return std::string(cardano_single_host_name_relay_get_dns(value), cardano_single_host_name_relay_get_dns_size(value) - 1U);
}

void
SingleHostNameRelay::setDns(std::string_view dns)
{
  validateDns(dns);

  checkCardanoError(cardano_single_host_name_relay_set_dns(dns.data(), dns.size(), checked()), "SingleHostNameRelay setDns");
}

std::optional<std::uint16_t>
SingleHostNameRelay::port() const
{
  const std::uint16_t* value = cardano_single_host_name_relay_get_port(checked());

  return value ? std::optional<std::uint16_t>(*value) : std::nullopt;
}

void
SingleHostNameRelay::setPort(std::optional<std::uint16_t> port)
{
  checkCardanoError(cardano_single_host_name_relay_set_port(checked(), port ? &*port : nullptr), "SingleHostNameRelay setPort");
}

std::vector<std::uint8_t>
SingleHostNameRelay::toCbor() const
{
  return encodeCbor(checked(), cardano_single_host_name_relay_to_cbor);
}

std::string
SingleHostNameRelay::toJson() const
{
  return encodeJson(checked(), cardano_single_host_name_relay_to_cip116_json);
}

void
SingleHostNameRelay::swap(SingleHostNameRelay& other) noexcept
{
  std::swap(m_value, other.m_value);
}

SingleHostNameRelay&
SingleHostNameRelay::operator=(const SingleHostNameRelay& other)
{
  if (this != &other)
  {
    SingleHostNameRelay copy(other);

    swap(copy);
  }

  return *this;
}

SingleHostNameRelay&
SingleHostNameRelay::operator=(SingleHostNameRelay&& other) noexcept
{
  if (this != &other)
  {
    cardano_single_host_name_relay_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
SingleHostNameRelay::operator==(const SingleHostNameRelay& other) const
{
  return toCbor() == other.toCbor();
}

bool
SingleHostNameRelay::operator!=(const SingleHostNameRelay& other) const
{
  return !(*this == other);
}

SingleHostNameRelay
SingleHostNameRelay::adopt(cardano_single_host_name_relay_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null SingleHostNameRelay");
  }

  return SingleHostNameRelay(*value);
}

cardano_single_host_name_relay_t*
SingleHostNameRelay::nativeHandle() noexcept
{
  return m_value;
}

const cardano_single_host_name_relay_t*
SingleHostNameRelay::nativeHandle() const noexcept
{
  return m_value;
}

cardano_single_host_name_relay_t*
SingleHostNameRelay::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

SingleHostNameRelay::SingleHostNameRelay(cardano_single_host_name_relay_t& value) noexcept
: m_value(&value)
{
}

cardano_single_host_name_relay_t*
SingleHostNameRelay::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from SingleHostNameRelay");
  }

  return m_value;
}

void
SingleHostNameRelay::validateDns(std::string_view dns)
{
  if (dns.empty() || dns.size() > 64U || dns.find('\0') != std::string_view::npos)
  {
    throw std::invalid_argument("Relay DNS name must contain 1 to 64 bytes without embedded NULs");
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(SingleHostNameRelay& lhs, SingleHostNameRelay& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
