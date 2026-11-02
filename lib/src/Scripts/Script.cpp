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

#include <cardano/scripts/native_scripts/native_script.h>
#include <cardano/scripts/plutus_scripts/plutus_v1_script.h>
#include <cardano/scripts/plutus_scripts/plutus_v2_script.h>
#include <cardano/scripts/plutus_scripts/plutus_v3_script.h>
#include <cardano/scripts/plutus_scripts/plutus_v4_script.h>
#include <cardano/scripts/script.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Script::Script(const NativeScript& script)
: m_value(nullptr)
{
  NativeScript copy(script);

  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_native(copy.checked(), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();
}

Script::Script(const PlutusV1Script& script)
: m_value(nullptr)
{
  PlutusV1Script copy(script);

  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_plutus_v1(copy.checked(), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();
}

Script::Script(const PlutusV2Script& script)
: m_value(nullptr)
{
  PlutusV2Script copy(script);

  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_plutus_v2(copy.checked(), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();
}

Script::Script(const PlutusV3Script& script)
: m_value(nullptr)
{
  PlutusV3Script copy(script);

  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_plutus_v3(copy.checked(), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();
}

Script::Script(const PlutusV4Script& script)
: m_value(nullptr)
{
  PlutusV4Script copy(script);

  cardano_script_t*     value  = nullptr;
  const cardano_error_t result = cardano_script_new_plutus_v4(copy.checked(), &value);

  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  checkCardanoError(result, "Script");

  m_value = owner.release();
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

  value.validate();

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

  const NativeScript component(*owner.release());

  return NativeScript(component);
}

PlutusV1Script
Script::asPlutusV1() const
{
  cardano_plutus_v1_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_script_to_plutus_v1(checked(), &value);

  CardanoObject<cardano_plutus_v1_script_t, cardano_plutus_v1_script_unref> owner(value);

  checkCardanoError(result, "Script asPlutusV1");

  const PlutusV1Script component(*owner.release());

  return PlutusV1Script(component);
}

PlutusV2Script
Script::asPlutusV2() const
{
  cardano_plutus_v2_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_script_to_plutus_v2(checked(), &value);

  CardanoObject<cardano_plutus_v2_script_t, cardano_plutus_v2_script_unref> owner(value);

  checkCardanoError(result, "Script asPlutusV2");

  const PlutusV2Script component(*owner.release());

  return PlutusV2Script(component);
}

PlutusV3Script
Script::asPlutusV3() const
{
  cardano_plutus_v3_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_script_to_plutus_v3(checked(), &value);

  CardanoObject<cardano_plutus_v3_script_t, cardano_plutus_v3_script_unref> owner(value);

  checkCardanoError(result, "Script asPlutusV3");

  const PlutusV3Script component(*owner.release());

  return PlutusV3Script(component);
}

PlutusV4Script
Script::asPlutusV4() const
{
  cardano_plutus_v4_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_script_to_plutus_v4(checked(), &value);

  CardanoObject<cardano_plutus_v4_script_t, cardano_plutus_v4_script_unref> owner(value);

  checkCardanoError(result, "Script asPlutusV4");

  const PlutusV4Script component(*owner.release());

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
Script::validate() const
{
  if (language() == ScriptLanguage::Native)
  {
    asNative().validate();
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Script& lhs, Script& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
