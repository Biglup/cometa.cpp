/**
 * @file ScriptDataHash.h
 *
 * @brief Transaction script-data commitments.
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

#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/ProtocolParams/CostModels.h>
#include <Cometa/WitnessSet/PlutusDataSet.h>
#include <Cometa/WitnessSet/RedeemerList.h>

#include <optional>

/* DECLARATIONS **************************************************************/

namespace Cometa
{
/**
 * @brief Computes the commitment to redeemers, witness datums, and language views.
 * @param costModels Only the language views required by the transaction.
 * @param redeemers The final redeemers and their execution budgets.
 * @param datums Witness datums; inline output datums are not included.
 * @return The 32-byte commitment, or std::nullopt when both data collections are empty.
 * @throws Error The operation cannot be completed with the supplied values.
 */
std::optional<Blake2bHash> computeScriptDataHash(const CostModels& costModels, const RedeemerList& redeemers, const PlutusDataSet& datums);
} // namespace Cometa
