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

#include <cometa/AssetName.h>

#include "Native.h"

#include <cardano/assets/asset_name.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
AssetName::AssetName(const std::vector<std::uint8_t>& bytes)
: m_value(
    Detail::create<cardano_asset_name_t, cardano_asset_name_unref>(
      [&](cardano_asset_name_t** out)
      {
        return cardano_asset_name_from_bytes(Detail::data(bytes), bytes.size(), out);
      },
      "AssetName from bytes"))
{
}

AssetName::AssetName(const AssetName& other)
: m_value(Detail::clone<cardano_asset_name_t, cardano_asset_name_unref>(other.checked(), cardano_asset_name_deep_clone))
{
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
  Detail::validateHex(hex);
  return AssetName(
    Detail::create<cardano_asset_name_t, cardano_asset_name_unref>(
      [&](cardano_asset_name_t** out)
      {
        return cardano_asset_name_from_hex(hex.empty() ? "" : hex.data(), hex.size(), out);
      },
      "AssetName fromHex"));
}

AssetName
AssetName::fromString(std::string_view text)
{
  return AssetName(
    Detail::create<cardano_asset_name_t, cardano_asset_name_unref>(
      [&](cardano_asset_name_t** out)
      {
        return cardano_asset_name_from_string(text.empty() ? "" : text.data(), text.size(), out);
      },
      "AssetName fromString"));
}

AssetName
AssetName::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return AssetName(Detail::decode<cardano_asset_name_t, cardano_asset_name_unref>(cbor, cardano_asset_name_from_cbor));
}

std::vector<std::uint8_t>
AssetName::toBytes() const
{
  const auto* value = checked();
  const auto* bytes = cardano_asset_name_get_bytes(value);
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
  const auto bytes = toBytes();
  return { bytes.begin(), bytes.end() };
}

std::vector<std::uint8_t>
AssetName::toCbor() const
{
  return Detail::encode(checked(), cardano_asset_name_to_cbor);
}

std::string
AssetName::toJson() const
{
  return Detail::json(checked(), cardano_asset_name_to_cip116_json);
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
