/**
 * @file NativeScript.cpp
 *
 * @brief A signature, timelock, guard, or composite native script.
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

#include <Cometa/Scripts/NativeScripts/NativeScript.h>

#include "../../CardanoInterop.h"
#include <Cometa/Json/JsonValue.h>
#include <Cometa/Scripts/NativeScripts/NativeScriptType.h>
#include <Cometa/Scripts/NativeScripts/ScriptAll.h>
#include <Cometa/Scripts/NativeScripts/ScriptAny.h>
#include <Cometa/Scripts/NativeScripts/ScriptInvalidAfter.h>
#include <Cometa/Scripts/NativeScripts/ScriptInvalidBefore.h>
#include <Cometa/Scripts/NativeScripts/ScriptNOfK.h>
#include <Cometa/Scripts/NativeScripts/ScriptPubkey.h>
#include <Cometa/Scripts/NativeScripts/ScriptRequireGuard.h>

#include <cardano/scripts/native_scripts/native_script.h>
#include <cardano/scripts/native_scripts/script_all.h>
#include <cardano/scripts/native_scripts/script_any.h>
#include <cardano/scripts/native_scripts/script_invalid_after.h>
#include <cardano/scripts/native_scripts/script_invalid_before.h>
#include <cardano/scripts/native_scripts/script_n_of_k.h>
#include <cardano/scripts/native_scripts/script_pubkey.h>
#include <cardano/scripts/native_scripts/script_require_guard.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
NativeScript::NativeScript(const ScriptPubkey& script)
: m_value(nullptr)
{
  ScriptPubkey copy(script);

  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_new_pubkey(copy.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();
}

NativeScript::NativeScript(ScriptPubkey&& script)
: m_value(nullptr)
{
  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_new_pubkey(script.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();

  ScriptPubkey consumed(std::move(script));
}

NativeScript::NativeScript(const ScriptAll& script)
: m_value(nullptr)
{
  ScriptAll copy(script);

  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_new_all(copy.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();
}

NativeScript::NativeScript(ScriptAll&& script)
: m_value(nullptr)
{
  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_new_all(script.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();

  ScriptAll consumed(std::move(script));
}

NativeScript::NativeScript(const ScriptAny& script)
: m_value(nullptr)
{
  ScriptAny copy(script);

  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_new_any(copy.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();
}

NativeScript::NativeScript(ScriptAny&& script)
: m_value(nullptr)
{
  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_new_any(script.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();

  ScriptAny consumed(std::move(script));
}

NativeScript::NativeScript(const ScriptNOfK& script)
: m_value(nullptr)
{
  ScriptNOfK copy(script);

  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_new_n_of_k(copy.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();
}

NativeScript::NativeScript(ScriptNOfK&& script)
: m_value(nullptr)
{
  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_new_n_of_k(script.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();

  ScriptNOfK consumed(std::move(script));
}

NativeScript::NativeScript(const ScriptInvalidBefore& script)
: m_value(nullptr)
{
  ScriptInvalidBefore copy(script);

  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_new_invalid_before(copy.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();
}

NativeScript::NativeScript(ScriptInvalidBefore&& script)
: m_value(nullptr)
{
  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_new_invalid_before(script.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();

  ScriptInvalidBefore consumed(std::move(script));
}

NativeScript::NativeScript(const ScriptInvalidAfter& script)
: m_value(nullptr)
{
  ScriptInvalidAfter copy(script);

  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_new_invalid_after(copy.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();
}

NativeScript::NativeScript(ScriptInvalidAfter&& script)
: m_value(nullptr)
{
  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_new_invalid_after(script.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();

  ScriptInvalidAfter consumed(std::move(script));
}

NativeScript::NativeScript(const ScriptRequireGuard& script)
: m_value(nullptr)
{
  ScriptRequireGuard copy(script);

  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_new_require_guard(copy.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();
}

NativeScript::NativeScript(ScriptRequireGuard&& script)
: m_value(nullptr)
{
  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_new_require_guard(script.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();

  ScriptRequireGuard consumed(std::move(script));
}

NativeScript::NativeScript(const NativeScript& other)
: m_value(nullptr)
{
  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_deep_clone(other.checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScript");

  m_value = owner.release();
}

NativeScript::NativeScript(NativeScript&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

NativeScript::~NativeScript()
{
  cardano_native_script_unref(&m_value);
}

NativeScript
NativeScript::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  NativeScript value(*decodeCbor<cardano_native_script_t, cardano_native_script_unref>(cbor, cardano_native_script_from_cbor));

  value.validate();

  return value;
}

NativeScript
NativeScript::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

NativeScript
NativeScript::fromJson(std::string_view json)
{
  const JsonValue   value    = JsonValue::parse(json);
  const std::string jsonType = value.at("type").asString();

  if (jsonType == "sig")
  {
    return NativeScript(ScriptPubkey::fromJson(json));
  }

  if (jsonType == "all")
  {
    return NativeScript(ScriptAll::fromJson(json));
  }

  if (jsonType == "any")
  {
    return NativeScript(ScriptAny::fromJson(json));
  }

  if (jsonType == "atLeast")
  {
    return NativeScript(ScriptNOfK::fromJson(json));
  }

  if (jsonType == "after")
  {
    return NativeScript(ScriptInvalidBefore::fromJson(json));
  }

  if (jsonType == "before")
  {
    return NativeScript(ScriptInvalidAfter::fromJson(json));
  }

  if (jsonType == "guard")
  {
    return NativeScript(ScriptRequireGuard::fromJson(json));
  }

  throw std::invalid_argument("Unknown native script jsonType");
}

NativeScriptType
NativeScript::type() const
{
  cardano_native_script_type_t value = CARDANO_NATIVE_SCRIPT_TYPE_REQUIRE_PUBKEY;

  checkCardanoError(cardano_native_script_get_type(checked(), &value), "NativeScript type");

  return static_cast<NativeScriptType>(value);
}

ScriptPubkey
NativeScript::asPubkey() const
{
  cardano_script_pubkey_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_to_pubkey(checked(), &value);

  CardanoObject<cardano_script_pubkey_t, cardano_script_pubkey_unref> owner(value);

  checkCardanoError(result, "NativeScript asPubkey");

  const ScriptPubkey script(*owner.release());

  return ScriptPubkey(script);
}

ScriptAll
NativeScript::asAll() const
{
  cardano_script_all_t* value  = nullptr;
  const cardano_error_t result = cardano_native_script_to_all(checked(), &value);

  CardanoObject<cardano_script_all_t, cardano_script_all_unref> owner(value);

  checkCardanoError(result, "NativeScript asAll");

  const ScriptAll script(*owner.release());

  return ScriptAll(script);
}

ScriptAny
NativeScript::asAny() const
{
  cardano_script_any_t* value  = nullptr;
  const cardano_error_t result = cardano_native_script_to_any(checked(), &value);

  CardanoObject<cardano_script_any_t, cardano_script_any_unref> owner(value);

  checkCardanoError(result, "NativeScript asAny");

  const ScriptAny script(*owner.release());

  return ScriptAny(script);
}

ScriptNOfK
NativeScript::asNOfK() const
{
  cardano_script_n_of_k_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_to_n_of_k(checked(), &value);

  CardanoObject<cardano_script_n_of_k_t, cardano_script_n_of_k_unref> owner(value);

  checkCardanoError(result, "NativeScript asNOfK");

  const ScriptNOfK script(*owner.release());

  return ScriptNOfK(script);
}

ScriptInvalidBefore
NativeScript::asInvalidBefore() const
{
  cardano_script_invalid_before_t* value  = nullptr;
  const cardano_error_t            result = cardano_native_script_to_invalid_before(checked(), &value);

  CardanoObject<cardano_script_invalid_before_t, cardano_script_invalid_before_unref> owner(value);

  checkCardanoError(result, "NativeScript asInvalidBefore");

  const ScriptInvalidBefore script(*owner.release());

  return ScriptInvalidBefore(script);
}

ScriptInvalidAfter
NativeScript::asInvalidAfter() const
{
  cardano_script_invalid_after_t* value  = nullptr;
  const cardano_error_t           result = cardano_native_script_to_invalid_after(checked(), &value);

  CardanoObject<cardano_script_invalid_after_t, cardano_script_invalid_after_unref> owner(value);

  checkCardanoError(result, "NativeScript asInvalidAfter");

  const ScriptInvalidAfter script(*owner.release());

  return ScriptInvalidAfter(script);
}

ScriptRequireGuard
NativeScript::asRequireGuard() const
{
  cardano_script_require_guard_t* value  = nullptr;
  const cardano_error_t           result = cardano_native_script_to_require_guard(checked(), &value);

  CardanoObject<cardano_script_require_guard_t, cardano_script_require_guard_unref> owner(value);

  checkCardanoError(result, "NativeScript asRequireGuard");

  const ScriptRequireGuard script(*owner.release());

  return ScriptRequireGuard(script);
}

Blake2bHash
NativeScript::hash() const
{
  std::vector<std::uint8_t> bytes = toCbor();
  bytes.insert(bytes.begin(), 0U);

  return Blake2bHash::compute(bytes, 28U);
}

std::vector<std::uint8_t>
NativeScript::toCbor() const
{
  return encodeCbor(checked(), cardano_native_script_to_cbor);
}

std::string
NativeScript::toJson() const
{
  return encodeJson(checked(), cardano_native_script_to_cip116_json);
}

void
NativeScript::normalizeEncoding()
{
  cardano_native_script_clear_cbor_cache(checked());
}

void
NativeScript::swap(NativeScript& other) noexcept
{
  std::swap(m_value, other.m_value);
}

NativeScript&
NativeScript::operator=(const NativeScript& other)
{
  if (this != &other)
  {
    NativeScript copy(other);

    swap(copy);
  }

  return *this;
}

NativeScript&
NativeScript::operator=(NativeScript&& other) noexcept
{
  if (this != &other)
  {
    cardano_native_script_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
NativeScript::operator==(const NativeScript& other) const
{
  return cardano_native_script_equals(checked(), other.checked());
}

bool
NativeScript::operator!=(const NativeScript& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

NativeScript::NativeScript(cardano_native_script_t& value) noexcept
: m_value(&value)
{
}

cardano_native_script_t*
NativeScript::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from NativeScript");
  }

  return m_value;
}

void
NativeScript::validate() const
{
  switch (type())
  {
    case NativeScriptType::RequirePubkey:
      if (asPubkey().keyHash().toBytes().size() != 28U)
      {
        throw std::invalid_argument("Script key hash must be 28 bytes");
      }
      break;
    case NativeScriptType::RequireAllOf:
      asAll().scripts().validate();
      break;
    case NativeScriptType::RequireAnyOf:
      asAny().scripts().validate();
      break;
    case NativeScriptType::RequireNOfK:
      asNOfK().scripts().validate();
      break;
    case NativeScriptType::InvalidBefore:
    case NativeScriptType::InvalidAfter:
    case NativeScriptType::RequireGuard:
      break;
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(NativeScript& lhs, NativeScript& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
