/**
 * @file Ed25519Signature.cpp
 *
 * @brief Ed25519Signature operations.
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

#include <Cometa/Crypto/Ed25519Signature.h>

#include "../CardanoInterop.h"

#include <cardano/crypto/ed25519_signature.h>

#include <cstring>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Ed25519Signature::Ed25519Signature(const std::vector<std::uint8_t>& bytes)
: m_value(nullptr)
{
  cardano_ed25519_signature_t* value  = nullptr;
  const cardano_error_t        result = cardano_ed25519_signature_from_bytes(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_ed25519_signature_t, cardano_ed25519_signature_unref> owner(value);

  checkCardanoError(result, "Ed25519Signature");

  m_value = owner.release();
}

Ed25519Signature::Ed25519Signature(const Ed25519Signature& other)
: m_value(nullptr)
{
  cardano_ed25519_signature_t* value  = nullptr;
  const cardano_error_t        result = cardano_ed25519_signature_deep_clone(other.checked(), &value);

  CardanoObject<cardano_ed25519_signature_t, cardano_ed25519_signature_unref> owner(value);

  checkCardanoError(result, "Ed25519Signature copy");

  m_value = owner.release();
}

Ed25519Signature::Ed25519Signature(Ed25519Signature&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Ed25519Signature::~Ed25519Signature()
{
  cardano_ed25519_signature_unref(&m_value);
}

Ed25519Signature
Ed25519Signature::fromHex(std::string_view hex)
{
  validateHex(hex);

  cardano_ed25519_signature_t* value  = nullptr;
  const cardano_error_t        result = cardano_ed25519_signature_from_hex(hex.empty() ? "" : hex.data(), hex.size(), &value);

  CardanoObject<cardano_ed25519_signature_t, cardano_ed25519_signature_unref> owner(value);

  checkCardanoError(result, "Ed25519Signature fromHex");

  return Ed25519Signature(*owner.release());
}

std::vector<std::uint8_t>
Ed25519Signature::toBytes() const
{
  std::vector<std::uint8_t> bytes(cardano_ed25519_signature_get_bytes_size(checked()));

  checkCardanoError(cardano_ed25519_signature_to_bytes(checked(), bytes.data(), bytes.size()), "Ed25519Signature toBytes");

  return bytes;
}

std::string
Ed25519Signature::toHex() const
{
  std::string hex(cardano_ed25519_signature_get_hex_size(checked()), '\0');

  checkCardanoError(cardano_ed25519_signature_to_hex(checked(), hex.data(), hex.size()), "Ed25519Signature toHex");

  hex.pop_back();

  return hex;
}

bool
Ed25519Signature::operator==(const Ed25519Signature& other) const
{
  return std::memcmp(cardano_ed25519_signature_get_data(checked()), cardano_ed25519_signature_get_data(other.checked()), 64) == 0;
}

bool
Ed25519Signature::operator!=(const Ed25519Signature& other) const
{
  return !(*this == other);
}

void
Ed25519Signature::swap(Ed25519Signature& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Ed25519Signature&
Ed25519Signature::operator=(const Ed25519Signature& other)
{
  if (this != &other)
  {
    Ed25519Signature copy(other);

    swap(copy);
  }

  return *this;
}

Ed25519Signature&
Ed25519Signature::operator=(Ed25519Signature&& other) noexcept
{
  if (this != &other)
  {
    cardano_ed25519_signature_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

Ed25519Signature
Ed25519Signature::adopt(cardano_ed25519_signature_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null Ed25519Signature");
  }

  return Ed25519Signature(*value);
}

cardano_ed25519_signature_t*
Ed25519Signature::nativeHandle() noexcept
{
  return m_value;
}

const cardano_ed25519_signature_t*
Ed25519Signature::nativeHandle() const noexcept
{
  return m_value;
}

cardano_ed25519_signature_t*
Ed25519Signature::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

Ed25519Signature::Ed25519Signature(cardano_ed25519_signature_t& value) noexcept
: m_value(&value)
{
}

cardano_ed25519_signature_t*
Ed25519Signature::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Ed25519Signature");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Ed25519Signature& lhs, Ed25519Signature& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
