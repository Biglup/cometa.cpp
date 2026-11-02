/**
 * @file RegisterDRepCert.cpp
 *
 * @brief DRep registration certificates.
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

#include <Cometa/Certificates/RegisterDRepCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/register_drep_cert.h>
#include <cardano/common/anchor.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
RegisterDRepCert::RegisterDRepCert(const Credential& credential, std::uint64_t deposit, std::optional<Anchor> anchor)
: m_value(nullptr)
{
  const Credential                                                              copy(credential);
  cardano_register_drep_cert_t*                                                 value  = nullptr;
  const cardano_error_t                                                         status = cardano_register_drep_cert_new(copy.checked(), deposit, anchor ? anchor->checked() : nullptr, &value);
  CardanoObject<cardano_register_drep_cert_t, cardano_register_drep_cert_unref> owner(value);

  checkCardanoError(status, "RegisterDRepCert");
  m_value = owner.release();
}

RegisterDRepCert::RegisterDRepCert(const RegisterDRepCert& other)
: m_value(nullptr)
{
  cardano_register_drep_cert_t*                                                 value  = nullptr;
  const cardano_error_t                                                         status = cardano_register_drep_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_register_drep_cert_t, cardano_register_drep_cert_unref> owner(value);

  checkCardanoError(status, "RegisterDRepCert copy");
  m_value = owner.release();
}

RegisterDRepCert::RegisterDRepCert(RegisterDRepCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

RegisterDRepCert::~RegisterDRepCert()
{
  cardano_register_drep_cert_unref(&m_value);
}

RegisterDRepCert
RegisterDRepCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return RegisterDRepCert(decodeCbor<cardano_register_drep_cert_t, cardano_register_drep_cert_unref>(
    cbor, cardano_register_drep_cert_from_cbor));
}

RegisterDRepCert
RegisterDRepCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Credential
RegisterDRepCert::credential() const
{
  cardano_credential_t* value = cardano_register_drep_cert_get_credential(checked());
  if (!value)
  {
    throw std::logic_error("DRep registration certificate has no credential");
  }

  const Credential result(value);
  return Credential(result);
}

void
RegisterDRepCert::setCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_register_drep_cert_set_credential(checked(), copy.checked()), "RegisterDRepCert setCredential");
}

std::uint64_t
RegisterDRepCert::deposit() const
{
  return cardano_register_drep_cert_get_deposit(checked());
}

void
RegisterDRepCert::setDeposit(std::uint64_t value)
{
  checkCardanoError(cardano_register_drep_cert_set_deposit(checked(), value), "RegisterDRepCert setDeposit");
}

std::optional<Anchor>
RegisterDRepCert::anchor() const
{
  cardano_anchor_t* value = cardano_register_drep_cert_get_anchor(checked());
  if (!value)
  {
    return std::nullopt;
  }

  const Anchor result(value);
  return Anchor(result);
}

void
RegisterDRepCert::setAnchor(std::optional<Anchor> value)
{
  RegisterDRepCert replacement(credential(), deposit(), std::move(value));
  swap(replacement);
}

std::vector<std::uint8_t>
RegisterDRepCert::toCbor() const
{
  return encodeCbor(checked(), cardano_register_drep_cert_to_cbor);
}

std::string
RegisterDRepCert::toJson() const
{
  return encodeJson(checked(), cardano_register_drep_cert_to_cip116_json);
}

void
RegisterDRepCert::swap(RegisterDRepCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

RegisterDRepCert&
RegisterDRepCert::operator=(const RegisterDRepCert& other)
{
  if (this != &other)
  {
    RegisterDRepCert copy(other);
    swap(copy);
  }

  return *this;
}

RegisterDRepCert&
RegisterDRepCert::operator=(RegisterDRepCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_register_drep_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
RegisterDRepCert::operator==(const RegisterDRepCert& other) const
{
  return credential() == other.credential() && deposit() == other.deposit() && anchor() == other.anchor();
}

bool
RegisterDRepCert::operator!=(const RegisterDRepCert& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

RegisterDRepCert::RegisterDRepCert(cardano_register_drep_cert_t* value) noexcept
: m_value(value)
{
}

cardano_register_drep_cert_t*
RegisterDRepCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from RegisterDRepCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(RegisterDRepCert& lhs, RegisterDRepCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
