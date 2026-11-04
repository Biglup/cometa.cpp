/**
 * @file Ed25519PrivateKey.cpp
 *
 * @brief Ed25519PrivateKey operations.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
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

#include <Cometa/Crypto/Ed25519PrivateKey.h>

#include "../CardanoInterop.h"

#include <cardano/crypto/ed25519_private_key.h>

#include <cstring>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Ed25519PrivateKey::Ed25519PrivateKey(const Ed25519PrivateKey& other)
: m_value(nullptr)
{
  cardano_ed25519_private_key_t* value  = nullptr;
  const cardano_error_t          result = cardano_ed25519_private_key_deep_clone(other.checked(), &value);

  CardanoObject<cardano_ed25519_private_key_t, cardano_ed25519_private_key_unref> owner(value);

  checkCardanoError(result, "Ed25519PrivateKey copy");

  m_value = owner.release();
}

Ed25519PrivateKey::Ed25519PrivateKey(Ed25519PrivateKey&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Ed25519PrivateKey::~Ed25519PrivateKey()
{
  cardano_ed25519_private_key_unref(&m_value);
}

Ed25519PrivateKey
Ed25519PrivateKey::fromNormalBytes(const std::vector<std::uint8_t>& bytes)
{
  cardano_ed25519_private_key_t* value  = nullptr;
  const cardano_error_t          result = cardano_ed25519_private_key_from_normal_bytes(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_ed25519_private_key_t, cardano_ed25519_private_key_unref> owner(value);

  checkCardanoError(result, "Ed25519PrivateKey fromNormalBytes");

  return Ed25519PrivateKey(*owner.release());
}

Ed25519PrivateKey
Ed25519PrivateKey::fromNormalHex(std::string_view hex)
{
  validateHex(hex);

  cardano_ed25519_private_key_t* value  = nullptr;
  const cardano_error_t          result = cardano_ed25519_private_key_from_normal_hex(hex.empty() ? "" : hex.data(), hex.size(), &value);

  CardanoObject<cardano_ed25519_private_key_t, cardano_ed25519_private_key_unref> owner(value);

  checkCardanoError(result, "Ed25519PrivateKey fromNormalHex");

  return Ed25519PrivateKey(*owner.release());
}

Ed25519PrivateKey
Ed25519PrivateKey::fromExtendedBytes(const std::vector<std::uint8_t>& bytes)
{
  cardano_ed25519_private_key_t* value  = nullptr;
  const cardano_error_t          result = cardano_ed25519_private_key_from_extended_bytes(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_ed25519_private_key_t, cardano_ed25519_private_key_unref> owner(value);

  checkCardanoError(result, "Ed25519PrivateKey fromExtendedBytes");

  return Ed25519PrivateKey(*owner.release());
}

Ed25519PrivateKey
Ed25519PrivateKey::fromExtendedHex(std::string_view hex)
{
  validateHex(hex);

  cardano_ed25519_private_key_t* value  = nullptr;
  const cardano_error_t          result = cardano_ed25519_private_key_from_extended_hex(hex.empty() ? "" : hex.data(), hex.size(), &value);

  CardanoObject<cardano_ed25519_private_key_t, cardano_ed25519_private_key_unref> owner(value);

  checkCardanoError(result, "Ed25519PrivateKey fromExtendedHex");

  return Ed25519PrivateKey(*owner.release());
}

std::vector<std::uint8_t>
Ed25519PrivateKey::toBytes() const
{
  std::vector<std::uint8_t> bytes(cardano_ed25519_private_key_get_bytes_size(checked()));

  checkCardanoError(cardano_ed25519_private_key_to_bytes(checked(), bytes.data(), bytes.size()), "Ed25519PrivateKey toBytes");

  return bytes;
}

std::string
Ed25519PrivateKey::toHex() const
{
  std::string hex(cardano_ed25519_private_key_get_hex_size(checked()), '\0');

  checkCardanoError(cardano_ed25519_private_key_to_hex(checked(), hex.data(), hex.size()), "Ed25519PrivateKey toHex");

  hex.pop_back();

  return hex;
}

bool
Ed25519PrivateKey::isExtended() const
{
  return cardano_ed25519_private_key_get_bytes_size(checked()) == 64;
}

Ed25519Signature
Ed25519PrivateKey::sign(const std::vector<std::uint8_t>& message) const
{
  cardano_ed25519_signature_t* value  = nullptr;
  const cardano_error_t        result = cardano_ed25519_private_key_sign(checked(), byteData(message), message.size(), &value);

  CardanoObject<cardano_ed25519_signature_t, cardano_ed25519_signature_unref> owner(value);

  checkCardanoError(result, "Ed25519PrivateKey sign");

  return Ed25519Signature::adopt(owner.release());
}

Ed25519PublicKey
Ed25519PrivateKey::publicKey() const
{
  cardano_ed25519_public_key_t* value  = nullptr;
  const cardano_error_t         result = cardano_ed25519_private_key_get_public_key(checked(), &value);

  CardanoObject<cardano_ed25519_public_key_t, cardano_ed25519_public_key_unref> owner(value);

  checkCardanoError(result, "Ed25519PrivateKey publicKey");

  return Ed25519PublicKey::adopt(owner.release());
}

void
Ed25519PrivateKey::swap(Ed25519PrivateKey& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Ed25519PrivateKey&
Ed25519PrivateKey::operator=(const Ed25519PrivateKey& other)
{
  if (this != &other)
  {
    Ed25519PrivateKey copy(other);

    swap(copy);
  }

  return *this;
}

Ed25519PrivateKey&
Ed25519PrivateKey::operator=(Ed25519PrivateKey&& other) noexcept
{
  if (this != &other)
  {
    cardano_ed25519_private_key_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

Ed25519PrivateKey
Ed25519PrivateKey::adopt(cardano_ed25519_private_key_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null Ed25519PrivateKey");
  }

  return Ed25519PrivateKey(*value);
}

cardano_ed25519_private_key_t*
Ed25519PrivateKey::nativeHandle() noexcept
{
  return m_value;
}

const cardano_ed25519_private_key_t*
Ed25519PrivateKey::nativeHandle() const noexcept
{
  return m_value;
}

cardano_ed25519_private_key_t*
Ed25519PrivateKey::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

Ed25519PrivateKey::Ed25519PrivateKey(cardano_ed25519_private_key_t& value) noexcept
: m_value(&value)
{
}

cardano_ed25519_private_key_t*
Ed25519PrivateKey::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Ed25519PrivateKey");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Ed25519PrivateKey& lhs, Ed25519PrivateKey& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
