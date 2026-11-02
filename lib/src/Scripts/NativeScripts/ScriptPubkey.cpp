/**
 * @file ScriptPubkey.cpp
 *
 * @brief Requires a signature from a payment key.
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

#include <Cometa/Scripts/NativeScripts/ScriptPubkey.h>

#include "../../CardanoInterop.h"
#include <Cometa/Json/JsonValue.h>

#include <cardano/scripts/native_scripts/script_pubkey.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ScriptPubkey::ScriptPubkey(const Blake2bHash& keyHash)
: m_value(nullptr)
{
  Blake2bHash copy(keyHash);

  if (copy.toBytes().size() != 28U)
  {
    throw std::invalid_argument("Script key hash must be 28 bytes");
  }

  cardano_script_pubkey_t* value  = nullptr;
  const cardano_error_t    result = cardano_script_pubkey_new(copy.checked(), &value);

  CardanoObject<cardano_script_pubkey_t, cardano_script_pubkey_unref> owner(value);

  checkCardanoError(result, "ScriptPubkey");

  m_value = owner.release();
}

ScriptPubkey::ScriptPubkey(const ScriptPubkey& other)
: m_value(nullptr)
{
  cardano_script_pubkey_t* value  = nullptr;
  const cardano_error_t    result = cardano_script_pubkey_deep_clone(other.checked(), &value);

  CardanoObject<cardano_script_pubkey_t, cardano_script_pubkey_unref> owner(value);

  checkCardanoError(result, "ScriptPubkey");

  m_value = owner.release();
}

ScriptPubkey::ScriptPubkey(ScriptPubkey&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ScriptPubkey::~ScriptPubkey()
{
  cardano_script_pubkey_unref(&m_value);
}

ScriptPubkey
ScriptPubkey::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  ScriptPubkey value(*decodeCbor<cardano_script_pubkey_t, cardano_script_pubkey_unref>(cbor, cardano_script_pubkey_from_cbor));

  if (value.keyHash().toBytes().size() != 28U)
  {
    throw std::invalid_argument("Script key hash must be 28 bytes");
  }

  return value;
}

ScriptPubkey
ScriptPubkey::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

ScriptPubkey
ScriptPubkey::fromJson(std::string_view json)
{
  const JsonValue value = JsonValue::parse(json);

  if (value.at("type").asString() != "sig")
  {
    throw std::invalid_argument("Unexpected native script type");
  }

  return ScriptPubkey(Blake2bHash::fromHex(value.at("keyHash").asString()));
}

Blake2bHash
ScriptPubkey::keyHash() const
{
  cardano_blake2b_hash_t* value  = nullptr;
  const cardano_error_t   result = cardano_script_pubkey_get_key_hash(checked(), &value);

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  checkCardanoError(result, "ScriptPubkey keyHash");

  const Blake2bHash component(owner.release());

  return Blake2bHash(component);
}

void
ScriptPubkey::setKeyHash(const Blake2bHash& keyHash)
{
  Blake2bHash copy(keyHash);

  if (copy.toBytes().size() != 28U)
  {
    throw std::invalid_argument("Script key hash must be 28 bytes");
  }

  checkCardanoError(cardano_script_pubkey_set_key_hash(checked(), copy.checked()), "ScriptPubkey setKeyHash");
}

std::vector<std::uint8_t>
ScriptPubkey::toCbor() const
{
  return encodeCbor(checked(), cardano_script_pubkey_to_cbor);
}

std::string
ScriptPubkey::toJson() const
{
  return encodeJson(checked(), cardano_script_pubkey_to_cip116_json);
}

void
ScriptPubkey::normalizeEncoding()
{
  cardano_script_pubkey_clear_cbor_cache(checked());
}

void
ScriptPubkey::swap(ScriptPubkey& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ScriptPubkey&
ScriptPubkey::operator=(const ScriptPubkey& other)
{
  if (this != &other)
  {
    ScriptPubkey copy(other);

    swap(copy);
  }

  return *this;
}

ScriptPubkey&
ScriptPubkey::operator=(ScriptPubkey&& other) noexcept
{
  if (this != &other)
  {
    cardano_script_pubkey_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
ScriptPubkey::operator==(const ScriptPubkey& other) const
{
  return cardano_script_pubkey_equals(checked(), other.checked());
}

bool
ScriptPubkey::operator!=(const ScriptPubkey& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

ScriptPubkey::ScriptPubkey(cardano_script_pubkey_t& value) noexcept
: m_value(&value)
{
}

cardano_script_pubkey_t*
ScriptPubkey::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ScriptPubkey");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ScriptPubkey& lhs, ScriptPubkey& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
