/**
 * @file CoinSelectionRequest.h
 *
 * @brief The available funds, required inputs, and target value for coin selection.
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

#include <Cometa/Address/Address.h>
#include <Cometa/Common/UtxoList.h>
#include <Cometa/ProtocolParams/ProtocolParameters.h>
#include <Cometa/TransactionBody/TransactionOutputList.h>
#include <Cometa/TransactionBody/Value.h>

/* STRUCTURE DECLARATION *****************************************************/

namespace Cometa
{
/**
 * @brief The available funds, required inputs, and target value for coin selection.
 *
 * The selector must cover the target while respecting preselected inputs. Protocol parameters and the
 * change address are used when constructing valid change outputs.
 */
struct CoinSelectionRequest
{
    /**
     * @brief Creates a selection request.
     * @param available Outputs available for selection.
     * @param target Value to cover.
     * @param changeAddress Address that receives change.
     * @param parameters Network protocol parameters.
     */
    CoinSelectionRequest(
      UtxoList           available,
      Value              target,
      Address            changeAddress,
      ProtocolParameters parameters);

    /** @brief Outputs that must be included in the selection. */
    UtxoList preselected;

    /** @brief Outputs available for selection. */
    UtxoList available;

    /** @brief Value the selection must cover. */
    Value target;

    /** @brief Payment outputs used to guide change distribution. */
    TransactionOutputList outputsToCover;

    /** @brief Address that receives change. */
    Address changeAddress;

    /** @brief Network parameters used to calculate minimum output values. */
    ProtocolParameters parameters;
};
} // namespace Cometa
