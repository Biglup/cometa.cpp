/**
 * @file Bip32PrivateKey.cpp
 *
 * @brief Bip32PrivateKey operations.
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

#include <Cometa/Crypto/Bip32PrivateKey.h>

#include "../CardanoInterop.h"

#include <cardano/crypto/bip32_private_key.h>

#include <cstring>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Bip32PrivateKey::Bip32PrivateKey(const std::vector<std::uint8_t>& bytes)
: m_value(nullptr)
{
  cardano_bip32_private_key_t* value  = nullptr;
  const cardano_error_t        result = cardano_bip32_private_key_from_bytes(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_bip32_private_key_t, cardano_bip32_private_key_unref> owner(value);

  checkCardanoError(result, "Bip32PrivateKey");

  m_value = owner.release();
}

Bip32PrivateKey::Bip32PrivateKey(const Bip32PrivateKey& other)
: m_value(nullptr)
{
  cardano_bip32_private_key_t* value  = nullptr;
  const cardano_error_t        result = cardano_bip32_private_key_deep_clone(other.checked(), &value);

  CardanoObject<cardano_bip32_private_key_t, cardano_bip32_private_key_unref> owner(value);

  checkCardanoError(result, "Bip32PrivateKey copy");

  m_value = owner.release();
}

Bip32PrivateKey::Bip32PrivateKey(Bip32PrivateKey&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Bip32PrivateKey::~Bip32PrivateKey()
{
  cardano_bip32_private_key_unref(&m_value);
}

Bip32PrivateKey
Bip32PrivateKey::fromHex(std::string_view hex)
{
  validateHex(hex);

  cardano_bip32_private_key_t* value  = nullptr;
  const cardano_error_t        result = cardano_bip32_private_key_from_hex(hex.empty() ? "" : hex.data(), hex.size(), &value);

  CardanoObject<cardano_bip32_private_key_t, cardano_bip32_private_key_unref> owner(value);

  checkCardanoError(result, "Bip32PrivateKey fromHex");

  return Bip32PrivateKey(*owner.release());
}

Bip32PrivateKey
Bip32PrivateKey::fromBip39Entropy(const std::vector<std::uint8_t>& entropy, std::string_view password)
{
  cardano_bip32_private_key_t* value  = nullptr;
  const cardano_error_t        result = cardano_bip32_private_key_from_bip39_entropy(reinterpret_cast<const std::uint8_t*>(password.empty() ? "" : password.data()), password.size(), byteData(entropy), entropy.size(), &value);

  CardanoObject<cardano_bip32_private_key_t, cardano_bip32_private_key_unref> owner(value);

  checkCardanoError(result, "Bip32PrivateKey fromBip39Entropy");

  return Bip32PrivateKey(*owner.release());
}

std::vector<std::uint8_t>
Bip32PrivateKey::toBytes() const
{
  std::vector<std::uint8_t> bytes(cardano_bip32_private_key_get_bytes_size(checked()));

  checkCardanoError(cardano_bip32_private_key_to_bytes(checked(), bytes.data(), bytes.size()), "Bip32PrivateKey toBytes");

  return bytes;
}

std::string
Bip32PrivateKey::toHex() const
{
  std::string hex(cardano_bip32_private_key_get_hex_size(checked()), '\0');

  checkCardanoError(cardano_bip32_private_key_to_hex(checked(), hex.data(), hex.size()), "Bip32PrivateKey toHex");

  hex.pop_back();

  return hex;
}

Bip32PrivateKey
Bip32PrivateKey::derive(const std::vector<std::uint32_t>& path) const
{
  if (path.empty())
  {
    return *this;
  }

  cardano_bip32_private_key_t* value  = nullptr;
  const cardano_error_t        result = cardano_bip32_private_key_derive(checked(), path.data(), path.size(), &value);

  CardanoObject<cardano_bip32_private_key_t, cardano_bip32_private_key_unref> owner(value);

  checkCardanoError(result, "Bip32PrivateKey derive");

  return Bip32PrivateKey(*owner.release());
}

Ed25519PrivateKey
Bip32PrivateKey::toEd25519() const
{
  cardano_ed25519_private_key_t* value  = nullptr;
  const cardano_error_t          result = cardano_bip32_private_key_to_ed25519_key(checked(), &value);

  CardanoObject<cardano_ed25519_private_key_t, cardano_ed25519_private_key_unref> owner(value);

  checkCardanoError(result, "Bip32PrivateKey toEd25519");

  return Ed25519PrivateKey::adopt(owner.release());
}

Bip32PublicKey
Bip32PrivateKey::publicKey() const
{
  cardano_bip32_public_key_t* value  = nullptr;
  const cardano_error_t       result = cardano_bip32_private_key_get_public_key(checked(), &value);

  CardanoObject<cardano_bip32_public_key_t, cardano_bip32_public_key_unref> owner(value);

  checkCardanoError(result, "Bip32PrivateKey publicKey");

  return Bip32PublicKey::adopt(owner.release());
}

void
Bip32PrivateKey::swap(Bip32PrivateKey& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Bip32PrivateKey&
Bip32PrivateKey::operator=(const Bip32PrivateKey& other)
{
  if (this != &other)
  {
    Bip32PrivateKey copy(other);

    swap(copy);
  }

  return *this;
}

Bip32PrivateKey&
Bip32PrivateKey::operator=(Bip32PrivateKey&& other) noexcept
{
  if (this != &other)
  {
    cardano_bip32_private_key_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

Bip32PrivateKey
Bip32PrivateKey::adopt(cardano_bip32_private_key_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null Bip32PrivateKey");
  }

  return Bip32PrivateKey(*value);
}

cardano_bip32_private_key_t*
Bip32PrivateKey::nativeHandle() noexcept
{
  return m_value;
}

const cardano_bip32_private_key_t*
Bip32PrivateKey::nativeHandle() const noexcept
{
  return m_value;
}

cardano_bip32_private_key_t*
Bip32PrivateKey::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

Bip32PrivateKey::Bip32PrivateKey(cardano_bip32_private_key_t& value) noexcept
: m_value(&value)
{
}

cardano_bip32_private_key_t*
Bip32PrivateKey::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Bip32PrivateKey");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Bip32PrivateKey& lhs, Bip32PrivateKey& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
