/**
 * @file CoinSelectorAdapterTest.cpp
 *
 * @brief Tests native dispatch to C++ coin selection strategies.
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

#include <Cometa/Cometa.h>
#include <Cometa/TransactionBuilder/CoinSelector.h>

#include "../../src/CardanoInterop.h"
#include "../../src/TransactionBuilder/CoinSelectorAdapter.h"

#include <cardano/address/address.h>
#include <cardano/common/utxo.h>
#include <cardano/common/utxo_list.h>
#include <cardano/protocol_params/protocol_parameters.h>
#include <cardano/transaction_body/transaction_output_list.h>
#include <cardano/transaction_body/value.h>
#include <cardano/transaction_builder/coin_selection/coin_selector.h>

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Makes the test payment address. @return The address. */
Address
sampleAddress()
{
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  return EnterpriseAddress(NetworkId::Testnet, credential).toAddress();
}

/** @brief Makes one available output. @return The output. */
Utxo
sampleUtxo()
{
  return Utxo(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(sampleAddress(), Value(2000000)));
}

/** @brief A deterministic strategy for native callback tests. */
class TestSelector : public CoinSelector
{
  public:

    /** @brief Enables a callback failure. */
    bool fail = false;

    /** @brief Gets the strategy name. @return Its name. */
    std::string
    name() const override
    {
      return "test-selector";
    }

    /** @brief Selects the only available output. @param request The input. @return Selected output and change. */
    CoinSelectionResult
    select(const CoinSelectionRequest& request) const override
    {
      if (fail)
      {
        throw std::runtime_error("Selection failed");
      }

      if (request.available.size() != 1 || request.target.coin() != 1000000 || request.changeAddress != sampleAddress() || request.parameters.minFeeA() != 44)
      {
        throw std::invalid_argument("Unexpected selection request");
      }

      CoinSelectionResult result;
      result.selected = request.available;
      result.change.pushBack(TransactionOutput(request.changeAddress, Value(1000000)));
      return result;
    }
};
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(CoinSelectorAdapter, DispatchesAndCopiesSelection)
{
  // Arrange
  const std::shared_ptr<TestSelector>                                 strategy = std::make_shared<TestSelector>();
  CardanoObject<cardano_coin_selector_t, cardano_coin_selector_unref> selector(createCoinSelectorAdapter(strategy));
  const Utxo                                                          utxo      = sampleUtxo();
  const std::vector<std::uint8_t>                                     utxoBytes = utxo.toCbor();
  CardanoObject<cardano_utxo_t, cardano_utxo_unref>                   nativeUtxo(
    decodeCbor<cardano_utxo_t, cardano_utxo_unref>(utxoBytes, cardano_utxo_from_cbor));

  cardano_utxo_list_t* available = nullptr;
  ASSERT_EQ(cardano_utxo_list_new(&available), CARDANO_SUCCESS);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> nativeAvailable(available);
  ASSERT_EQ(cardano_utxo_list_add(nativeAvailable.get(), nativeUtxo.get()), CARDANO_SUCCESS);

  const Address                   address            = sampleAddress();
  const std::vector<std::uint8_t> addressBytes       = address.toBytes();
  cardano_address_t*              nativeAddressValue = nullptr;
  ASSERT_EQ(cardano_address_from_bytes(addressBytes.data(), addressBytes.size(), &nativeAddressValue), CARDANO_SUCCESS);
  CardanoObject<cardano_address_t, cardano_address_unref> nativeAddress(nativeAddressValue);

  const Value                                         value(1000000);
  CardanoObject<cardano_value_t, cardano_value_unref> nativeValue(
    decodeCbor<cardano_value_t, cardano_value_unref>(value.toCbor(), cardano_value_from_cbor));

  cardano_protocol_parameters_t* nativeParametersValue = nullptr;
  ASSERT_EQ(cardano_protocol_parameters_new(&nativeParametersValue), CARDANO_SUCCESS);
  CardanoObject<cardano_protocol_parameters_t, cardano_protocol_parameters_unref> nativeParameters(nativeParametersValue);
  ASSERT_EQ(cardano_protocol_parameters_set_min_fee_a(nativeParameters.get(), 44), CARDANO_SUCCESS);

  cardano_coin_selection_request_t request {};
  request.available_utxo  = nativeAvailable.get();
  request.target          = nativeValue.get();
  request.change_address  = nativeAddress.get();
  request.protocol_params = nativeParameters.get();

  // Act
  cardano_utxo_list_t*                                                                    selected  = nullptr;
  cardano_utxo_list_t*                                                                    remaining = nullptr;
  cardano_transaction_output_list_t*                                                      change    = nullptr;
  const cardano_error_t                                                                   status    = cardano_coin_selector_select(selector.get(), &request, &selected, &remaining, &change);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref>                             selectedOwner(selected);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref>                             remainingOwner(remaining);
  CardanoObject<cardano_transaction_output_list_t, cardano_transaction_output_list_unref> changeOwner(change);

  // Assert
  EXPECT_EQ(status, CARDANO_SUCCESS);
  ASSERT_NE(selectedOwner, nullptr);
  ASSERT_NE(remainingOwner, nullptr);
  ASSERT_NE(changeOwner, nullptr);
  EXPECT_EQ(cardano_utxo_list_get_length(selectedOwner.get()), 1U);
  EXPECT_EQ(cardano_utxo_list_get_length(remainingOwner.get()), 0U);
  EXPECT_EQ(cardano_transaction_output_list_get_length(changeOwner.get()), 1U);

  // Arrange
  strategy->fail = true;
  selected       = nullptr;
  remaining      = nullptr;
  change         = nullptr;

  // Act
  const cardano_error_t failure = cardano_coin_selector_select(selector.get(), &request, &selected, &remaining, &change);

  // Assert
  EXPECT_EQ(failure, CARDANO_ERROR_GENERIC);
  EXPECT_STREQ(cardano_coin_selector_get_last_error(selector.get()), "Selection failed");
  EXPECT_EQ(selected, nullptr);
  EXPECT_EQ(remaining, nullptr);
  EXPECT_EQ(change, nullptr);
}
