/**
 * @file DeferredRedeemer.h
 *
 * @brief Redeemer generation after transaction balancing.
 *
 * @author angel.castillo
 * @date   Nov 03, 2026
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

#include <Cometa/Common/UtxoList.h>
#include <Cometa/PlutusData/PlutusData.h>
#include <Cometa/Transaction/Transaction.h>

#include <functional>

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/**
 * @brief Produces Plutus redeemer data from a balanced transaction draft and resolved inputs.
 *
 * The Transaction argument is the current draft, and the UtxoList supplies resolved input values. Use
 * the draft's lookup methods for ledger indices instead of assuming insertion order. The callback may
 * run on every balancing iteration; it must be deterministic and avoid side effects. References passed
 * to the callback are valid only for that invocation.
 */
using DeferredRedeemer = std::function<PlutusData(const Transaction&, const UtxoList&)>;
} // namespace Cometa
