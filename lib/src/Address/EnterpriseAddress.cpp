/**
 * @file EnterpriseAddress.cpp
 *
 * @brief EnterpriseAddress operations.
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

#include <Cometa/Address/EnterpriseAddress.h>

#include <Cometa/Address/Address.h>
#include <Cometa/Encoding/Base58.h>
#include <Cometa/Encoding/Bech32.h>

#include "../CardanoInterop.h"

#include <cardano/address/enterprise_address.h>

#include <algorithm>
#include <cstring>
#include <limits>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
EnterpriseAddress::EnterpriseAddress(NetworkId network, const Credential& payment)
: m_value(nullptr)
{
  if (static_cast<std::uint8_t>(network) > 15)
  {
    throw std::invalid_argument("The network ID must fit in four bits");
  }

  const Credential paymentCopy(payment);

  cardano_enterprise_address_t* value  = nullptr;
  const cardano_error_t         result = cardano_enterprise_address_from_credentials(static_cast<cardano_network_id_t>(network), paymentCopy.checked(), &value);

  CardanoObject<cardano_enterprise_address_t, cardano_enterprise_address_unref> owner(value);

  checkCardanoError(result, "EnterpriseAddress");

  m_value = owner.release();
}

EnterpriseAddress::EnterpriseAddress(const std::vector<std::uint8_t>& bytes)
: EnterpriseAddress(fromAddress(Address(bytes)))
{
}

EnterpriseAddress::EnterpriseAddress(const EnterpriseAddress& other)
: m_value(nullptr)
{
  cardano_enterprise_address_t* value  = nullptr;
  const cardano_error_t         result = cardano_enterprise_address_deep_clone(other.checked(), &value);

  CardanoObject<cardano_enterprise_address_t, cardano_enterprise_address_unref> owner(value);

  checkCardanoError(result, "EnterpriseAddress copy");

  m_value = owner.release();
}

EnterpriseAddress::EnterpriseAddress(EnterpriseAddress&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

EnterpriseAddress::~EnterpriseAddress()
{
  cardano_enterprise_address_unref(&m_value);
}

EnterpriseAddress
EnterpriseAddress::fromBech32(std::string_view text)
{
  return fromAddress(Address::fromString(text));
}

EnterpriseAddress
EnterpriseAddress::fromAddress(const Address& address)
{
  cardano_enterprise_address_t* value  = nullptr;
  const cardano_error_t         result = cardano_enterprise_address_from_address(address.checked(), &value);

  CardanoObject<cardano_enterprise_address_t, cardano_enterprise_address_unref> owner(value);

  checkCardanoError(result, "EnterpriseAddress fromAddress");

  return EnterpriseAddress(*owner.release());
}

std::vector<std::uint8_t>
EnterpriseAddress::toBytes() const
{
  std::vector<std::uint8_t> bytes(cardano_enterprise_address_get_bytes_size(checked()));

  checkCardanoError(cardano_enterprise_address_to_bytes(checked(), bytes.data(), bytes.size()), "EnterpriseAddress toBytes");

  return bytes;
}

std::string
EnterpriseAddress::toString() const
{
  return cardano_enterprise_address_get_string(checked());
}

NetworkId
EnterpriseAddress::networkId() const
{
  cardano_network_id_t network;

  checkCardanoError(cardano_enterprise_address_get_network_id(checked(), &network), "EnterpriseAddress networkId");

  return static_cast<NetworkId>(network);
}

Address
EnterpriseAddress::toAddress() const
{
  return Address(toBytes());
}

Credential
EnterpriseAddress::paymentCredential() const
{
  CardanoObject<cardano_credential_t, cardano_credential_unref> borrowed(cardano_enterprise_address_get_payment_credential(checked()));
  cardano_credential_t*                                         copy   = nullptr;
  const cardano_error_t                                         result = cardano_credential_deep_clone(borrowed.get(), &copy);

  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(copy);

  checkCardanoError(result, "EnterpriseAddress paymentCredential");

  return Credential(owner.release());
}

void
EnterpriseAddress::swap(EnterpriseAddress& other) noexcept
{
  std::swap(m_value, other.m_value);
}

EnterpriseAddress&
EnterpriseAddress::operator=(const EnterpriseAddress& other)
{
  if (this != &other)
  {
    EnterpriseAddress copy(other);

    swap(copy);
  }

  return *this;
}

EnterpriseAddress&
EnterpriseAddress::operator=(EnterpriseAddress&& other) noexcept
{
  if (this != &other)
  {
    cardano_enterprise_address_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
EnterpriseAddress::operator==(const EnterpriseAddress& other) const
{
  const std::size_t size = cardano_enterprise_address_get_bytes_size(checked());

  return size == cardano_enterprise_address_get_bytes_size(other.checked()) && std::memcmp(cardano_enterprise_address_get_bytes(checked()), cardano_enterprise_address_get_bytes(other.checked()), size) == 0;
}

bool
EnterpriseAddress::operator!=(const EnterpriseAddress& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

EnterpriseAddress::EnterpriseAddress(cardano_enterprise_address_t& value) noexcept
: m_value(&value)
{
}

cardano_enterprise_address_t*
EnterpriseAddress::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from EnterpriseAddress");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(EnterpriseAddress& lhs, EnterpriseAddress& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
