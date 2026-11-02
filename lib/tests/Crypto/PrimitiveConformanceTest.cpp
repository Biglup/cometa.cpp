/**
 * @file PrimitiveConformanceTest.cpp
 *
 * @brief Checks cryptographic operations against the shared native corpus.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
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

#include <gtest/gtest.h>

#include <fstream>
#include <iterator>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/**
 * @brief Loads the pinned native primitive corpus without changing its expected results.
 * @return The primitive cases.
 */
JsonValue
primitiveCases()
{
  std::ifstream input(std::string(COMETA_TEST_VECTOR_DIR) + "/primitives.json", std::ios::binary);

  if (!input)
  {
    throw std::runtime_error("Cannot open the native primitive conformance corpus");
  }

  const std::string text((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

  return JsonValue::parse(text).at("cases");
}
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(PrimitiveConformanceTest, MatchesEd25519SigningFixtures)
{
  // Arrange
  const JsonValue cases   = primitiveCases();
  std::size_t     checked = 0;

  // Act & Assert
  for (std::size_t index = 0; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);

    if (item.at("operation").asString() != "ed25519")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    const JsonValue                 input     = item.at("input");
    const JsonValue                 expected  = item.at("expected");
    const Ed25519PrivateKey         key       = Ed25519PrivateKey::fromNormalHex(input.at("seed").asString());
    const std::vector<std::uint8_t> message   = decodeHex(input.at("message").asString());
    const Ed25519Signature          signature = key.sign(message);

    EXPECT_EQ(key.publicKey().toHex(), expected.at("public_key").asString());
    EXPECT_EQ(signature.toHex(), expected.at("signature").asString());
    EXPECT_TRUE(key.publicKey().verify(signature, message));
    ++checked;
  }

  EXPECT_EQ(checked, 9U);
}

TEST(PrimitiveConformanceTest, MatchesEd25519VerificationFixtures)
{
  // Arrange
  const JsonValue cases   = primitiveCases();
  std::size_t     checked = 0;

  // Act & Assert
  for (std::size_t index = 0; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);

    if (item.at("operation").asString() != "verify_ed25519")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    const JsonValue                 input     = item.at("input");
    const Ed25519PublicKey          key       = Ed25519PublicKey::fromHex(input.at("public_key").asString());
    const Ed25519Signature          signature = Ed25519Signature::fromHex(input.at("signature").asString());
    const std::vector<std::uint8_t> message   = decodeHex(input.at("message").asString());

    EXPECT_EQ(key.verify(signature, message), item.at("expected").at("valid").asBool());
    ++checked;
  }

  EXPECT_EQ(checked, 4U);
}

TEST(PrimitiveConformanceTest, MatchesBip32DerivationFixtures)
{
  // Arrange
  const JsonValue cases   = primitiveCases();
  std::size_t     checked = 0;

  // Act & Assert
  for (std::size_t index = 0; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);

    if (item.at("operation").asString() != "bip32")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    const JsonValue            input    = item.at("input");
    const JsonValue            expected = item.at("expected");
    const JsonValue            indices  = input.at("path");
    std::vector<std::uint32_t> path;

    for (std::size_t child = 0; child < indices.size(); ++child)
    {
      path.push_back(static_cast<std::uint32_t>(indices.at(child).asUInt()));
    }

    const std::vector<std::uint8_t> passwordBytes = decodeHex(input.at("password").asString());
    const std::string               password(passwordBytes.begin(), passwordBytes.end());
    const Bip32PrivateKey           key = Bip32PrivateKey::fromBip39Entropy(decodeHex(input.at("entropy").asString()), password).derive(path);

    EXPECT_EQ(key.toHex(), expected.at("private_key").asString());
    EXPECT_EQ(key.publicKey().toHex(), expected.at("public_key").asString());
    EXPECT_EQ(key.toEd25519().publicKey().toHex(), expected.at("ed25519_public_key").asString());
    ++checked;
  }

  EXPECT_EQ(checked, 3U);
}

TEST(PrimitiveConformanceTest, MatchesBlake2bFixturesIncludingInvalidInput)
{
  // Arrange
  const JsonValue cases   = primitiveCases();
  std::size_t     checked = 0;

  // Act & Assert
  for (std::size_t index = 0; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);

    if (item.at("operation").asString() != "blake2b")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    const JsonValue                 input    = item.at("input");
    const JsonValue                 expected = item.at("expected");
    const std::vector<std::uint8_t> message  = decodeHex(input.at("message").asString());
    const std::size_t               size     = static_cast<std::size_t>(input.at("size").asUInt());

    if (expected.contains("error"))
    {
      EXPECT_THROW(Blake2bHash::compute(message, size), std::invalid_argument);
    }
    else
    {
      EXPECT_EQ(Blake2bHash::compute(message, size).toHex(), expected.at("hash").asString());
    }

    ++checked;
  }

  EXPECT_EQ(checked, 9U);
}

TEST(PrimitiveConformanceTest, MatchesAllAddressFixtures)
{
  // Arrange
  const JsonValue cases   = primitiveCases();
  std::size_t     checked = 0;

  // Act & Assert
  for (std::size_t index = 0; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);

    if (item.at("operation").asString() != "address")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    const std::vector<std::uint8_t> bytes = decodeHex(item.at("input").at("bytes").asString());
    const Address                   address(bytes);
    const std::string               expected = item.at("expected").at("address").asString();

    EXPECT_EQ(address.toString(), expected);
    EXPECT_EQ(Address::fromString(expected).toBytes(), bytes);
    ++checked;
  }

  EXPECT_EQ(checked, 16U);
}
