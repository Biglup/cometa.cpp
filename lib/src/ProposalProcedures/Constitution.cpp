/**
 * @file Constitution.cpp
 *
 * @brief Constitution content.
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

#include <Cometa/ProposalProcedures/Constitution.h>

#include "../CardanoInterop.h"

#include <cardano/proposal_procedures/constitution.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Constitution::Constitution(const Anchor& anchor, const std::optional<Blake2bHash>& scriptHash)
: m_value(nullptr)
{
  const Anchor                                                      anchorCopy(anchor);
  const std::optional<Blake2bHash>                                  scriptCopy(scriptHash);
  cardano_blake2b_hash_t*                                           script = scriptCopy ? scriptCopy->checked() : nullptr;
  cardano_constitution_t*                                           value  = nullptr;
  const cardano_error_t                                             status = cardano_constitution_new(anchorCopy.checked(), script, &value);
  CardanoObject<cardano_constitution_t, cardano_constitution_unref> owner(value);

  checkCardanoError(status, "Constitution");
  m_value = owner.release();
}

Constitution::Constitution(const Constitution& other)
: m_value(nullptr)
{
  cardano_constitution_t*                                           value  = nullptr;
  const cardano_error_t                                             status = cardano_constitution_deep_clone(other.checked(), &value);
  CardanoObject<cardano_constitution_t, cardano_constitution_unref> owner(value);

  checkCardanoError(status, "Constitution copy");
  m_value = owner.release();
}

Constitution::Constitution(Constitution&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Constitution::~Constitution()
{
  cardano_constitution_unref(&m_value);
}

Constitution
Constitution::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return Constitution(decodeCbor<cardano_constitution_t, cardano_constitution_unref>(cbor, cardano_constitution_from_cbor));
}

Constitution
Constitution::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Anchor
Constitution::anchor() const
{
  cardano_anchor_t*                                     value = cardano_constitution_get_anchor(checked());
  CardanoObject<cardano_anchor_t, cardano_anchor_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("Constitution has no anchor");
  }

  const Anchor found(owner.release());
  return Anchor(found);
}

void
Constitution::setAnchor(const Anchor& value)
{
  const Anchor copy(value);
  checkCardanoError(cardano_constitution_set_anchor(checked(), copy.checked()), "Constitution setAnchor");
}

std::optional<Blake2bHash>
Constitution::scriptHash() const
{
  cardano_blake2b_hash_t*                                           value = cardano_constitution_get_script_hash(checked());
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const Blake2bHash found(owner.release());
  return Blake2bHash(found);
}

void
Constitution::setScriptHash(const std::optional<Blake2bHash>& value)
{
  const std::optional<Blake2bHash> copy(value);
  cardano_blake2b_hash_t*          data = copy ? copy->checked() : nullptr;
  checkCardanoError(cardano_constitution_set_script_hash(checked(), data), "Constitution setScriptHash");
}

std::vector<std::uint8_t>
Constitution::toCbor() const
{
  return encodeCbor(checked(), cardano_constitution_to_cbor);
}

std::string
Constitution::toJson() const
{
  return encodeJson(checked(), cardano_constitution_to_cip116_json);
}

void
Constitution::swap(Constitution& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Constitution&
Constitution::operator=(const Constitution& other)
{
  if (this != &other)
  {
    Constitution copy(other);
    swap(copy);
  }
  return *this;
}

Constitution&
Constitution::operator=(Constitution&& other) noexcept
{
  if (this != &other)
  {
    cardano_constitution_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
Constitution::operator==(const Constitution& other) const
{
  return toCbor() == other.toCbor();
}

bool
Constitution::operator!=(const Constitution& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

Constitution::Constitution(cardano_constitution_t* value) noexcept
: m_value(value)
{
}

cardano_constitution_t*
Constitution::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Constitution");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Constitution& lhs, Constitution& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
