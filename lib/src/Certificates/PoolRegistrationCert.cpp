/**
 * @file PoolRegistrationCert.cpp
 *
 * @brief Legacy pool registration certificates.
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

#include <Cometa/Certificates/PoolRegistrationCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/pool_registration_cert.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PoolRegistrationCert::PoolRegistrationCert(const PoolParams& params)
: m_value(nullptr)
{
  const PoolParams                                                                      copy(params);
  cardano_pool_registration_cert_t*                                                     value  = nullptr;
  const cardano_error_t                                                                 status = cardano_pool_registration_cert_new(checkedNativeHandle(copy), &value);
  CardanoObject<cardano_pool_registration_cert_t, cardano_pool_registration_cert_unref> owner(value);

  checkCardanoError(status, "PoolRegistrationCert");
  m_value = owner.release();
}

PoolRegistrationCert::PoolRegistrationCert(const PoolRegistrationCert& other)
: m_value(nullptr)
{
  cardano_pool_registration_cert_t*                                                     value  = nullptr;
  const cardano_error_t                                                                 status = cardano_pool_registration_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_pool_registration_cert_t, cardano_pool_registration_cert_unref> owner(value);

  checkCardanoError(status, "PoolRegistrationCert copy");
  m_value = owner.release();
}

PoolRegistrationCert::PoolRegistrationCert(PoolRegistrationCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PoolRegistrationCert::~PoolRegistrationCert()
{
  cardano_pool_registration_cert_unref(&m_value);
}

PoolRegistrationCert
PoolRegistrationCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return PoolRegistrationCert(decodeCbor<cardano_pool_registration_cert_t, cardano_pool_registration_cert_unref>(
    cbor, cardano_pool_registration_cert_from_cbor));
}

PoolRegistrationCert
PoolRegistrationCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

PoolParams
PoolRegistrationCert::params() const
{
  cardano_pool_params_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_pool_registration_cert_get_params(checked(), &value);
  CardanoObject<cardano_pool_params_t, cardano_pool_params_unref> owner(value);

  checkCardanoError(status, "PoolRegistrationCert params");
  const PoolParams result = PoolParams::adopt(owner.release());
  return PoolParams(result);
}

void
PoolRegistrationCert::setParams(const PoolParams& value)
{
  const PoolParams copy(value);
  checkCardanoError(cardano_pool_registration_cert_set_params(checked(), checkedNativeHandle(copy)), "PoolRegistrationCert setParams");
}

std::vector<std::uint8_t>
PoolRegistrationCert::toCbor() const
{
  return encodeCbor(checked(), cardano_pool_registration_cert_to_cbor);
}

std::string
PoolRegistrationCert::toJson() const
{
  return encodeJson(checked(), cardano_pool_registration_cert_to_cip116_json);
}

void
PoolRegistrationCert::swap(PoolRegistrationCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PoolRegistrationCert&
PoolRegistrationCert::operator=(const PoolRegistrationCert& other)
{
  if (this != &other)
  {
    PoolRegistrationCert copy(other);
    swap(copy);
  }

  return *this;
}

PoolRegistrationCert&
PoolRegistrationCert::operator=(PoolRegistrationCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_pool_registration_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
PoolRegistrationCert::operator==(const PoolRegistrationCert& other) const
{
  return params() == other.params();
}

bool
PoolRegistrationCert::operator!=(const PoolRegistrationCert& other) const
{
  return !(*this == other);
}

PoolRegistrationCert
PoolRegistrationCert::adopt(cardano_pool_registration_cert_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null PoolRegistrationCert");
  }

  return PoolRegistrationCert(value);
}

cardano_pool_registration_cert_t*
PoolRegistrationCert::nativeHandle() noexcept
{
  return m_value;
}

const cardano_pool_registration_cert_t*
PoolRegistrationCert::nativeHandle() const noexcept
{
  return m_value;
}

cardano_pool_registration_cert_t*
PoolRegistrationCert::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

PoolRegistrationCert::PoolRegistrationCert(cardano_pool_registration_cert_t* value) noexcept
: m_value(value)
{
}

cardano_pool_registration_cert_t*
PoolRegistrationCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PoolRegistrationCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PoolRegistrationCert& lhs, PoolRegistrationCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
