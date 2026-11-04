/**
 * @file GovernanceActionId.cpp
 *
 * @brief A transaction hash and governance action index.
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

#include <Cometa/Common/GovernanceActionId.h>

#include "../CardanoInterop.h"

#include <cardano/common/governance_action_id.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
GovernanceActionId::GovernanceActionId(const Blake2bHash& hash, std::uint64_t index)
: m_value(nullptr)
{
  const std::vector<std::uint8_t> bytes = hash.toBytes();

  if (bytes.size() != 32)
  {
    throw std::invalid_argument("GovernanceActionId hash must be 32 bytes");
  }

  cardano_governance_action_id_t* value  = nullptr;
  const cardano_error_t           result = cardano_governance_action_id_from_hash_bytes(bytes.data(), bytes.size(), index, &value);

  CardanoObject<cardano_governance_action_id_t, cardano_governance_action_id_unref> owner(value);

  checkCardanoError(result, "GovernanceActionId");

  m_value = owner.release();
}

GovernanceActionId::GovernanceActionId(const GovernanceActionId& other)
: m_value(nullptr)
{
  cardano_governance_action_id_t* value  = nullptr;
  const cardano_error_t           result = cardano_governance_action_id_deep_clone(other.checked(), &value);

  CardanoObject<cardano_governance_action_id_t, cardano_governance_action_id_unref> owner(value);

  checkCardanoError(result, "GovernanceActionId copy");

  m_value = owner.release();
}

GovernanceActionId::GovernanceActionId(GovernanceActionId&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

GovernanceActionId::~GovernanceActionId()
{
  cardano_governance_action_id_unref(&m_value);
}

GovernanceActionId
GovernanceActionId::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return GovernanceActionId(decodeCbor<cardano_governance_action_id_t, cardano_governance_action_id_unref>(cbor, cardano_governance_action_id_from_cbor));
}

GovernanceActionId
GovernanceActionId::fromCbor(const std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

GovernanceActionId
GovernanceActionId::fromBech32(std::string_view bech32)
{
  const std::string               text(bech32);
  cardano_governance_action_id_t* value  = nullptr;
  const cardano_error_t           result = cardano_governance_action_id_from_bech32(text.c_str(), text.size(), &value);

  CardanoObject<cardano_governance_action_id_t, cardano_governance_action_id_unref> owner(value);

  checkCardanoError(result, "GovernanceActionId fromBech32");

  return GovernanceActionId(owner.release());
}

Blake2bHash
GovernanceActionId::hash() const
{
  const cardano_governance_action_id_t* value = checked();
  const std::uint8_t*                   bytes = cardano_governance_action_id_get_hash_bytes(value);

  return Blake2bHash(std::vector<std::uint8_t>(bytes, bytes + cardano_governance_action_id_get_hash_bytes_size(value)));
}

void
GovernanceActionId::setHash(const Blake2bHash& hash)
{
  // Reconstruct to refresh the encoded identifier as well as its hash.
  GovernanceActionId replacement(hash, index());

  swap(replacement);
}

std::uint64_t
GovernanceActionId::index() const
{
  std::uint64_t index = 0;

  checkCardanoError(cardano_governance_action_id_get_index(checked(), &index), "GovernanceActionId index");

  return index;
}

void
GovernanceActionId::setIndex(std::uint64_t index)
{
  // The native index setter does not refresh its cached Bech32 representation.
  GovernanceActionId replacement(hash(), index);

  swap(replacement);
}

std::string
GovernanceActionId::toBech32() const
{
  if (index() > 255)
  {
    throw std::overflow_error("CIP-129 action index exceeds one byte");
  }

  return cardano_governance_action_id_get_string(checked());
}

std::vector<std::uint8_t>
GovernanceActionId::toCbor() const
{
  return encodeCbor(checked(), cardano_governance_action_id_to_cbor);
}

std::string
GovernanceActionId::toJson() const
{
  return encodeJson(checked(), cardano_governance_action_id_to_cip116_json);
}

void
GovernanceActionId::swap(GovernanceActionId& other) noexcept
{
  std::swap(m_value, other.m_value);
}

GovernanceActionId&
GovernanceActionId::operator=(const GovernanceActionId& other)
{
  if (this != &other)
  {
    GovernanceActionId copy(other);

    swap(copy);
  }

  return *this;
}

GovernanceActionId&
GovernanceActionId::operator=(GovernanceActionId&& other) noexcept
{
  if (this != &other)
  {
    cardano_governance_action_id_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
GovernanceActionId::operator==(const GovernanceActionId& other) const
{
  return cardano_governance_action_id_equals(checked(), other.checked());
}

bool
GovernanceActionId::operator!=(const GovernanceActionId& other) const
{
  return !(*this == other);
}

GovernanceActionId
GovernanceActionId::adopt(cardano_governance_action_id_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null GovernanceActionId");
  }

  return GovernanceActionId(value);
}

cardano_governance_action_id_t*
GovernanceActionId::nativeHandle() noexcept
{
  return m_value;
}

const cardano_governance_action_id_t*
GovernanceActionId::nativeHandle() const noexcept
{
  return m_value;
}

cardano_governance_action_id_t*
GovernanceActionId::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

GovernanceActionId::GovernanceActionId(cardano_governance_action_id_t* value) noexcept
: m_value(value)
{
}

cardano_governance_action_id_t*
GovernanceActionId::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from GovernanceActionId");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(GovernanceActionId& lhs, GovernanceActionId& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
