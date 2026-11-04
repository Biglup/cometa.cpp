/**
 * @file PoolMetadata.cpp
 *
 * @brief A pool metadata URL and content hash.
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

#include <Cometa/PoolParams/PoolMetadata.h>

#include "../CardanoInterop.h"

#include <cardano/pool_params/pool_metadata.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PoolMetadata::PoolMetadata(std::string_view url, const Blake2bHash& hash)
: m_value(nullptr)
{
  validateUrl(url);
  validateHash(hash);

  Blake2bHash copy(hash);

  cardano_pool_metadata_t* value  = nullptr;
  const cardano_error_t    result = cardano_pool_metadata_new(url.empty() ? "" : url.data(), url.size(), checkedNativeHandle(copy), &value);

  CardanoObject<cardano_pool_metadata_t, cardano_pool_metadata_unref> owner(value);

  checkCardanoError(result, "PoolMetadata");

  m_value = owner.release();
}

PoolMetadata::PoolMetadata(const PoolMetadata& other)
: m_value(nullptr)
{
  cardano_pool_metadata_t* value  = nullptr;
  const cardano_error_t    result = cardano_pool_metadata_deep_clone(other.checked(), &value);

  CardanoObject<cardano_pool_metadata_t, cardano_pool_metadata_unref> owner(value);

  checkCardanoError(result, "PoolMetadata");

  m_value = owner.release();
}

PoolMetadata::PoolMetadata(PoolMetadata&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PoolMetadata::~PoolMetadata()
{
  cardano_pool_metadata_unref(&m_value);
}

PoolMetadata
PoolMetadata::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  PoolMetadata value(*decodeCbor<cardano_pool_metadata_t, cardano_pool_metadata_unref>(cbor, cardano_pool_metadata_from_cbor));

  validateHash(value.hash());

  return value;
}

PoolMetadata
PoolMetadata::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::string
PoolMetadata::url() const
{
  const cardano_pool_metadata_t* value = checked();

  return std::string(cardano_pool_metadata_get_url(value), cardano_pool_metadata_get_url_size(value));
}

void
PoolMetadata::setUrl(std::string_view url)
{
  validateUrl(url);

  checkCardanoError(cardano_pool_metadata_set_url(url.empty() ? "" : url.data(), url.size(), checked()), "PoolMetadata setUrl");
}

Blake2bHash
PoolMetadata::hash() const
{
  cardano_blake2b_hash_t* value  = nullptr;
  const cardano_error_t   result = cardano_pool_metadata_get_hash(checked(), &value);

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  checkCardanoError(result, "PoolMetadata hash");

  const Blake2bHash data = Blake2bHash::adopt(owner.release());

  return Blake2bHash(data);
}

void
PoolMetadata::setHash(const Blake2bHash& hash)
{
  validateHash(hash);

  Blake2bHash copy(hash);

  checkCardanoError(cardano_pool_metadata_set_hash(checked(), checkedNativeHandle(copy)), "PoolMetadata setHash");
}

std::vector<std::uint8_t>
PoolMetadata::toCbor() const
{
  return encodeCbor(checked(), cardano_pool_metadata_to_cbor);
}

std::string
PoolMetadata::toJson() const
{
  return encodeJson(checked(), cardano_pool_metadata_to_cip116_json);
}

void
PoolMetadata::swap(PoolMetadata& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PoolMetadata&
PoolMetadata::operator=(const PoolMetadata& other)
{
  if (this != &other)
  {
    PoolMetadata copy(other);

    swap(copy);
  }

  return *this;
}

PoolMetadata&
PoolMetadata::operator=(PoolMetadata&& other) noexcept
{
  if (this != &other)
  {
    cardano_pool_metadata_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
PoolMetadata::operator==(const PoolMetadata& other) const
{
  return url() == other.url() && hash() == other.hash();
}

bool
PoolMetadata::operator!=(const PoolMetadata& other) const
{
  return !(*this == other);
}

PoolMetadata
PoolMetadata::adopt(cardano_pool_metadata_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null PoolMetadata");
  }

  return PoolMetadata(*value);
}

cardano_pool_metadata_t*
PoolMetadata::nativeHandle() noexcept
{
  return m_value;
}

const cardano_pool_metadata_t*
PoolMetadata::nativeHandle() const noexcept
{
  return m_value;
}

cardano_pool_metadata_t*
PoolMetadata::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

PoolMetadata::PoolMetadata(cardano_pool_metadata_t& value) noexcept
: m_value(&value)
{
}

cardano_pool_metadata_t*
PoolMetadata::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PoolMetadata");
  }

  return m_value;
}

void
PoolMetadata::validateUrl(std::string_view url)
{
  if (url.size() > 128U || url.find('\0') != std::string_view::npos)
  {
    throw std::invalid_argument("Pool metadata URL must contain at most 128 bytes without embedded NULs");
  }
}

void
PoolMetadata::validateHash(const Blake2bHash& hash)
{
  if (hash.toBytes().size() != 32U)
  {
    throw std::invalid_argument("Pool metadata hash must be 32 bytes");
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PoolMetadata& lhs, PoolMetadata& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
