/**
 * @file TreasuryWithdrawalsAction.cpp
 *
 * @brief Treasury withdrawal governance action.
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

#include <Cometa/ProposalProcedures/TreasuryWithdrawalsAction.h>

#include "../CardanoInterop.h"

#include <cardano/proposal_procedures/treasury_withdrawals_action.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
TreasuryWithdrawalsAction::TreasuryWithdrawalsAction(const WithdrawalMap& withdrawals, const std::optional<Blake2bHash>& policyHash)
: m_value(nullptr)
{
  const WithdrawalMap                                                                             withdrawalsCopy(withdrawals);
  const std::optional<Blake2bHash>                                                                policyCopy(policyHash);
  cardano_blake2b_hash_t*                                                                         policy = policyCopy ? policyCopy->checked() : nullptr;
  cardano_treasury_withdrawals_action_t*                                                          value  = nullptr;
  const cardano_error_t                                                                           status = cardano_treasury_withdrawals_action_new(withdrawalsCopy.checked(), policy, &value);
  CardanoObject<cardano_treasury_withdrawals_action_t, cardano_treasury_withdrawals_action_unref> owner(value);

  checkCardanoError(status, "TreasuryWithdrawalsAction");
  m_value = owner.release();
}

TreasuryWithdrawalsAction::TreasuryWithdrawalsAction(const TreasuryWithdrawalsAction& other)
: m_value(nullptr)
{
  cardano_treasury_withdrawals_action_t*                                                          value  = nullptr;
  const cardano_error_t                                                                           status = cardano_treasury_withdrawals_action_deep_clone(other.checked(), &value);
  CardanoObject<cardano_treasury_withdrawals_action_t, cardano_treasury_withdrawals_action_unref> owner(value);

  checkCardanoError(status, "TreasuryWithdrawalsAction copy");
  m_value = owner.release();
}

TreasuryWithdrawalsAction::TreasuryWithdrawalsAction(TreasuryWithdrawalsAction&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

TreasuryWithdrawalsAction::~TreasuryWithdrawalsAction()
{
  cardano_treasury_withdrawals_action_unref(&m_value);
}

TreasuryWithdrawalsAction
TreasuryWithdrawalsAction::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return TreasuryWithdrawalsAction(decodeCbor<cardano_treasury_withdrawals_action_t, cardano_treasury_withdrawals_action_unref>(cbor, cardano_treasury_withdrawals_action_from_cbor));
}

TreasuryWithdrawalsAction
TreasuryWithdrawalsAction::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

WithdrawalMap
TreasuryWithdrawalsAction::withdrawals() const
{
  cardano_withdrawal_map_t*                                             value = cardano_treasury_withdrawals_action_get_withdrawals(checked());
  CardanoObject<cardano_withdrawal_map_t, cardano_withdrawal_map_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("TreasuryWithdrawalsAction has no withdrawals");
  }

  const WithdrawalMap found(*owner.release());
  return WithdrawalMap(found);
}

void
TreasuryWithdrawalsAction::setWithdrawals(const WithdrawalMap& value)
{
  const WithdrawalMap copy(value);
  checkCardanoError(cardano_treasury_withdrawals_action_set_withdrawals(checked(), copy.checked()), "TreasuryWithdrawalsAction setWithdrawals");
}

std::optional<Blake2bHash>
TreasuryWithdrawalsAction::policyHash() const
{
  cardano_blake2b_hash_t*                                           value = cardano_treasury_withdrawals_action_get_policy_hash(checked());
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const Blake2bHash found(owner.release());
  return Blake2bHash(found);
}

void
TreasuryWithdrawalsAction::setPolicyHash(const std::optional<Blake2bHash>& value)
{
  const std::optional<Blake2bHash> copy(value);
  cardano_blake2b_hash_t*          data = copy ? copy->checked() : nullptr;
  checkCardanoError(cardano_treasury_withdrawals_action_set_policy_hash(checked(), data), "TreasuryWithdrawalsAction setPolicyHash");
}

std::vector<std::uint8_t>
TreasuryWithdrawalsAction::toCbor() const
{
  return encodeCbor(checked(), cardano_treasury_withdrawals_action_to_cbor);
}

std::string
TreasuryWithdrawalsAction::toJson() const
{
  return encodeJson(checked(), cardano_treasury_withdrawals_action_to_cip116_json);
}

void
TreasuryWithdrawalsAction::swap(TreasuryWithdrawalsAction& other) noexcept
{
  std::swap(m_value, other.m_value);
}

TreasuryWithdrawalsAction&
TreasuryWithdrawalsAction::operator=(const TreasuryWithdrawalsAction& other)
{
  if (this != &other)
  {
    TreasuryWithdrawalsAction copy(other);
    swap(copy);
  }
  return *this;
}

TreasuryWithdrawalsAction&
TreasuryWithdrawalsAction::operator=(TreasuryWithdrawalsAction&& other) noexcept
{
  if (this != &other)
  {
    cardano_treasury_withdrawals_action_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
TreasuryWithdrawalsAction::operator==(const TreasuryWithdrawalsAction& other) const
{
  return toCbor() == other.toCbor();
}

bool
TreasuryWithdrawalsAction::operator!=(const TreasuryWithdrawalsAction& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

TreasuryWithdrawalsAction::TreasuryWithdrawalsAction(cardano_treasury_withdrawals_action_t* value) noexcept
: m_value(value)
{
}

cardano_treasury_withdrawals_action_t*
TreasuryWithdrawalsAction::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from TreasuryWithdrawalsAction");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(TreasuryWithdrawalsAction& lhs, TreasuryWithdrawalsAction& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
