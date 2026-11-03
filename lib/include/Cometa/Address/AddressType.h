/**
 * @file AddressType.h
 *
 * @brief AddressType value.
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

#pragma once

/* INCLUDES ******************************************************************/

#include <cstdint>

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/**
 * @brief Identifies an address format and its credential kinds.
 *
 * Use Address::type() to choose the matching address accessor. Payment and reward addresses have
 * different ledger roles.
 */
enum class AddressType : std::uint8_t
{
  /** @brief Base address with key payment and stake credentials. */
  BasePaymentKeyStakeKey = 0,

  /** @brief Base address with script payment and key stake credentials. */
  BasePaymentScriptStakeKey = 1,

  /** @brief Base address with key payment and script stake credentials. */
  BasePaymentKeyStakeScript = 2,

  /** @brief Base address with script payment and stake credentials. */
  BasePaymentScriptStakeScript = 3,

  /** @brief Pointer address with a key payment credential. */
  PointerKey = 4,

  /** @brief Pointer address with a script payment credential. */
  PointerScript = 5,

  /** @brief Enterprise address with a key payment credential. */
  EnterpriseKey = 6,

  /** @brief Enterprise address with a script payment credential. */
  EnterpriseScript = 7,

  /** @brief Byron bootstrap address. */
  Byron = 8,

  /** @brief Reward account with a key credential. */
  RewardKey = 14,

  /** @brief Reward account with a script credential. */
  RewardScript = 15
};
} // namespace Cometa
