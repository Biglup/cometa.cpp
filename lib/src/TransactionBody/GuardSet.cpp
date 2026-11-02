/**
 * @file GuardSet.cpp
 *
 * @brief Transaction guards in ledger order.
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

#include <Cometa/TransactionBody/GuardSet.h>

#include "../CardanoInterop.h"

#include <cardano/common/credential.h>
#include <cardano/common/guard_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
GuardSet::GuardSet()
: m_value(nullptr)
{
  cardano_guard_set_t*                                        value  = nullptr;
  const cardano_error_t                                       status = cardano_guard_set_new(&value);
  CardanoObject<cardano_guard_set_t, cardano_guard_set_unref> owner(value);

  checkCardanoError(status, "GuardSet");
  m_value = owner.release();
}

GuardSet::GuardSet(const GuardSet& other)
: m_value(nullptr)
{
  cardano_guard_set_t*                                        value  = nullptr;
  const cardano_error_t                                       status = cardano_guard_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_guard_set_t, cardano_guard_set_unref> owner(value);

  checkCardanoError(status, "GuardSet copy");
  m_value = owner.release();
}

GuardSet::GuardSet(GuardSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

GuardSet::~GuardSet()
{
  cardano_guard_set_unref(&m_value);
}

GuardSet
GuardSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return GuardSet(decodeCbor<cardano_guard_set_t, cardano_guard_set_unref>(cbor, cardano_guard_set_from_cbor));
}

GuardSet
GuardSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
GuardSet::size() const
{
  return cardano_guard_set_get_length(checked());
}

void
GuardSet::add(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_guard_set_add(checked(), copy.checked()), "GuardSet add");
}

Credential
GuardSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Credential index is out of range");
  }

  cardano_credential_t*                                         value  = nullptr;
  const cardano_error_t                                         status = cardano_guard_set_get(checked(), index, &value);
  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(value);

  checkCardanoError(status, "GuardSet at");
  const Credential copied(owner.release());
  return Credential(copied);
}

bool
GuardSet::useTag() const
{
  return cardano_guard_set_is_tagged(checked());
}

void
GuardSet::setUseTag(bool value)
{
  checkCardanoError(cardano_guard_set_set_use_tag(checked(), value), "GuardSet setUseTag");
}

std::vector<std::uint8_t>
GuardSet::toCbor() const
{
  return encodeCbor(checked(), cardano_guard_set_to_cbor);
}

std::string
GuardSet::toJson() const
{
  return encodeJson(checked(), cardano_guard_set_to_cip116_json);
}

void
GuardSet::swap(GuardSet& other) noexcept
{
  std::swap(m_value, other.m_value);
}

GuardSet&
GuardSet::operator=(const GuardSet& other)
{
  if (this != &other)
  {
    GuardSet copy(other);
    swap(copy);
  }
  return *this;
}

GuardSet&
GuardSet::operator=(GuardSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_guard_set_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
GuardSet::operator==(const GuardSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
GuardSet::operator!=(const GuardSet& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

GuardSet::GuardSet(cardano_guard_set_t* value) noexcept
: m_value(value)
{
}

cardano_guard_set_t*
GuardSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from GuardSet");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(GuardSet& lhs, GuardSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
