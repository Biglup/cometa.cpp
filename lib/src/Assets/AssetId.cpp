/**
 * @file AssetId.cpp
 *
 * @brief Asset identifier operations.
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

#include <Cometa/Assets/AssetId.h>

#include "../CardanoInterop.h"

#include <cardano/assets/asset_id.h>

#include <algorithm>
#include <cstring>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
AssetId::AssetId(const Blake2bHash& policyId, const AssetName& assetName)
: m_value(nullptr)
{
  const Blake2bHash     policyCopy(policyId);
  const AssetName       nameCopy(assetName);
  cardano_asset_id_t*   value  = nullptr;
  const cardano_error_t result = cardano_asset_id_new(checkedNativeHandle(policyCopy), checkedNativeHandle(nameCopy), &value);

  CardanoObject<cardano_asset_id_t, cardano_asset_id_unref> owner(value);

  checkCardanoError(result, "AssetId");

  m_value = owner.release();
}

AssetId::AssetId(const std::vector<std::uint8_t>& bytes)
: m_value(nullptr)
{
  cardano_asset_id_t*   value  = nullptr;
  const cardano_error_t result = bytes.empty() ? cardano_asset_id_new_lovelace(&value) : cardano_asset_id_from_bytes(bytes.data(), bytes.size(), &value);

  CardanoObject<cardano_asset_id_t, cardano_asset_id_unref> owner(value);

  checkCardanoError(result, "AssetId from bytes");

  m_value = owner.release();
}

AssetId::AssetId(const AssetId& other)
: m_value(nullptr)
{
  cardano_asset_id_t*   value  = nullptr;
  const cardano_error_t result = cardano_asset_id_deep_clone(other.checked(), &value);

  CardanoObject<cardano_asset_id_t, cardano_asset_id_unref> owner(value);

  checkCardanoError(result, "AssetId copy");

  m_value = owner.release();
}

AssetId::AssetId(AssetId&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

AssetId::~AssetId()
{
  cardano_asset_id_unref(&m_value);
}

AssetId
AssetId::lovelace()
{
  return AssetId(std::vector<std::uint8_t> {});
}

AssetId
AssetId::fromHex(std::string_view hex)
{
  return AssetId(decodeHex(hex));
}

bool
AssetId::isLovelace() const
{
  return cardano_asset_id_is_lovelace(checked());
}

std::optional<Blake2bHash>
AssetId::policyId() const
{
  if (isLovelace())
  {
    return std::nullopt;
  }

  const Blake2bHash policy = Blake2bHash::adopt(cardano_asset_id_get_policy_id(checked()));

  return policy;
}

std::optional<AssetName>
AssetId::assetName() const
{
  if (isLovelace())
  {
    return std::nullopt;
  }

  const AssetName name = AssetName::adopt(cardano_asset_id_get_asset_name(checked()));

  return name;
}

std::vector<std::uint8_t>
AssetId::toBytes() const
{
  const std::uint8_t* data = cardano_asset_id_get_bytes(checked());

  return std::vector<std::uint8_t>(data, data + cardano_asset_id_get_bytes_size(checked()));
}

std::string
AssetId::toHex() const
{
  return cardano_asset_id_get_hex(checked());
}

void
AssetId::swap(AssetId& other) noexcept
{
  std::swap(m_value, other.m_value);
}

AssetId&
AssetId::operator=(const AssetId& other)
{
  if (this != &other)
  {
    AssetId copy(other);

    swap(copy);
  }

  return *this;
}

AssetId&
AssetId::operator=(AssetId&& other) noexcept
{
  if (this != &other)
  {
    cardano_asset_id_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
AssetId::operator==(const AssetId& other) const
{
  const std::size_t size = cardano_asset_id_get_bytes_size(checked());

  return size == cardano_asset_id_get_bytes_size(other.checked()) && std::memcmp(cardano_asset_id_get_bytes(checked()), cardano_asset_id_get_bytes(other.checked()), size) == 0;
}

bool
AssetId::operator!=(const AssetId& other) const
{
  return !(*this == other);
}

bool
AssetId::operator<(const AssetId& other) const
{
  const std::uint8_t* left  = cardano_asset_id_get_bytes(checked());
  const std::uint8_t* right = cardano_asset_id_get_bytes(other.checked());

  return std::lexicographical_compare(left, left + cardano_asset_id_get_bytes_size(checked()), right, right + cardano_asset_id_get_bytes_size(other.checked()));
}

AssetId
AssetId::adopt(cardano_asset_id_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null AssetId");
  }

  return AssetId(value);
}

cardano_asset_id_t*
AssetId::nativeHandle() noexcept
{
  return m_value;
}

const cardano_asset_id_t*
AssetId::nativeHandle() const noexcept
{
  return m_value;
}

cardano_asset_id_t*
AssetId::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

AssetId::AssetId(cardano_asset_id_t* value) noexcept
: m_value(value)
{
}

cardano_asset_id_t*
AssetId::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from AssetId");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(AssetId& lhs, AssetId& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
