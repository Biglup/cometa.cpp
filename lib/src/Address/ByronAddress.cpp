/**
 * @file ByronAddress.cpp
 *
 * @brief ByronAddress operations.
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

#include <Cometa/Address/ByronAddress.h>

#include <Cometa/Address/Address.h>
#include <Cometa/Encoding/Base58.h>
#include <Cometa/Encoding/Bech32.h>

#include "../CardanoInterop.h"

#include <cardano/address/byron_address.h>

#include <algorithm>
#include <cstring>
#include <limits>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ByronAddress::ByronAddress(const Blake2bHash& root, const ByronAddressAttributes& attributes, ByronAddressType type)
: m_value(nullptr)
{
  if (root.toBytes().size() != 28 || attributes.derivationPath.size() > 64 || (attributes.magic && *attributes.magic < 0) || static_cast<std::uint8_t>(type) > 2)
  {
    throw std::invalid_argument("Invalid Byron root, attributes, or type");
  }

  cardano_byron_address_attributes_t nativeAttributes {};
  nativeAttributes.derivation_path_size = attributes.derivationPath.size();
  nativeAttributes.magic                = attributes.magic.value_or(-1);
  std::copy(attributes.derivationPath.begin(), attributes.derivationPath.end(), nativeAttributes.derivation_path);

  cardano_byron_address_t* value  = nullptr;
  const cardano_error_t    result = cardano_byron_address_from_credentials(checkedNativeHandle(root), nativeAttributes, static_cast<cardano_byron_address_type_t>(type), &value);

  CardanoObject<cardano_byron_address_t, cardano_byron_address_unref> owner(value);

  checkCardanoError(result, "ByronAddress");

  m_value = owner.release();
}

ByronAddress::ByronAddress(const std::vector<std::uint8_t>& bytes)
: ByronAddress(fromAddress(Address(bytes)))
{
}

ByronAddress::ByronAddress(const ByronAddress& other)
: m_value(nullptr)
{
  cardano_byron_address_t* value  = nullptr;
  const cardano_error_t    result = cardano_byron_address_deep_clone(other.checked(), &value);

  CardanoObject<cardano_byron_address_t, cardano_byron_address_unref> owner(value);

  checkCardanoError(result, "ByronAddress copy");

  m_value = owner.release();
}

ByronAddress::ByronAddress(ByronAddress&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ByronAddress::~ByronAddress()
{
  cardano_byron_address_unref(&m_value);
}

ByronAddress
ByronAddress::fromBase58(std::string_view text)
{
  return fromAddress(Address::fromString(text));
}

ByronAddress
ByronAddress::fromAddress(const Address& address)
{
  cardano_byron_address_t* value  = nullptr;
  const cardano_error_t    result = cardano_byron_address_from_address(checkedNativeHandle(address), &value);

  CardanoObject<cardano_byron_address_t, cardano_byron_address_unref> owner(value);

  checkCardanoError(result, "ByronAddress fromAddress");

  return ByronAddress(*owner.release());
}

std::vector<std::uint8_t>
ByronAddress::toBytes() const
{
  std::vector<std::uint8_t> bytes(cardano_byron_address_get_bytes_size(checked()));

  checkCardanoError(cardano_byron_address_to_bytes(checked(), bytes.data(), bytes.size()), "ByronAddress toBytes");

  return bytes;
}

std::string
ByronAddress::toString() const
{
  return cardano_byron_address_get_string(checked());
}

NetworkId
ByronAddress::networkId() const
{
  cardano_network_id_t network;

  checkCardanoError(cardano_byron_address_get_network_id(checked(), &network), "ByronAddress networkId");

  return static_cast<NetworkId>(network);
}

Address
ByronAddress::toAddress() const
{
  return Address(toBytes());
}

ByronAddressAttributes
ByronAddress::attributes() const
{
  cardano_byron_address_attributes_t value {};

  checkCardanoError(cardano_byron_address_get_attributes(checked(), &value), "ByronAddress attributes");

  ByronAddressAttributes attributes;
  attributes.derivationPath.assign(value.derivation_path, value.derivation_path + value.derivation_path_size);

  if (value.magic >= 0)
  {
    attributes.magic = value.magic;
  }

  return attributes;
}

ByronAddressType
ByronAddress::type() const
{
  cardano_byron_address_type_t value;

  checkCardanoError(cardano_byron_address_get_type(checked(), &value), "ByronAddress type");

  return static_cast<ByronAddressType>(value);
}

Blake2bHash
ByronAddress::root() const
{
  cardano_blake2b_hash_t* value  = nullptr;
  const cardano_error_t   result = cardano_byron_address_get_root(checked(), &value);

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  checkCardanoError(result, "ByronAddress root");

  return Blake2bHash::adopt(owner.release());
}

void
ByronAddress::swap(ByronAddress& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ByronAddress&
ByronAddress::operator=(const ByronAddress& other)
{
  if (this != &other)
  {
    ByronAddress copy(other);

    swap(copy);
  }

  return *this;
}

ByronAddress&
ByronAddress::operator=(ByronAddress&& other) noexcept
{
  if (this != &other)
  {
    cardano_byron_address_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
ByronAddress::operator==(const ByronAddress& other) const
{
  const std::size_t size = cardano_byron_address_get_bytes_size(checked());

  return size == cardano_byron_address_get_bytes_size(other.checked()) && std::memcmp(cardano_byron_address_get_bytes(checked()), cardano_byron_address_get_bytes(other.checked()), size) == 0;
}

bool
ByronAddress::operator!=(const ByronAddress& other) const
{
  return !(*this == other);
}

ByronAddress
ByronAddress::adopt(cardano_byron_address_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null ByronAddress");
  }

  return ByronAddress(*value);
}

cardano_byron_address_t*
ByronAddress::nativeHandle() noexcept
{
  return m_value;
}

const cardano_byron_address_t*
ByronAddress::nativeHandle() const noexcept
{
  return m_value;
}

cardano_byron_address_t*
ByronAddress::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

ByronAddress::ByronAddress(cardano_byron_address_t& value) noexcept
: m_value(&value)
{
}

cardano_byron_address_t*
ByronAddress::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ByronAddress");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ByronAddress& lhs, ByronAddress& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
