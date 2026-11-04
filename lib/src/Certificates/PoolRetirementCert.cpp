/**
 * @file PoolRetirementCert.cpp
 *
 * @brief Pool retirement certificates.
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

#include <Cometa/Certificates/PoolRetirementCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/pool_retirement_cert.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PoolRetirementCert::PoolRetirementCert(const Blake2bHash& poolKeyHash, std::uint64_t epoch)
: m_value(nullptr)
{
  if (poolKeyHash.toBytes().size() != 28U)
  {
    throw std::invalid_argument("Pool key hash must be 28 bytes");
  }

  const Blake2bHash                                                                 copy(poolKeyHash);
  cardano_pool_retirement_cert_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_pool_retirement_cert_new(checkedNativeHandle(copy), epoch, &value);
  CardanoObject<cardano_pool_retirement_cert_t, cardano_pool_retirement_cert_unref> owner(value);

  checkCardanoError(status, "PoolRetirementCert");
  m_value = owner.release();
}

PoolRetirementCert::PoolRetirementCert(const PoolRetirementCert& other)
: m_value(nullptr)
{
  cardano_pool_retirement_cert_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_pool_retirement_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_pool_retirement_cert_t, cardano_pool_retirement_cert_unref> owner(value);

  checkCardanoError(status, "PoolRetirementCert copy");
  m_value = owner.release();
}

PoolRetirementCert::PoolRetirementCert(PoolRetirementCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PoolRetirementCert::~PoolRetirementCert()
{
  cardano_pool_retirement_cert_unref(&m_value);
}

PoolRetirementCert
PoolRetirementCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  PoolRetirementCert result(decodeCbor<cardano_pool_retirement_cert_t, cardano_pool_retirement_cert_unref>(
    cbor, cardano_pool_retirement_cert_from_cbor));
  if (result.poolKeyHash().toBytes().size() != 28U)
  {
    throw std::invalid_argument("Pool key hash must be 28 bytes");
  }

  return result;
}

PoolRetirementCert
PoolRetirementCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Blake2bHash
PoolRetirementCert::poolKeyHash() const
{
  cardano_blake2b_hash_t* value = cardano_pool_retirement_cert_get_pool_key_hash(checked());
  if (!value)
  {
    throw std::logic_error("Pool retirement certificate has no pool key hash");
  }

  const Blake2bHash result = Blake2bHash::adopt(value);
  return Blake2bHash(result);
}

void
PoolRetirementCert::setPoolKeyHash(const Blake2bHash& value)
{
  if (value.toBytes().size() != 28U)
  {
    throw std::invalid_argument("Pool key hash must be 28 bytes");
  }

  const Blake2bHash copy(value);
  checkCardanoError(cardano_pool_retirement_cert_set_pool_key_hash(checked(), checkedNativeHandle(copy)), "PoolRetirementCert setPoolKeyHash");
}

std::uint64_t
PoolRetirementCert::epoch() const
{
  return cardano_pool_retirement_cert_get_epoch(checked());
}

void
PoolRetirementCert::setEpoch(std::uint64_t value)
{
  checkCardanoError(cardano_pool_retirement_cert_set_epoch(checked(), value), "PoolRetirementCert setEpoch");
}

std::vector<std::uint8_t>
PoolRetirementCert::toCbor() const
{
  return encodeCbor(checked(), cardano_pool_retirement_cert_to_cbor);
}

std::string
PoolRetirementCert::toJson() const
{
  return encodeJson(checked(), cardano_pool_retirement_cert_to_cip116_json);
}

void
PoolRetirementCert::swap(PoolRetirementCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PoolRetirementCert&
PoolRetirementCert::operator=(const PoolRetirementCert& other)
{
  if (this != &other)
  {
    PoolRetirementCert copy(other);
    swap(copy);
  }

  return *this;
}

PoolRetirementCert&
PoolRetirementCert::operator=(PoolRetirementCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_pool_retirement_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
PoolRetirementCert::operator==(const PoolRetirementCert& other) const
{
  return poolKeyHash() == other.poolKeyHash() && epoch() == other.epoch();
}

bool
PoolRetirementCert::operator!=(const PoolRetirementCert& other) const
{
  return !(*this == other);
}

PoolRetirementCert
PoolRetirementCert::adopt(cardano_pool_retirement_cert_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null PoolRetirementCert");
  }

  return PoolRetirementCert(value);
}

cardano_pool_retirement_cert_t*
PoolRetirementCert::nativeHandle() noexcept
{
  return m_value;
}

const cardano_pool_retirement_cert_t*
PoolRetirementCert::nativeHandle() const noexcept
{
  return m_value;
}

cardano_pool_retirement_cert_t*
PoolRetirementCert::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

PoolRetirementCert::PoolRetirementCert(cardano_pool_retirement_cert_t* value) noexcept
: m_value(value)
{
}

cardano_pool_retirement_cert_t*
PoolRetirementCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PoolRetirementCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PoolRetirementCert& lhs, PoolRetirementCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
