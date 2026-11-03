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
 * @brief Produces a redeemer from the final draft layout and resolved inputs.
 *
 * The function may run on every balancing iteration. It must return the same
 * result for the same arguments and avoid side effects.
 */
using DeferredRedeemer = std::function<PlutusData(const Transaction&, const UtxoList&)>;
} // namespace Cometa
