/**
 * @file ResignCommitteeColdCert.cpp
 *
 * @brief Committee cold-key resignation certificates.
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

#include <Cometa/Certificates/ResignCommitteeColdCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/resign_committee_cold_cert.h>
#include <cardano/common/anchor.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ResignCommitteeColdCert::ResignCommitteeColdCert(const Credential& credential, std::optional<Anchor> anchor)
: m_value(nullptr)
{
  const Credential                                                                              copy(credential);
  cardano_resign_committee_cold_cert_t*                                                         value  = nullptr;
  const cardano_error_t                                                                         status = cardano_resign_committee_cold_cert_new(checkedNativeHandle(copy), anchor ? checkedNativeHandle(*anchor) : nullptr, &value);
  CardanoObject<cardano_resign_committee_cold_cert_t, cardano_resign_committee_cold_cert_unref> owner(value);

  checkCardanoError(status, "ResignCommitteeColdCert");
  m_value = owner.release();
}

ResignCommitteeColdCert::ResignCommitteeColdCert(const ResignCommitteeColdCert& other)
: m_value(nullptr)
{
  cardano_resign_committee_cold_cert_t*                                                         value  = nullptr;
  const cardano_error_t                                                                         status = cardano_resign_committee_cold_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_resign_committee_cold_cert_t, cardano_resign_committee_cold_cert_unref> owner(value);

  checkCardanoError(status, "ResignCommitteeColdCert copy");
  m_value = owner.release();
}

ResignCommitteeColdCert::ResignCommitteeColdCert(ResignCommitteeColdCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ResignCommitteeColdCert::~ResignCommitteeColdCert()
{
  cardano_resign_committee_cold_cert_unref(&m_value);
}

ResignCommitteeColdCert
ResignCommitteeColdCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return ResignCommitteeColdCert(decodeCbor<cardano_resign_committee_cold_cert_t, cardano_resign_committee_cold_cert_unref>(
    cbor, cardano_resign_committee_cold_cert_from_cbor));
}

ResignCommitteeColdCert
ResignCommitteeColdCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Credential
ResignCommitteeColdCert::credential() const
{
  cardano_credential_t* value = cardano_resign_committee_cold_cert_get_credential(checked());
  if (!value)
  {
    throw std::logic_error("Committee resignation certificate has no credential");
  }

  const Credential result = Credential::adopt(value);
  return Credential(result);
}

void
ResignCommitteeColdCert::setCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_resign_committee_cold_cert_set_credential(checked(), checkedNativeHandle(copy)), "ResignCommitteeColdCert setCredential");
}

std::optional<Anchor>
ResignCommitteeColdCert::anchor() const
{
  cardano_anchor_t* value = cardano_resign_committee_cold_cert_get_anchor(checked());
  if (!value)
  {
    return std::nullopt;
  }

  const Anchor result = Anchor::adopt(value);
  return Anchor(result);
}

void
ResignCommitteeColdCert::setAnchor(std::optional<Anchor> value)
{
  ResignCommitteeColdCert replacement(credential(), std::move(value));
  swap(replacement);
}

std::vector<std::uint8_t>
ResignCommitteeColdCert::toCbor() const
{
  return encodeCbor(checked(), cardano_resign_committee_cold_cert_to_cbor);
}

std::string
ResignCommitteeColdCert::toJson() const
{
  return encodeJson(checked(), cardano_resign_committee_cold_cert_to_cip116_json);
}

void
ResignCommitteeColdCert::swap(ResignCommitteeColdCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ResignCommitteeColdCert&
ResignCommitteeColdCert::operator=(const ResignCommitteeColdCert& other)
{
  if (this != &other)
  {
    ResignCommitteeColdCert copy(other);
    swap(copy);
  }

  return *this;
}

ResignCommitteeColdCert&
ResignCommitteeColdCert::operator=(ResignCommitteeColdCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_resign_committee_cold_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
ResignCommitteeColdCert::operator==(const ResignCommitteeColdCert& other) const
{
  return credential() == other.credential() && anchor() == other.anchor();
}

bool
ResignCommitteeColdCert::operator!=(const ResignCommitteeColdCert& other) const
{
  return !(*this == other);
}

ResignCommitteeColdCert
ResignCommitteeColdCert::adopt(cardano_resign_committee_cold_cert_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null ResignCommitteeColdCert");
  }

  return ResignCommitteeColdCert(value);
}

cardano_resign_committee_cold_cert_t*
ResignCommitteeColdCert::nativeHandle() noexcept
{
  return m_value;
}

const cardano_resign_committee_cold_cert_t*
ResignCommitteeColdCert::nativeHandle() const noexcept
{
  return m_value;
}

cardano_resign_committee_cold_cert_t*
ResignCommitteeColdCert::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

ResignCommitteeColdCert::ResignCommitteeColdCert(cardano_resign_committee_cold_cert_t* value) noexcept
: m_value(value)
{
}

cardano_resign_committee_cold_cert_t*
ResignCommitteeColdCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ResignCommitteeColdCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ResignCommitteeColdCert& lhs, ResignCommitteeColdCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
