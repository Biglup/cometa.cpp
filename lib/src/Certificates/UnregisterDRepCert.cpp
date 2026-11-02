/**
 * @file UnregisterDRepCert.cpp
 *
 * @brief DRep unregistration certificates.
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

#include <Cometa/Certificates/UnregisterDRepCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/unregister_drep_cert.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
UnregisterDRepCert::UnregisterDRepCert(const Credential& credential, std::uint64_t deposit)
: m_value(nullptr)
{
  const Credential                                                                  copy(credential);
  cardano_unregister_drep_cert_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_unregister_drep_cert_new(copy.checked(), deposit, &value);
  CardanoObject<cardano_unregister_drep_cert_t, cardano_unregister_drep_cert_unref> owner(value);

  checkCardanoError(status, "UnregisterDRepCert");
  m_value = owner.release();
}

UnregisterDRepCert::UnregisterDRepCert(const UnregisterDRepCert& other)
: m_value(nullptr)
{
  cardano_unregister_drep_cert_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_unregister_drep_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_unregister_drep_cert_t, cardano_unregister_drep_cert_unref> owner(value);

  checkCardanoError(status, "UnregisterDRepCert copy");
  m_value = owner.release();
}

UnregisterDRepCert::UnregisterDRepCert(UnregisterDRepCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

UnregisterDRepCert::~UnregisterDRepCert()
{
  cardano_unregister_drep_cert_unref(&m_value);
}

UnregisterDRepCert
UnregisterDRepCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return UnregisterDRepCert(decodeCbor<cardano_unregister_drep_cert_t, cardano_unregister_drep_cert_unref>(
    cbor, cardano_unregister_drep_cert_from_cbor));
}

UnregisterDRepCert
UnregisterDRepCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Credential
UnregisterDRepCert::credential() const
{
  cardano_credential_t* value = cardano_unregister_drep_cert_get_credential(checked());
  if (!value)
  {
    throw std::logic_error("DRep unregistration certificate has no credential");
  }

  const Credential result(value);
  return Credential(result);
}

void
UnregisterDRepCert::setCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_unregister_drep_cert_set_credential(checked(), copy.checked()), "UnregisterDRepCert setCredential");
}

std::uint64_t
UnregisterDRepCert::deposit() const
{
  return cardano_unregister_drep_cert_get_deposit(checked());
}

void
UnregisterDRepCert::setDeposit(std::uint64_t value)
{
  checkCardanoError(cardano_unregister_drep_cert_set_deposit(checked(), value), "UnregisterDRepCert setDeposit");
}

std::vector<std::uint8_t>
UnregisterDRepCert::toCbor() const
{
  return encodeCbor(checked(), cardano_unregister_drep_cert_to_cbor);
}

std::string
UnregisterDRepCert::toJson() const
{
  return encodeJson(checked(), cardano_unregister_drep_cert_to_cip116_json);
}

void
UnregisterDRepCert::swap(UnregisterDRepCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

UnregisterDRepCert&
UnregisterDRepCert::operator=(const UnregisterDRepCert& other)
{
  if (this != &other)
  {
    UnregisterDRepCert copy(other);
    swap(copy);
  }

  return *this;
}

UnregisterDRepCert&
UnregisterDRepCert::operator=(UnregisterDRepCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_unregister_drep_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
UnregisterDRepCert::operator==(const UnregisterDRepCert& other) const
{
  return credential() == other.credential() && deposit() == other.deposit();
}

bool
UnregisterDRepCert::operator!=(const UnregisterDRepCert& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

UnregisterDRepCert::UnregisterDRepCert(cardano_unregister_drep_cert_t* value) noexcept
: m_value(value)
{
}

cardano_unregister_drep_cert_t*
UnregisterDRepCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from UnregisterDRepCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(UnregisterDRepCert& lhs, UnregisterDRepCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
