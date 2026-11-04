/**
 * @file ProposedParamUpdates.cpp
 *
 * @brief Protocol parameter proposals by genesis delegate.
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

#include <Cometa/ProtocolParams/ProposedParamUpdates.h>

#include "../CardanoInterop.h"

#include <cardano/crypto/blake2b_hash.h>
#include <cardano/protocol_params/proposed_param_updates.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ProposedParamUpdates::ProposedParamUpdates()
: m_value(nullptr)
{
  cardano_proposed_param_updates_t*                                                     value  = nullptr;
  const cardano_error_t                                                                 status = cardano_proposed_param_updates_new(&value);
  CardanoObject<cardano_proposed_param_updates_t, cardano_proposed_param_updates_unref> owner(value);

  checkCardanoError(status, "ProposedParamUpdates");
  m_value = owner.release();
}

ProposedParamUpdates::ProposedParamUpdates(const ProposedParamUpdates& other)
: m_value(nullptr)
{
  cardano_proposed_param_updates_t*                                                     value  = nullptr;
  const cardano_error_t                                                                 status = cardano_proposed_param_updates_deep_clone(other.checked(), &value);
  CardanoObject<cardano_proposed_param_updates_t, cardano_proposed_param_updates_unref> owner(value);

  checkCardanoError(status, "ProposedParamUpdates copy");
  m_value = owner.release();
}

ProposedParamUpdates::ProposedParamUpdates(ProposedParamUpdates&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ProposedParamUpdates::~ProposedParamUpdates()
{
  cardano_proposed_param_updates_unref(&m_value);
}

ProposedParamUpdates
ProposedParamUpdates::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return ProposedParamUpdates(decodeCbor<cardano_proposed_param_updates_t, cardano_proposed_param_updates_unref>(cbor, cardano_proposed_param_updates_from_cbor));
}

ProposedParamUpdates
ProposedParamUpdates::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
ProposedParamUpdates::size() const
{
  return cardano_proposed_param_updates_get_size(checked());
}

void
ProposedParamUpdates::add(const Blake2bHash& key, const ProtocolParamUpdate& update)
{
  const Blake2bHash         keyCopy(key);
  const ProtocolParamUpdate updateCopy(update);
  checkCardanoError(cardano_proposed_param_updates_insert(checked(), checkedNativeHandle(keyCopy), checkedNativeHandle(updateCopy)), "ProposedParamUpdates add");
}

std::optional<ProtocolParamUpdate>
ProposedParamUpdates::get(const Blake2bHash& key) const
{
  cardano_protocol_param_update_t*                                                    value  = nullptr;
  const cardano_error_t                                                               status = cardano_proposed_param_updates_get(checked(), checkedNativeHandle(key), &value);
  CardanoObject<cardano_protocol_param_update_t, cardano_protocol_param_update_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProposedParamUpdates get");
  const ProtocolParamUpdate found = ProtocolParamUpdate::adopt(owner.release());
  return ProtocolParamUpdate(found);
}

Blake2bHash
ProposedParamUpdates::keyAt(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("ProposedParamUpdates index is out of range");
  }

  cardano_blake2b_hash_t*                                           value  = nullptr;
  const cardano_error_t                                             status = cardano_proposed_param_updates_get_key_at(checked(), index, &value);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  checkCardanoError(status, "ProposedParamUpdates keyAt");
  const Blake2bHash found = Blake2bHash::adopt(owner.release());
  return Blake2bHash(found);
}

ProtocolParamUpdate
ProposedParamUpdates::valueAt(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("ProposedParamUpdates index is out of range");
  }

  cardano_protocol_param_update_t*                                                    value  = nullptr;
  const cardano_error_t                                                               status = cardano_proposed_param_updates_get_value_at(checked(), index, &value);
  CardanoObject<cardano_protocol_param_update_t, cardano_protocol_param_update_unref> owner(value);

  checkCardanoError(status, "ProposedParamUpdates valueAt");
  const ProtocolParamUpdate found = ProtocolParamUpdate::adopt(owner.release());
  return ProtocolParamUpdate(found);
}

std::pair<Blake2bHash, ProtocolParamUpdate>
ProposedParamUpdates::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("ProposedParamUpdates index is out of range");
  }

  cardano_blake2b_hash_t*                                                             key    = nullptr;
  cardano_protocol_param_update_t*                                                    update = nullptr;
  const cardano_error_t                                                               status = cardano_proposed_param_updates_get_key_value_at(checked(), index, &key, &update);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref>                   keyOwner(key);
  CardanoObject<cardano_protocol_param_update_t, cardano_protocol_param_update_unref> updateOwner(update);

  checkCardanoError(status, "ProposedParamUpdates at");
  const Blake2bHash         copiedKey    = Blake2bHash::adopt(keyOwner.release());
  const ProtocolParamUpdate copiedUpdate = ProtocolParamUpdate::adopt(updateOwner.release());
  return { Blake2bHash(copiedKey), ProtocolParamUpdate(copiedUpdate) };
}

std::vector<std::uint8_t>
ProposedParamUpdates::toCbor() const
{
  return encodeCbor(checked(), cardano_proposed_param_updates_to_cbor);
}

std::string
ProposedParamUpdates::toJson() const
{
  return encodeJson(checked(), cardano_proposed_param_updates_to_cip116_json);
}

void
ProposedParamUpdates::swap(ProposedParamUpdates& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ProposedParamUpdates&
ProposedParamUpdates::operator=(const ProposedParamUpdates& other)
{
  if (this != &other)
  {
    ProposedParamUpdates copy(other);
    swap(copy);
  }
  return *this;
}

ProposedParamUpdates&
ProposedParamUpdates::operator=(ProposedParamUpdates&& other) noexcept
{
  if (this != &other)
  {
    cardano_proposed_param_updates_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
ProposedParamUpdates::operator==(const ProposedParamUpdates& other) const
{
  return toCbor() == other.toCbor();
}

bool
ProposedParamUpdates::operator!=(const ProposedParamUpdates& other) const
{
  return !(*this == other);
}

ProposedParamUpdates
ProposedParamUpdates::adopt(cardano_proposed_param_updates_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null ProposedParamUpdates");
  }

  return ProposedParamUpdates(value);
}

cardano_proposed_param_updates_t*
ProposedParamUpdates::nativeHandle() noexcept
{
  return m_value;
}

const cardano_proposed_param_updates_t*
ProposedParamUpdates::nativeHandle() const noexcept
{
  return m_value;
}

cardano_proposed_param_updates_t*
ProposedParamUpdates::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

ProposedParamUpdates::ProposedParamUpdates(cardano_proposed_param_updates_t* value) noexcept
: m_value(value)
{
}

cardano_proposed_param_updates_t*
ProposedParamUpdates::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ProposedParamUpdates");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ProposedParamUpdates& lhs, ProposedParamUpdates& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
