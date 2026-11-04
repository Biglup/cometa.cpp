/**
 * @file Update.cpp
 *
 * @brief Protocol parameter update proposal for an epoch.
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

#include <Cometa/ProtocolParams/Update.h>

#include "../CardanoInterop.h"

#include <cardano/protocol_params/update.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Update::Update(std::uint64_t epoch, const ProposedParamUpdates& proposals)
: m_value(nullptr)
{
  const ProposedParamUpdates                            copy(proposals);
  cardano_update_t*                                     value  = nullptr;
  const cardano_error_t                                 status = cardano_update_new(epoch, checkedNativeHandle(copy), &value);
  CardanoObject<cardano_update_t, cardano_update_unref> owner(value);

  checkCardanoError(status, "Update");
  m_value = owner.release();
}

Update::Update(const Update& other)
: m_value(nullptr)
{
  cardano_update_t*                                     value  = nullptr;
  const cardano_error_t                                 status = cardano_update_deep_clone(other.checked(), &value);
  CardanoObject<cardano_update_t, cardano_update_unref> owner(value);

  checkCardanoError(status, "Update copy");
  m_value = owner.release();
}

Update::Update(Update&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Update::~Update()
{
  cardano_update_unref(&m_value);
}

Update
Update::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return Update(decodeCbor<cardano_update_t, cardano_update_unref>(cbor, cardano_update_from_cbor));
}

Update
Update::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::uint64_t
Update::epoch() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_update_get_epoch(checked(), &value);

  checkCardanoError(status, "Update epoch");
  return value;
}

ProposedParamUpdates
Update::proposals() const
{
  cardano_proposed_param_updates_t* value = nullptr;
  checkCardanoError(cardano_update_get_proposed_parameters(checked(), &value), "Update proposals");

  if (!value)
  {
    throw std::logic_error("Update has no proposed parameters");
  }

  cardano_proposed_param_updates_t*                                                     clone  = nullptr;
  const cardano_error_t                                                                 status = cardano_proposed_param_updates_deep_clone(value, &clone);
  CardanoObject<cardano_proposed_param_updates_t, cardano_proposed_param_updates_unref> owner(clone);

  checkCardanoError(status, "Update proposals copy");
  return ProposedParamUpdates::adopt(owner.release());
}

void
Update::setEpoch(std::uint64_t value)
{
  checkCardanoError(cardano_update_set_epoch(checked(), value), "Update setEpoch");
}

void
Update::setProposals(const ProposedParamUpdates& value)
{
  const ProposedParamUpdates copy(value);
  checkCardanoError(cardano_update_set_proposed_parameters(checked(), checkedNativeHandle(copy)), "Update setProposals");
}

std::vector<std::uint8_t>
Update::toCbor() const
{
  return encodeCbor(checked(), cardano_update_to_cbor);
}

std::string
Update::toJson() const
{
  return encodeJson(checked(), cardano_update_to_cip116_json);
}

void
Update::swap(Update& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Update&
Update::operator=(const Update& other)
{
  if (this != &other)
  {
    Update copy(other);
    swap(copy);
  }
  return *this;
}

Update&
Update::operator=(Update&& other) noexcept
{
  if (this != &other)
  {
    cardano_update_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
Update::operator==(const Update& other) const
{
  return toCbor() == other.toCbor();
}

bool
Update::operator!=(const Update& other) const
{
  return !(*this == other);
}

Update
Update::adopt(cardano_update_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null Update");
  }

  return Update(value);
}

cardano_update_t*
Update::nativeHandle() noexcept
{
  return m_value;
}

const cardano_update_t*
Update::nativeHandle() const noexcept
{
  return m_value;
}

cardano_update_t*
Update::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

Update::Update(cardano_update_t* value) noexcept
: m_value(value)
{
}

cardano_update_t*
Update::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Update");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Update& lhs, Update& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
