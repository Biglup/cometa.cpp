/**
 * @file CborConformanceTest.cpp
 *
 * @brief Checks C++ CBOR values against the shared native corpus.
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

#include <gtest/gtest.h>

#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Loads a pinned native CBOR document. @param name The file name. @return The document. */
JsonValue
loadDocument(const char* name)
{
  std::ifstream input(std::string(COMETA_TEST_VECTOR_DIR) + "/" + name, std::ios::binary);

  if (!input)
  {
    throw std::runtime_error("Cannot open the native CBOR conformance corpus");
  }

  const std::string document((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  return JsonValue::parse(document);
}

/** @brief Loads the cases from a native corpus file. @param name The file name. @return The case array. */
JsonValue
loadCases(const char* name)
{
  return loadDocument(name).at("cases");
}

/** @brief Encodes bytes as lowercase hexadecimal. @param bytes The bytes. @return The encoded text. */
std::string
toHex(const std::vector<std::uint8_t>& bytes)
{
  constexpr char HEX[] = "0123456789abcdef";
  std::string    result;
  result.reserve(bytes.size() * 2);

  for (const std::uint8_t value: bytes)
  {
    result.push_back(HEX[value >> 4]);
    result.push_back(HEX[value & 15]);
  }

  return result;
}

/** @brief Checks CBOR and inspection through a public C++ value. @param item The corpus case. */
template<class T>
void
checkValue(const JsonValue& item)
{
  const T         value    = T::fromCbor(item.at("input").at("cbor").asString());
  const JsonValue expected = item.at("expected");

  EXPECT_EQ(toHex(value.toCbor()), expected.at("cbor").asString());
  EXPECT_EQ(JsonValue::parse(value.toJson()).toJson(), expected.at("inspection").toJson());
}

/** @brief Checks the body ID and signature for a transaction case. @param item The corpus case. */
template<class T>
void
checkSignedTransaction(const JsonValue& item)
{
  const T                 value    = T::fromCbor(item.at("input").at("cbor").asString());
  const Ed25519PrivateKey key      = Ed25519PrivateKey::fromNormalHex(item.at("input").at("signing_seed").asString());
  const JsonValue         expected = item.at("expected");

  EXPECT_EQ(toHex(value.body().toCbor()), expected.at("body_cbor").asString());
  EXPECT_EQ(value.id().toHex(), expected.at("hash").asString());
  EXPECT_EQ(key.publicKey().toHex(), expected.at("verification_key").asString());
  EXPECT_EQ(key.sign(value.id().toBytes()).toHex(), expected.at("signature").asString());
}

/** @brief Checks a script hash against the native corpus. @param item The corpus case. */
template<class T>
void
checkScriptHash(const JsonValue& item)
{
  const T value = T::fromCbor(item.at("input").at("cbor").asString());
  EXPECT_EQ(value.hash().toHex(), item.at("expected").at("hash").asString());
}

/** @brief Checks that malformed CBOR is rejected through a public value. @param item The corpus case. */
template<class T>
void
checkInvalid(const JsonValue& item)
{
  EXPECT_ANY_THROW(T::fromCbor(item.at("input").at("cbor").asString()));
}

/** @brief Checks a value in the valid or invalid corpus. @param item The case. @param invalid Whether decoding must fail. */
template<class T>
void
checkCase(const JsonValue& item, bool invalid)
{
  if (invalid)
  {
    checkInvalid<T>(item);
  }
  else
  {
    checkValue<T>(item);
  }
}
} // namespace

/* IMPLEMENTATION ************************************************************/

/** @brief Checks one native CBOR corpus file. @param name The file name. @param invalid Whether decoding must fail. @param expectedCount The expected case count. */
void
checkCorpus(const char* name, bool invalid, std::size_t expectedCount)
{
  // Arrange
  const JsonValue cases   = loadCases(name);
  std::size_t     checked = 0;

  // Act & Assert
  for (std::size_t index = 0; index < cases.size(); ++index)
  {
    const JsonValue   item = cases.at(index);
    const std::string type = item.at("type").asString();
    SCOPED_TRACE(item.at("id").asString());

    if (type == "transaction")
    {
      checkCase<Transaction>(item, invalid);
      if (!invalid)
      {
        checkSignedTransaction<Transaction>(item);
      }
    }
    else if (type == "sub_transaction")
    {
      checkCase<SubTransaction>(item, invalid);
      if (!invalid)
      {
        checkSignedTransaction<SubTransaction>(item);
      }
    }
    else if (type == "transaction_body")
    {
      checkCase<TransactionBody>(item, invalid);
    }
    else if (type == "transaction_input")
    {
      checkCase<TransactionInput>(item, invalid);
    }
    else if (type == "transaction_output")
    {
      checkCase<TransactionOutput>(item, invalid);
    }
    else if (type == "value")
    {
      checkCase<Value>(item, invalid);
    }
    else if (type == "credential")
    {
      checkCase<Credential>(item, invalid);
    }
    else if (type == "account_balance_interval")
    {
      checkCase<AccountBalanceInterval>(item, invalid);
    }
    else if (type == "certificate")
    {
      checkCase<Certificate>(item, invalid);
    }
    else if (type == "proposal_procedure")
    {
      checkCase<ProposalProcedure>(item, invalid);
    }
    else if (type == "voting_procedures")
    {
      checkCase<VotingProcedures>(item, invalid);
    }
    else if (type == "native_script")
    {
      checkCase<NativeScript>(item, invalid);
      if (!invalid)
      {
        checkScriptHash<NativeScript>(item);
      }
    }
    else if (type == "script")
    {
      checkCase<Script>(item, invalid);
      if (!invalid)
      {
        checkScriptHash<Script>(item);
      }
    }
    else if (type == "plutus_data")
    {
      checkCase<PlutusData>(item, invalid);
    }
    else
    {
      FAIL() << "Unsupported corpus type: " << type;
    }

    ++checked;
  }

  EXPECT_EQ(checked, expectedCount);
}

TEST(CborConformanceTest, MatchesSerializationCorpus)
{
  checkCorpus("serialization.json", false, 114);
}

TEST(CborConformanceTest, RejectsInvalidCorpus)
{
  checkCorpus("invalid.json", true, 45);
}

TEST(CborConformanceTest, MatchesTransactionFixtures)
{
  // Arrange
  const char* files[] = {
    "conway-key-guard.json",
    "dijkstra-account-balances.json",
    "dijkstra-signed-subtransaction.json"
  };

  // Act & Assert
  for (const char* file: files)
  {
    const JsonValue fixture = loadDocument(file);
    SCOPED_TRACE(file);
    const Transaction       transaction = Transaction::fromCbor(fixture.at("transaction_cbor").asString());
    const Ed25519PrivateKey signer      = Ed25519PrivateKey::fromNormalHex(fixture.at("signing_seed").asString());

    EXPECT_EQ(toHex(transaction.toCbor()), fixture.at("transaction_cbor").asString());
    EXPECT_EQ(toHex(transaction.body().toCbor()), fixture.at("body_cbor").asString());
    EXPECT_EQ(transaction.id().toHex(), fixture.at("transaction_id").asString());
    EXPECT_EQ(JsonValue::parse(transaction.toJson()).toJson(), fixture.at("inspection").toJson());
    EXPECT_EQ(signer.publicKey().toHex(), fixture.at("verification_key").asString());
    EXPECT_EQ(signer.sign(transaction.id().toBytes()).toHex(), fixture.at("signature").asString());
  }
}
