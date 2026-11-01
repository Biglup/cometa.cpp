/**
 * \file asset_name.cpp
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
#include <cardano/assets/asset_name.h>
#include <cometa/asset_name.hpp>

namespace Biglup::Cometa
{
AssetName::AssetName(cardano_asset_name_t* value) noexcept
: value_(value)
{
}

AssetName::~AssetName()
{
  cardano_asset_name_unref(&value_);
}

cardano_asset_name_t*
AssetName::checked() const
{
  if (!value_)
  {
    throw std::logic_error("Cannot observe a moved-from AssetName");
  }
  return value_;
}

AssetName::AssetName(const AssetName& other)
: value_(Detail::clone<cardano_asset_name_t, cardano_asset_name_unref>(other.checked(), cardano_asset_name_deep_clone))
{
}

AssetName::AssetName(AssetName&& other) noexcept
: value_(std::exchange(other.value_, nullptr))
{
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
    cardano_asset_name_unref(&value_);
    value_ = std::exchange(other.value_, nullptr);
  }
  return *this;
}

void
AssetName::swap(AssetName& other) noexcept
{
  std::swap(value_, other.value_);
}

void
swap(AssetName& lhs, AssetName& rhs) noexcept
{
  lhs.swap(rhs);
}

AssetName
AssetName::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return AssetName(Detail::decode<cardano_asset_name_t, cardano_asset_name_unref>(cbor, cardano_asset_name_from_cbor));
}

std::vector<std::uint8_t>
AssetName::toCbor() const
{
  return Detail::encode(checked(), cardano_asset_name_to_cbor);
}

bool
AssetName::operator!=(const AssetName& other) const
{
  return !(*this == other);
}

AssetName::AssetName(const std::vector<std::uint8_t>& bytes)
: value_(Detail::create<cardano_asset_name_t, cardano_asset_name_unref>([&](cardano_asset_name_t** out)
                                                                        { return cardano_asset_name_from_bytes(Detail::data(bytes), bytes.size(), out); },
                                                                        "AssetName from bytes"))
{
}

AssetName
AssetName::fromHex(std::string_view hex)
{
  Detail::validateHex(hex);
  return AssetName(Detail::create<cardano_asset_name_t, cardano_asset_name_unref>([&](cardano_asset_name_t** out)
                                                                                  { return cardano_asset_name_from_hex(hex.empty() ? "" : hex.data(), hex.size(), out); },
                                                                                  "AssetName fromHex"));
}

AssetName
AssetName::fromString(std::string_view text)
{
  return AssetName(Detail::create<cardano_asset_name_t, cardano_asset_name_unref>([&](cardano_asset_name_t** out)
                                                                                  { return cardano_asset_name_from_string(text.empty() ? "" : text.data(), text.size(), out); },
                                                                                  "AssetName fromString"));
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

bool
AssetName::operator==(const AssetName& other) const
{
  return toBytes() == other.toBytes();
}

std::string
AssetName::toJson() const
{
  return Detail::json(checked(), cardano_asset_name_to_cip116_json);
}

bool
AssetName::operator<(const AssetName& other) const
{
  return toBytes() < other.toBytes();
}

} // namespace Biglup::Cometa
