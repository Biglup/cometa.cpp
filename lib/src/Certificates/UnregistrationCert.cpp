/**
 * @file UnregistrationCert.cpp
 *
 * @brief Unregistration certificates.
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

#include <Cometa/Certificates/UnregistrationCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/unregistration_cert.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
UnregistrationCert::UnregistrationCert(const Credential& credential, std::uint64_t deposit)
: m_value(nullptr)
{
  const Credential                                                                copy(credential);
  cardano_unregistration_cert_t*                                                  value  = nullptr;
  const cardano_error_t                                                           status = cardano_unregistration_cert_new(checkedNativeHandle(copy), deposit, &value);
  CardanoObject<cardano_unregistration_cert_t, cardano_unregistration_cert_unref> owner(value);

  checkCardanoError(status, "UnregistrationCert");
  m_value = owner.release();
}

UnregistrationCert::UnregistrationCert(const UnregistrationCert& other)
: m_value(nullptr)
{
  cardano_unregistration_cert_t*                                                  value  = nullptr;
  const cardano_error_t                                                           status = cardano_unregistration_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_unregistration_cert_t, cardano_unregistration_cert_unref> owner(value);

  checkCardanoError(status, "UnregistrationCert copy");
  m_value = owner.release();
}

UnregistrationCert::UnregistrationCert(UnregistrationCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

UnregistrationCert::~UnregistrationCert()
{
  cardano_unregistration_cert_unref(&m_value);
}

UnregistrationCert
UnregistrationCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return UnregistrationCert(decodeCbor<cardano_unregistration_cert_t, cardano_unregistration_cert_unref>(
    cbor, cardano_unregistration_cert_from_cbor));
}

UnregistrationCert
UnregistrationCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Credential
UnregistrationCert::credential() const
{
  cardano_credential_t* value = cardano_unregistration_cert_get_credential(checked());
  if (!value)
  {
    throw std::logic_error("Unregistration certificate has no credential");
  }

  const Credential result = Credential::adopt(value);
  return Credential(result);
}

void
UnregistrationCert::setCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_unregistration_cert_set_credential(checked(), checkedNativeHandle(copy)), "UnregistrationCert setCredential");
}

std::uint64_t
UnregistrationCert::deposit() const
{
  return cardano_unregistration_cert_get_deposit(checked());
}

void
UnregistrationCert::setDeposit(std::uint64_t value)
{
  checkCardanoError(cardano_unregistration_cert_set_deposit(checked(), value), "UnregistrationCert setDeposit");
}

std::vector<std::uint8_t>
UnregistrationCert::toCbor() const
{
  return encodeCbor(checked(), cardano_unregistration_cert_to_cbor);
}

std::string
UnregistrationCert::toJson() const
{
  return encodeJson(checked(), cardano_unregistration_cert_to_cip116_json);
}

void
UnregistrationCert::swap(UnregistrationCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

UnregistrationCert&
UnregistrationCert::operator=(const UnregistrationCert& other)
{
  if (this != &other)
  {
    UnregistrationCert copy(other);
    swap(copy);
  }

  return *this;
}

UnregistrationCert&
UnregistrationCert::operator=(UnregistrationCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_unregistration_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
UnregistrationCert::operator==(const UnregistrationCert& other) const
{
  return credential() == other.credential() && deposit() == other.deposit();
}

bool
UnregistrationCert::operator!=(const UnregistrationCert& other) const
{
  return !(*this == other);
}

UnregistrationCert
UnregistrationCert::adopt(cardano_unregistration_cert_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null UnregistrationCert");
  }

  return UnregistrationCert(value);
}

cardano_unregistration_cert_t*
UnregistrationCert::nativeHandle() noexcept
{
  return m_value;
}

const cardano_unregistration_cert_t*
UnregistrationCert::nativeHandle() const noexcept
{
  return m_value;
}

cardano_unregistration_cert_t*
UnregistrationCert::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

UnregistrationCert::UnregistrationCert(cardano_unregistration_cert_t* value) noexcept
: m_value(value)
{
}

cardano_unregistration_cert_t*
UnregistrationCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from UnregistrationCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(UnregistrationCert& lhs, UnregistrationCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
