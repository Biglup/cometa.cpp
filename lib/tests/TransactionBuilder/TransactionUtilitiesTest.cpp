/**
 * @file TransactionUtilitiesTest.cpp
 *
 * @brief Tests standalone transaction operations and governance construction.
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
#include <Cometa/TransactionBuilder/LargeFirstCoinSelector.h>
#include <Cometa/TransactionBuilder/TransactionBuilder.h>

#include <gtest/gtest.h>
#include <src/allocators.h>

#include <cstdlib>
#include <limits>
#include <memory>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Creates test protocol parameters. @return The parameters. */
ProtocolParameters
parameters()
{
  ProtocolParameters result;
  result.setProtocolVersion(ProtocolVersion(12, 0));
  result.setMinFeeA(44);
  result.setMinFeeB(155381);
  result.setAdaPerUtxoByte(4310);
  result.setMaxTxSize(16384);
  result.setKeyDeposit(2'000'000);
  result.setGovernanceActionDeposit(1'000'000);
  result.setExecutionCosts(ExUnitPrices(UnitInterval(1, 2), UnitInterval(1, 4)));
  result.setRefScriptCostPerByte(UnitInterval(2, 1));

  return result;
}

/** @brief Creates a payment address. @return The address. */
Address
address()
{
  return EnterpriseAddress(NetworkId::Testnet, Credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1))));
}

/** @brief Creates a funded output. @param byte The transaction ID byte. @param coin The lovelace value. @return The UTxO. */
Utxo
funding(std::uint8_t byte = 2, std::int64_t coin = 10'000'000)
{
  return Utxo(TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, byte)), 0), TransactionOutput(address(), Value(coin)));
}

/** @brief Creates a draft payment. @return The unsigned draft. */
Transaction
draft()
{
  return Transaction(TransactionBody(TransactionInputSet(), TransactionOutputList { TransactionOutput(address(), Value(3'000'000)) }, 0), WitnessSet());
}

/** @brief Counts evaluator calls without evaluating scripts. */
class CountingEvaluator : public TransactionEvaluator
{
  public:

    /** @brief Number of evaluation invocations. */
    mutable std::size_t calls = 0;

    /** @brief Gets the test evaluator name. @return The name. */
    std::string
    name() const override
    {
      return "counting";
    }

    /** @brief Returns the draft's budgets. @param transaction The draft. @param additionalUtxos Resolved context. @return Existing redeemers or an empty list. */
    RedeemerList
    evaluate(const Transaction& transaction, const UtxoList& additionalUtxos) const override
    {
      (void)additionalUtxos;
      ++calls;
      return transaction.witnessSet().redeemers().value_or(RedeemerList());
    }
};

/** @brief Counts selection calls and can simulate a service failure. */
class CountingSelector : public CoinSelector
{
  public:

    /** @brief Number of selection invocations. */
    mutable std::size_t calls = 0;

    /** @brief Whether selection should fail. */
    bool fail = false;

    /** @brief Gets the test selector name. @return The name. */
    std::string
    name() const override
    {
      return "counting";
    }

    /** @brief Selects funds or reports a service error. @param request The selection request. @return Selected funds and change. */
    CoinSelectionResult
    select(const CoinSelectionRequest& request) const override
    {
      ++calls;

      if (fail)
      {
        throw std::runtime_error("Test selection failure");
      }

      return LargeFirstCoinSelector().select(request);
    }
};

/** @brief Next native allocation to fail. */
std::size_t failureIndex = 0;
/** @brief Number of native allocations attempted. */
std::size_t allocationIndex = 0;

/** @brief Allocates or injects a failure. @param size The byte count. @return Memory or null. */
void*
failingMalloc(std::size_t size)
{
  return allocationIndex++ == failureIndex ? nullptr : std::malloc(size);
}

/** @brief Reallocates or injects a failure. @param pointer Existing memory. @param size The byte count. @return Memory or null. */
void*
failingRealloc(void* pointer, std::size_t size)
{
  return allocationIndex++ == failureIndex ? nullptr : std::realloc(pointer, size);
}

/** @brief Restores allocators after a failure injection. */
class AllocationFailure
{
  public:

    /** @brief Selects a failing allocation. @param index The zero-based allocation index. */
    explicit AllocationFailure(std::size_t index)
    {
      failureIndex    = index;
      allocationIndex = 0;
      cardano_set_allocators(failingMalloc, failingRealloc, std::free);
    }

    /** @brief Restores the ordinary allocator. */
    ~AllocationFailure()
    {
      cardano_set_allocators(std::malloc, std::realloc, std::free);
    }
};

/** @brief Dispatches an operation for allocation-failure cleanup checks. @param operation The operation index. @param transaction The immutable source. @param params The protocol parameters. @param utxos Available funds. */
void
exercise(std::size_t operation, const Transaction& transaction, const ProtocolParameters& params, const UtxoList& utxos)
{
  switch (operation)
  {
    case 0:
      (void)computeTransactionFee(transaction, utxos, params);
      break;
    case 1:
      (void)computeMinAdaRequired(utxos.front().output(), 4310);
      break;
    case 2:
      (void)computeImplicitCoin(transaction, params);
      break;
    case 3:
      (void)computeTransactionImbalance(transaction, utxos, params);
      break;
    case 4:
      (void)computeScriptDataHash(CostModels(), RedeemerList(), PlutusDataSet { PlutusData(BigInt(42)) });
      break;
    case 5:
      (void)totalExecutionUnits(RedeemerList { Redeemer(RedeemerTag::Spend, 0, PlutusData(BigInt(0)), ExUnits(10, 20)) });
      break;
    case 6:
    {
      TransactionBalanceRequest request(transaction, params, address(), SlotConfig::forNetwork(NetworkMagic::Preprod));
      request.available    = utxos;
      request.coinSelector = std::make_shared<LargeFirstCoinSelector>();
      (void)balanceTransaction(request);
      break;
    }
    default:
      throw std::invalid_argument("Unknown test operation");
  }
}

/** @brief Tests each public utility failure path under native allocation failure. */
class UtilityAllocation : public testing::TestWithParam<std::size_t>
{
};
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(TransactionUtilities, PricesBytesWithoutChangingInputs)
{
  // Arrange
  const Transaction               transaction = draft();
  const TransactionOutput         output      = funding().output();
  const std::vector<std::uint8_t> original    = transaction.toCbor();

  // Act & Assert
  EXPECT_EQ(serializedTransactionSize(transaction), original.size());
  EXPECT_EQ(serializedOutputSize(output), output.toCbor().size());
  EXPECT_EQ(serializedCoinSize(23), 1U);
  EXPECT_EQ(serializedCoinSize(24), 2U);
  EXPECT_EQ(serializedCoinSize(256), 3U);
  EXPECT_EQ(serializedCoinSize(std::numeric_limits<std::uint64_t>::max()), 9U);
  EXPECT_EQ(computeMinFeeWithoutScripts(transaction, 155381, 44), 155381U + 44U * (original.size() - 1));
  EXPECT_EQ(computeTransactionFee(transaction, {}, parameters()), computeMinFeeWithoutScripts(transaction, 155381, 44));
  EXPECT_EQ(computeMinAdaRequired(output, 4310), (160U + output.toCbor().size()) * 4310U);
  EXPECT_EQ(transaction.toCbor(), original);
}

TEST(TransactionUtilities, SumsBudgetsAndPricesExecution)
{
  // Arrange
  const RedeemerList redeemers {
    Redeemer(RedeemerTag::Spend, 0, PlutusData(BigInt(0)), ExUnits(10, 20)),
    Redeemer(RedeemerTag::Mint, 0, PlutusData(BigInt(1)), ExUnits(30, 40))
  };
  Transaction transaction = draft();
  WitnessSet  witnesses;
  witnesses.setRedeemers(redeemers);
  transaction.setWitnessSet(witnesses);

  // Act & Assert
  EXPECT_EQ(totalExecutionUnits(redeemers), ExUnits(40, 60));
  EXPECT_EQ(totalExecutionUnits(RedeemerList()), ExUnits(0, 0));
  EXPECT_EQ(computeMinScriptFee(transaction, ExUnitPrices(UnitInterval(1, 2), UnitInterval(1, 4)), {}, UnitInterval(0, 1)), 35U);
  EXPECT_EQ(computeTransactionFee(transaction, {}, parameters()), computeMinFeeWithoutScripts(transaction, 155381, 44) + 35U);
}

TEST(TransactionUtilities, PricesReferenceScriptsAndCustomTiers)
{
  // Arrange
  const PlutusV4Script plutus = PlutusV4Script::fromHex("450100002499");
  const Script         script = plutus;
  TransactionOutput    output = funding().output();
  output.setScriptRef(script);
  const Utxo         reference(funding().input(), output);
  ProtocolParameters params = parameters();
  params.setRefScriptCostStride(2);
  params.setRefScriptCostMultiplier(UnitInterval(2, 1));

  // Act & Assert
  const std::size_t size = serializedScriptSize(script);
  EXPECT_EQ(size, plutus.toBytes().size());
  EXPECT_EQ(computeScriptRefFee({ reference }, UnitInterval(2, 1)), size * 2U);
  EXPECT_GT(computeScriptRefFee({ reference }, params), size * 2U);
  EXPECT_EQ(computeScriptRefFee({}, params), 0U);
  EXPECT_EQ(serializedScriptSize(ScriptInvalidBefore(100)), NativeScript(ScriptInvalidBefore(100)).toCbor().size());
}

TEST(TransactionUtilities, ComputesOptionalScriptDataCommitment)
{
  // Arrange
  const PlutusDataSet             datums { PlutusData(BigInt(42)) };
  std::vector<std::uint8_t>       preimage { 0xa0 };
  const std::vector<std::uint8_t> encoded = datums.toCbor();
  preimage.insert(preimage.end(), encoded.begin(), encoded.end());
  preimage.push_back(0xa0);

  // Act
  const std::optional<Blake2bHash> hash = computeScriptDataHash(CostModels(), RedeemerList(), datums);

  // Assert
  EXPECT_FALSE(computeScriptDataHash(CostModels(), RedeemerList(), PlutusDataSet()));
  ASSERT_TRUE(hash);
  EXPECT_EQ(*hash, Blake2bHash::compute(preimage, 32));
}

TEST(TransactionUtilities, RejectsExecutionBudgetOverflow)
{
  // Arrange
  const std::uint64_t maximum = std::numeric_limits<std::uint64_t>::max();
  const RedeemerList  memoryOverflow {
    Redeemer(RedeemerTag::Spend, 0, PlutusData(0), ExUnits(maximum, 0)),
    Redeemer(RedeemerTag::Spend, 1, PlutusData(0), ExUnits(1, 0))
  };
  const RedeemerList stepsOverflow {
    Redeemer(RedeemerTag::Spend, 0, PlutusData(0), ExUnits(0, maximum)),
    Redeemer(RedeemerTag::Spend, 1, PlutusData(0), ExUnits(0, 1))
  };

  // Act & Assert
  EXPECT_THROW(totalExecutionUnits(memoryOverflow), Error);
  EXPECT_THROW(totalExecutionUnits(stepsOverflow), Error);
  EXPECT_EQ(totalExecutionUnits(RedeemerList()), ExUnits(0, 0));
}

TEST(TransactionUtilities, ReportsSurplusDeficitAndCertificateDeposits)
{
  // Arrange
  const Utxo       input = funding();
  TransactionBody  body(TransactionInputSet { input.input() }, TransactionOutputList { TransactionOutput(address(), Value(7'000'000)) }, 1'000'000);
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 3)));
  body.setCertificates(CertificateSet { RegistrationCert(credential, 2'000'000) });
  Transaction transaction(body, WitnessSet());

  // Act & Assert
  EXPECT_EQ(computeImplicitCoin(transaction, parameters()).deposits, 2'000'000U);
  EXPECT_TRUE(isTransactionBalanced(transaction, { input }, parameters()));
  EXPECT_TRUE(computeTransactionImbalance(transaction, { input }, parameters()).isZero());
  body.setFee(2'000'000);
  transaction.setBody(body);
  EXPECT_EQ(computeTransactionImbalance(transaction, { input }, parameters()).coin(), -1'000'000);
  EXPECT_FALSE(isTransactionBalanced(transaction, { input }, parameters()));
  body.setFee(0);
  transaction.setBody(body);
  EXPECT_EQ(computeTransactionBatchImbalance(transaction, { input }, parameters()).coin(), 1'000'000);
}

TEST(TransactionUtilities, PreservesSignedAssetAndLovelaceDeficits)
{
  // Arrange
  const AssetId asset(Blake2bHash(std::vector<std::uint8_t>(28, 4)), AssetName::fromString("Token"));
  Value         outputValue(11'000'000);
  outputValue.addAsset(asset, 2);
  const Utxo        input = funding();
  const Transaction transaction(TransactionBody(TransactionInputSet { input.input() }, TransactionOutputList { TransactionOutput(address(), outputValue) }, 0), WitnessSet());
  Value             expected(-1'000'000);
  expected.addAsset(asset, -2);

  // Act
  const Value imbalance = computeTransactionImbalance(transaction, { input }, parameters());

  // Assert
  EXPECT_EQ(imbalance, expected);
}

TEST(TransactionUtilities, BalancesACopyWithDefaultAndInjectedServices)
{
  // Arrange
  const Transaction         original = draft();
  TransactionBalanceRequest request(original, parameters(), address(), SlotConfig::forNetwork(NetworkMagic::Preprod));
  request.available = { funding() };

  // Act
  const Transaction defaultResult                    = balanceTransaction(request);
  request.coinSelector                               = std::make_shared<LargeFirstCoinSelector>();
  const std::shared_ptr<CountingEvaluator> evaluator = std::make_shared<CountingEvaluator>();
  request.evaluator                                  = evaluator;
  const Transaction injectedResult                   = balanceTransaction(request);

  // Assert
  EXPECT_EQ(request.transaction.toCbor(), original.toCbor());
  EXPECT_TRUE(isTransactionBalanced(defaultResult, request.available, request.parameters));
  EXPECT_TRUE(isTransactionBalanced(injectedResult, request.available, request.parameters));
  EXPECT_GT(injectedResult.body().fee(), 0U);
  EXPECT_EQ(injectedResult.body().inputs().size(), 1U);
}

TEST(TransactionUtilities, PreservesMandatoryInputsAndRejectsMissingResolution)
{
  // Arrange
  Transaction     original = draft();
  TransactionBody body     = original.body();
  body.setInputs(TransactionInputSet { funding().input() });
  original.setBody(body);
  TransactionBalanceRequest request(original, parameters(), address(), SlotConfig::forNetwork(NetworkMagic::Preprod));

  // Act & Assert
  EXPECT_THROW(balanceTransaction(request), std::invalid_argument);
  request.available          = { funding(), funding(3) };
  const Transaction balanced = balanceTransaction(request);
  EXPECT_TRUE(balanced.body().inputs().contains(funding().input()));
  EXPECT_EQ(request.transaction, original);
}

TEST(TransactionUtilities, RejectsInvalidSpendingRedeemerIndexWithoutChangingDraft)
{
  // Arrange
  Transaction transaction = draft();
  WitnessSet  witnesses;
  witnesses.setRedeemers(RedeemerList { Redeemer(RedeemerTag::Spend, 99, PlutusData(BigInt(0)), ExUnits(1, 1)) });
  transaction.setWitnessSet(witnesses);
  TransactionBalanceRequest request(transaction, parameters(), address(), SlotConfig::forNetwork(NetworkMagic::Preprod));
  request.available = { funding() };

  // Act & Assert
  EXPECT_THROW(balanceTransaction(request), std::invalid_argument);
  EXPECT_EQ(request.transaction, transaction);
}

TEST(TransactionUtilities, RejectsSignedDraftsAndPreservesInputsOnServiceFailure)
{
  // Arrange
  const Transaction         original = draft();
  TransactionBalanceRequest request(original, parameters(), address(), SlotConfig::forNetwork(NetworkMagic::Preprod));
  request.available                                = { funding() };
  const std::shared_ptr<CountingSelector> selector = std::make_shared<CountingSelector>();
  selector->fail                                   = true;
  request.coinSelector                             = selector;

  // Act & Assert
  EXPECT_THROW(balanceTransaction(request), Error);
  EXPECT_GT(selector->calls, 0U);
  EXPECT_EQ(request.transaction, original);

  WitnessSet witnesses;
  witnesses.setVkeys(VkeyWitnessSet { VkeyWitness(Ed25519PublicKey(std::vector<std::uint8_t>(32, 1)), Ed25519Signature(std::vector<std::uint8_t>(64, 2))) });
  request.transaction.setWitnessSet(witnesses);
  const Transaction signedDraft = request.transaction;

  EXPECT_THROW(balanceTransaction(request), std::invalid_argument);
  EXPECT_EQ(request.transaction, signedDraft);
}

TEST(TransactionUtilities, ReindexesSpendingRedeemersAndUsesInjectedEvaluator)
{
  // Arrange
  const PlutusV4Script script        = PlutusV4Script::fromHex("450100002499");
  const Address        scriptAddress = EnterpriseAddress(NetworkId::Testnet, Credential(CredentialType::ScriptHash, script.hash()));
  const Utxo           scriptInput(TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 5)), 0), TransactionOutput(scriptAddress, Value(2'000'000)));
  Transaction          transaction = draft();
  TransactionBody      body        = transaction.body();
  body.setInputs(TransactionInputSet { scriptInput.input() });
  transaction.setBody(body);

  WitnessSet witnesses;
  witnesses.setPlutusV4Scripts(PlutusV4ScriptSet { script });
  witnesses.setRedeemers(RedeemerList { Redeemer(RedeemerTag::Spend, 0, PlutusData(BigInt(7)), ExUnits(10, 20)) });
  transaction.setWitnessSet(witnesses);

  ProtocolParameters params = parameters();
  params.setCollateralPercentage(150);
  params.setMaxCollateralInputs(3);
  CostModels models;
  models.insert(CostModel(PlutusLanguageVersion::V4, std::vector<std::int64_t>(179, 0)));
  params.setCostModels(models);

  TransactionBalanceRequest request(transaction, params, address(), SlotConfig::forNetwork(NetworkMagic::Preprod));
  request.preselected                                = { scriptInput };
  request.available                                  = { funding() };
  request.collateral                                 = { funding(9) };
  request.coinSelector                               = std::make_shared<LargeFirstCoinSelector>();
  const std::shared_ptr<CountingEvaluator> evaluator = std::make_shared<CountingEvaluator>();
  request.evaluator                                  = evaluator;

  // Act
  const Transaction balanced = balanceTransaction(request);

  // Assert
  EXPECT_GT(evaluator->calls, 0U);
  ASSERT_EQ(balanced.body().inputs().size(), 2U);
  EXPECT_EQ(balanced.body().inputs().at(1), scriptInput.input());
  ASSERT_TRUE(balanced.witnessSet().redeemers());
  const Redeemer redeemer = balanced.witnessSet().redeemers()->at(0);
  EXPECT_EQ(redeemer.index(), 1U);
  EXPECT_EQ(redeemer.data(), PlutusData(BigInt(7)));
  EXPECT_TRUE(isTransactionBalanced(balanced, { scriptInput, funding() }, params));
  EXPECT_EQ(request.transaction, transaction);
}

TEST(TransactionUtilities, IncludesSubTransactionValueInBatchImbalance)
{
  // Arrange
  const Utxo           input = funding();
  const SubTransaction intent(SubTransactionBody(TransactionInputSet { input.input() }, TransactionOutputList { TransactionOutput(address(), Value(9'000'000)) }), WitnessSet());
  TransactionBody      body(TransactionInputSet(), TransactionOutputList(), 1'000'000);
  body.setSubTransactions(SubTransactionSet { intent });
  const Transaction transaction(body, WitnessSet());

  // Act & Assert
  EXPECT_EQ(computeTransactionImbalance(transaction, { input }, parameters()).coin(), -1'000'000);
  EXPECT_EQ(computeSubTransactionImbalance(intent, { input }, parameters()).coin(), 1'000'000);
  EXPECT_TRUE(computeTransactionBatchImbalance(transaction, { input }, parameters()).isZero());
}

TEST_P(UtilityAllocation, PreservesInputsAndReleasesPartialResults)
{
  // Arrange
  const Transaction               transaction = draft();
  const ProtocolParameters        params      = parameters();
  const UtxoList                  utxos { funding() };
  const std::vector<std::uint8_t> original    = transaction.toCbor();
  std::size_t                     allocations = 0;

  {
    AllocationFailure track(std::numeric_limits<std::size_t>::max());
    exercise(GetParam(), transaction, params, utxos);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);
  std::size_t failures = 0;

  // Act
  for (std::size_t index = 0; index < allocations; ++index)
  {
    {
      AllocationFailure fail(index);
      try
      {
        exercise(GetParam(), transaction, params, utxos);
      }
      catch (const std::exception&)
      {
        ++failures;
      }
    }

    // Assert
    EXPECT_EQ(transaction.toCbor(), original);
  }

  EXPECT_GT(failures, 0U);
}

INSTANTIATE_TEST_SUITE_P(Operations, UtilityAllocation, testing::Values(0U, 1U, 2U, 3U, 4U, 5U, 6U));
