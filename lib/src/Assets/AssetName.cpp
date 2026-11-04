/**
 * @file AssetName.cpp
 *
 * @brief Implementation of the AssetName value type.
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

#include <Cometa/Assets/AssetName.h>

#include "../CardanoInterop.h"

#include <cardano/assets/asset_name.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
AssetName::AssetName(const std::vector<std::uint8_t>& bytes)
: m_value(nullptr)
{
  cardano_asset_name_t* value  = nullptr;
  const cardano_error_t result = cardano_asset_name_from_bytes(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_asset_name_t, cardano_asset_name_unref> owner(value);

  checkCardanoError(result, "AssetName");

  m_value = owner.release();
}

AssetName::AssetName(const AssetName& other)
: m_value(nullptr)
{
  cardano_asset_name_t* value  = nullptr;
  const cardano_error_t result = cardano_asset_name_deep_clone(other.checked(), &value);

  CardanoObject<cardano_asset_name_t, cardano_asset_name_unref> owner(value);

  checkCardanoError(result, "AssetName copy");

  m_value = owner.release();
}

AssetName::AssetName(AssetName&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

AssetName::~AssetName()
{
  cardano_asset_name_unref(&m_value);
}

AssetName
AssetName::fromHex(std::string_view hex)
{
  validateHex(hex);

  cardano_asset_name_t* value  = nullptr;
  const cardano_error_t result = cardano_asset_name_from_hex(hex.empty() ? "" : hex.data(), hex.size(), &value);

  CardanoObject<cardano_asset_name_t, cardano_asset_name_unref> owner(value);

  checkCardanoError(result, "AssetName fromHex");

  return AssetName(owner.release());
}

AssetName
AssetName::fromString(std::string_view text)
{
  cardano_asset_name_t* value  = nullptr;
  const cardano_error_t result = cardano_asset_name_from_string(text.empty() ? "" : text.data(), text.size(), &value);

  CardanoObject<cardano_asset_name_t, cardano_asset_name_unref> owner(value);

  checkCardanoError(result, "AssetName fromString");

  return AssetName(owner.release());
}

AssetName
AssetName::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return AssetName(decodeCbor<cardano_asset_name_t, cardano_asset_name_unref>(cbor, cardano_asset_name_from_cbor));
}

AssetName
AssetName::fromCbor(const std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::vector<std::uint8_t>
AssetName::toBytes() const
{
  const cardano_asset_name_t* value = checked();
  const std::uint8_t*         bytes = cardano_asset_name_get_bytes(value);

  return { bytes, bytes + cardano_asset_name_get_bytes_size(value) };
}

std::string
AssetName::toHex() const
{
  return cardano_asset_name_get_hex(checked());
}

std::string
AssetName::toString() const
{
  const std::vector<std::uint8_t> bytes = toBytes();

  return { bytes.begin(), bytes.end() };
}

std::vector<std::uint8_t>
AssetName::toCbor() const
{
  return encodeCbor(checked(), cardano_asset_name_to_cbor);
}

std::string
AssetName::toJson() const
{
  return encodeJson(checked(), cardano_asset_name_to_cip116_json);
}

void
AssetName::swap(AssetName& other) noexcept
{
  std::swap(m_value, other.m_value);
}

AssetName&
AssetName::operator=(const AssetName& other)
{
  if (this != &other)
  {
    AssetName copy(other);

    swap(copy);
  }

  return *this;
}

AssetName&
AssetName::operator=(AssetName&& other) noexcept
{
  if (this != &other)
  {
    cardano_asset_name_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
AssetName::operator==(const AssetName& other) const
{
  return toBytes() == other.toBytes();
}

bool
AssetName::operator!=(const AssetName& other) const
{
  return !(*this == other);
}

bool
AssetName::operator<(const AssetName& other) const
{
  return toBytes() < other.toBytes();
}

AssetName
AssetName::adopt(cardano_asset_name_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null AssetName");
  }

  return AssetName(value);
}

cardano_asset_name_t*
AssetName::nativeHandle() noexcept
{
  return m_value;
}

const cardano_asset_name_t*
AssetName::nativeHandle() const noexcept
{
  return m_value;
}

cardano_asset_name_t*
AssetName::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

AssetName::AssetName(cardano_asset_name_t* value) noexcept
: m_value(value)
{
}

cardano_asset_name_t*
AssetName::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from AssetName");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(AssetName& lhs, AssetName& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
