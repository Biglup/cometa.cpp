/**
 * @file PoolOwners.cpp
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

#include <Cometa/PoolParams/PoolOwners.h>

#include <Cometa/Cbor/CborReader.h>

#include "../CardanoInterop.h"

#include <cardano/pool_params/pool_owners.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PoolOwners::PoolOwners()
: m_value(nullptr)
{
  cardano_pool_owners_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_pool_owners_new(&value);
  CardanoObject<cardano_pool_owners_t, cardano_pool_owners_unref> owner(value);

  checkCardanoError(status, "PoolOwners");
  m_value = owner.release();
}

PoolOwners::PoolOwners(std::initializer_list<Blake2bHash> values)
: PoolOwners()
{
  for (const Blake2bHash& value: values)
  {
    insert(value);
  }
}

PoolOwners::PoolOwners(const PoolOwners& other)
: m_value(nullptr)
{
  cardano_pool_owners_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_pool_owners_deep_clone(other.checked(), &value);
  CardanoObject<cardano_pool_owners_t, cardano_pool_owners_unref> owner(value);

  checkCardanoError(status, "PoolOwners copy");
  m_value = owner.release();
}

PoolOwners::PoolOwners(PoolOwners&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PoolOwners::~PoolOwners()
{
  cardano_pool_owners_unref(&m_value);
}

PoolOwners
PoolOwners::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  const CborReader reader(cbor);

  if (reader.peekState() == CborReaderState::Tag)
  {
    (void)reader.peekTag();
  }

  PoolOwners                     result(decodeCbor<cardano_pool_owners_t, cardano_pool_owners_unref>(cbor, cardano_pool_owners_from_cbor));
  const std::vector<Blake2bHash> elements = result.values();

  for (const Blake2bHash& element: elements)
  {
    validateHash(element);
  }

  for (std::size_t i = 1; i < elements.size(); ++i)
  {
    if (elements[i - 1] == elements[i])
    {
      throw std::invalid_argument("Duplicate element in CBOR set");
    }
  }

  return result;
}

PoolOwners
PoolOwners::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PoolOwners::size() const
{
  return cardano_pool_owners_get_length(checked());
}

bool
PoolOwners::empty() const
{
  return size() == 0;
}

bool
PoolOwners::contains(const Blake2bHash& value) const
{
  validateHash(value);
  const std::size_t count = size();

  for (std::size_t i = 0; i < count; ++i)
  {
    cardano_blake2b_hash_t*                                           element = nullptr;
    const cardano_error_t                                             status  = cardano_pool_owners_get(checked(), i, &element);
    CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(element);

    checkCardanoError(status, "PoolOwners contains");

    if (cardano_blake2b_hash_equals(owner.get(), value.checked()))
    {
      return true;
    }
  }

  return false;
}

bool
PoolOwners::insert(const Blake2bHash& value)
{
  validateHash(value);
  if (contains(value))
  {
    return false;
  }

  const Blake2bHash copy(value);

  checkCardanoError(cardano_pool_owners_add(checked(), copy.checked()), "PoolOwners insert");
  return true;
}

Blake2bHash
PoolOwners::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Set index is out of range");
  }

  cardano_blake2b_hash_t*                                           element = nullptr;
  const cardano_error_t                                             status  = cardano_pool_owners_get(checked(), index, &element);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(element);

  checkCardanoError(status, "PoolOwners at");
  const Blake2bHash value(owner.release());

  return Blake2bHash(value);
}

std::vector<Blake2bHash>
PoolOwners::values() const
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
PoolOwners::toCbor() const
{
  return encodeCbor(checked(), cardano_pool_owners_to_cbor);
}

std::string
PoolOwners::toJson() const
{
  return encodeJson(checked(), cardano_pool_owners_to_cip116_json);
}

void
PoolOwners::swap(PoolOwners& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PoolOwners&
PoolOwners::operator=(const PoolOwners& other)
{
  if (this != &other)
  {
    PoolOwners copy(other);
    swap(copy);
  }

  return *this;
}

PoolOwners&
PoolOwners::operator=(PoolOwners&& other) noexcept
{
  if (this != &other)
  {
    cardano_pool_owners_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
PoolOwners::operator==(const PoolOwners& other) const
{
  const std::size_t count = size();

  if (count != other.size())
  {
    return false;
  }

  for (std::size_t i = 0; i < count; ++i)
  {
    cardano_blake2b_hash_t*                                           left       = nullptr;
    const cardano_error_t                                             leftStatus = cardano_pool_owners_get(checked(), i, &left);
    CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> leftOwner(left);

    checkCardanoError(leftStatus, "PoolOwners comparison");

    cardano_blake2b_hash_t*                                           right       = nullptr;
    const cardano_error_t                                             rightStatus = cardano_pool_owners_get(other.checked(), i, &right);
    CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> rightOwner(right);

    checkCardanoError(rightStatus, "PoolOwners comparison");

    if (!cardano_blake2b_hash_equals(leftOwner.get(), rightOwner.get()))
    {
      return false;
    }
  }

  return true;
}

bool
PoolOwners::operator!=(const PoolOwners& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

PoolOwners::PoolOwners(cardano_pool_owners_t* value) noexcept
: m_value(value)
{
}

cardano_pool_owners_t*
PoolOwners::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PoolOwners");
  }

  return m_value;
}

void
PoolOwners::validateHash(const Blake2bHash& value)
{
  if (value.toBytes().size() != 28U)
  {
    throw std::invalid_argument("Pool owner key hash must be 28 bytes");
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PoolOwners& lhs, PoolOwners& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
