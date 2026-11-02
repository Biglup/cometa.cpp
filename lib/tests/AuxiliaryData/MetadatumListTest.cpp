/**
 * @file MetadatumListTest.cpp
 *
 * @brief Unit tests for MetadatumList.
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

#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <limits>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(MetadatumList, RetainsOrderDuplicatesAndIndependentValues)
{
  // Arrange
  Metadatum     original("first");
  MetadatumList values { original, original };

  // Act
  original = Metadatum("changed");
  values.pushBack(original);
  Metadatum returned = values.at(0);
  returned           = original;

  // Assert
  EXPECT_EQ(values.size(), 3U);
  EXPECT_FALSE(values.empty());
  EXPECT_EQ(values.at(0).asText(), "first");
  EXPECT_EQ(values.at(1).asText(), "first");
  EXPECT_EQ(values.at(2), original);
  EXPECT_EQ(returned, original);
  EXPECT_EQ(values.values().size(), 3U);
  EXPECT_THROW(values.at(3), std::out_of_range);
  EXPECT_EQ(MetadatumList::fromCbor(values.toCbor()), values);
}

TEST(MetadatumList, PreservesCollectionLengthForm)
{
  // Arrange
  const MetadatumList definite   = MetadatumList::fromCbor("8101");
  MetadatumList       indefinite = MetadatumList::fromCbor("9f01ff");
  CborReader          expected("9f0102ff");

  // Act
  indefinite.pushBack(Metadatum(std::int64_t { 2 }));

  // Assert
  EXPECT_EQ(indefinite.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(definite.at(0).asInteger(), BigInt(1));
  EXPECT_TRUE(MetadatumList().empty());
  EXPECT_EQ(MetadatumList().toJson(), R"({"tag":"list","contents":[]})");
  EXPECT_THROW(MetadatumList::fromCbor("80ff"), std::invalid_argument);
  EXPECT_THROW(MetadatumList::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(MetadatumList::fromCbor("00"), Error);
}
