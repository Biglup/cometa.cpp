/**
 * @file TransactionBalanceRequest.h
 *
 * @brief Inputs and services for standalone transaction balancing.
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

#include <Cometa/Address/Address.h>
#include <Cometa/Common/UtxoList.h>
#include <Cometa/ProtocolParams/ProtocolParameters.h>
#include <Cometa/SlotConfig.h>
#include <Cometa/Transaction/Transaction.h>
#include <Cometa/TransactionBuilder/CoinSelector.h>
#include <Cometa/TransactionBuilder/TransactionEvaluator.h>

#include <cstddef>
#include <memory>
#include <optional>

/* DECLARATIONS **************************************************************/

namespace Cometa
{
/**
 * @brief An unsigned draft, its available funds, and the network context for balancing.
 *
 * Initial spending inputs are resolved from preselected or available UTxOs. Additional preselected
 * inputs are mandatory. Provide batch spending and reference UTxOs in available or referenceInputs.
 */
struct TransactionBalanceRequest
{
    /**
     * @brief Creates a balancing request without changing the supplied draft.
     * @param transaction The unsigned draft to balance.
     * @param parameters Current network parameters.
     * @param changeAddress The address receiving surplus value.
     * @param slots The network slot timeline used by the default evaluator.
     */
    TransactionBalanceRequest(Transaction transaction, ProtocolParameters parameters, Address changeAddress, SlotConfig slots);

    /** @brief The unsigned draft; top-level key signatures must be absent. */
    Transaction transaction;

    /** @brief Network fees, deposits, limits, and Plutus cost models. */
    ProtocolParameters parameters;

    /** @brief The address receiving ordinary change. */
    Address changeAddress;

    /** @brief The timeline used by the default local evaluator. */
    SlotConfig slots;

    /** @brief UTxOs that must be spent in addition to the draft's initial inputs. */
    UtxoList preselected;

    /** @brief UTxOs eligible for selection, also resolving the draft and batch inputs. */
    UtxoList available;

    /** @brief Resolved reference inputs and any additional batch input context. */
    UtxoList referenceInputs;

    /** @brief UTxOs available as collateral when Plutus redeemers are present. */
    UtxoList collateral;

    /** @brief The collateral return address; defaults to changeAddress when absent. */
    std::optional<Address> collateralChangeAddress;

    /** @brief Additional signing keys whose witness size must be included in fee estimation. */
    std::size_t foreignSignatureCount = 0;

    /** @brief The selection service; an empty pointer selects random-improve. */
    std::shared_ptr<CoinSelector> coinSelector;

    /** @brief The evaluation service; an empty pointer selects local Plutus evaluation. */
    std::shared_ptr<TransactionEvaluator> evaluator;
};
} // namespace Cometa
