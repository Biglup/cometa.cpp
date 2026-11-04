/**
 * @file MirToStakeCredentialsCert.cpp
 *
 * @brief Historical instantaneous rewards to stake credentials.
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

#include <Cometa/Certificates/MirToStakeCredentialsCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/mir_to_stake_creds_cert.h>
#include <cardano/common/credential.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
MirToStakeCredentialsCert::MirToStakeCredentialsCert(MirPotType pot)
: m_value(nullptr)
{
  validatePot(pot);

  cardano_mir_to_stake_creds_cert_t*                                                      value  = nullptr;
  const cardano_error_t                                                                   status = cardano_mir_to_stake_creds_cert_new(static_cast<cardano_mir_cert_pot_type_t>(pot), &value);
  CardanoObject<cardano_mir_to_stake_creds_cert_t, cardano_mir_to_stake_creds_cert_unref> owner(value);

  checkCardanoError(status, "MirToStakeCredentialsCert");
  m_value = owner.release();
}

MirToStakeCredentialsCert::MirToStakeCredentialsCert(const MirToStakeCredentialsCert& other)
: m_value(nullptr)
{
  cardano_mir_to_stake_creds_cert_t*                                                      value  = nullptr;
  const cardano_error_t                                                                   status = cardano_mir_to_stake_creds_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_mir_to_stake_creds_cert_t, cardano_mir_to_stake_creds_cert_unref> owner(value);

  checkCardanoError(status, "MirToStakeCredentialsCert copy");
  m_value = owner.release();
}

MirToStakeCredentialsCert::MirToStakeCredentialsCert(MirToStakeCredentialsCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

MirToStakeCredentialsCert::~MirToStakeCredentialsCert()
{
  cardano_mir_to_stake_creds_cert_unref(&m_value);
}

MirToStakeCredentialsCert
MirToStakeCredentialsCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  MirToStakeCredentialsCert result(decodeCbor<cardano_mir_to_stake_creds_cert_t, cardano_mir_to_stake_creds_cert_unref>(cbor, cardano_mir_to_stake_creds_cert_from_cbor));
  validatePot(result.pot());
  return result;
}

MirToStakeCredentialsCert
MirToStakeCredentialsCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

MirPotType
MirToStakeCredentialsCert::pot() const
{
  cardano_mir_cert_pot_type_t value = CARDANO_MIR_CERT_POT_TYPE_RESERVE;
  checkCardanoError(cardano_mir_to_stake_creds_cert_get_pot(checked(), &value), "MirToStakeCredentialsCert pot");
  return static_cast<MirPotType>(value);
}

void
MirToStakeCredentialsCert::setPot(MirPotType value)
{
  validatePot(value);
  checkCardanoError(cardano_mir_to_stake_creds_cert_set_pot(checked(), static_cast<cardano_mir_cert_pot_type_t>(value)), "MirToStakeCredentialsCert setPot");
}

std::size_t
MirToStakeCredentialsCert::size() const
{
  return cardano_mir_to_stake_creds_cert_get_size(checked());
}

void
MirToStakeCredentialsCert::insert(const Credential& credential, std::uint64_t amount)
{
  checkCardanoError(cardano_mir_to_stake_creds_cert_insert(checked(), checkedNativeHandle(credential), amount), "MirToStakeCredentialsCert insert");
}

std::pair<Credential, std::uint64_t>
MirToStakeCredentialsCert::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("MIR credential index is out of range");
  }

  cardano_credential_t*                                         value  = nullptr;
  std::uint64_t                                                 amount = 0;
  const cardano_error_t                                         status = cardano_mir_to_stake_creds_cert_get_key_value_at(checked(), index, &value, &amount);
  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(value);

  checkCardanoError(status, "MirToStakeCredentialsCert at");
  Credential copied = Credential::adopt(owner.release());
  return std::make_pair(Credential(copied), amount);
}

std::vector<std::uint8_t>
MirToStakeCredentialsCert::toCbor() const
{
  return encodeCbor(checked(), cardano_mir_to_stake_creds_cert_to_cbor);
}

std::string
MirToStakeCredentialsCert::toJson() const
{
  return encodeJson(checked(), cardano_mir_to_stake_creds_cert_to_cip116_json);
}

void
MirToStakeCredentialsCert::swap(MirToStakeCredentialsCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

MirToStakeCredentialsCert&
MirToStakeCredentialsCert::operator=(const MirToStakeCredentialsCert& other)
{
  if (this != &other)
  {
    MirToStakeCredentialsCert copy(other);
    swap(copy);
  }
  return *this;
}

MirToStakeCredentialsCert&
MirToStakeCredentialsCert::operator=(MirToStakeCredentialsCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_mir_to_stake_creds_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
MirToStakeCredentialsCert::operator==(const MirToStakeCredentialsCert& other) const
{
  return toCbor() == other.toCbor();
}

bool
MirToStakeCredentialsCert::operator!=(const MirToStakeCredentialsCert& other) const
{
  return !(*this == other);
}

MirToStakeCredentialsCert
MirToStakeCredentialsCert::adopt(cardano_mir_to_stake_creds_cert_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null MirToStakeCredentialsCert");
  }

  return MirToStakeCredentialsCert(value);
}

cardano_mir_to_stake_creds_cert_t*
MirToStakeCredentialsCert::nativeHandle() noexcept
{
  return m_value;
}

const cardano_mir_to_stake_creds_cert_t*
MirToStakeCredentialsCert::nativeHandle() const noexcept
{
  return m_value;
}

cardano_mir_to_stake_creds_cert_t*
MirToStakeCredentialsCert::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

MirToStakeCredentialsCert::MirToStakeCredentialsCert(cardano_mir_to_stake_creds_cert_t* value) noexcept
: m_value(value)
{
}

cardano_mir_to_stake_creds_cert_t*
MirToStakeCredentialsCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from MirToStakeCredentialsCert");
  }
  return m_value;
}

void
MirToStakeCredentialsCert::validatePot(MirPotType value)
{
  if (value != MirPotType::Reserve && value != MirPotType::Treasury)
  {
    throw std::invalid_argument("Unknown MIR source pot");
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(MirToStakeCredentialsCert& lhs, MirToStakeCredentialsCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
