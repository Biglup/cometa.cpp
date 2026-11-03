/**
 * @file DeferredRedeemerAdapterTest.cpp
 *
 * @brief Tests deferred redeemer callback conversion and error preservation.
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

#include "../../src/CardanoInterop.h"
#include "../../src/TransactionBuilder/DeferredRedeemerAdapter.h"

#include <cardano/common/utxo.h>
#include <cardano/common/utxo_list.h>
#include <cardano/plutus_data/plutus_data.h>
#include <cardano/transaction/transaction.h>

#include <gtest/gtest.h>

#include <stdexcept>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Produces a layout-dependent redeemer. @param draft The draft. @param inputs Resolved inputs. @return The redeemer. */
PlutusData
makeRedeemer(const Transaction& draft, const UtxoList& inputs)
{
  if (draft.body().fee() != 10 || inputs.size() != 1)
  {
    throw std::invalid_argument("Unexpected deferred redeemer request");
  }

  return PlutusData(42);
}

/** @brief Simulates a user callback failure. @param draft The draft. @param inputs Resolved inputs. @return Nothing. */
PlutusData
throwingRedeemer(const Transaction& draft, const UtxoList& inputs)
{
  static_cast<void>(draft);
  static_cast<void>(inputs);
  throw std::runtime_error("Deferred callback failed");
}
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(DeferredRedeemerAdapter, CopiesDraftInputsAndPreservesErrors)
{
  // Arrange
  const Transaction                                               draft(TransactionBody(TransactionInputSet(), TransactionOutputList(), 10), WitnessSet());
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> nativeDraft(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(draft.toCbor(), cardano_transaction_from_cbor));

  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const Address    address = EnterpriseAddress(NetworkId::Testnet, credential).toAddress();
  const Utxo       utxo(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(2000000)));
  CardanoObject<cardano_utxo_t, cardano_utxo_unref> nativeUtxo(
    decodeCbor<cardano_utxo_t, cardano_utxo_unref>(utxo.toCbor(), cardano_utxo_from_cbor));
  cardano_utxo_list_t* nativeInputsValue = nullptr;
  ASSERT_EQ(cardano_utxo_list_new(&nativeInputsValue), CARDANO_SUCCESS);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> nativeInputs(nativeInputsValue);
  ASSERT_EQ(cardano_utxo_list_add(nativeInputs.get(), nativeUtxo.get()), CARDANO_SUCCESS);
  DeferredRedeemerContext context { &makeRedeemer, nullptr };

  // Act
  cardano_plutus_data_t* resultValue = nullptr;
  const cardano_error_t  status      = invokeDeferredRedeemer(
    &context, nativeDraft.get(), nativeInputs.get(), &resultValue);
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> result(resultValue);

  // Assert
  EXPECT_EQ(status, CARDANO_SUCCESS);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(PlutusData::fromCbor(encodeCbor(result.get(), cardano_plutus_data_to_cbor)), PlutusData(42));

  // Arrange
  context.callback = &throwingRedeemer;
  resultValue      = nullptr;

  // Act
  const cardano_error_t failure = invokeDeferredRedeemer(
    &context, nativeDraft.get(), nativeInputs.get(), &resultValue);

  // Assert
  EXPECT_EQ(failure, CARDANO_ERROR_GENERIC);
  EXPECT_EQ(resultValue, nullptr);
  ASSERT_TRUE(context.error);
  EXPECT_THROW(std::rethrow_exception(context.error), std::runtime_error);
}
