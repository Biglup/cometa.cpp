/**
 * @file PoolParams.cpp
 *
 * @brief Stake pool registration parameters.
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

#include <Cometa/PoolParams/PoolParams.h>

#include "../CardanoInterop.h"

#include <cardano/pool_params/pool_params.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PoolParams::PoolParams(const Blake2bHash& operatorKeyHash, const Blake2bHash& vrfKeyHash, std::uint64_t pledge, std::uint64_t cost, const UnitInterval& margin, const RewardAddress& rewardAccount, const PoolOwners& owners, const Relays& relays, const std::optional<PoolMetadata>& metadata)
: m_value(nullptr)
{
  validateHash(operatorKeyHash, 28);
  validateHash(vrfKeyHash, 32);

  const Blake2bHash                 operatorCopy(operatorKeyHash);
  const Blake2bHash                 vrfCopy(vrfKeyHash);
  const UnitInterval                marginCopy(margin);
  const RewardAddress               rewardCopy(rewardAccount);
  const PoolOwners                  ownersCopy(owners);
  const Relays                      relaysCopy(relays);
  const std::optional<PoolMetadata> metadataCopy(metadata);

  cardano_pool_params_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_pool_params_new(operatorCopy.checked(), vrfCopy.checked(), pledge, cost, marginCopy.checked(), rewardCopy.checked(), ownersCopy.checked(), relaysCopy.checked(), metadataCopy ? metadataCopy->checked() : nullptr, &value);
  CardanoObject<cardano_pool_params_t, cardano_pool_params_unref> owner(value);

  checkCardanoError(status, "PoolParams");
  m_value = owner.release();
}

PoolParams::PoolParams(const PoolParams& other)
: m_value(nullptr)
{
  cardano_pool_params_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_pool_params_deep_clone(other.checked(), &value);
  CardanoObject<cardano_pool_params_t, cardano_pool_params_unref> owner(value);

  checkCardanoError(status, "PoolParams copy");
  m_value = owner.release();
}

PoolParams::PoolParams(PoolParams&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PoolParams::~PoolParams()
{
  cardano_pool_params_unref(&m_value);
}

PoolParams
PoolParams::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  PoolParams value(decodeCbor<cardano_pool_params_t, cardano_pool_params_unref>(cbor, cardano_pool_params_from_cbor));

  validateHash(value.operatorKeyHash(), 28);
  validateHash(value.vrfKeyHash(), 32);
  (void)PoolOwners::fromCbor(value.owners().toCbor());

  return value;
}

PoolParams
PoolParams::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::uint64_t
PoolParams::pledge() const
{
  std::uint64_t result = 0;
  checkCardanoError(cardano_pool_params_get_pledge(checked(), &result), "PoolParams pledge");
  return result;
}

void
PoolParams::setPledge(std::uint64_t value)
{
  checkCardanoError(cardano_pool_params_set_pledge(checked(), value), "PoolParams set pledge");
}

std::uint64_t
PoolParams::cost() const
{
  std::uint64_t result = 0;
  checkCardanoError(cardano_pool_params_get_cost(checked(), &result), "PoolParams cost");
  return result;
}

void
PoolParams::setCost(std::uint64_t value)
{
  checkCardanoError(cardano_pool_params_set_cost(checked(), value), "PoolParams set cost");
}

Blake2bHash
PoolParams::operatorKeyHash() const
{
  cardano_blake2b_hash_t*                                           value  = nullptr;
  const cardano_error_t                                             status = cardano_pool_params_get_operator_key_hash(checked(), &value);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  checkCardanoError(status, "PoolParams operatorKeyHash");
  const Blake2bHash result(owner.release());
  return Blake2bHash(result);
}

void
PoolParams::setOperatorKeyHash(const Blake2bHash& value)
{
  validateHash(value, 28);
  const Blake2bHash copy(value);
  checkCardanoError(cardano_pool_params_set_operator_key_hash(checked(), copy.checked()), "PoolParams set operatorKeyHash");
}

Blake2bHash
PoolParams::vrfKeyHash() const
{
  cardano_blake2b_hash_t*                                           value  = nullptr;
  const cardano_error_t                                             status = cardano_pool_params_get_vrf_vk_hash(checked(), &value);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  checkCardanoError(status, "PoolParams vrfKeyHash");
  const Blake2bHash result(owner.release());
  return Blake2bHash(result);
}

void
PoolParams::setVrfKeyHash(const Blake2bHash& value)
{
  validateHash(value, 32);
  const Blake2bHash copy(value);
  checkCardanoError(cardano_pool_params_set_vrf_vk_hash(checked(), copy.checked()), "PoolParams set vrfKeyHash");
}

UnitInterval
PoolParams::margin() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_pool_params_get_margin(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "PoolParams margin");
  const UnitInterval result(owner.release());
  return UnitInterval(result);
}

void
PoolParams::setMargin(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_pool_params_set_margin(checked(), copy.checked()), "PoolParams set margin");
}

RewardAddress
PoolParams::rewardAccount() const
{
  cardano_reward_address_t*                                             value  = nullptr;
  const cardano_error_t                                                 status = cardano_pool_params_get_reward_account(checked(), &value);
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> owner(value);

  checkCardanoError(status, "PoolParams rewardAccount");
  const RewardAddress result(*owner.release());
  return RewardAddress(result);
}

void
PoolParams::setRewardAccount(const RewardAddress& value)
{
  const RewardAddress copy(value);
  checkCardanoError(cardano_pool_params_set_reward_account(checked(), copy.checked()), "PoolParams set rewardAccount");
}

PoolOwners
PoolParams::owners() const
{
  cardano_pool_owners_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_pool_params_get_owners(checked(), &value);
  CardanoObject<cardano_pool_owners_t, cardano_pool_owners_unref> owner(value);

  checkCardanoError(status, "PoolParams owners");
  const PoolOwners result(owner.release());
  return PoolOwners(result);
}

void
PoolParams::setOwners(const PoolOwners& value)
{
  const PoolOwners copy(value);
  checkCardanoError(cardano_pool_params_set_owners(checked(), copy.checked()), "PoolParams set owners");
}

Relays
PoolParams::relays() const
{
  cardano_relays_t*                                     value  = nullptr;
  const cardano_error_t                                 status = cardano_pool_params_get_relays(checked(), &value);
  CardanoObject<cardano_relays_t, cardano_relays_unref> owner(value);

  checkCardanoError(status, "PoolParams relays");
  const Relays result(*owner.release());
  return Relays(result);
}

void
PoolParams::setRelays(const Relays& value)
{
  const Relays copy(value);
  checkCardanoError(cardano_pool_params_set_relays(checked(), copy.checked()), "PoolParams set relays");
}

std::optional<BlsKey>
PoolParams::blsKey() const
{
  cardano_bls_key_t*                                      value  = nullptr;
  const cardano_error_t                                   status = cardano_pool_params_get_bls_key(checked(), &value);
  CardanoObject<cardano_bls_key_t, cardano_bls_key_unref> owner(value);

  checkCardanoError(status, "PoolParams blsKey");
  if (!owner)
  {
    return std::nullopt;
  }

  const BlsKey result(*owner.release());
  return BlsKey(result);
}

void
PoolParams::setBlsKey(const std::optional<BlsKey>& value)
{
  const std::optional<BlsKey> copy(value);
  checkCardanoError(cardano_pool_params_set_bls_key(checked(), copy ? copy->checked() : nullptr), "PoolParams set blsKey");
}

std::optional<PoolMetadata>
PoolParams::metadata() const
{
  cardano_pool_metadata_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_pool_params_get_metadata(checked(), &value);
  CardanoObject<cardano_pool_metadata_t, cardano_pool_metadata_unref> owner(value);

  checkCardanoError(status, "PoolParams metadata");
  if (!owner)
  {
    return std::nullopt;
  }

  const PoolMetadata result(*owner.release());
  return PoolMetadata(result);
}

void
PoolParams::setMetadata(const std::optional<PoolMetadata>& value)
{
  const std::optional<PoolMetadata> copy(value);
  checkCardanoError(cardano_pool_params_set_metadata(checked(), copy ? copy->checked() : nullptr), "PoolParams set metadata");
}

std::vector<std::uint8_t>
PoolParams::toCbor() const
{
  return encodeCbor(checked(), cardano_pool_params_to_cbor);
}

std::string
PoolParams::toJson() const
{
  return encodeJson(checked(), cardano_pool_params_to_cip116_json);
}

void
PoolParams::swap(PoolParams& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PoolParams&
PoolParams::operator=(const PoolParams& other)
{
  if (this != &other)
  {
    PoolParams copy(other);
    swap(copy);
  }

  return *this;
}

PoolParams&
PoolParams::operator=(PoolParams&& other) noexcept
{
  if (this != &other)
  {
    cardano_pool_params_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
PoolParams::operator==(const PoolParams& other) const
{
  return operatorKeyHash() == other.operatorKeyHash() && vrfKeyHash() == other.vrfKeyHash() &&
    blsKey() == other.blsKey() && pledge() == other.pledge() && cost() == other.cost() &&
    margin() == other.margin() && rewardAccount() == other.rewardAccount() && owners() == other.owners() &&
    relays() == other.relays() && metadata() == other.metadata();
}

bool
PoolParams::operator!=(const PoolParams& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

PoolParams::PoolParams(cardano_pool_params_t* value) noexcept
: m_value(value)
{
}

cardano_pool_params_t*
PoolParams::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PoolParams");
  }

  return m_value;
}

void
PoolParams::validateHash(const Blake2bHash& value, std::size_t size)
{
  if (value.toBytes().size() != size)
  {
    throw std::invalid_argument("Invalid pool parameter hash length");
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PoolParams& lhs, PoolParams& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
