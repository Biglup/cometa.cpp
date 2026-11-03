/**
 * @file TransactionEvaluatorAdapterTest.cpp
 *
 * @brief Tests native dispatch to C++ transaction evaluators.
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
#include <Cometa/TransactionBuilder/NativeTransactionEvaluator.h>
#include <Cometa/TransactionBuilder/TransactionEvaluator.h>

#include "../../src/CardanoInterop.h"
#include "../../src/TransactionBuilder/TransactionEvaluatorAdapter.h"

#include <cardano/common/utxo.h>
#include <cardano/common/utxo_list.h>
#include <cardano/transaction/transaction.h>
#include <cardano/transaction_builder/evaluation/tx_evaluator.h>
#include <cardano/witness_set/redeemer_list.h>

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Makes one resolved output. @return The output. */
Utxo
sampleUtxo()
{
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const Address    address = EnterpriseAddress(NetworkId::Testnet, credential).toAddress();
  return Utxo(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(2000000)));
}

/** @brief Supplies deterministic execution budgets to the native bridge. */
class TestEvaluator : public TransactionEvaluator
{
  public:

    /** @brief Enables a callback failure. */
    bool fail = false;

    /** @brief Gets the evaluator name. @return Its name. */
    std::string
    name() const override
    {
      return "test-evaluator";
    }

    /** @brief Evaluates the test transaction. @param transaction The transaction. @param additionalUtxos Resolved outputs. @return No redeemers. */
    RedeemerList
    evaluate(const Transaction& transaction, const UtxoList& additionalUtxos) const override
    {
      if (fail)
      {
        throw std::runtime_error("Evaluation failed");
      }

      if (transaction.body().fee() != 10 || additionalUtxos.size() != 1 || additionalUtxos[0] != sampleUtxo())
      {
        throw std::invalid_argument("Unexpected evaluation request");
      }

      return RedeemerList();
    }
};
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(TransactionEvaluatorAdapter, DispatchesAndReportsFailure)
{
  // Arrange
  const std::shared_ptr<TestEvaluator>                              evaluator = std::make_shared<TestEvaluator>();
  CardanoObject<cardano_tx_evaluator_t, cardano_tx_evaluator_unref> native(createTransactionEvaluatorAdapter(evaluator));
  const Transaction                                                 transaction(TransactionBody(TransactionInputSet(), TransactionOutputList(), 10), WitnessSet());
  CardanoObject<cardano_transaction_t, cardano_transaction_unref>   nativeTransaction(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(transaction.toCbor(), cardano_transaction_from_cbor));

  const Utxo                                        utxo = sampleUtxo();
  CardanoObject<cardano_utxo_t, cardano_utxo_unref> nativeUtxo(
    decodeCbor<cardano_utxo_t, cardano_utxo_unref>(utxo.toCbor(), cardano_utxo_from_cbor));
  cardano_utxo_list_t* nativeUtxosValue = nullptr;
  ASSERT_EQ(cardano_utxo_list_new(&nativeUtxosValue), CARDANO_SUCCESS);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> nativeUtxos(nativeUtxosValue);
  ASSERT_EQ(cardano_utxo_list_add(nativeUtxos.get(), nativeUtxo.get()), CARDANO_SUCCESS);

  // Act
  cardano_redeemer_list_t* resultValue = nullptr;
  const cardano_error_t    status      = cardano_tx_evaluator_evaluate(
    native.get(), nativeTransaction.get(), nativeUtxos.get(), &resultValue);
  CardanoObject<cardano_redeemer_list_t, cardano_redeemer_list_unref> result(resultValue);
  const RedeemerList                                                  cppResult = evaluateWithNative(native.get(), transaction, { utxo });

  // Assert
  EXPECT_EQ(status, CARDANO_SUCCESS);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(cardano_redeemer_list_get_length(result.get()), 0U);
  EXPECT_EQ(cppResult.size(), 0U);
  EXPECT_STREQ(cardano_tx_evaluator_get_name(native.get()), "test-evaluator");

  // Arrange
  evaluator->fail = true;
  resultValue     = nullptr;

  // Act
  const cardano_error_t failure = cardano_tx_evaluator_evaluate(
    native.get(), nativeTransaction.get(), nativeUtxos.get(), &resultValue);

  // Assert
  EXPECT_EQ(failure, CARDANO_ERROR_GENERIC);
  EXPECT_STREQ(cardano_tx_evaluator_get_last_error(native.get()), "Evaluation failed");
  EXPECT_EQ(resultValue, nullptr);
}

TEST(TransactionEvaluatorAdapter, NativeEvaluatorHandlesTransactionWithoutScripts)
{
  // Arrange
  const NativeTransactionEvaluator evaluator(SlotConfig::forNetwork(NetworkMagic::Preprod), CostModels(), 11);
  const Transaction                transaction(TransactionBody(TransactionInputSet(), TransactionOutputList(), 10), WitnessSet());

  // Act
  const RedeemerList result = evaluator.evaluate(transaction, {});

  // Assert
  EXPECT_EQ(evaluator.name(), "native");
  EXPECT_EQ(result.size(), 0U);
}
