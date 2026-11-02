/**
 * @file PlutusV4Script.cpp
 *
 * @brief A compiled Plutus V4 script.
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

#include <Cometa/Scripts/PlutusScripts/PlutusV4Script.h>

#include "../../CardanoInterop.h"
#include <cardano/uplc/uplc_apply_params.h>

#include <cardano/scripts/plutus_scripts/plutus_v4_script.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusV4Script::PlutusV4Script(const std::vector<std::uint8_t>& bytes)
: m_value(nullptr)
{
  if (bytes.empty())
  {
    throw std::invalid_argument("Compiled script bytes must not be empty");
  }

  cardano_plutus_v4_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_plutus_v4_script_new_bytes(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_plutus_v4_script_t, cardano_plutus_v4_script_unref> owner(value);

  checkCardanoError(result, "PlutusV4Script");

  m_value = owner.release();
}

PlutusV4Script::PlutusV4Script(const PlutusV4Script& other)
: m_value(nullptr)
{
  cardano_plutus_v4_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_plutus_v4_script_deep_clone(other.checked(), &value);

  CardanoObject<cardano_plutus_v4_script_t, cardano_plutus_v4_script_unref> owner(value);

  checkCardanoError(result, "PlutusV4Script");

  m_value = owner.release();
}

PlutusV4Script::PlutusV4Script(PlutusV4Script&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PlutusV4Script::~PlutusV4Script()
{
  cardano_plutus_v4_script_unref(&m_value);
}

PlutusV4Script
PlutusV4Script::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return PlutusV4Script(*decodeCbor<cardano_plutus_v4_script_t, cardano_plutus_v4_script_unref>(cbor, cardano_plutus_v4_script_from_cbor));
}

PlutusV4Script
PlutusV4Script::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

PlutusV4Script
PlutusV4Script::fromHex(std::string_view hex)
{
  return PlutusV4Script(decodeHex(hex));
}

std::vector<std::uint8_t>
PlutusV4Script::toBytes() const
{
  cardano_buffer_t*     value  = nullptr;
  const cardano_error_t result = cardano_plutus_v4_script_to_raw_bytes(checked(), &value);

  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);

  checkCardanoError(result, "PlutusV4Script toBytes");

  const std::uint8_t* bytes  = cardano_buffer_get_data(value);
  const std::size_t   length = cardano_buffer_get_size(value);

  return std::vector<std::uint8_t>(bytes, bytes + length);
}

std::string
PlutusV4Script::toHex() const
{
  const std::vector<std::uint8_t> bytes    = toBytes();
  static const char               DIGITS[] = "0123456789abcdef";
  std::string                     hex;
  hex.reserve(bytes.size() * 2U);

  for (const std::uint8_t byte: bytes)
  {
    hex.push_back(DIGITS[byte >> 4U]);
    hex.push_back(DIGITS[byte & 15U]);
  }

  return hex;
}

PlutusLanguageVersion
PlutusV4Script::languageVersion() const
{
  checked();

  return PlutusLanguageVersion::V4;
}

Blake2bHash
PlutusV4Script::hash() const
{
  std::vector<std::uint8_t> bytes = toBytes();
  bytes.insert(bytes.begin(), 4U);

  return Blake2bHash::compute(bytes, 28U);
}

PlutusV4Script
PlutusV4Script::applyParameters(const PlutusList& parameters) const
{
  const std::vector<std::uint8_t> bytes  = toBytes();
  cardano_buffer_t*               value  = nullptr;
  const cardano_error_t           result = cardano_uplc_apply_params_to_script(parameters.checked(), byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);

  checkCardanoError(result, "PlutusV4Script applyParameters");

  const std::uint8_t* output = cardano_buffer_get_data(value);
  const std::size_t   length = cardano_buffer_get_size(value);

  return PlutusV4Script(std::vector<std::uint8_t>(output, output + length));
}

std::vector<std::uint8_t>
PlutusV4Script::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_v4_script_to_cbor);
}

std::string
PlutusV4Script::toJson() const
{
  return encodeJson(checked(), cardano_plutus_v4_script_to_cip116_json);
}

void
PlutusV4Script::swap(PlutusV4Script& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PlutusV4Script&
PlutusV4Script::operator=(const PlutusV4Script& other)
{
  if (this != &other)
  {
    PlutusV4Script copy(other);

    swap(copy);
  }

  return *this;
}

PlutusV4Script&
PlutusV4Script::operator=(PlutusV4Script&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_v4_script_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
PlutusV4Script::operator==(const PlutusV4Script& other) const
{
  return cardano_plutus_v4_script_equals(checked(), other.checked());
}

bool
PlutusV4Script::operator!=(const PlutusV4Script& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusV4Script::PlutusV4Script(cardano_plutus_v4_script_t& value) noexcept
: m_value(&value)
{
}

cardano_plutus_v4_script_t*
PlutusV4Script::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusV4Script");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusV4Script& lhs, PlutusV4Script& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
