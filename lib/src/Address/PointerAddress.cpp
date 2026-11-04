/**
 * @file PointerAddress.cpp
 *
 * @brief PointerAddress operations.
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

#include <Cometa/Address/PointerAddress.h>

#include <Cometa/Address/Address.h>
#include <Cometa/Encoding/Base58.h>
#include <Cometa/Encoding/Bech32.h>

#include "../CardanoInterop.h"

#include <cardano/address/pointer_address.h>

#include <algorithm>
#include <cstring>
#include <limits>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PointerAddress::PointerAddress(NetworkId network, const Credential& payment, const StakePointer& pointer)
: m_value(nullptr)
{
  if (static_cast<std::uint8_t>(network) > 15)
  {
    throw std::invalid_argument("The network ID must fit in four bits");
  }

  const Credential paymentCopy(payment);

  if (pointer.slot > std::numeric_limits<std::uint32_t>::max() || pointer.transactionIndex > std::numeric_limits<std::uint32_t>::max() || pointer.certificateIndex > std::numeric_limits<std::uint32_t>::max())
  {
    throw std::invalid_argument("Pointer address fields must fit in 32 bits");
  }

  const cardano_stake_pointer_t nativePointer { pointer.slot, pointer.transactionIndex, pointer.certificateIndex };

  cardano_pointer_address_t* value  = nullptr;
  const cardano_error_t      result = cardano_pointer_address_from_credentials(static_cast<cardano_network_id_t>(network), checkedNativeHandle(paymentCopy), nativePointer, &value);

  CardanoObject<cardano_pointer_address_t, cardano_pointer_address_unref> owner(value);

  checkCardanoError(result, "PointerAddress");

  m_value = owner.release();
}

PointerAddress::PointerAddress(const std::vector<std::uint8_t>& bytes)
: PointerAddress(fromAddress(Address(bytes)))
{
}

PointerAddress::PointerAddress(const PointerAddress& other)
: m_value(nullptr)
{
  cardano_pointer_address_t* value  = nullptr;
  const cardano_error_t      result = cardano_pointer_address_deep_clone(other.checked(), &value);

  CardanoObject<cardano_pointer_address_t, cardano_pointer_address_unref> owner(value);

  checkCardanoError(result, "PointerAddress copy");

  m_value = owner.release();
}

PointerAddress::PointerAddress(PointerAddress&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PointerAddress::~PointerAddress()
{
  cardano_pointer_address_unref(&m_value);
}

PointerAddress
PointerAddress::fromBech32(std::string_view text)
{
  return fromAddress(Address::fromString(text));
}

PointerAddress
PointerAddress::fromAddress(const Address& address)
{
  cardano_pointer_address_t* value  = nullptr;
  const cardano_error_t      result = cardano_pointer_address_from_address(checkedNativeHandle(address), &value);

  CardanoObject<cardano_pointer_address_t, cardano_pointer_address_unref> owner(value);

  checkCardanoError(result, "PointerAddress fromAddress");

  return PointerAddress(*owner.release());
}

std::vector<std::uint8_t>
PointerAddress::toBytes() const
{
  std::vector<std::uint8_t> bytes(cardano_pointer_address_get_bytes_size(checked()));

  checkCardanoError(cardano_pointer_address_to_bytes(checked(), bytes.data(), bytes.size()), "PointerAddress toBytes");

  return bytes;
}

std::string
PointerAddress::toString() const
{
  return cardano_pointer_address_get_string(checked());
}

NetworkId
PointerAddress::networkId() const
{
  cardano_network_id_t network;

  checkCardanoError(cardano_pointer_address_get_network_id(checked(), &network), "PointerAddress networkId");

  return static_cast<NetworkId>(network);
}

Address
PointerAddress::toAddress() const
{
  return Address(toBytes());
}

Credential
PointerAddress::paymentCredential() const
{
  CardanoObject<cardano_credential_t, cardano_credential_unref> borrowed(cardano_pointer_address_get_payment_credential(checked()));
  cardano_credential_t*                                         copy   = nullptr;
  const cardano_error_t                                         result = cardano_credential_deep_clone(borrowed.get(), &copy);

  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(copy);

  checkCardanoError(result, "PointerAddress paymentCredential");

  return Credential::adopt(owner.release());
}

StakePointer
PointerAddress::stakePointer() const
{
  cardano_stake_pointer_t value {};

  checkCardanoError(cardano_pointer_address_get_stake_pointer(checked(), &value), "PointerAddress stakePointer");

  return { value.slot, value.tx_index, value.cert_index };
}

void
PointerAddress::swap(PointerAddress& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PointerAddress&
PointerAddress::operator=(const PointerAddress& other)
{
  if (this != &other)
  {
    PointerAddress copy(other);

    swap(copy);
  }

  return *this;
}

PointerAddress&
PointerAddress::operator=(PointerAddress&& other) noexcept
{
  if (this != &other)
  {
    cardano_pointer_address_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
PointerAddress::operator==(const PointerAddress& other) const
{
  const std::size_t size = cardano_pointer_address_get_bytes_size(checked());

  return size == cardano_pointer_address_get_bytes_size(other.checked()) && std::memcmp(cardano_pointer_address_get_bytes(checked()), cardano_pointer_address_get_bytes(other.checked()), size) == 0;
}

bool
PointerAddress::operator!=(const PointerAddress& other) const
{
  return !(*this == other);
}

PointerAddress
PointerAddress::adopt(cardano_pointer_address_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null PointerAddress");
  }

  return PointerAddress(*value);
}

cardano_pointer_address_t*
PointerAddress::nativeHandle() noexcept
{
  return m_value;
}

const cardano_pointer_address_t*
PointerAddress::nativeHandle() const noexcept
{
  return m_value;
}

cardano_pointer_address_t*
PointerAddress::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

PointerAddress::PointerAddress(cardano_pointer_address_t& value) noexcept
: m_value(&value)
{
}

cardano_pointer_address_t*
PointerAddress::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PointerAddress");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PointerAddress& lhs, PointerAddress& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
