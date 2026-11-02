/**
 * @file CredentialSetTest.cpp
 *
 * @brief Unit tests for CredentialSet.
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

#include <Cometa/Cbor/CborReader.h>
#include <Cometa/Common/CredentialSet.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <stdexcept>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(CredentialSet, InsertsUniqueValuesInSortedOrder)
{
  // Arrange
  const Credential first  = Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential second = Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '2')));
  CredentialSet    values;

  // Act
  const bool insertedSecond = values.insert(second);
  const bool insertedFirst  = values.insert(first);
  const bool duplicate      = values.insert(first);

  // Assert
  EXPECT_TRUE(insertedFirst);
  EXPECT_TRUE(insertedSecond);
  EXPECT_FALSE(duplicate);
  EXPECT_FALSE(values.empty());
  EXPECT_EQ(values.size(), 2U);
  EXPECT_TRUE(values.contains(first));
  EXPECT_EQ(values.at(0), first);
  EXPECT_EQ(values.at(1), second);
  EXPECT_EQ(values.values(), (std::vector<Credential> { first, second }));
}

TEST(CredentialSet, InitializerIgnoresDuplicates)
{
  // Arrange
  const Credential first = Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));

  // Act
  const CredentialSet values { first, first };

  // Assert
  EXPECT_EQ(values.size(), 1U);
  EXPECT_FALSE(values.contains(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '2')))));
}

TEST(CredentialSet, EmptySetHasTaggedEncoding)
{
  // Arrange
  const CredentialSet values;

  // Act & Assert
  EXPECT_TRUE(values.empty());
  EXPECT_EQ(values.toCbor(), (std::vector<std::uint8_t> { 0xd9, 0x01, 0x02, 0x80 }));
  EXPECT_EQ(values.toJson(), "[]");
  EXPECT_THROW(values.at(0), std::out_of_range);
  EXPECT_EQ(CredentialSet::fromCbor("80"), values);
}

TEST(CredentialSet, NormalizesUntaggedUnsortedAndIndefiniteInput)
{
  // Arrange
  const std::string untagged   = "828200581c222222222222222222222222222222222222222222222222222222228200581c11111111111111111111111111111111111111111111111111111111";
  const std::string tagged     = "d90102828200581c111111111111111111111111111111111111111111111111111111118200581c22222222222222222222222222222222222222222222222222222222";
  const std::string indefinite = "9f8200581c222222222222222222222222222222222222222222222222222222228200581c11111111111111111111111111111111111111111111111111111111ff";

  // Act
  const CredentialSet values   = CredentialSet::fromCbor(untagged);
  const CredentialSet expected = CredentialSet::fromCbor(tagged);

  // Assert
  EXPECT_EQ(values, expected);
  CborReader expectedEncoding(tagged);
  EXPECT_EQ(values.toCbor(), expectedEncoding.readEncodedValue());
  EXPECT_EQ(CredentialSet::fromCbor(indefinite), expected);
  EXPECT_EQ(CredentialSet::fromCbor(values.toCbor()), expected);
  EXPECT_EQ(values.at(0), Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1'))));
}

TEST(CredentialSet, RejectsDuplicatesWrongTagsAndMalformedInput)
{
  // Act & Assert
  EXPECT_THROW(CredentialSet::fromCbor("828200581c111111111111111111111111111111111111111111111111111111118200581c11111111111111111111111111111111111111111111111111111111"), std::invalid_argument);
  EXPECT_THROW(CredentialSet::fromCbor("d9010380"), Error);
  EXPECT_THROW(CredentialSet::fromCbor("db000000010000010280"), std::out_of_range);
  EXPECT_THROW(CredentialSet::fromCbor("8180"), Error);
  EXPECT_THROW(CredentialSet::fromCbor("81"), Error);
  EXPECT_THROW(CredentialSet::fromCbor("a0"), Error);
  EXPECT_THROW(CredentialSet::fromCbor("8000"), std::invalid_argument);
  EXPECT_THROW(CredentialSet::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(CredentialSet::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(CredentialSet::fromCbor("z0"), std::invalid_argument);
  EXPECT_THROW(CredentialSet::fromCbor("0"), std::invalid_argument);
}

TEST(CredentialSet, CopiesMovesAndSwapsIndependentSets)
{
  // Arrange
  const Credential    first = Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const CredentialSet source { first };
  CredentialSet       copy(source);

  // Act
  copy.insert(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '2'))));
  CredentialSet moved(std::move(copy));
  copy = source;
  swap(copy, moved);
  CredentialSet& alias = copy;
  copy                 = alias;
  copy                 = std::move(alias);

  // Assert
  EXPECT_EQ(source.size(), 1U);
  EXPECT_EQ(copy.size(), 2U);
  EXPECT_EQ(moved, source);
  EXPECT_NE(copy, source);
  EXPECT_THROW(copy.at(2), std::out_of_range);
}

TEST(CredentialSet, ReturnsIndependentSnapshots)
{
  // Arrange
  const Credential        first = Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const CredentialSet     source { first };
  std::vector<Credential> snapshot = source.values();

  // Act
  snapshot.front() = Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '2')));

  // Assert
  EXPECT_EQ(source.at(0), first);
  EXPECT_NE(source.at(0), snapshot.front());
}

TEST(CredentialSet, MutableElementsDoNotChangeSetMembership)
{
  // Arrange
  Credential          credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential    original(credential);
  const CredentialSet set { credential };

  // Act
  credential.setType(CredentialType::ScriptHash);
  Credential retrieved = set.at(0);
  retrieved.setHash(Blake2bHash::fromHex(std::string(56, '2')));

  // Assert
  EXPECT_TRUE(set.contains(original));
  EXPECT_FALSE(set.contains(credential));
  EXPECT_FALSE(set.contains(retrieved));
  EXPECT_EQ(set.at(0), original);
}

TEST(CredentialSet, KeepsDifferentCredentialKindsDistinct)
{
  // Arrange
  const Blake2bHash hash = Blake2bHash::fromHex(std::string(56, '1'));
  const Credential  key(CredentialType::KeyHash, hash);
  const Credential  script(CredentialType::ScriptHash, hash);

  // Act
  const CredentialSet set { script, key };

  // Assert
  EXPECT_EQ(set.size(), 2U);
  EXPECT_TRUE(set.contains(key));
  EXPECT_TRUE(set.contains(script));
  EXPECT_EQ(CredentialSet::fromCbor(set.toCbor()), set);
}

TEST(CredentialSet, SerializesKnownJson)
{
  // Arrange
  const std::string   hash(56, '1');
  const CredentialSet set { Credential(CredentialType::KeyHash, Blake2bHash::fromHex(hash)) };

  // Act
  const std::string json = set.toJson();

  // Assert
  EXPECT_EQ(json, "[{\"tag\":\"pubkey_hash\",\"value\":\"" + hash + "\"}]");
}
