/**
 * @file Address.cpp
 *
 * @brief Address operations.
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

#include <Cometa/Address/Address.h>

#include <Cometa/Address/Address.h>
#include <Cometa/Encoding/Base58.h>
#include <Cometa/Encoding/Bech32.h>

#include "../CardanoInterop.h"

#include <cardano/address/address.h>

#include <algorithm>
#include <cstring>
#include <limits>

#include <utility>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
/**
 * @brief Converts an ASCII letter to lowercase.
 *
 * @param character The character to convert.
 *
 * @return The lowercase letter, or the original nonletter.
 */
char
toLowerAscii(char character)
{
  return character >= 'A' && character <= 'Z' ? static_cast<char>(character + ('a' - 'A')) : character;
}
} // namespace

Address::Address(const std::vector<std::uint8_t>& bytes)
: m_value(nullptr)
{
  cardano_address_t*    value  = nullptr;
  const cardano_error_t result = cardano_address_from_bytes(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_address_t, cardano_address_unref> owner(value);

  checkCardanoError(result, "Address");

  if ((bytes.front() >> 4) != 8)
  {
    const std::size_t size = cardano_address_get_bytes_size(owner.get());

    if (size != bytes.size() || std::memcmp(bytes.data(), cardano_address_get_bytes(owner.get()), size) != 0)
    {
      throw std::invalid_argument("Address bytes are noncanonical, out of range, or contain trailing data");
    }
  }

  m_value = owner.release();
}

/* CLASS IMPLEMENTATION ******************************************************/
Address::Address(const BaseAddress& address)
: Address(address.toAddress())
{
}

Address::Address(const EnterpriseAddress& address)
: Address(address.toAddress())
{
}

Address::Address(const PointerAddress& address)
: Address(address.toAddress())
{
}

Address::Address(const RewardAddress& address)
: Address(address.toAddress())
{
}

Address::Address(const ByronAddress& address)
: Address(address.toAddress())
{
}

Address::Address(const Address& other)
: m_value(nullptr)
{
  cardano_address_t*    value  = nullptr;
  const cardano_error_t result = cardano_address_deep_clone(other.checked(), &value);

  CardanoObject<cardano_address_t, cardano_address_unref> owner(value);

  checkCardanoError(result, "Address copy");

  m_value = owner.release();
}

Address::Address(Address&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Address::~Address()
{
  cardano_address_unref(&m_value);
}

Address
Address::fromString(std::string_view text)
{
  std::string prefix(text.substr(0, std::min<std::size_t>(text.size(), 5)));

  std::transform(prefix.begin(), prefix.end(), prefix.begin(), toLowerAscii);

  if (prefix.compare(0, 4, "addr") == 0 || prefix == "stake")
  {
    const Bech32Data  decoded = decodeBech32(text);
    Address           address(decoded.bytes);
    const std::string canonical     = address.toString();
    std::string       decodedPrefix = decoded.prefix;

    std::transform(decodedPrefix.begin(), decodedPrefix.end(), decodedPrefix.begin(), toLowerAscii);

    if (address.type() == AddressType::Byron || decodedPrefix != canonical.substr(0, canonical.rfind('1')))
    {
      throw std::invalid_argument("Address prefix does not match its type and network");
    }

    return address;
  }

  Address address(decodeBase58(text));

  if (address.type() != AddressType::Byron)
  {
    throw std::invalid_argument("Base58 is only supported for Byron addresses");
  }

  return address;
}

bool
Address::isValid(std::string_view text)
{
  try
  {
    const Address address = fromString(text);

    return true;
  }
  catch (const Error&)
  {
    return false;
  }
  catch (const std::invalid_argument&)
  {
    return false;
  }
}

bool
Address::isValidBech32(std::string_view text)
{
  try
  {
    const Address address = fromString(text);

    return address.type() != AddressType::Byron;
  }
  catch (const Error&)
  {
    return false;
  }
  catch (const std::invalid_argument&)
  {
    return false;
  }
}

bool
Address::isValidByron(std::string_view text)
{
  try
  {
    const Address address = fromString(text);

    return address.type() == AddressType::Byron;
  }
  catch (const Error&)
  {
    return false;
  }
  catch (const std::invalid_argument&)
  {
    return false;
  }
}

std::vector<std::uint8_t>
Address::toBytes() const
{
  std::vector<std::uint8_t> bytes(cardano_address_get_bytes_size(checked()));

  checkCardanoError(cardano_address_to_bytes(checked(), bytes.data(), bytes.size()), "Address toBytes");

  return bytes;
}

std::string
Address::toString() const
{
  return cardano_address_get_string(checked());
}

NetworkId
Address::networkId() const
{
  cardano_network_id_t network;

  checkCardanoError(cardano_address_get_network_id(checked(), &network), "Address networkId");

  return static_cast<NetworkId>(network);
}

AddressType
Address::type() const
{
  cardano_address_type_t value;

  checkCardanoError(cardano_address_get_type(checked(), &value), "Address type");

  return static_cast<AddressType>(value);
}

std::optional<BaseAddress>
Address::asBase() const
{
  try
  {
    return BaseAddress::fromAddress(*this);
  }
  catch (const Error& error)
  {
    if (error.code() == makeErrorCode(ErrorCode::InvalidAddressType))
    {
      return std::nullopt;
    }

    throw;
  }
}

std::optional<EnterpriseAddress>
Address::asEnterprise() const
{
  try
  {
    return EnterpriseAddress::fromAddress(*this);
  }
  catch (const Error& error)
  {
    if (error.code() == makeErrorCode(ErrorCode::InvalidAddressType))
    {
      return std::nullopt;
    }

    throw;
  }
}

std::optional<PointerAddress>
Address::asPointer() const
{
  try
  {
    return PointerAddress::fromAddress(*this);
  }
  catch (const Error& error)
  {
    if (error.code() == makeErrorCode(ErrorCode::InvalidAddressType))
    {
      return std::nullopt;
    }

    throw;
  }
}

std::optional<RewardAddress>
Address::asReward() const
{
  try
  {
    return RewardAddress::fromAddress(*this);
  }
  catch (const Error& error)
  {
    if (error.code() == makeErrorCode(ErrorCode::InvalidAddressType))
    {
      return std::nullopt;
    }

    throw;
  }
}

std::optional<ByronAddress>
Address::asByron() const
{
  try
  {
    return ByronAddress::fromAddress(*this);
  }
  catch (const Error& error)
  {
    if (error.code() == makeErrorCode(ErrorCode::InvalidAddressType))
    {
      return std::nullopt;
    }

    throw;
  }
}

void
Address::swap(Address& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Address&
Address::operator=(const Address& other)
{
  if (this != &other)
  {
    Address copy(other);

    swap(copy);
  }

  return *this;
}

Address&
Address::operator=(Address&& other) noexcept
{
  if (this != &other)
  {
    cardano_address_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
Address::operator==(const Address& other) const
{
  const std::size_t size = cardano_address_get_bytes_size(checked());

  return size == cardano_address_get_bytes_size(other.checked()) && std::memcmp(cardano_address_get_bytes(checked()), cardano_address_get_bytes(other.checked()), size) == 0;
}

bool
Address::operator!=(const Address& other) const
{
  return !(*this == other);
}

Address
Address::adopt(cardano_address_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null Address");
  }

  return Address(*value);
}

cardano_address_t*
Address::nativeHandle() noexcept
{
  return m_value;
}

const cardano_address_t*
Address::nativeHandle() const noexcept
{
  return m_value;
}

cardano_address_t*
Address::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

Address::Address(cardano_address_t& value) noexcept
: m_value(&value)
{
}

cardano_address_t*
Address::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Address");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Address& lhs, Address& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
