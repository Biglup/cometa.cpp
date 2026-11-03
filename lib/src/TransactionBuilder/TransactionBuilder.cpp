/**
 * @file TransactionBuilder.cpp
 *
 * @brief Balanced transaction construction.
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

#include <Cometa/TransactionBuilder/TransactionBuilder.h>

#include "../CardanoInterop.h"
#include "CoinSelectorAdapter.h"
#include "TransactionEvaluatorAdapter.h"

#include <cardano/address/address.h>
#include <cardano/common/utxo.h>
#include <cardano/common/utxo_list.h>
#include <cardano/plutus_data/plutus_data.h>
#include <cardano/slot_config.h>
#include <cardano/transaction/transaction.h>
#include <cardano/transaction_body/transaction_output.h>
#include <cardano/transaction_body/value.h>
#include <cardano/transaction_builder/coin_selection/coin_selector.h>
#include <cardano/transaction_builder/evaluation/tx_evaluator.h>
#include <cardano/transaction_builder/transaction_builder.h>

#include <stdexcept>
#include <string>
#include <utility>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
/**
 * @brief Copies a payment address to native storage.
 * @param address The address to copy.
 * @return An owned native address.
 */
cardano_address_t*
copyAddress(const Address& address)
{
  const std::vector<std::uint8_t>                         bytes  = address.toBytes();
  cardano_address_t*                                      value  = nullptr;
  const cardano_error_t                                   status = cardano_address_from_bytes(bytes.data(), bytes.size(), &value);
  CardanoObject<cardano_address_t, cardano_address_unref> owner(value);
  checkCardanoError(status, "TransactionBuilder address");
  return owner.release();
}

/**
 * @brief Copies unspent outputs to native storage.
 * @param utxos The outputs to copy.
 * @return An owned native list.
 */
cardano_utxo_list_t*
copyUtxos(const UtxoList& utxos)
{
  cardano_utxo_list_t*                                        value  = nullptr;
  const cardano_error_t                                       status = cardano_utxo_list_new(&value);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> owner(value);
  checkCardanoError(status, "TransactionBuilder UtxoList");

  for (const Utxo& utxo: utxos)
  {
    CardanoObject<cardano_utxo_t, cardano_utxo_unref> native(
      decodeCbor<cardano_utxo_t, cardano_utxo_unref>(utxo.toCbor(), cardano_utxo_from_cbor));
    checkCardanoError(cardano_utxo_list_add(owner.get(), native.get()), "TransactionBuilder UtxoList add");
  }

  return owner.release();
}

/**
 * @brief Copies optional Plutus data to native storage.
 * @param value The data to copy, or no value.
 * @return An owned native datum or null.
 */
cardano_plutus_data_t*
copyPlutusData(const std::optional<PlutusData>& value)
{
  if (!value)
  {
    return nullptr;
  }

  return decodeCbor<cardano_plutus_data_t, cardano_plutus_data_unref>(
    value->toCbor(), cardano_plutus_data_from_cbor);
}
} // namespace

/* IMPLEMENTATION ************************************************************/

TransactionBuilder::TransactionBuilder(const ProtocolParameters& parameters, SlotConfig slots)
: m_value(nullptr)
{
  if (slots.slotLength == 0)
  {
    throw std::invalid_argument("Slot duration must be positive");
  }

  const cardano_slot_config_t nativeSlots { slots.zeroTime, slots.zeroSlot, slots.slotLength };
  m_value = cardano_tx_builder_new(parameters.checked(), &nativeSlots);

  if (!m_value)
  {
    throw std::bad_alloc();
  }
}

TransactionBuilder::TransactionBuilder(TransactionBuilder&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_coinSelector(std::move(other.m_coinSelector))
, m_evaluator(std::move(other.m_evaluator))
{
}

TransactionBuilder::~TransactionBuilder()
{
  cardano_tx_builder_unref(&m_value);
}

TransactionBuilder&
TransactionBuilder::setCoinSelector(std::shared_ptr<CoinSelector> selector)
{
  CardanoObject<cardano_coin_selector_t, cardano_coin_selector_unref> native(
    createCoinSelectorAdapter(selector));
  cardano_tx_builder_set_coin_selector(checked(), native.get());
  m_coinSelector = std::move(selector);
  return *this;
}

TransactionBuilder&
TransactionBuilder::setTransactionEvaluator(std::shared_ptr<TransactionEvaluator> evaluator)
{
  CardanoObject<cardano_tx_evaluator_t, cardano_tx_evaluator_unref> native(
    createTransactionEvaluatorAdapter(evaluator));
  cardano_tx_builder_set_tx_evaluator(checked(), native.get());
  m_evaluator = std::move(evaluator);
  return *this;
}

TransactionBuilder&
TransactionBuilder::setNetworkId(NetworkId network)
{
  cardano_tx_builder_set_network_id(checked(), static_cast<cardano_network_id_t>(network));
  return *this;
}

TransactionBuilder&
TransactionBuilder::setChangeAddress(const Address& address)
{
  CardanoObject<cardano_address_t, cardano_address_unref> native(copyAddress(address));
  cardano_tx_builder_set_change_address(checked(), native.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::setUtxos(const UtxoList& utxos)
{
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> native(copyUtxos(utxos));
  cardano_tx_builder_set_utxos(checked(), native.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::setCollateralUtxos(const UtxoList& utxos)
{
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> native(copyUtxos(utxos));
  cardano_tx_builder_set_collateral_utxos(checked(), native.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::setInvalidBefore(std::uint64_t slot)
{
  cardano_tx_builder_set_invalid_before(checked(), slot);
  return *this;
}

TransactionBuilder&
TransactionBuilder::setInvalidAfter(std::uint64_t slot)
{
  cardano_tx_builder_set_invalid_after(checked(), slot);
  return *this;
}

TransactionBuilder&
TransactionBuilder::setMinimumFee(std::uint64_t lovelace)
{
  cardano_tx_builder_set_minimum_fee(checked(), lovelace);
  return *this;
}

TransactionBuilder&
TransactionBuilder::sendLovelace(const Address& address, std::uint64_t lovelace)
{
  CardanoObject<cardano_address_t, cardano_address_unref> native(copyAddress(address));
  cardano_tx_builder_send_lovelace(checked(), native.get(), lovelace);
  return *this;
}

TransactionBuilder&
TransactionBuilder::sendValue(const Address& address, const Value& value)
{
  CardanoObject<cardano_address_t, cardano_address_unref> nativeAddress(copyAddress(address));
  CardanoObject<cardano_value_t, cardano_value_unref>     nativeValue(
    decodeCbor<cardano_value_t, cardano_value_unref>(value.toCbor(), cardano_value_from_cbor));
  cardano_tx_builder_send_value(checked(), nativeAddress.get(), nativeValue.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::addInput(
  const Utxo&                      utxo,
  const std::optional<PlutusData>& redeemer,
  const std::optional<PlutusData>& datum)
{
  CardanoObject<cardano_utxo_t, cardano_utxo_unref> nativeUtxo(
    decodeCbor<cardano_utxo_t, cardano_utxo_unref>(utxo.toCbor(), cardano_utxo_from_cbor));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeRedeemer(copyPlutusData(redeemer));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeDatum(copyPlutusData(datum));

  cardano_tx_builder_add_input(checked(), nativeUtxo.get(), nativeRedeemer.get(), nativeDatum.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::addOutput(const TransactionOutput& output)
{
  CardanoObject<cardano_transaction_output_t, cardano_transaction_output_unref> native(
    decodeCbor<cardano_transaction_output_t, cardano_transaction_output_unref>(
      output.toCbor(), cardano_transaction_output_from_cbor));
  cardano_tx_builder_add_output(checked(), native.get());
  return *this;
}

Transaction
TransactionBuilder::build()
{
  cardano_transaction_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_tx_builder_build(checked(), &value);
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> owner(value);

  if (status != CARDANO_SUCCESS)
  {
    const std::string message = std::string("TransactionBuilder build: ") + cardano_tx_builder_get_last_error(checked());
    throw Error(static_cast<ErrorCode>(status), message.c_str());
  }

  return Transaction::fromCbor(encodeCbor(owner.get(), cardano_transaction_to_cbor));
}

TransactionBuilder&
TransactionBuilder::operator=(TransactionBuilder&& other) noexcept
{
  if (this != &other)
  {
    cardano_tx_builder_unref(&m_value);
    m_value        = std::exchange(other.m_value, nullptr);
    m_coinSelector = std::move(other.m_coinSelector);
    m_evaluator    = std::move(other.m_evaluator);
  }

  return *this;
}

cardano_tx_builder_t*
TransactionBuilder::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("TransactionBuilder has no value");
  }

  return m_value;
}
} // namespace Cometa
