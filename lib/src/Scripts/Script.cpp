/**
 * @file Script.cpp
 *
 * @brief A native or Plutus script with its ledger language.
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

#include <Cometa/Scripts/Script.h>

#include "../CardanoInterop.h"
#include "../ValueValidation.h"

#include <cardano/scripts/native_scripts/native_script.h>
#include <cardano/scripts/plutus_scripts/plutus_v1_script.h>
#include <cardano/scripts/plutus_scripts/plutus_v2_script.h>
#include <cardano/scripts/plutus_scripts/plutus_v3_script.h>
#include <cardano/scripts/plutus_scripts/plutus_v4_script.h>
#include <cardano/scripts/script.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Script::Script(const ScriptPubkey& value)
: Script(NativeScript(value))
{
}

Script::Script(const ScriptAll& value)
: Script(NativeScript(value))
{
}

Script::Script(const ScriptAny& value)
: Script(NativeScript(value))
{
}

Script::Script(const ScriptNOfK& value)
: Script(NativeScript(value))
{
}

Script::Script(const ScriptInvalidBefore& value)
: Script(NativeScript(value))
{
}

Script::Script(const ScriptInvalidAfter& value)
: Script(NativeScript(value))
{
}

Script::Script(const ScriptRequireGuard& value)
: Script(NativeScript(value))
{
}

Script::Script(const NativeScript& script)
: m_value(nullptr)
{
  NativeScript copy(script);

  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_native(checkedNativeHandle(copy), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();
}

Script::Script(NativeScript&& script)
: m_value(nullptr)
{
  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_native(checkedNativeHandle(script), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();

  NativeScript consumed(std::move(script));
}

Script::Script(const PlutusV1Script& script)
: m_value(nullptr)
{
  PlutusV1Script copy(script);

  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_plutus_v1(checkedNativeHandle(copy), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();
}

Script::Script(PlutusV1Script&& script)
: m_value(nullptr)
{
  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_plutus_v1(checkedNativeHandle(script), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();

  PlutusV1Script consumed(std::move(script));
}

Script::Script(const PlutusV2Script& script)
: m_value(nullptr)
{
  PlutusV2Script copy(script);

  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_plutus_v2(checkedNativeHandle(copy), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();
}

Script::Script(PlutusV2Script&& script)
: m_value(nullptr)
{
  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_plutus_v2(checkedNativeHandle(script), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();

  PlutusV2Script consumed(std::move(script));
}

Script::Script(const PlutusV3Script& script)
: m_value(nullptr)
{
  PlutusV3Script copy(script);

  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_plutus_v3(checkedNativeHandle(copy), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();
}

Script::Script(PlutusV3Script&& script)
: m_value(nullptr)
{
  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_plutus_v3(checkedNativeHandle(script), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();

  PlutusV3Script consumed(std::move(script));
}

Script::Script(const PlutusV4Script& script)
: m_value(nullptr)
{
  PlutusV4Script copy(script);

  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_plutus_v4(checkedNativeHandle(copy), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();
}

Script::Script(PlutusV4Script&& script)
: m_value(nullptr)
{
  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_plutus_v4(checkedNativeHandle(script), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();

  PlutusV4Script consumed(std::move(script));
}

Script::Script(const Script& other)
: m_value(nullptr)
{
  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_deep_clone(other.checked(), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();
}

Script::Script(Script&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Script::~Script()
{
  cardano_script_unref(&m_value);
}

Script
Script::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  Script value(*decodeCbor<cardano_script_t, cardano_script_unref>(cbor, cardano_script_from_cbor));

  validateValue(value);

  return value;
}

Script
Script::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

ScriptLanguage
Script::language() const
{
  cardano_script_language_t value = CARDANO_SCRIPT_LANGUAGE_NATIVE;
  checkCardanoError(cardano_script_get_language(checked(), &value), "Script language");

  return static_cast<ScriptLanguage>(value);
}

NativeScript
Script::asNative() const
{
  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_script_to_native(checked(), &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "Script asNative");

  const NativeScript component = NativeScript::adopt(owner.release());

  return NativeScript(component);
}

PlutusV1Script
Script::asPlutusV1() const
{
  cardano_plutus_v1_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_script_to_plutus_v1(checked(), &value);

  CardanoObject<cardano_plutus_v1_script_t, cardano_plutus_v1_script_unref> owner(value);

  checkCardanoError(result, "Script asPlutusV1");

  const PlutusV1Script component = PlutusV1Script::adopt(owner.release());

  return PlutusV1Script(component);
}

PlutusV2Script
Script::asPlutusV2() const
{
  cardano_plutus_v2_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_script_to_plutus_v2(checked(), &value);

  CardanoObject<cardano_plutus_v2_script_t, cardano_plutus_v2_script_unref> owner(value);

  checkCardanoError(result, "Script asPlutusV2");

  const PlutusV2Script component = PlutusV2Script::adopt(owner.release());

  return PlutusV2Script(component);
}

PlutusV3Script
Script::asPlutusV3() const
{
  cardano_plutus_v3_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_script_to_plutus_v3(checked(), &value);

  CardanoObject<cardano_plutus_v3_script_t, cardano_plutus_v3_script_unref> owner(value);

  checkCardanoError(result, "Script asPlutusV3");

  const PlutusV3Script component = PlutusV3Script::adopt(owner.release());

  return PlutusV3Script(component);
}

PlutusV4Script
Script::asPlutusV4() const
{
  cardano_plutus_v4_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_script_to_plutus_v4(checked(), &value);

  CardanoObject<cardano_plutus_v4_script_t, cardano_plutus_v4_script_unref> owner(value);

  checkCardanoError(result, "Script asPlutusV4");

  const PlutusV4Script component = PlutusV4Script::adopt(owner.release());

  return PlutusV4Script(component);
}

Blake2bHash
Script::hash() const
{
  switch (language())
  {
    case ScriptLanguage::Native:
      return asNative().hash();
    case ScriptLanguage::PlutusV1:
      return asPlutusV1().hash();
    case ScriptLanguage::PlutusV2:
      return asPlutusV2().hash();
    case ScriptLanguage::PlutusV3:
      return asPlutusV3().hash();
    case ScriptLanguage::PlutusV4:
      return asPlutusV4().hash();
  }

  throw std::logic_error("Invalid script language");
}

std::vector<std::uint8_t>
Script::toCbor() const
{
  return encodeCbor(checked(), cardano_script_to_cbor);
}

std::string
Script::toJson() const
{
  return encodeJson(checked(), cardano_script_to_cip116_json);
}

void
Script::swap(Script& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Script&
Script::operator=(const Script& other)
{
  if (this != &other)
  {
    Script copy(other);

    swap(copy);
  }

  return *this;
}

Script&
Script::operator=(Script&& other) noexcept
{
  if (this != &other)
  {
    cardano_script_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
Script::operator==(const Script& other) const
{
  return cardano_script_equals(checked(), other.checked());
}

bool
Script::operator!=(const Script& other) const
{
  return !(*this == other);
}

Script
Script::adopt(cardano_script_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null Script");
  }

  return Script(*value);
}

cardano_script_t*
Script::nativeHandle() noexcept
{
  return m_value;
}

const cardano_script_t*
Script::nativeHandle() const noexcept
{
  return m_value;
}

cardano_script_t*
Script::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

Script::Script(cardano_script_t& value) noexcept
: m_value(&value)
{
}

cardano_script_t*
Script::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Script");
  }

  return m_value;
}

void
validateValue(const Script& value)
{
  if (value.language() == ScriptLanguage::Native)
  {
    validateValue(value.asNative());
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Script& lhs, Script& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
