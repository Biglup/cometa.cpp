/**
 * @file BaseAddress.cpp
 *
 * @brief BaseAddress operations.
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

#include <Cometa/Address/BaseAddress.h>

#include <Cometa/Address/Address.h>
#include <Cometa/Encoding/Base58.h>
#include <Cometa/Encoding/Bech32.h>

#include "../CardanoInterop.h"

#include <cardano/address/base_address.h>

#include <algorithm>
#include <cstring>
#include <limits>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
BaseAddress::BaseAddress(NetworkId network, const Credential& payment, const Credential& stake)
: m_value(nullptr)
{
  if (static_cast<std::uint8_t>(network) > 15)
  {
    throw std::invalid_argument("The network ID must fit in four bits");
  }

  const Credential paymentCopy(payment);
  const Credential stakeCopy(stake);

  cardano_base_address_t* value  = nullptr;
  const cardano_error_t   result = cardano_base_address_from_credentials(static_cast<cardano_network_id_t>(network), checkedNativeHandle(paymentCopy), checkedNativeHandle(stakeCopy), &value);

  CardanoObject<cardano_base_address_t, cardano_base_address_unref> owner(value);

  checkCardanoError(result, "BaseAddress");

  m_value = owner.release();
}

BaseAddress::BaseAddress(const std::vector<std::uint8_t>& bytes)
: BaseAddress(fromAddress(Address(bytes)))
{
}

BaseAddress::BaseAddress(const BaseAddress& other)
: m_value(nullptr)
{
  cardano_base_address_t* value  = nullptr;
  const cardano_error_t   result = cardano_base_address_deep_clone(other.checked(), &value);

  CardanoObject<cardano_base_address_t, cardano_base_address_unref> owner(value);

  checkCardanoError(result, "BaseAddress copy");

  m_value = owner.release();
}

BaseAddress::BaseAddress(BaseAddress&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

BaseAddress::~BaseAddress()
{
  cardano_base_address_unref(&m_value);
}

BaseAddress
BaseAddress::fromBech32(std::string_view text)
{
  return fromAddress(Address::fromString(text));
}

BaseAddress
BaseAddress::fromAddress(const Address& address)
{
  cardano_base_address_t* value  = nullptr;
  const cardano_error_t   result = cardano_base_address_from_address(checkedNativeHandle(address), &value);

  CardanoObject<cardano_base_address_t, cardano_base_address_unref> owner(value);

  checkCardanoError(result, "BaseAddress fromAddress");

  return BaseAddress(*owner.release());
}

std::vector<std::uint8_t>
BaseAddress::toBytes() const
{
  std::vector<std::uint8_t> bytes(cardano_base_address_get_bytes_size(checked()));

  checkCardanoError(cardano_base_address_to_bytes(checked(), bytes.data(), bytes.size()), "BaseAddress toBytes");

  return bytes;
}

std::string
BaseAddress::toString() const
{
  return cardano_base_address_get_string(checked());
}

NetworkId
BaseAddress::networkId() const
{
  cardano_network_id_t network;

  checkCardanoError(cardano_base_address_get_network_id(checked(), &network), "BaseAddress networkId");

  return static_cast<NetworkId>(network);
}

Address
BaseAddress::toAddress() const
{
  return Address(toBytes());
}

Credential
BaseAddress::paymentCredential() const
{
  CardanoObject<cardano_credential_t, cardano_credential_unref> borrowed(cardano_base_address_get_payment_credential(checked()));
  cardano_credential_t*                                         copy   = nullptr;
  const cardano_error_t                                         result = cardano_credential_deep_clone(borrowed.get(), &copy);

  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(copy);

  checkCardanoError(result, "BaseAddress paymentCredential");

  return Credential::adopt(owner.release());
}

Credential
BaseAddress::stakeCredential() const
{
  CardanoObject<cardano_credential_t, cardano_credential_unref> borrowed(cardano_base_address_get_stake_credential(checked()));
  cardano_credential_t*                                         copy   = nullptr;
  const cardano_error_t                                         result = cardano_credential_deep_clone(borrowed.get(), &copy);

  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(copy);

  checkCardanoError(result, "BaseAddress stakeCredential");

  return Credential::adopt(owner.release());
}

void
BaseAddress::swap(BaseAddress& other) noexcept
{
  std::swap(m_value, other.m_value);
}

BaseAddress&
BaseAddress::operator=(const BaseAddress& other)
{
  if (this != &other)
  {
    BaseAddress copy(other);

    swap(copy);
  }

  return *this;
}

BaseAddress&
BaseAddress::operator=(BaseAddress&& other) noexcept
{
  if (this != &other)
  {
    cardano_base_address_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
BaseAddress::operator==(const BaseAddress& other) const
{
  const std::size_t size = cardano_base_address_get_bytes_size(checked());

  return size == cardano_base_address_get_bytes_size(other.checked()) && std::memcmp(cardano_base_address_get_bytes(checked()), cardano_base_address_get_bytes(other.checked()), size) == 0;
}

bool
BaseAddress::operator!=(const BaseAddress& other) const
{
  return !(*this == other);
}

BaseAddress
BaseAddress::adopt(cardano_base_address_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null BaseAddress");
  }

  return BaseAddress(*value);
}

cardano_base_address_t*
BaseAddress::nativeHandle() noexcept
{
  return m_value;
}

const cardano_base_address_t*
BaseAddress::nativeHandle() const noexcept
{
  return m_value;
}

cardano_base_address_t*
BaseAddress::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

BaseAddress::BaseAddress(cardano_base_address_t& value) noexcept
: m_value(&value)
{
}

cardano_base_address_t*
BaseAddress::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from BaseAddress");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(BaseAddress& lhs, BaseAddress& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
