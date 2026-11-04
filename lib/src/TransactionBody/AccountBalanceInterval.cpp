/**
 * @file AccountBalanceInterval.cpp
 *
 * @brief Dijkstra account balance requirements.
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

#include <Cometa/TransactionBody/AccountBalanceInterval.h>

#include "../CardanoInterop.h"

#include <cardano/transaction_body/account_balance_interval.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
AccountBalanceInterval::AccountBalanceInterval(std::optional<std::uint64_t> lower, std::optional<std::uint64_t> upper)
: m_value(nullptr)
{
  cardano_account_balance_interval_t*                                                       value  = nullptr;
  const cardano_error_t                                                                     status = cardano_account_balance_interval_new(lower ? &*lower : nullptr, upper ? &*upper : nullptr, &value);
  CardanoObject<cardano_account_balance_interval_t, cardano_account_balance_interval_unref> owner(value);

  checkCardanoError(status, "AccountBalanceInterval");
  m_value = owner.release();
}

AccountBalanceInterval::AccountBalanceInterval(const AccountBalanceInterval& other)
: m_value(nullptr)
{
  cardano_account_balance_interval_t*                                                       value  = nullptr;
  const cardano_error_t                                                                     status = cardano_account_balance_interval_deep_clone(other.checked(), &value);
  CardanoObject<cardano_account_balance_interval_t, cardano_account_balance_interval_unref> owner(value);

  checkCardanoError(status, "AccountBalanceInterval copy");
  m_value = owner.release();
}

AccountBalanceInterval::AccountBalanceInterval(AccountBalanceInterval&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

AccountBalanceInterval::~AccountBalanceInterval()
{
  cardano_account_balance_interval_unref(&m_value);
}

AccountBalanceInterval
AccountBalanceInterval::exact(std::uint64_t balance)
{
  cardano_account_balance_interval_t*                                                       value  = nullptr;
  const cardano_error_t                                                                     status = cardano_account_balance_interval_new_exact(balance, &value);
  CardanoObject<cardano_account_balance_interval_t, cardano_account_balance_interval_unref> owner(value);

  checkCardanoError(status, "AccountBalanceInterval exact");
  return AccountBalanceInterval(owner.release());
}

AccountBalanceInterval
AccountBalanceInterval::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return AccountBalanceInterval(decodeCbor<cardano_account_balance_interval_t, cardano_account_balance_interval_unref>(cbor, cardano_account_balance_interval_from_cbor));
}

AccountBalanceInterval
AccountBalanceInterval::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::optional<std::uint64_t>
AccountBalanceInterval::inclusiveLowerBound() const
{
  const std::uint64_t* value = cardano_account_balance_interval_get_inclusive_lower_bound(checked());
  return value ? std::optional<std::uint64_t>(*value) : std::nullopt;
}

std::optional<std::uint64_t>
AccountBalanceInterval::exclusiveUpperBound() const
{
  const std::uint64_t* value = cardano_account_balance_interval_get_exclusive_upper_bound(checked());
  return value ? std::optional<std::uint64_t>(*value) : std::nullopt;
}

std::optional<std::uint64_t>
AccountBalanceInterval::exactBalance() const
{
  const std::uint64_t* value = cardano_account_balance_interval_get_exact_balance(checked());
  return value ? std::optional<std::uint64_t>(*value) : std::nullopt;
}

bool
AccountBalanceInterval::isExact() const
{
  return cardano_account_balance_interval_is_exact(checked());
}

std::vector<std::uint8_t>
AccountBalanceInterval::toCbor() const
{
  return encodeCbor(checked(), cardano_account_balance_interval_to_cbor);
}

std::string
AccountBalanceInterval::toJson() const
{
  return encodeJson(checked(), cardano_account_balance_interval_to_cip116_json);
}

void
AccountBalanceInterval::swap(AccountBalanceInterval& other) noexcept
{
  std::swap(m_value, other.m_value);
}

AccountBalanceInterval&
AccountBalanceInterval::operator=(const AccountBalanceInterval& other)
{
  if (this != &other)
  {
    AccountBalanceInterval copy(other);
    swap(copy);
  }
  return *this;
}

AccountBalanceInterval&
AccountBalanceInterval::operator=(AccountBalanceInterval&& other) noexcept
{
  if (this != &other)
  {
    cardano_account_balance_interval_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
AccountBalanceInterval::operator==(const AccountBalanceInterval& other) const
{
  return toCbor() == other.toCbor();
}

bool
AccountBalanceInterval::operator!=(const AccountBalanceInterval& other) const
{
  return !(*this == other);
}

AccountBalanceInterval
AccountBalanceInterval::adopt(cardano_account_balance_interval_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null AccountBalanceInterval");
  }

  return AccountBalanceInterval(value);
}

cardano_account_balance_interval_t*
AccountBalanceInterval::nativeHandle() noexcept
{
  return m_value;
}

const cardano_account_balance_interval_t*
AccountBalanceInterval::nativeHandle() const noexcept
{
  return m_value;
}

cardano_account_balance_interval_t*
AccountBalanceInterval::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

AccountBalanceInterval::AccountBalanceInterval(cardano_account_balance_interval_t* value) noexcept
: m_value(value)
{
}

cardano_account_balance_interval_t*
AccountBalanceInterval::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from AccountBalanceInterval");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(AccountBalanceInterval& lhs, AccountBalanceInterval& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
