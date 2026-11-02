/**
 * @file StakeRegistrationCert.cpp
 *
 * @brief Legacy stake registration certificates.
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

#include <Cometa/Certificates/StakeRegistrationCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/stake_registration_cert.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
StakeRegistrationCert::StakeRegistrationCert(const Credential& credential)
: m_value(nullptr)
{
  const Credential                                                                        copy(credential);
  cardano_stake_registration_cert_t*                                                      value  = nullptr;
  const cardano_error_t                                                                   status = cardano_stake_registration_cert_new(copy.checked(), &value);
  CardanoObject<cardano_stake_registration_cert_t, cardano_stake_registration_cert_unref> owner(value);

  checkCardanoError(status, "StakeRegistrationCert");
  m_value = owner.release();
}

StakeRegistrationCert::StakeRegistrationCert(const StakeRegistrationCert& other)
: m_value(nullptr)
{
  cardano_stake_registration_cert_t*                                                      value  = nullptr;
  const cardano_error_t                                                                   status = cardano_stake_registration_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_stake_registration_cert_t, cardano_stake_registration_cert_unref> owner(value);

  checkCardanoError(status, "StakeRegistrationCert copy");
  m_value = owner.release();
}

StakeRegistrationCert::StakeRegistrationCert(StakeRegistrationCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

StakeRegistrationCert::~StakeRegistrationCert()
{
  cardano_stake_registration_cert_unref(&m_value);
}

StakeRegistrationCert
StakeRegistrationCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return StakeRegistrationCert(decodeCbor<cardano_stake_registration_cert_t, cardano_stake_registration_cert_unref>(
    cbor, cardano_stake_registration_cert_from_cbor));
}

StakeRegistrationCert
StakeRegistrationCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Credential
StakeRegistrationCert::credential() const
{
  cardano_credential_t* value = cardano_stake_registration_cert_get_credential(checked());
  if (!value)
  {
    throw std::logic_error("Stake registration certificate has no credential");
  }

  const Credential result(value);
  return Credential(result);
}

void
StakeRegistrationCert::setCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_stake_registration_cert_set_credential(checked(), copy.checked()), "StakeRegistrationCert setCredential");
}

std::vector<std::uint8_t>
StakeRegistrationCert::toCbor() const
{
  return encodeCbor(checked(), cardano_stake_registration_cert_to_cbor);
}

std::string
StakeRegistrationCert::toJson() const
{
  return encodeJson(checked(), cardano_stake_registration_cert_to_cip116_json);
}

void
StakeRegistrationCert::swap(StakeRegistrationCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

StakeRegistrationCert&
StakeRegistrationCert::operator=(const StakeRegistrationCert& other)
{
  if (this != &other)
  {
    StakeRegistrationCert copy(other);
    swap(copy);
  }

  return *this;
}

StakeRegistrationCert&
StakeRegistrationCert::operator=(StakeRegistrationCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_stake_registration_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
StakeRegistrationCert::operator==(const StakeRegistrationCert& other) const
{
  return credential() == other.credential();
}

bool
StakeRegistrationCert::operator!=(const StakeRegistrationCert& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

StakeRegistrationCert::StakeRegistrationCert(cardano_stake_registration_cert_t* value) noexcept
: m_value(value)
{
}

cardano_stake_registration_cert_t*
StakeRegistrationCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from StakeRegistrationCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(StakeRegistrationCert& lhs, StakeRegistrationCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
