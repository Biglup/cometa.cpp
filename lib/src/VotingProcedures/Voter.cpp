/**
 * @file Voter.cpp
 *
 * @brief Governance voter credentials.
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

#include <Cometa/VotingProcedures/Voter.h>

#include "../CardanoInterop.h"

#include <cardano/common/credential.h>
#include <cardano/voting_procedures/voter.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Voter::Voter(VoterType type, const Credential& credential)
: m_value(nullptr)
{
  const Credential                                    copy(credential);
  cardano_voter_t*                                    value  = nullptr;
  const cardano_error_t                               status = cardano_voter_new(static_cast<cardano_voter_type_t>(type), checkedNativeHandle(copy), &value);
  CardanoObject<cardano_voter_t, cardano_voter_unref> owner(value);

  checkCardanoError(status, "Voter");
  m_value = owner.release();
}

Voter::Voter(const Voter& other)
: m_value(nullptr)
{
  cardano_voter_t*                                    value  = nullptr;
  const cardano_error_t                               status = cardano_voter_deep_clone(other.checked(), &value);
  CardanoObject<cardano_voter_t, cardano_voter_unref> owner(value);

  checkCardanoError(status, "Voter copy");
  m_value = owner.release();
}

Voter::Voter(Voter&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Voter::~Voter()
{
  cardano_voter_unref(&m_value);
}

Voter
Voter::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return Voter(decodeCbor<cardano_voter_t, cardano_voter_unref>(cbor, cardano_voter_from_cbor));
}

Voter
Voter::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

VoterType
Voter::type() const
{
  cardano_voter_type_t value = CARDANO_VOTER_TYPE_CONSTITUTIONAL_COMMITTEE_KEY_HASH;
  checkCardanoError(cardano_voter_get_type(checked(), &value), "Voter type");
  return static_cast<VoterType>(value);
}

void
Voter::setType(VoterType value)
{
  checkCardanoError(cardano_voter_set_type(checked(), static_cast<cardano_voter_type_t>(value)), "Voter setType");
}

Credential
Voter::credential() const
{
  cardano_credential_t*                                         value = cardano_voter_get_credential(checked());
  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("Voter has no credential");
  }

  const Credential found = Credential::adopt(owner.release());
  return Credential(found);
}

void
Voter::setCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_voter_set_credential(checked(), checkedNativeHandle(copy)), "Voter setCredential");
}

std::vector<std::uint8_t>
Voter::toCbor() const
{
  return encodeCbor(checked(), cardano_voter_to_cbor);
}

std::string
Voter::toJson() const
{
  return encodeJson(checked(), cardano_voter_to_cip116_json);
}

void
Voter::swap(Voter& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Voter&
Voter::operator=(const Voter& other)
{
  if (this != &other)
  {
    Voter copy(other);
    swap(copy);
  }
  return *this;
}

Voter&
Voter::operator=(Voter&& other) noexcept
{
  if (this != &other)
  {
    cardano_voter_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
Voter::operator==(const Voter& other) const
{
  return cardano_voter_equals(checked(), other.checked());
}

bool
Voter::operator!=(const Voter& other) const
{
  return !(*this == other);
}

bool
Voter::operator<(const Voter& other) const
{
  return cardano_voter_compare(checked(), other.checked()) < 0;
}

Voter
Voter::adopt(cardano_voter_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null Voter");
  }

  return Voter(value);
}

cardano_voter_t*
Voter::nativeHandle() noexcept
{
  return m_value;
}

const cardano_voter_t*
Voter::nativeHandle() const noexcept
{
  return m_value;
}

cardano_voter_t*
Voter::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

Voter::Voter(cardano_voter_t* value) noexcept
: m_value(value)
{
}

cardano_voter_t*
Voter::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Voter");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Voter& lhs, Voter& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
