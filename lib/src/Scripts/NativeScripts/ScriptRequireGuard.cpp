/**
 * @file ScriptRequireGuard.cpp
 *
 * @brief Requires a top-level guard credential.
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

#include <Cometa/Scripts/NativeScripts/ScriptRequireGuard.h>

#include "../../CardanoInterop.h"
#include <Cometa/Json/JsonValue.h>

#include <cardano/scripts/native_scripts/script_require_guard.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ScriptRequireGuard::ScriptRequireGuard(const Credential& credential)
: m_value(nullptr)
{
  Credential copy(credential);

  cardano_script_require_guard_t* value  = nullptr;
  const cardano_error_t           result = cardano_script_require_guard_new(checkedNativeHandle(copy), &value);

  CardanoObject<cardano_script_require_guard_t, cardano_script_require_guard_unref> owner(value);

  checkCardanoError(result, "ScriptRequireGuard");

  m_value = owner.release();
}

ScriptRequireGuard::ScriptRequireGuard(const ScriptRequireGuard& other)
: m_value(nullptr)
{
  cardano_script_require_guard_t* value  = nullptr;
  const cardano_error_t           result = cardano_script_require_guard_deep_clone(other.checked(), &value);

  CardanoObject<cardano_script_require_guard_t, cardano_script_require_guard_unref> owner(value);

  checkCardanoError(result, "ScriptRequireGuard");

  m_value = owner.release();
}

ScriptRequireGuard::ScriptRequireGuard(ScriptRequireGuard&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ScriptRequireGuard::~ScriptRequireGuard()
{
  cardano_script_require_guard_unref(&m_value);
}

ScriptRequireGuard
ScriptRequireGuard::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  ScriptRequireGuard value(*decodeCbor<cardano_script_require_guard_t, cardano_script_require_guard_unref>(cbor, cardano_script_require_guard_from_cbor));

  return value;
}

ScriptRequireGuard
ScriptRequireGuard::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

ScriptRequireGuard
ScriptRequireGuard::fromJson(std::string_view json)
{
  const JsonValue value = JsonValue::parse(json);

  if (value.at("type").asString() != "guard")
  {
    throw std::invalid_argument("Unexpected native script type");
  }

  const bool hasKey    = value.contains("keyHash");
  const bool hasScript = value.contains("scriptHash");

  if (hasKey == hasScript)
  {
    throw std::invalid_argument("Guard requires exactly one credential hash");
  }

  const CredentialType type = hasKey ? CredentialType::KeyHash : CredentialType::ScriptHash;
  const Blake2bHash    hash = Blake2bHash::fromHex(value.at(hasKey ? "keyHash" : "scriptHash").asString());

  return ScriptRequireGuard(Credential(type, hash));
}

Credential
ScriptRequireGuard::credential() const
{
  cardano_credential_t* value  = nullptr;
  const cardano_error_t result = cardano_script_require_guard_get_credential(checked(), &value);

  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(value);

  checkCardanoError(result, "ScriptRequireGuard credential");

  const Credential component = Credential::adopt(owner.release());

  return Credential(component);
}

void
ScriptRequireGuard::setCredential(const Credential& credential)
{
  Credential copy(credential);

  checkCardanoError(cardano_script_require_guard_set_credential(checked(), checkedNativeHandle(copy)), "ScriptRequireGuard setCredential");
}

std::vector<std::uint8_t>
ScriptRequireGuard::toCbor() const
{
  return encodeCbor(checked(), cardano_script_require_guard_to_cbor);
}

std::string
ScriptRequireGuard::toJson() const
{
  return encodeJson(checked(), cardano_script_require_guard_to_cip116_json);
}

void
ScriptRequireGuard::normalizeEncoding()
{
  cardano_script_require_guard_clear_cbor_cache(checked());
}

void
ScriptRequireGuard::swap(ScriptRequireGuard& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ScriptRequireGuard&
ScriptRequireGuard::operator=(const ScriptRequireGuard& other)
{
  if (this != &other)
  {
    ScriptRequireGuard copy(other);

    swap(copy);
  }

  return *this;
}

ScriptRequireGuard&
ScriptRequireGuard::operator=(ScriptRequireGuard&& other) noexcept
{
  if (this != &other)
  {
    cardano_script_require_guard_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
ScriptRequireGuard::operator==(const ScriptRequireGuard& other) const
{
  return cardano_script_require_guard_equals(checked(), other.checked());
}

bool
ScriptRequireGuard::operator!=(const ScriptRequireGuard& other) const
{
  return !(*this == other);
}

ScriptRequireGuard
ScriptRequireGuard::adopt(cardano_script_require_guard_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null ScriptRequireGuard");
  }

  return ScriptRequireGuard(*value);
}

cardano_script_require_guard_t*
ScriptRequireGuard::nativeHandle() noexcept
{
  return m_value;
}

const cardano_script_require_guard_t*
ScriptRequireGuard::nativeHandle() const noexcept
{
  return m_value;
}

cardano_script_require_guard_t*
ScriptRequireGuard::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

ScriptRequireGuard::ScriptRequireGuard(cardano_script_require_guard_t& value) noexcept
: m_value(&value)
{
}

cardano_script_require_guard_t*
ScriptRequireGuard::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ScriptRequireGuard");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ScriptRequireGuard& lhs, ScriptRequireGuard& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
