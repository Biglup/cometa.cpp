/**
 * @file Blake2bHashSet.cpp
 *
 * @brief Sorted unique Blake2bHash set operations.
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

#include <Cometa/Crypto/Blake2bHashSet.h>

#include <Cometa/Cbor/CborReader.h>

#include "../CardanoInterop.h"

#include <cardano/crypto/blake2b_hash_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Blake2bHashSet::Blake2bHashSet()
: m_value(nullptr)
{
  cardano_blake2b_hash_set_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_blake2b_hash_set_new(&value);
  CardanoObject<cardano_blake2b_hash_set_t, cardano_blake2b_hash_set_unref> owner(value);

  checkCardanoError(status, "Blake2bHashSet");
  m_value = owner.release();
}

Blake2bHashSet::Blake2bHashSet(std::initializer_list<Blake2bHash> values)
: Blake2bHashSet()
{
  for (const Blake2bHash& value: values)
  {
    insert(value);
  }
}

Blake2bHashSet::Blake2bHashSet(const Blake2bHashSet& other)
: m_value(nullptr)
{
  cardano_blake2b_hash_set_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_blake2b_hash_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_blake2b_hash_set_t, cardano_blake2b_hash_set_unref> owner(value);

  checkCardanoError(status, "Blake2bHashSet copy");
  m_value = owner.release();
}

Blake2bHashSet::Blake2bHashSet(Blake2bHashSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Blake2bHashSet::~Blake2bHashSet()
{
  cardano_blake2b_hash_set_unref(&m_value);
}

Blake2bHashSet
Blake2bHashSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  const CborReader reader(cbor);

  if (reader.peekState() == CborReaderState::Tag)
  {
    (void)reader.peekTag();
  }

  Blake2bHashSet                 result(decodeCbor<cardano_blake2b_hash_set_t, cardano_blake2b_hash_set_unref>(cbor, cardano_blake2b_hash_set_from_cbor));
  const std::vector<Blake2bHash> elements = result.values();

  for (std::size_t i = 1; i < elements.size(); ++i)
  {
    if (elements[i - 1] == elements[i])
    {
      throw std::invalid_argument("Duplicate element in CBOR set");
    }
  }

  return result;
}

Blake2bHashSet
Blake2bHashSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
Blake2bHashSet::size() const
{
  return cardano_blake2b_hash_set_get_length(checked());
}

bool
Blake2bHashSet::empty() const
{
  return size() == 0;
}

bool
Blake2bHashSet::contains(const Blake2bHash& value) const
{
  const std::size_t count = size();

  for (std::size_t i = 0; i < count; ++i)
  {
    cardano_blake2b_hash_t*                                           element = nullptr;
    const cardano_error_t                                             status  = cardano_blake2b_hash_set_get(checked(), i, &element);
    CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(element);

    checkCardanoError(status, "Blake2bHashSet contains");

    if (cardano_blake2b_hash_equals(owner.get(), value.checked()))
    {
      return true;
    }
  }

  return false;
}

bool
Blake2bHashSet::insert(const Blake2bHash& value)
{
  if (contains(value))
  {
    return false;
  }

  const Blake2bHash copy(value);

  checkCardanoError(cardano_blake2b_hash_set_add(checked(), copy.checked()), "Blake2bHashSet insert");
  return true;
}

Blake2bHash
Blake2bHashSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Set index is out of range");
  }

  cardano_blake2b_hash_t*                                           element = nullptr;
  const cardano_error_t                                             status  = cardano_blake2b_hash_set_get(checked(), index, &element);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(element);

  checkCardanoError(status, "Blake2bHashSet at");
  const Blake2bHash value(owner.release());

  return Blake2bHash(value);
}

std::vector<Blake2bHash>
Blake2bHashSet::values() const
{
  std::vector<Blake2bHash> result;
  const std::size_t        count = size();
  result.reserve(count);

  for (std::size_t i = 0; i < count; ++i)
  {
    result.push_back(at(i));
  }

  return result;
}

std::vector<std::uint8_t>
Blake2bHashSet::toCbor() const
{
  return encodeCbor(checked(), cardano_blake2b_hash_set_to_cbor);
}

std::string
Blake2bHashSet::toJson() const
{
  return encodeJson(checked(), cardano_blake2b_hash_set_to_cip116_json);
}

void
Blake2bHashSet::swap(Blake2bHashSet& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Blake2bHashSet&
Blake2bHashSet::operator=(const Blake2bHashSet& other)
{
  if (this != &other)
  {
    Blake2bHashSet copy(other);
    swap(copy);
  }

  return *this;
}

Blake2bHashSet&
Blake2bHashSet::operator=(Blake2bHashSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_blake2b_hash_set_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
Blake2bHashSet::operator==(const Blake2bHashSet& other) const
{
  const std::size_t count = size();

  if (count != other.size())
  {
    return false;
  }

  for (std::size_t i = 0; i < count; ++i)
  {
    cardano_blake2b_hash_t*                                           left       = nullptr;
    const cardano_error_t                                             leftStatus = cardano_blake2b_hash_set_get(checked(), i, &left);
    CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> leftOwner(left);

    checkCardanoError(leftStatus, "Blake2bHashSet comparison");

    cardano_blake2b_hash_t*                                           right       = nullptr;
    const cardano_error_t                                             rightStatus = cardano_blake2b_hash_set_get(other.checked(), i, &right);
    CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> rightOwner(right);

    checkCardanoError(rightStatus, "Blake2bHashSet comparison");

    if (!cardano_blake2b_hash_equals(leftOwner.get(), rightOwner.get()))
    {
      return false;
    }
  }

  return true;
}

bool
Blake2bHashSet::operator!=(const Blake2bHashSet& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

Blake2bHashSet::Blake2bHashSet(cardano_blake2b_hash_set_t* value) noexcept
: m_value(value)
{
}

cardano_blake2b_hash_set_t*
Blake2bHashSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Blake2bHashSet");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Blake2bHashSet& lhs, Blake2bHashSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
