/**
 * @file CertificateSet.cpp
 *
 * @brief Ledger certificate collections.
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

#include <Cometa/Certificates/CertificateSet.h>

#include "../CardanoInterop.h"

#include <cardano/certs/certificate_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
CertificateSet::CertificateSet()
: m_value(nullptr)
{
  cardano_certificate_set_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_certificate_set_new(&value);
  CardanoObject<cardano_certificate_set_t, cardano_certificate_set_unref> owner(value);

  checkCardanoError(status, "CertificateSet");
  m_value = owner.release();
}

CertificateSet::CertificateSet(const CertificateSet& other)
: m_value(nullptr)
{
  cardano_certificate_set_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_certificate_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_certificate_set_t, cardano_certificate_set_unref> owner(value);

  checkCardanoError(status, "CertificateSet copy");
  m_value = owner.release();
}

CertificateSet::CertificateSet(CertificateSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

CertificateSet::~CertificateSet()
{
  cardano_certificate_set_unref(&m_value);
}

CertificateSet
CertificateSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return CertificateSet(decodeCbor<cardano_certificate_set_t, cardano_certificate_set_unref>(cbor, cardano_certificate_set_from_cbor));
}

CertificateSet
CertificateSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
CertificateSet::size() const
{
  return cardano_certificate_set_get_length(checked());
}

void
CertificateSet::add(const Certificate& value)
{
  const Certificate copy(value);
  checkCardanoError(cardano_certificate_set_add(checked(), copy.checked()), "CertificateSet add");
}

Certificate
CertificateSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Certificate index is out of range");
  }

  cardano_certificate_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_certificate_set_get(checked(), index, &value);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(value);

  checkCardanoError(status, "CertificateSet at");
  const Certificate copied(owner.release());
  return Certificate(copied);
}

std::vector<std::uint8_t>
CertificateSet::toCbor() const
{
  return encodeCbor(checked(), cardano_certificate_set_to_cbor);
}

std::string
CertificateSet::toJson() const
{
  return encodeJson(checked(), cardano_certificate_set_to_cip116_json);
}

void
CertificateSet::swap(CertificateSet& other) noexcept
{
  std::swap(m_value, other.m_value);
}

CertificateSet&
CertificateSet::operator=(const CertificateSet& other)
{
  if (this != &other)
  {
    CertificateSet copy(other);
    swap(copy);
  }
  return *this;
}

CertificateSet&
CertificateSet::operator=(CertificateSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_certificate_set_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
CertificateSet::operator==(const CertificateSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
CertificateSet::operator!=(const CertificateSet& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

CertificateSet::CertificateSet(cardano_certificate_set_t* value) noexcept
: m_value(value)
{
}

cardano_certificate_set_t*
CertificateSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from CertificateSet");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(CertificateSet& lhs, CertificateSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
