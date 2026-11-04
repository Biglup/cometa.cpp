/**
 * @file MirCert.cpp
 *
 * @brief Historical MIR certificate variants.
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

#include <Cometa/Certificates/MirCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/mir_cert.h>
#include <cardano/certs/mir_to_pot_cert.h>
#include <cardano/certs/mir_to_stake_creds_cert.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
MirCert::MirCert(const MirToPotCert& value)
: m_value(nullptr)
{
  MirToPotCert                                              copy(value);
  cardano_mir_cert_t*                                       native = nullptr;
  const cardano_error_t                                     status = cardano_mir_cert_new_to_other_pot(checkedNativeHandle(copy), &native);
  CardanoObject<cardano_mir_cert_t, cardano_mir_cert_unref> owner(native);

  checkCardanoError(status, "MirCert pot transfer");
  m_value = owner.release();
}

MirCert::MirCert(MirToPotCert&& value)
: m_value(nullptr)
{
  cardano_mir_cert_t*                                       native = nullptr;
  const cardano_error_t                                     status = cardano_mir_cert_new_to_other_pot(checkedNativeHandle(value), &native);
  CardanoObject<cardano_mir_cert_t, cardano_mir_cert_unref> owner(native);

  checkCardanoError(status, "MirCert pot transfer");
  m_value = owner.release();

  MirToPotCert consumed(std::move(value));
}

MirCert::MirCert(const MirToStakeCredentialsCert& value)
: m_value(nullptr)
{
  MirToStakeCredentialsCert                                 copy(value);
  cardano_mir_cert_t*                                       native = nullptr;
  const cardano_error_t                                     status = cardano_mir_cert_new_to_stake_creds(checkedNativeHandle(copy), &native);
  CardanoObject<cardano_mir_cert_t, cardano_mir_cert_unref> owner(native);

  checkCardanoError(status, "MirCert stake distribution");
  m_value = owner.release();
}

MirCert::MirCert(MirToStakeCredentialsCert&& value)
: m_value(nullptr)
{
  cardano_mir_cert_t*                                       native = nullptr;
  const cardano_error_t                                     status = cardano_mir_cert_new_to_stake_creds(checkedNativeHandle(value), &native);
  CardanoObject<cardano_mir_cert_t, cardano_mir_cert_unref> owner(native);

  checkCardanoError(status, "MirCert stake distribution");
  m_value = owner.release();

  MirToStakeCredentialsCert consumed(std::move(value));
}

MirCert::MirCert(const MirCert& other)
: m_value(nullptr)
{
  cardano_mir_cert_t*                                       native = nullptr;
  const cardano_error_t                                     status = cardano_mir_cert_deep_clone(other.checked(), &native);
  CardanoObject<cardano_mir_cert_t, cardano_mir_cert_unref> owner(native);

  checkCardanoError(status, "MirCert copy");
  m_value = owner.release();
}

MirCert::MirCert(MirCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

MirCert::~MirCert()
{
  cardano_mir_cert_unref(&m_value);
}

MirCert
MirCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  MirCert result(decodeCbor<cardano_mir_cert_t, cardano_mir_cert_unref>(cbor, cardano_mir_cert_from_cbor));

  if (result.type() != MirCertType::ToPot && result.type() != MirCertType::ToStakeCredentials)
  {
    throw std::invalid_argument("Unknown MIR certificate type");
  }

  return result;
}

MirCert
MirCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

MirCertType
MirCert::type() const
{
  cardano_mir_cert_type_t value = CARDANO_MIR_CERT_TYPE_TO_POT;
  checkCardanoError(cardano_mir_cert_get_type(checked(), &value), "MirCert type");
  return static_cast<MirCertType>(value);
}

MirToPotCert
MirCert::asToPot() const
{
  if (type() != MirCertType::ToPot)
  {
    throw std::logic_error("MIR certificate is not a pot transfer");
  }

  cardano_mir_to_pot_cert_t*                                              native = nullptr;
  const cardano_error_t                                                   status = cardano_mir_cert_as_to_other_pot(checked(), &native);
  CardanoObject<cardano_mir_to_pot_cert_t, cardano_mir_to_pot_cert_unref> owner(native);

  checkCardanoError(status, "MirCert asToPot");
  MirToPotCert value = MirToPotCert::adopt(owner.release());
  return MirToPotCert(value);
}

MirToStakeCredentialsCert
MirCert::asToStakeCredentials() const
{
  if (type() != MirCertType::ToStakeCredentials)
  {
    throw std::logic_error("MIR certificate is not a stake distribution");
  }

  cardano_mir_to_stake_creds_cert_t*                                                      native = nullptr;
  const cardano_error_t                                                                   status = cardano_mir_cert_as_to_stake_creds(checked(), &native);
  CardanoObject<cardano_mir_to_stake_creds_cert_t, cardano_mir_to_stake_creds_cert_unref> owner(native);

  checkCardanoError(status, "MirCert asToStakeCredentials");
  MirToStakeCredentialsCert value = MirToStakeCredentialsCert::adopt(owner.release());
  return MirToStakeCredentialsCert(value);
}

std::vector<std::uint8_t>
MirCert::toCbor() const
{
  return encodeCbor(checked(), cardano_mir_cert_to_cbor);
}

std::string
MirCert::toJson() const
{
  return encodeJson(checked(), cardano_mir_cert_to_cip116_json);
}

void
MirCert::swap(MirCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

MirCert&
MirCert::operator=(const MirCert& other)
{
  if (this != &other)
  {
    MirCert copy(other);
    swap(copy);
  }
  return *this;
}

MirCert&
MirCert::operator=(MirCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_mir_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
MirCert::operator==(const MirCert& other) const
{
  return toCbor() == other.toCbor();
}

bool
MirCert::operator!=(const MirCert& other) const
{
  return !(*this == other);
}

MirCert
MirCert::adopt(cardano_mir_cert_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null MirCert");
  }

  return MirCert(value);
}

cardano_mir_cert_t*
MirCert::nativeHandle() noexcept
{
  return m_value;
}

const cardano_mir_cert_t*
MirCert::nativeHandle() const noexcept
{
  return m_value;
}

cardano_mir_cert_t*
MirCert::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

MirCert::MirCert(cardano_mir_cert_t* value) noexcept
: m_value(value)
{
}

cardano_mir_cert_t*
MirCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from MirCert");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(MirCert& lhs, MirCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
