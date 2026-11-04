/**
 * @file Ed25519PublicKey.cpp
 *
 * @brief Ed25519PublicKey operations.
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

#include <Cometa/Crypto/Ed25519PublicKey.h>

#include "../CardanoInterop.h"

#include <cardano/crypto/ed25519_public_key.h>

#include <cstring>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Ed25519PublicKey::Ed25519PublicKey(const std::vector<std::uint8_t>& bytes)
: m_value(nullptr)
{
  cardano_ed25519_public_key_t* value  = nullptr;
  const cardano_error_t         result = cardano_ed25519_public_key_from_bytes(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_ed25519_public_key_t, cardano_ed25519_public_key_unref> owner(value);

  checkCardanoError(result, "Ed25519PublicKey");

  m_value = owner.release();
}

Ed25519PublicKey::Ed25519PublicKey(const Ed25519PublicKey& other)
: m_value(nullptr)
{
  cardano_ed25519_public_key_t* value  = nullptr;
  const cardano_error_t         result = cardano_ed25519_public_key_deep_clone(other.checked(), &value);

  CardanoObject<cardano_ed25519_public_key_t, cardano_ed25519_public_key_unref> owner(value);

  checkCardanoError(result, "Ed25519PublicKey copy");

  m_value = owner.release();
}

Ed25519PublicKey::Ed25519PublicKey(Ed25519PublicKey&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Ed25519PublicKey::~Ed25519PublicKey()
{
  cardano_ed25519_public_key_unref(&m_value);
}

Ed25519PublicKey
Ed25519PublicKey::fromHex(std::string_view hex)
{
  validateHex(hex);

  cardano_ed25519_public_key_t* value  = nullptr;
  const cardano_error_t         result = cardano_ed25519_public_key_from_hex(hex.empty() ? "" : hex.data(), hex.size(), &value);

  CardanoObject<cardano_ed25519_public_key_t, cardano_ed25519_public_key_unref> owner(value);

  checkCardanoError(result, "Ed25519PublicKey fromHex");

  return Ed25519PublicKey(*owner.release());
}

std::vector<std::uint8_t>
Ed25519PublicKey::toBytes() const
{
  std::vector<std::uint8_t> bytes(cardano_ed25519_public_key_get_bytes_size(checked()));

  checkCardanoError(cardano_ed25519_public_key_to_bytes(checked(), bytes.data(), bytes.size()), "Ed25519PublicKey toBytes");

  return bytes;
}

std::string
Ed25519PublicKey::toHex() const
{
  std::string hex(cardano_ed25519_public_key_get_hex_size(checked()), '\0');

  checkCardanoError(cardano_ed25519_public_key_to_hex(checked(), hex.data(), hex.size()), "Ed25519PublicKey toHex");

  hex.pop_back();

  return hex;
}

bool
Ed25519PublicKey::verify(const Ed25519Signature& signature, const std::vector<std::uint8_t>& message) const
{
  return cardano_ed25519_public_verify(checked(), checkedNativeHandle(signature), byteData(message), message.size());
}

Blake2bHash
Ed25519PublicKey::hash() const
{
  cardano_blake2b_hash_t* value  = nullptr;
  const cardano_error_t   result = cardano_ed25519_public_key_to_hash(checked(), &value);

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  checkCardanoError(result, "Ed25519PublicKey hash");

  return Blake2bHash::adopt(owner.release());
}

bool
Ed25519PublicKey::operator==(const Ed25519PublicKey& other) const
{
  return std::memcmp(cardano_ed25519_public_key_get_data(checked()), cardano_ed25519_public_key_get_data(other.checked()), 32) == 0;
}

bool
Ed25519PublicKey::operator!=(const Ed25519PublicKey& other) const
{
  return !(*this == other);
}

void
Ed25519PublicKey::swap(Ed25519PublicKey& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Ed25519PublicKey&
Ed25519PublicKey::operator=(const Ed25519PublicKey& other)
{
  if (this != &other)
  {
    Ed25519PublicKey copy(other);

    swap(copy);
  }

  return *this;
}

Ed25519PublicKey&
Ed25519PublicKey::operator=(Ed25519PublicKey&& other) noexcept
{
  if (this != &other)
  {
    cardano_ed25519_public_key_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

Ed25519PublicKey
Ed25519PublicKey::adopt(cardano_ed25519_public_key_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null Ed25519PublicKey");
  }

  return Ed25519PublicKey(*value);
}

cardano_ed25519_public_key_t*
Ed25519PublicKey::nativeHandle() noexcept
{
  return m_value;
}

const cardano_ed25519_public_key_t*
Ed25519PublicKey::nativeHandle() const noexcept
{
  return m_value;
}

cardano_ed25519_public_key_t*
Ed25519PublicKey::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

Ed25519PublicKey::Ed25519PublicKey(cardano_ed25519_public_key_t& value) noexcept
: m_value(&value)
{
}

cardano_ed25519_public_key_t*
Ed25519PublicKey::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Ed25519PublicKey");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Ed25519PublicKey& lhs, Ed25519PublicKey& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
