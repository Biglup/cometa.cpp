/**
 * \file credential.cpp
 *
 * \author angel.castillo
 * \date   Nov 01, 2026
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

#include "native.hpp"
#include <cardano/common/credential.h>
#include <cometa/credential.hpp>

namespace Biglup::Cometa
{
static_assert(static_cast<int>(CredentialType::KeyHash) == CARDANO_CREDENTIAL_TYPE_KEY_HASH);
static_assert(static_cast<int>(CredentialType::ScriptHash) == CARDANO_CREDENTIAL_TYPE_SCRIPT_HASH);

Credential::Credential(cardano_credential_t* value) noexcept
: value_(value)
{
}

Credential::~Credential()
{
  cardano_credential_unref(&value_);
}

cardano_credential_t*
Credential::checked() const
{
  if (!value_)
  {
    throw std::logic_error("Cannot observe a moved-from Credential");
  }
  return value_;
}

Credential::Credential(const Credential& other)
: value_(Detail::clone<cardano_credential_t, cardano_credential_unref>(other.checked(), cardano_credential_deep_clone))
{
}

Credential::Credential(Credential&& other) noexcept
: value_(std::exchange(other.value_, nullptr))
{
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
    cardano_credential_unref(&value_);
    value_ = std::exchange(other.value_, nullptr);
  }
  return *this;
}

void
Credential::swap(Credential& other) noexcept
{
  std::swap(value_, other.value_);
}

void
swap(Credential& lhs, Credential& rhs) noexcept
{
  lhs.swap(rhs);
}

Credential
Credential::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return Credential(Detail::decode<cardano_credential_t, cardano_credential_unref>(cbor, cardano_credential_from_cbor));
}

std::vector<std::uint8_t>
Credential::toCbor() const
{
  return Detail::encode(checked(), cardano_credential_to_cbor);
}

bool
Credential::operator!=(const Credential& other) const
{
  return !(*this == other);
}

namespace
{
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

Credential::Credential(CredentialType type, const Blake2bHash& hash)
: value_(Detail::create<cardano_credential_t, cardano_credential_unref>([&](cardano_credential_t** out)
                                                                        {
      const auto bytes = credentialHash(hash);
      return cardano_credential_from_hash_bytes(bytes.data(), bytes.size(), nativeType(type), out); },
                                                                        "Credential"))
{
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
  const auto                                                        bytes = credentialHash(hash);
  Detail::Owned<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> native(
    Detail::create<cardano_blake2b_hash_t, cardano_blake2b_hash_unref>([&](cardano_blake2b_hash_t** out)
                                                                       { return cardano_blake2b_hash_from_bytes(bytes.data(), bytes.size(), out); },
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

bool
Credential::operator==(const Credential& other) const
{
  return cardano_credential_equals(checked(), other.checked());
}

std::string
Credential::toJson() const
{
  return Detail::json(checked(), cardano_credential_to_cip116_json);
}

bool
Credential::operator<(const Credential& other) const
{
  return cardano_credential_compare(checked(), other.checked()) < 0;
}

} // namespace Biglup::Cometa
