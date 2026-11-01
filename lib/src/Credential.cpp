/**
 * @file Credential.cpp
 *
 * @brief Implementation of the Credential value type.
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

#include <cometa/Credential.h>

#include "Native.h"

#include <cardano/common/credential.h>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
static_assert(static_cast<int>(CredentialType::KeyHash) == CARDANO_CREDENTIAL_TYPE_KEY_HASH);
static_assert(static_cast<int>(CredentialType::ScriptHash) == CARDANO_CREDENTIAL_TYPE_SCRIPT_HASH);

namespace
{
/**
 * @brief Converts a checked C++ credential discriminator to the native enum.
 *
 * @param type The discriminator; unknown values throw std::invalid_argument.
 *
 * @return The native discriminator.
 */
cardano_credential_type_t
nativeType(CredentialType type)
{
  switch (type)
  {
    case CredentialType::KeyHash:
      return CARDANO_CREDENTIAL_TYPE_KEY_HASH;
    case CredentialType::ScriptHash:
      return CARDANO_CREDENTIAL_TYPE_SCRIPT_HASH;
  }
  throw std::invalid_argument("Unknown credential type");
}

/**
 * @brief Copies a credential hash after checking its byte length.
 *
 * @param hash The source hash; its length must be 28 bytes.
 *
 * @return The copied bytes.
 */
std::vector<std::uint8_t>
credentialHash(const Blake2bHash& hash)
{
  auto bytes = hash.toBytes();
  if (bytes.size() != 28)
  {
    throw std::invalid_argument("Credential hash must be 28 bytes");
  }
  return bytes;
}
} // namespace

/* CLASS IMPLEMENTATION ******************************************************/

Credential::Credential(CredentialType type, const Blake2bHash& hash)
: m_value(
    Detail::create<cardano_credential_t, cardano_credential_unref>(
      [&](cardano_credential_t** out)
      {
        const auto bytes = credentialHash(hash);
        return cardano_credential_from_hash_bytes(bytes.data(), bytes.size(), nativeType(type), out);
      },
      "Credential"))
{
}

Credential::Credential(const Credential& other)
: m_value(Detail::clone<cardano_credential_t, cardano_credential_unref>(other.checked(), cardano_credential_deep_clone))
{
}

Credential::Credential(Credential&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Credential::~Credential()
{
  cardano_credential_unref(&m_value);
}

Credential
Credential::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return Credential(Detail::decode<cardano_credential_t, cardano_credential_unref>(cbor, cardano_credential_from_cbor));
}

Blake2bHash
Credential::hash() const
{
  const auto* value = checked();
  const auto* bytes = cardano_credential_get_hash_bytes(value);
  return Blake2bHash(std::vector<std::uint8_t>(bytes, bytes + cardano_credential_get_hash_bytes_size(value)));
}

void
Credential::setHash(const Blake2bHash& hash)
{
  const auto bytes = credentialHash(hash);

  Detail::Owned<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> native(
    Detail::create<cardano_blake2b_hash_t, cardano_blake2b_hash_unref>(
      [&](cardano_blake2b_hash_t** out)
      {
        return cardano_blake2b_hash_from_bytes(bytes.data(), bytes.size(), out);
      },
      "Credential setHash"));
  Detail::check(cardano_credential_set_hash(checked(), native.get()), "Credential setHash");
}

CredentialType
Credential::type() const
{
  cardano_credential_type_t type;
  Detail::check(cardano_credential_get_type(checked(), &type), "Credential type");
  return static_cast<CredentialType>(type);
}

void
Credential::setType(CredentialType type)
{
  Detail::check(cardano_credential_set_type(checked(), nativeType(type)), "Credential setType");
}

std::vector<std::uint8_t>
Credential::toCbor() const
{
  return Detail::encode(checked(), cardano_credential_to_cbor);
}

std::string
Credential::toJson() const
{
  return Detail::json(checked(), cardano_credential_to_cip116_json);
}

void
Credential::swap(Credential& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Credential&
Credential::operator=(const Credential& other)
{
  if (this != &other)
  {
    Credential copy(other);
    swap(copy);
  }
  return *this;
}

Credential&
Credential::operator=(Credential&& other) noexcept
{
  if (this != &other)
  {
    cardano_credential_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
Credential::operator==(const Credential& other) const
{
  return cardano_credential_equals(checked(), other.checked());
}

bool
Credential::operator!=(const Credential& other) const
{
  return !(*this == other);
}

bool
Credential::operator<(const Credential& other) const
{
  return cardano_credential_compare(checked(), other.checked()) < 0;
}

/* PRIVATE FUNCTIONS *********************************************************/

Credential::Credential(cardano_credential_t* value) noexcept
: m_value(value)
{
}

cardano_credential_t*
Credential::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Credential");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Credential& lhs, Credential& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
