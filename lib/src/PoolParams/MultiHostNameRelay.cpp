/**
 * @file MultiHostNameRelay.cpp
 *
 * @brief A pool relay resolved through a DNS SRV name.
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

#include <Cometa/PoolParams/MultiHostNameRelay.h>

#include "../CardanoInterop.h"

#include <cardano/pool_params/multi_host_name_relay.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
MultiHostNameRelay::MultiHostNameRelay(std::string_view dns)
: m_value(nullptr)
{
  validateDns(dns);

  cardano_multi_host_name_relay_t* value  = nullptr;
  const cardano_error_t            result = cardano_multi_host_name_relay_new(dns.data(), dns.size(), &value);

  CardanoObject<cardano_multi_host_name_relay_t, cardano_multi_host_name_relay_unref> owner(value);

  checkCardanoError(result, "MultiHostNameRelay");

  m_value = owner.release();
}

MultiHostNameRelay::MultiHostNameRelay(const MultiHostNameRelay& other)
: m_value(nullptr)
{
  cardano_multi_host_name_relay_t* value  = nullptr;
  const cardano_error_t            result = cardano_multi_host_name_relay_deep_clone(other.checked(), &value);

  CardanoObject<cardano_multi_host_name_relay_t, cardano_multi_host_name_relay_unref> owner(value);

  checkCardanoError(result, "MultiHostNameRelay");

  m_value = owner.release();
}

MultiHostNameRelay::MultiHostNameRelay(MultiHostNameRelay&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

MultiHostNameRelay::~MultiHostNameRelay()
{
  cardano_multi_host_name_relay_unref(&m_value);
}

MultiHostNameRelay
MultiHostNameRelay::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  MultiHostNameRelay value(*decodeCbor<cardano_multi_host_name_relay_t, cardano_multi_host_name_relay_unref>(cbor, cardano_multi_host_name_relay_from_cbor));

  return value;
}

MultiHostNameRelay
MultiHostNameRelay::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::string
MultiHostNameRelay::dns() const
{
  const cardano_multi_host_name_relay_t* value = checked();

  return std::string(cardano_multi_host_name_relay_get_dns(value), cardano_multi_host_name_relay_get_dns_size(value) - 1U);
}

void
MultiHostNameRelay::setDns(std::string_view dns)
{
  validateDns(dns);

  checkCardanoError(cardano_multi_host_name_relay_set_dns(dns.data(), dns.size(), checked()), "MultiHostNameRelay setDns");
}

std::vector<std::uint8_t>
MultiHostNameRelay::toCbor() const
{
  return encodeCbor(checked(), cardano_multi_host_name_relay_to_cbor);
}

std::string
MultiHostNameRelay::toJson() const
{
  return encodeJson(checked(), cardano_multi_host_name_relay_to_cip116_json);
}

void
MultiHostNameRelay::swap(MultiHostNameRelay& other) noexcept
{
  std::swap(m_value, other.m_value);
}

MultiHostNameRelay&
MultiHostNameRelay::operator=(const MultiHostNameRelay& other)
{
  if (this != &other)
  {
    MultiHostNameRelay copy(other);

    swap(copy);
  }

  return *this;
}

MultiHostNameRelay&
MultiHostNameRelay::operator=(MultiHostNameRelay&& other) noexcept
{
  if (this != &other)
  {
    cardano_multi_host_name_relay_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
MultiHostNameRelay::operator==(const MultiHostNameRelay& other) const
{
  return toCbor() == other.toCbor();
}

bool
MultiHostNameRelay::operator!=(const MultiHostNameRelay& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

MultiHostNameRelay::MultiHostNameRelay(cardano_multi_host_name_relay_t& value) noexcept
: m_value(&value)
{
}

cardano_multi_host_name_relay_t*
MultiHostNameRelay::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from MultiHostNameRelay");
  }

  return m_value;
}

void
MultiHostNameRelay::validateDns(std::string_view dns)
{
  if (dns.empty() || dns.size() > 64U || dns.find('\0') != std::string_view::npos)
  {
    throw std::invalid_argument("Relay DNS name must contain 1 to 64 bytes without embedded NULs");
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(MultiHostNameRelay& lhs, MultiHostNameRelay& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
