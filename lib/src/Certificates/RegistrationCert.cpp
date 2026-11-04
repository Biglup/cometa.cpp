/**
 * @file RegistrationCert.cpp
 *
 * @brief Registration certificates.
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

#include <Cometa/Certificates/RegistrationCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/registration_cert.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
RegistrationCert::RegistrationCert(const Credential& credential, std::uint64_t deposit)
: m_value(nullptr)
{
  const Credential                                                            copy(credential);
  cardano_registration_cert_t*                                                value  = nullptr;
  const cardano_error_t                                                       status = cardano_registration_cert_new(checkedNativeHandle(copy), deposit, &value);
  CardanoObject<cardano_registration_cert_t, cardano_registration_cert_unref> owner(value);

  checkCardanoError(status, "RegistrationCert");
  m_value = owner.release();
}

RegistrationCert::RegistrationCert(const RegistrationCert& other)
: m_value(nullptr)
{
  cardano_registration_cert_t*                                                value  = nullptr;
  const cardano_error_t                                                       status = cardano_registration_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_registration_cert_t, cardano_registration_cert_unref> owner(value);

  checkCardanoError(status, "RegistrationCert copy");
  m_value = owner.release();
}

RegistrationCert::RegistrationCert(RegistrationCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

RegistrationCert::~RegistrationCert()
{
  cardano_registration_cert_unref(&m_value);
}

RegistrationCert
RegistrationCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return RegistrationCert(decodeCbor<cardano_registration_cert_t, cardano_registration_cert_unref>(
    cbor, cardano_registration_cert_from_cbor));
}

RegistrationCert
RegistrationCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Credential
RegistrationCert::credential() const
{
  cardano_credential_t* value = cardano_registration_cert_get_stake_credential(checked());
  if (!value)
  {
    throw std::logic_error("Registration certificate has no credential");
  }

  const Credential result = Credential::adopt(value);
  return Credential(result);
}

void
RegistrationCert::setCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_registration_cert_set_stake_credential(checked(), checkedNativeHandle(copy)), "RegistrationCert setCredential");
}

std::uint64_t
RegistrationCert::deposit() const
{
  return cardano_registration_cert_get_deposit(checked());
}

void
RegistrationCert::setDeposit(std::uint64_t value)
{
  checkCardanoError(cardano_registration_cert_set_deposit(checked(), value), "RegistrationCert setDeposit");
}

std::vector<std::uint8_t>
RegistrationCert::toCbor() const
{
  return encodeCbor(checked(), cardano_registration_cert_to_cbor);
}

std::string
RegistrationCert::toJson() const
{
  return encodeJson(checked(), cardano_registration_cert_to_cip116_json);
}

void
RegistrationCert::swap(RegistrationCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

RegistrationCert&
RegistrationCert::operator=(const RegistrationCert& other)
{
  if (this != &other)
  {
    RegistrationCert copy(other);
    swap(copy);
  }

  return *this;
}

RegistrationCert&
RegistrationCert::operator=(RegistrationCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_registration_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
RegistrationCert::operator==(const RegistrationCert& other) const
{
  return credential() == other.credential() && deposit() == other.deposit();
}

bool
RegistrationCert::operator!=(const RegistrationCert& other) const
{
  return !(*this == other);
}

RegistrationCert
RegistrationCert::adopt(cardano_registration_cert_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null RegistrationCert");
  }

  return RegistrationCert(value);
}

cardano_registration_cert_t*
RegistrationCert::nativeHandle() noexcept
{
  return m_value;
}

const cardano_registration_cert_t*
RegistrationCert::nativeHandle() const noexcept
{
  return m_value;
}

cardano_registration_cert_t*
RegistrationCert::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

RegistrationCert::RegistrationCert(cardano_registration_cert_t* value) noexcept
: m_value(value)
{
}

cardano_registration_cert_t*
RegistrationCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from RegistrationCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(RegistrationCert& lhs, RegistrationCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
