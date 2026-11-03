/**
 * @file CoinSelectorAdapter.cpp
 *
 * @brief Native callback bridge for C++ coin selectors.
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

/* INCLUDES ******************************************************************/

#include "CoinSelectorAdapter.h"

#include "../CardanoInterop.h"

#include <cardano/address/address.h>
#include <cardano/common/utxo.h>
#include <cardano/common/utxo_list.h>
#include <cardano/object.h>
#include <cardano/protocol_params/protocol_parameters.h>
#include <cardano/transaction_body/transaction_output.h>
#include <cardano/transaction_body/transaction_output_list.h>
#include <cardano/transaction_body/value.h>
#include <cardano/transaction_builder/coin_selection/coin_selector.h>
#include <cardano/transaction_builder/coin_selection/coin_selector_impl.h>

#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <vector>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
/** @brief Keeps a C++ selector alive while native calls can reach it. */
struct SelectorContext : cardano_object_t
{
    /** @brief The retained strategy. */
    std::shared_ptr<CoinSelector> selector;

    /** @brief Stores the strategy. @param value The strategy to retain. */
    explicit SelectorContext(std::shared_ptr<CoinSelector> value)
    : cardano_object_t { 1, &SelectorContext::deallocate, {} }
    , selector(std::move(value))
    {
    }

    /** @brief Destroys the context. @param value The context to release. */
    static void
    deallocate(void* value)
    {
      delete static_cast<SelectorContext*>(static_cast<cardano_object_t*>(value));
    }
};

/**
 * @brief Copies a native UTxO list to C++ values.
 * @param value The list to copy, or null.
 * @return Independent UTxO values.
 */
UtxoList
copyUtxos(const cardano_utxo_list_t* value)
{
  UtxoList outputs;
  if (!value)
  {
    return outputs;
  }

  const std::size_t count = cardano_utxo_list_get_length(value);
  outputs.reserve(count);

  for (std::size_t index = 0; index < count; ++index)
  {
    cardano_utxo_t*                                   item   = nullptr;
    const cardano_error_t                             status = cardano_utxo_list_get(value, index, &item);
    CardanoObject<cardano_utxo_t, cardano_utxo_unref> owner(item);
    checkCardanoError(status, "CoinSelector UtxoList get");

    outputs.push_back(Utxo::fromCbor(encodeCbor(owner.get(), cardano_utxo_to_cbor)));
  }

  return outputs;
}

/**
 * @brief Copies C++ UTxOs into a native list.
 * @param values The values to copy.
 * @return An owned native list.
 */
cardano_utxo_list_t*
copyUtxos(const UtxoList& values)
{
  cardano_utxo_list_t*                                        value  = nullptr;
  const cardano_error_t                                       status = cardano_utxo_list_new(&value);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> owner(value);
  checkCardanoError(status, "CoinSelector UtxoList");

  for (const Utxo& item: values)
  {
    const std::vector<std::uint8_t>                   bytes = item.toCbor();
    CardanoObject<cardano_utxo_t, cardano_utxo_unref> native(
      decodeCbor<cardano_utxo_t, cardano_utxo_unref>(bytes, cardano_utxo_from_cbor));

    checkCardanoError(cardano_utxo_list_add(owner.get(), native.get()), "CoinSelector UtxoList add");
  }

  return owner.release();
}

/**
 * @brief Copies a native output list to C++ values.
 * @param value The list to copy, or null.
 * @return An independent output list.
 */
TransactionOutputList
copyOutputs(const cardano_transaction_output_list_t* value)
{
  if (!value)
  {
    return TransactionOutputList();
  }

  return TransactionOutputList::fromCbor(encodeCbor(value, cardano_transaction_output_list_to_cbor));
}

/**
 * @brief Copies a C++ output list to native storage.
 * @param value The list to copy.
 * @return An owned native list.
 */
cardano_transaction_output_list_t*
copyOutputs(const TransactionOutputList& value)
{
  return decodeCbor<cardano_transaction_output_list_t, cardano_transaction_output_list_unref>(
    value.toCbor(), cardano_transaction_output_list_from_cbor);
}

/**
 * @brief Copies a native address into a C++ value.
 * @param value The address to copy.
 * @return An independent address.
 */
Address
copyAddress(const cardano_address_t* value)
{
  std::vector<std::uint8_t> bytes(cardano_address_get_bytes_size(value));
  checkCardanoError(cardano_address_to_bytes(value, bytes.data(), bytes.size()), "CoinSelector address");
  return Address(bytes);
}
} // namespace

/** @brief Bridges a C++ selection strategy to a native callback. */
class CoinSelectorAdapter
{
  public:

    /**
     * @brief Runs a C++ strategy for a native request.
     * @param implementation The native strategy.
     * @param request The selection request.
     * @param selected Receives the selected outputs.
     * @param remaining Receives the remaining outputs.
     * @param change Receives the change outputs.
     * @return The native status.
     */
    static cardano_error_t
    select(
      cardano_coin_selector_impl_t*           implementation,
      const cardano_coin_selection_request_t* request,
      cardano_utxo_list_t**                   selected,
      cardano_utxo_list_t**                   remaining,
      cardano_transaction_output_list_t**     change) noexcept
    {
      try
      {
        ProtocolParameters parameters;
        cardano_protocol_parameters_unref(&parameters.m_value);
        checkCardanoError(
          cardano_protocol_parameters_deep_clone(request->protocol_params, &parameters.m_value),
          "CoinSelector parameters");

        const std::vector<std::uint8_t> targetBytes = encodeCbor(request->target, cardano_value_to_cbor);
        CoinSelectionRequest            selectionRequest(
          copyUtxos(request->available_utxo),
          Value::fromCbor(targetBytes),
          copyAddress(request->change_address),
          std::move(parameters));

        selectionRequest.preselected    = copyUtxos(request->pre_selected_utxo);
        selectionRequest.outputsToCover = copyOutputs(request->outputs_to_cover);

        const SelectorContext*    context = static_cast<const SelectorContext*>(implementation->context);
        const CoinSelectionResult result  = context->selector->select(selectionRequest);

        CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref>                             selectedOwner(copyUtxos(result.selected));
        CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref>                             remainingOwner(copyUtxos(result.remaining));
        CardanoObject<cardano_transaction_output_list_t, cardano_transaction_output_list_unref> changeOwner(
          copyOutputs(result.change));

        *selected  = selectedOwner.release();
        *remaining = remainingOwner.release();
        *change    = changeOwner.release();

        return CARDANO_SUCCESS;
      }
      catch (const std::bad_alloc&)
      {
        return CARDANO_ERROR_MEMORY_ALLOCATION_FAILED;
      }
      catch (const std::exception& error)
      {
        std::snprintf(implementation->error_message, sizeof(implementation->error_message), "%s", error.what());
        return CARDANO_ERROR_GENERIC;
      }
      catch (...)
      {
        std::snprintf(implementation->error_message, sizeof(implementation->error_message), "%s", "Coin selection failed");
        return CARDANO_ERROR_GENERIC;
      }
    }
};

/* FUNCTIONS IMPLEMENTATION **************************************************/

cardano_coin_selector_t*
createCoinSelectorAdapter(std::shared_ptr<CoinSelector> selector)
{
  if (!selector)
  {
    throw std::invalid_argument("CoinSelector is required");
  }

  const std::string name = selector->name();
  if (name.empty() || name.size() >= 256 || name.find('\0') != std::string::npos)
  {
    throw std::invalid_argument("CoinSelector name must contain 1 to 255 characters without NUL");
  }

  SelectorContext*             context = new SelectorContext(std::move(selector));
  cardano_coin_selector_impl_t implementation {};

  std::memcpy(implementation.name, name.data(), name.size());
  implementation.context = context;
  implementation.select  = &CoinSelectorAdapter::select;

  cardano_coin_selector_t* value  = nullptr;
  const cardano_error_t    status = cardano_coin_selector_new(implementation, &value);
  if (status != CARDANO_SUCCESS)
  {
    delete context;
    checkCardanoError(status, "CoinSelector adapter");
  }

  return value;
}
} // namespace Cometa
