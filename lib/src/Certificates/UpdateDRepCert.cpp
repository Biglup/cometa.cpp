/**
 * @file UpdateDRepCert.cpp
 *
 * @brief DRep metadata update certificates.
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

#include <Cometa/Certificates/UpdateDRepCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/update_drep_cert.h>
#include <cardano/common/anchor.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
UpdateDRepCert::UpdateDRepCert(const Credential& credential, std::optional<Anchor> anchor)
: m_value(nullptr)
{
  const Credential                                                          copy(credential);
  cardano_update_drep_cert_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_update_drep_cert_new(checkedNativeHandle(copy), anchor ? checkedNativeHandle(*anchor) : nullptr, &value);
  CardanoObject<cardano_update_drep_cert_t, cardano_update_drep_cert_unref> owner(value);

  checkCardanoError(status, "UpdateDRepCert");
  m_value = owner.release();
}

UpdateDRepCert::UpdateDRepCert(const UpdateDRepCert& other)
: m_value(nullptr)
{
  cardano_update_drep_cert_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_update_drep_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_update_drep_cert_t, cardano_update_drep_cert_unref> owner(value);

  checkCardanoError(status, "UpdateDRepCert copy");
  m_value = owner.release();
}

UpdateDRepCert::UpdateDRepCert(UpdateDRepCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

UpdateDRepCert::~UpdateDRepCert()
{
  cardano_update_drep_cert_unref(&m_value);
}

UpdateDRepCert
UpdateDRepCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return UpdateDRepCert(decodeCbor<cardano_update_drep_cert_t, cardano_update_drep_cert_unref>(
    cbor, cardano_update_drep_cert_from_cbor));
}

UpdateDRepCert
UpdateDRepCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Credential
UpdateDRepCert::credential() const
{
  cardano_credential_t* value = cardano_update_drep_cert_get_credential(checked());
  if (!value)
  {
    throw std::logic_error("DRep update certificate has no credential");
  }

  const Credential result = Credential::adopt(value);
  return Credential(result);
}

void
UpdateDRepCert::setCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_update_drep_cert_set_credential(checked(), checkedNativeHandle(copy)), "UpdateDRepCert setCredential");
}

std::optional<Anchor>
UpdateDRepCert::anchor() const
{
  cardano_anchor_t* value = cardano_update_drep_cert_get_anchor(checked());
  if (!value)
  {
    return std::nullopt;
  }

  const Anchor result = Anchor::adopt(value);
  return Anchor(result);
}

void
UpdateDRepCert::setAnchor(std::optional<Anchor> value)
{
  UpdateDRepCert replacement(credential(), std::move(value));
  swap(replacement);
}

std::vector<std::uint8_t>
UpdateDRepCert::toCbor() const
{
  return encodeCbor(checked(), cardano_update_drep_cert_to_cbor);
}

std::string
UpdateDRepCert::toJson() const
{
  return encodeJson(checked(), cardano_update_drep_cert_to_cip116_json);
}

void
UpdateDRepCert::swap(UpdateDRepCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

UpdateDRepCert&
UpdateDRepCert::operator=(const UpdateDRepCert& other)
{
  if (this != &other)
  {
    UpdateDRepCert copy(other);
    swap(copy);
  }

  return *this;
}

UpdateDRepCert&
UpdateDRepCert::operator=(UpdateDRepCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_update_drep_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
UpdateDRepCert::operator==(const UpdateDRepCert& other) const
{
  return credential() == other.credential() && anchor() == other.anchor();
}

bool
UpdateDRepCert::operator!=(const UpdateDRepCert& other) const
{
  return !(*this == other);
}

UpdateDRepCert
UpdateDRepCert::adopt(cardano_update_drep_cert_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null UpdateDRepCert");
  }

  return UpdateDRepCert(value);
}

cardano_update_drep_cert_t*
UpdateDRepCert::nativeHandle() noexcept
{
  return m_value;
}

const cardano_update_drep_cert_t*
UpdateDRepCert::nativeHandle() const noexcept
{
  return m_value;
}

cardano_update_drep_cert_t*
UpdateDRepCert::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

UpdateDRepCert::UpdateDRepCert(cardano_update_drep_cert_t* value) noexcept
: m_value(value)
{
}

cardano_update_drep_cert_t*
UpdateDRepCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from UpdateDRepCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(UpdateDRepCert& lhs, UpdateDRepCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
