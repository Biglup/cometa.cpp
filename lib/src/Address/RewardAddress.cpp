/**
 * @file RewardAddress.cpp
 *
 * @brief RewardAddress operations.
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

#include <Cometa/Address/RewardAddress.h>

#include <Cometa/Address/Address.h>
#include <Cometa/Encoding/Base58.h>
#include <Cometa/Encoding/Bech32.h>

#include "../CardanoInterop.h"

#include <cardano/address/reward_address.h>

#include <algorithm>
#include <cstring>
#include <limits>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
RewardAddress::RewardAddress(NetworkId network, const Credential& credential)
: m_value(nullptr)
{
  if (static_cast<std::uint8_t>(network) > 15)
  {
    throw std::invalid_argument("The network ID must fit in four bits");
  }

  const Credential credentialCopy(credential);

  cardano_reward_address_t* value  = nullptr;
  const cardano_error_t     result = cardano_reward_address_from_credentials(static_cast<cardano_network_id_t>(network), checkedNativeHandle(credentialCopy), &value);

  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> owner(value);

  checkCardanoError(result, "RewardAddress");

  m_value = owner.release();
}

RewardAddress::RewardAddress(const std::vector<std::uint8_t>& bytes)
: RewardAddress(fromAddress(Address(bytes)))
{
}

RewardAddress::RewardAddress(const RewardAddress& other)
: m_value(nullptr)
{
  cardano_reward_address_t* value  = nullptr;
  const cardano_error_t     result = cardano_reward_address_deep_clone(other.checked(), &value);

  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> owner(value);

  checkCardanoError(result, "RewardAddress copy");

  m_value = owner.release();
}

RewardAddress::RewardAddress(RewardAddress&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

RewardAddress::~RewardAddress()
{
  cardano_reward_address_unref(&m_value);
}

RewardAddress
RewardAddress::fromBech32(std::string_view text)
{
  return fromAddress(Address::fromString(text));
}

RewardAddress
RewardAddress::fromAddress(const Address& address)
{
  cardano_reward_address_t* value  = nullptr;
  const cardano_error_t     result = cardano_reward_address_from_address(checkedNativeHandle(address), &value);

  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> owner(value);

  checkCardanoError(result, "RewardAddress fromAddress");

  return RewardAddress(*owner.release());
}

std::vector<std::uint8_t>
RewardAddress::toBytes() const
{
  std::vector<std::uint8_t> bytes(cardano_reward_address_get_bytes_size(checked()));

  checkCardanoError(cardano_reward_address_to_bytes(checked(), bytes.data(), bytes.size()), "RewardAddress toBytes");

  return bytes;
}

std::string
RewardAddress::toString() const
{
  return cardano_reward_address_get_string(checked());
}

NetworkId
RewardAddress::networkId() const
{
  cardano_network_id_t network;

  checkCardanoError(cardano_reward_address_get_network_id(checked(), &network), "RewardAddress networkId");

  return static_cast<NetworkId>(network);
}

Address
RewardAddress::toAddress() const
{
  return Address(toBytes());
}

Credential
RewardAddress::credential() const
{
  CardanoObject<cardano_credential_t, cardano_credential_unref> borrowed(cardano_reward_address_get_credential(checked()));
  cardano_credential_t*                                         copy   = nullptr;
  const cardano_error_t                                         result = cardano_credential_deep_clone(borrowed.get(), &copy);

  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(copy);

  checkCardanoError(result, "RewardAddress credential");

  return Credential::adopt(owner.release());
}

void
RewardAddress::swap(RewardAddress& other) noexcept
{
  std::swap(m_value, other.m_value);
}

RewardAddress&
RewardAddress::operator=(const RewardAddress& other)
{
  if (this != &other)
  {
    RewardAddress copy(other);

    swap(copy);
  }

  return *this;
}

RewardAddress&
RewardAddress::operator=(RewardAddress&& other) noexcept
{
  if (this != &other)
  {
    cardano_reward_address_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
RewardAddress::operator==(const RewardAddress& other) const
{
  const std::size_t size = cardano_reward_address_get_bytes_size(checked());

  return size == cardano_reward_address_get_bytes_size(other.checked()) && std::memcmp(cardano_reward_address_get_bytes(checked()), cardano_reward_address_get_bytes(other.checked()), size) == 0;
}

bool
RewardAddress::operator!=(const RewardAddress& other) const
{
  return !(*this == other);
}

RewardAddress
RewardAddress::adopt(cardano_reward_address_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null RewardAddress");
  }

  return RewardAddress(*value);
}

cardano_reward_address_t*
RewardAddress::nativeHandle() noexcept
{
  return m_value;
}

const cardano_reward_address_t*
RewardAddress::nativeHandle() const noexcept
{
  return m_value;
}

cardano_reward_address_t*
RewardAddress::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

RewardAddress::RewardAddress(cardano_reward_address_t& value) noexcept
: m_value(&value)
{
}

cardano_reward_address_t*
RewardAddress::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from RewardAddress");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(RewardAddress& lhs, RewardAddress& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
