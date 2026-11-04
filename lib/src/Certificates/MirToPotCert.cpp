/**
 * @file MirToPotCert.cpp
 *
 * @brief Historical MIR certificate values.
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

#include <Cometa/Certificates/MirToPotCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/mir_to_pot_cert.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
MirToPotCert::MirToPotCert(MirPotType pot, std::uint64_t amount)
: m_value(nullptr)
{
  validatePot(pot);

  cardano_mir_to_pot_cert_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_mir_to_pot_cert_new(static_cast<cardano_mir_cert_pot_type_t>(pot), amount, &value);
  CardanoObject<cardano_mir_to_pot_cert_t, cardano_mir_to_pot_cert_unref> owner(value);

  checkCardanoError(status, "MirToPotCert");
  m_value = owner.release();
}

MirToPotCert::MirToPotCert(const MirToPotCert& other)
: m_value(nullptr)
{
  cardano_mir_to_pot_cert_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_mir_to_pot_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_mir_to_pot_cert_t, cardano_mir_to_pot_cert_unref> owner(value);

  checkCardanoError(status, "MirToPotCert copy");
  m_value = owner.release();
}

MirToPotCert::MirToPotCert(MirToPotCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

MirToPotCert::~MirToPotCert()
{
  cardano_mir_to_pot_cert_unref(&m_value);
}

MirToPotCert
MirToPotCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  MirToPotCert result(decodeCbor<cardano_mir_to_pot_cert_t, cardano_mir_to_pot_cert_unref>(cbor, cardano_mir_to_pot_cert_from_cbor));
  validatePot(result.pot());
  return result;
}

MirToPotCert
MirToPotCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

MirPotType
MirToPotCert::pot() const
{
  cardano_mir_cert_pot_type_t value = CARDANO_MIR_CERT_POT_TYPE_RESERVE;
  checkCardanoError(cardano_mir_to_pot_cert_get_pot(checked(), &value), "MirToPotCert pot");
  return static_cast<MirPotType>(value);
}

void
MirToPotCert::setPot(MirPotType value)
{
  validatePot(value);
  checkCardanoError(cardano_mir_to_pot_cert_set_pot(checked(), static_cast<cardano_mir_cert_pot_type_t>(value)), "MirToPotCert setPot");
}

std::uint64_t
MirToPotCert::amount() const
{
  std::uint64_t value = 0;
  checkCardanoError(cardano_mir_to_pot_cert_get_amount(checked(), &value), "MirToPotCert amount");
  return value;
}

void
MirToPotCert::setAmount(std::uint64_t value)
{
  checkCardanoError(cardano_mir_to_pot_cert_set_amount(checked(), value), "MirToPotCert setAmount");
}

std::vector<std::uint8_t>
MirToPotCert::toCbor() const
{
  return encodeCbor(checked(), cardano_mir_to_pot_cert_to_cbor);
}

std::string
MirToPotCert::toJson() const
{
  return encodeJson(checked(), cardano_mir_to_pot_cert_to_cip116_json);
}

void
MirToPotCert::swap(MirToPotCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

MirToPotCert&
MirToPotCert::operator=(const MirToPotCert& other)
{
  if (this != &other)
  {
    MirToPotCert copy(other);
    swap(copy);
  }
  return *this;
}

MirToPotCert&
MirToPotCert::operator=(MirToPotCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_mir_to_pot_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
MirToPotCert::operator==(const MirToPotCert& other) const
{
  return pot() == other.pot() && amount() == other.amount();
}

bool
MirToPotCert::operator!=(const MirToPotCert& other) const
{
  return !(*this == other);
}

MirToPotCert
MirToPotCert::adopt(cardano_mir_to_pot_cert_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null MirToPotCert");
  }

  return MirToPotCert(value);
}

cardano_mir_to_pot_cert_t*
MirToPotCert::nativeHandle() noexcept
{
  return m_value;
}

const cardano_mir_to_pot_cert_t*
MirToPotCert::nativeHandle() const noexcept
{
  return m_value;
}

cardano_mir_to_pot_cert_t*
MirToPotCert::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

MirToPotCert::MirToPotCert(cardano_mir_to_pot_cert_t* value) noexcept
: m_value(value)
{
}

cardano_mir_to_pot_cert_t*
MirToPotCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from MirToPotCert");
  }
  return m_value;
}

void
MirToPotCert::validatePot(MirPotType value)
{
  if (value != MirPotType::Reserve && value != MirPotType::Treasury)
  {
    throw std::invalid_argument("Unknown MIR source pot");
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(MirToPotCert& lhs, MirToPotCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
