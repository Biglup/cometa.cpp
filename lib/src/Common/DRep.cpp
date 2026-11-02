/**
 * @file DRep.cpp
 *
 * @brief Delegated representative value.
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

#include <Cometa/Common/DRep.h>

#include "../CardanoInterop.h"

#include <cardano/common/credential.h>
#include <cardano/common/drep.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
DRep::DRep(DRepType type, std::optional<Credential> credential)
: m_value(nullptr)
{
  validate(type, credential);

  cardano_drep_t*                                   value  = nullptr;
  const cardano_error_t                             status = cardano_drep_new(static_cast<cardano_drep_type_t>(type), credential ? credential->checked() : nullptr, &value);
  CardanoObject<cardano_drep_t, cardano_drep_unref> owner(value);

  checkCardanoError(status, "DRep");
  m_value = owner.release();
}

DRep::DRep(const Credential& credential)
: DRep(credential.type() == CredentialType::KeyHash ? DRepType::KeyHash : DRepType::ScriptHash, credential)
{
}

DRep::DRep(const DRep& other)
: m_value(nullptr)
{
  cardano_drep_t*                                   value  = nullptr;
  const cardano_error_t                             status = cardano_drep_deep_clone(other.checked(), &value);
  CardanoObject<cardano_drep_t, cardano_drep_unref> owner(value);

  checkCardanoError(status, "DRep copy");
  m_value = owner.release();
}

DRep::DRep(DRep&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

DRep::~DRep()
{
  cardano_drep_unref(&m_value);
}

DRep
DRep::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  DRep result(decodeCbor<cardano_drep_t, cardano_drep_unref>(cbor, cardano_drep_from_cbor));
  validate(result.type(), result.credential());
  return result;
}

DRep
DRep::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

DRep
DRep::fromString(std::string_view value)
{
  if (value.empty())
  {
    throw std::invalid_argument("DRep identifier must not be empty");
  }

  cardano_drep_t*                                   native = nullptr;
  const cardano_error_t                             status = cardano_drep_from_string(value.data(), value.size(), &native);
  CardanoObject<cardano_drep_t, cardano_drep_unref> owner(native);

  checkCardanoError(status, "DRep fromString");
  DRep result(owner.release());
  validate(result.type(), result.credential());
  return result;
}

DRepType
DRep::type() const
{
  cardano_drep_type_t value = CARDANO_DREP_TYPE_KEY_HASH;
  checkCardanoError(cardano_drep_get_type(checked(), &value), "DRep type");
  return static_cast<DRepType>(value);
}

std::optional<Credential>
DRep::credential() const
{
  const DRepType kind = type();
  if (kind == DRepType::Abstain || kind == DRepType::NoConfidence)
  {
    return std::nullopt;
  }

  cardano_credential_t*                                         value  = nullptr;
  const cardano_error_t                                         status = cardano_drep_get_credential(checked(), &value);
  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(value);

  checkCardanoError(status, "DRep credential");
  Credential copied(owner.release());
  return Credential(copied);
}

void
DRep::setCredential(const Credential& value)
{
  validate(type(), value);

  Credential copy(value);
  checkCardanoError(cardano_drep_set_credential(checked(), copy.checked()), "DRep setCredential");
}

void
DRep::setType(DRepType type, std::optional<Credential> value)
{
  DRep replacement(type, std::move(value));
  swap(replacement);
}

std::vector<std::uint8_t>
DRep::toCbor() const
{
  return encodeCbor(checked(), cardano_drep_to_cbor);
}

std::string
DRep::toString() const
{
  if (type() != DRepType::KeyHash && type() != DRepType::ScriptHash)
  {
    throw std::logic_error("Predefined DRep choices have no bech32 identifier");
  }

  const std::size_t size = cardano_drep_get_string_size(checked());
  std::string       value(size, '\0');
  checkCardanoError(cardano_drep_to_string(checked(), value.data(), value.size()), "DRep toString");
  if (!value.empty() && value.back() == '\0')
  {
    value.pop_back();
  }
  return value;
}

std::string
DRep::toJson() const
{
  return encodeJson(checked(), cardano_drep_to_cip116_json);
}

void
DRep::swap(DRep& other) noexcept
{
  std::swap(m_value, other.m_value);
}

DRep&
DRep::operator=(const DRep& other)
{
  if (this != &other)
  {
    DRep copy(other);
    swap(copy);
  }
  return *this;
}

DRep&
DRep::operator=(DRep&& other) noexcept
{
  if (this != &other)
  {
    cardano_drep_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
DRep::operator==(const DRep& other) const
{
  return toCbor() == other.toCbor();
}

bool
DRep::operator!=(const DRep& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

DRep::DRep(cardano_drep_t* value) noexcept
: m_value(value)
{
}

cardano_drep_t*
DRep::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from DRep");
  }
  return m_value;
}

void
DRep::validate(DRepType type, const std::optional<Credential>& value)
{
  switch (type)
  {
    case DRepType::KeyHash:
      if (value && value->type() == CredentialType::KeyHash)
      {
        return;
      }
      break;
    case DRepType::ScriptHash:
      if (value && value->type() == CredentialType::ScriptHash)
      {
        return;
      }
      break;
    case DRepType::Abstain:
    case DRepType::NoConfidence:
      if (!value)
      {
        return;
      }
      break;
  }

  throw std::invalid_argument("DRep type and credential do not match");
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(DRep& lhs, DRep& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
