/**
 * @file PoolOwnersTest.cpp
 *
 * @brief Unit tests for pool owner key hash sets.
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

/* INCLUDES ******************************************************************/

#include <Cometa/Cometa.h>

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(PoolOwners, InsertsUniqueSortedOwnerHashes)
{
  // Arrange
  const Blake2bHash first  = Blake2bHash::fromHex(std::string(56, '0'));
  const Blake2bHash second = Blake2bHash::fromHex(std::string(56, '1'));
  PoolOwners        owners;
  CborReader        expected("d9010282581c" + std::string(56, '0') + "581c" + std::string(56, '1'));

  // Act
  const bool insertedSecond = owners.insert(second);
  const bool insertedFirst  = owners.insert(first);
  const bool duplicate      = owners.insert(first);

  // Assert
  EXPECT_TRUE(insertedSecond);
  EXPECT_TRUE(insertedFirst);
  EXPECT_FALSE(duplicate);
  EXPECT_EQ(owners.size(), 2U);
  EXPECT_FALSE(owners.empty());
  EXPECT_TRUE(owners.contains(first));
  EXPECT_EQ(owners.at(0), first);
  EXPECT_EQ(owners.at(1), second);
  EXPECT_EQ(owners.values(), (std::vector<Blake2bHash> { first, second }));
  EXPECT_EQ(owners.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(PoolOwners::fromCbor(owners.toCbor()), owners);
}

TEST(PoolOwners, DecodesTaggedAndUntaggedSets)
{
  // Arrange
  const std::string entry    = "581c" + std::string(56, '1');
  const PoolOwners  tagged   = PoolOwners::fromCbor("d9010281" + entry);
  const PoolOwners  untagged = PoolOwners::fromCbor("81" + entry);

  // Act & Assert
  EXPECT_EQ(tagged, untagged);
  EXPECT_EQ(tagged.toCbor(), untagged.toCbor());
  EXPECT_EQ(tagged.toCbor(), CborReader("d9010281" + entry).readEncodedValue());
  EXPECT_EQ(PoolOwners::fromCbor("d9010280"), PoolOwners());
  EXPECT_THROW(untagged.at(1), std::out_of_range);
}

TEST(PoolOwners, CopiesInputsAndReturnedHashes)
{
  // Arrange
  Blake2bHash      hash = Blake2bHash::fromHex(std::string(56, '1'));
  PoolOwners       owners { hash, hash };
  const PoolOwners original(owners);
  Blake2bHash      returned = owners.at(0);

  // Act
  hash     = Blake2bHash::fromHex(std::string(56, '2'));
  returned = hash;

  // Assert
  EXPECT_EQ(owners, original);
  EXPECT_EQ(owners.size(), 1U);
  EXPECT_EQ(owners.at(0).toHex(), std::string(56, '1'));
  EXPECT_EQ(returned.toHex(), std::string(56, '2'));
}

TEST(PoolOwners, RejectsInvalidHashesAndCbor)
{
  // Arrange
  PoolOwners        owners;
  const Blake2bHash shortHash = Blake2bHash::fromHex("11");

  // Act & Assert
  EXPECT_THROW(owners.insert(shortHash), std::invalid_argument);
  EXPECT_THROW(owners.contains(shortHash), std::invalid_argument);
  EXPECT_THROW((void)PoolOwners { shortHash }, std::invalid_argument);
  EXPECT_THROW(PoolOwners::fromCbor("d9010282581c" + std::string(56, '1') + "581c" + std::string(56, '1')), std::invalid_argument);
  EXPECT_THROW(PoolOwners::fromCbor("d90102814111"), std::invalid_argument);
  EXPECT_THROW(PoolOwners::fromCbor("d9010380"), Error);
  EXPECT_THROW(PoolOwners::fromCbor("d9010280ff"), std::invalid_argument);
  EXPECT_THROW(PoolOwners::fromCbor("z"), std::invalid_argument);
  EXPECT_THROW(PoolOwners::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_TRUE(owners.empty());
}
