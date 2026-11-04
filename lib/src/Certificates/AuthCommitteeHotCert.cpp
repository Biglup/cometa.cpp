/**
 * @file AuthCommitteeHotCert.cpp
 *
 * @brief Committee hot-key authorization certificates.
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

#include <Cometa/Certificates/AuthCommitteeHotCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/auth_committee_hot_cert.h>
#include <cardano/common/credential.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
AuthCommitteeHotCert::AuthCommitteeHotCert(const Credential& coldCredential, const Credential& hotCredential)
: m_value(nullptr)
{
  const Credential                                                                        coldCopy(coldCredential);
  const Credential                                                                        hotCopy(hotCredential);
  cardano_auth_committee_hot_cert_t*                                                      value  = nullptr;
  const cardano_error_t                                                                   status = cardano_auth_committee_hot_cert_new(checkedNativeHandle(coldCopy), checkedNativeHandle(hotCopy), &value);
  CardanoObject<cardano_auth_committee_hot_cert_t, cardano_auth_committee_hot_cert_unref> owner(value);

  checkCardanoError(status, "AuthCommitteeHotCert");
  m_value = owner.release();
}

AuthCommitteeHotCert::AuthCommitteeHotCert(const AuthCommitteeHotCert& other)
: m_value(nullptr)
{
  cardano_auth_committee_hot_cert_t*                                                      value  = nullptr;
  const cardano_error_t                                                                   status = cardano_auth_committee_hot_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_auth_committee_hot_cert_t, cardano_auth_committee_hot_cert_unref> owner(value);

  checkCardanoError(status, "AuthCommitteeHotCert copy");
  m_value = owner.release();
}

AuthCommitteeHotCert::AuthCommitteeHotCert(AuthCommitteeHotCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

AuthCommitteeHotCert::~AuthCommitteeHotCert()
{
  cardano_auth_committee_hot_cert_unref(&m_value);
}

AuthCommitteeHotCert
AuthCommitteeHotCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return AuthCommitteeHotCert(decodeCbor<cardano_auth_committee_hot_cert_t, cardano_auth_committee_hot_cert_unref>(
    cbor, cardano_auth_committee_hot_cert_from_cbor));
}

AuthCommitteeHotCert
AuthCommitteeHotCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Credential
AuthCommitteeHotCert::coldCredential() const
{
  cardano_credential_t*                                         value  = nullptr;
  const cardano_error_t                                         status = cardano_auth_committee_hot_cert_get_cold_cred(checked(), &value);
  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(value);

  checkCardanoError(status, "AuthCommitteeHotCert coldCredential");
  const Credential result = Credential::adopt(owner.release());
  return Credential(result);
}

void
AuthCommitteeHotCert::setColdCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_auth_committee_hot_cert_set_cold_cred(checked(), checkedNativeHandle(copy)), "AuthCommitteeHotCert setColdCredential");
}

Credential
AuthCommitteeHotCert::hotCredential() const
{
  cardano_credential_t*                                         value  = nullptr;
  const cardano_error_t                                         status = cardano_auth_committee_hot_cert_get_hot_cred(checked(), &value);
  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(value);

  checkCardanoError(status, "AuthCommitteeHotCert hotCredential");
  const Credential result = Credential::adopt(owner.release());
  return Credential(result);
}

void
AuthCommitteeHotCert::setHotCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_auth_committee_hot_cert_set_hot_cred(checked(), checkedNativeHandle(copy)), "AuthCommitteeHotCert setHotCredential");
}

std::vector<std::uint8_t>
AuthCommitteeHotCert::toCbor() const
{
  return encodeCbor(checked(), cardano_auth_committee_hot_cert_to_cbor);
}

std::string
AuthCommitteeHotCert::toJson() const
{
  return encodeJson(checked(), cardano_auth_committee_hot_cert_to_cip116_json);
}

void
AuthCommitteeHotCert::swap(AuthCommitteeHotCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

AuthCommitteeHotCert&
AuthCommitteeHotCert::operator=(const AuthCommitteeHotCert& other)
{
  if (this != &other)
  {
    AuthCommitteeHotCert copy(other);
    swap(copy);
  }

  return *this;
}

AuthCommitteeHotCert&
AuthCommitteeHotCert::operator=(AuthCommitteeHotCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_auth_committee_hot_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
AuthCommitteeHotCert::operator==(const AuthCommitteeHotCert& other) const
{
  return coldCredential() == other.coldCredential() && hotCredential() == other.hotCredential();
}

bool
AuthCommitteeHotCert::operator!=(const AuthCommitteeHotCert& other) const
{
  return !(*this == other);
}

AuthCommitteeHotCert
AuthCommitteeHotCert::adopt(cardano_auth_committee_hot_cert_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null AuthCommitteeHotCert");
  }

  return AuthCommitteeHotCert(value);
}

cardano_auth_committee_hot_cert_t*
AuthCommitteeHotCert::nativeHandle() noexcept
{
  return m_value;
}

const cardano_auth_committee_hot_cert_t*
AuthCommitteeHotCert::nativeHandle() const noexcept
{
  return m_value;
}

cardano_auth_committee_hot_cert_t*
AuthCommitteeHotCert::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

AuthCommitteeHotCert::AuthCommitteeHotCert(cardano_auth_committee_hot_cert_t* value) noexcept
: m_value(value)
{
}

cardano_auth_committee_hot_cert_t*
AuthCommitteeHotCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from AuthCommitteeHotCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(AuthCommitteeHotCert& lhs, AuthCommitteeHotCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
