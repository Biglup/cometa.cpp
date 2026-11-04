/**
 * @file Bip32PublicKey.cpp
 *
 * @brief Bip32PublicKey operations.
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

#include <Cometa/Crypto/Bip32PublicKey.h>

#include "../CardanoInterop.h"

#include <cardano/crypto/bip32_public_key.h>

#include <cstring>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Bip32PublicKey::Bip32PublicKey(const std::vector<std::uint8_t>& bytes)
: m_value(nullptr)
{
  cardano_bip32_public_key_t* value  = nullptr;
  const cardano_error_t       result = cardano_bip32_public_key_from_bytes(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_bip32_public_key_t, cardano_bip32_public_key_unref> owner(value);

  checkCardanoError(result, "Bip32PublicKey");

  m_value = owner.release();
}

Bip32PublicKey::Bip32PublicKey(const Bip32PublicKey& other)
: m_value(nullptr)
{
  cardano_bip32_public_key_t* value  = nullptr;
  const cardano_error_t       result = cardano_bip32_public_key_deep_clone(other.checked(), &value);

  CardanoObject<cardano_bip32_public_key_t, cardano_bip32_public_key_unref> owner(value);

  checkCardanoError(result, "Bip32PublicKey copy");

  m_value = owner.release();
}

Bip32PublicKey::Bip32PublicKey(Bip32PublicKey&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Bip32PublicKey::~Bip32PublicKey()
{
  cardano_bip32_public_key_unref(&m_value);
}

Bip32PublicKey
Bip32PublicKey::fromHex(std::string_view hex)
{
  validateHex(hex);

  cardano_bip32_public_key_t* value  = nullptr;
  const cardano_error_t       result = cardano_bip32_public_key_from_hex(hex.empty() ? "" : hex.data(), hex.size(), &value);

  CardanoObject<cardano_bip32_public_key_t, cardano_bip32_public_key_unref> owner(value);

  checkCardanoError(result, "Bip32PublicKey fromHex");

  return Bip32PublicKey(*owner.release());
}

std::vector<std::uint8_t>
Bip32PublicKey::toBytes() const
{
  std::vector<std::uint8_t> bytes(cardano_bip32_public_key_get_bytes_size(checked()));

  checkCardanoError(cardano_bip32_public_key_to_bytes(checked(), bytes.data(), bytes.size()), "Bip32PublicKey toBytes");

  return bytes;
}

std::string
Bip32PublicKey::toHex() const
{
  std::string hex(cardano_bip32_public_key_get_hex_size(checked()), '\0');

  checkCardanoError(cardano_bip32_public_key_to_hex(checked(), hex.data(), hex.size()), "Bip32PublicKey toHex");

  hex.pop_back();

  return hex;
}

Blake2bHash
Bip32PublicKey::hash() const
{
  cardano_blake2b_hash_t* value  = nullptr;
  const cardano_error_t   result = cardano_bip32_public_key_to_hash(checked(), &value);

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  checkCardanoError(result, "Bip32PublicKey hash");

  return Blake2bHash::adopt(owner.release());
}

Bip32PublicKey
Bip32PublicKey::derive(const std::vector<std::uint32_t>& path) const
{
  if (path.empty())
  {
    return *this;
  }

  cardano_bip32_public_key_t* value  = nullptr;
  const cardano_error_t       result = cardano_bip32_public_key_derive(checked(), path.data(), path.size(), &value);

  CardanoObject<cardano_bip32_public_key_t, cardano_bip32_public_key_unref> owner(value);

  checkCardanoError(result, "Bip32PublicKey derive");

  return Bip32PublicKey(*owner.release());
}

Ed25519PublicKey
Bip32PublicKey::toEd25519() const
{
  cardano_ed25519_public_key_t* value  = nullptr;
  const cardano_error_t         result = cardano_bip32_public_key_to_ed25519_key(checked(), &value);

  CardanoObject<cardano_ed25519_public_key_t, cardano_ed25519_public_key_unref> owner(value);

  checkCardanoError(result, "Bip32PublicKey toEd25519");

  return Ed25519PublicKey::adopt(owner.release());
}

bool
Bip32PublicKey::operator==(const Bip32PublicKey& other) const
{
  return std::memcmp(cardano_bip32_public_key_get_data(checked()), cardano_bip32_public_key_get_data(other.checked()), 64) == 0;
}

bool
Bip32PublicKey::operator!=(const Bip32PublicKey& other) const
{
  return !(*this == other);
}

void
Bip32PublicKey::swap(Bip32PublicKey& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Bip32PublicKey&
Bip32PublicKey::operator=(const Bip32PublicKey& other)
{
  if (this != &other)
  {
    Bip32PublicKey copy(other);

    swap(copy);
  }

  return *this;
}

Bip32PublicKey&
Bip32PublicKey::operator=(Bip32PublicKey&& other) noexcept
{
  if (this != &other)
  {
    cardano_bip32_public_key_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

Bip32PublicKey
Bip32PublicKey::adopt(cardano_bip32_public_key_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null Bip32PublicKey");
  }

  return Bip32PublicKey(*value);
}

cardano_bip32_public_key_t*
Bip32PublicKey::nativeHandle() noexcept
{
  return m_value;
}

const cardano_bip32_public_key_t*
Bip32PublicKey::nativeHandle() const noexcept
{
  return m_value;
}

cardano_bip32_public_key_t*
Bip32PublicKey::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

Bip32PublicKey::Bip32PublicKey(cardano_bip32_public_key_t& value) noexcept
: m_value(&value)
{
}

cardano_bip32_public_key_t*
Bip32PublicKey::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Bip32PublicKey");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Bip32PublicKey& lhs, Bip32PublicKey& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
