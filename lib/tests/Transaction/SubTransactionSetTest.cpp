/**
 * @file SubTransactionSetTest.cpp
 *
 * @brief Unit tests for ordered Dijkstra sub-transactions.
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

/* USINGS ********************************************************************/

using namespace Cometa;

/* CONSTANTS *****************************************************************/

static constexpr const char* SUB_WITH_AUX_DATA_CBOR    = "83a300d90102800180031864a0a1016474657374";
static constexpr const char* SUB_WITHOUT_AUX_DATA_CBOR = "83a200d90102800180a0f6";
static constexpr const char* SUB_DUPLICATE_ID_CBOR     = "83a200d90102800180a0a1016474657374";
static constexpr const char* SET_TAGGED_CBOR           = "d901028283a300d90102800180031864a0a101647465737483a200d90102800180a0f6";
static constexpr const char* SET_BARE_CBOR             = "8283a300d90102800180031864a0a101647465737483a200d90102800180a0f6";
static constexpr const char* SET_REVERSED_CBOR         = "d901028283a200d90102800180a0f683a300d90102800180031864a0a1016474657374";

/* IMPLEMENTATION ************************************************************/

TEST(SubTransactionSet, PreservesOrderAndFindsById)
{
  // Arrange
  const SubTransactionSet set    = SubTransactionSet::fromCbor(SET_TAGGED_CBOR);
  const SubTransaction    first  = SubTransaction::fromCbor(SUB_WITH_AUX_DATA_CBOR);
  const SubTransaction    second = SubTransaction::fromCbor(SUB_WITHOUT_AUX_DATA_CBOR);
  CborReader              reader(SET_TAGGED_CBOR);

  // Act
  const std::optional<SubTransaction> found = set.findById(second.id());

  // Assert
  ASSERT_EQ(set.size(), 2U);
  EXPECT_FALSE(set.empty());
  EXPECT_TRUE(set.useTag());
  EXPECT_EQ(set.at(0), first);
  EXPECT_EQ(set.at(1), second);
  ASSERT_TRUE(found.has_value());
  EXPECT_EQ(*found, second);
  EXPECT_EQ(set.toCbor(), reader.readEncodedValue());
  EXPECT_FALSE(set.toJson().empty());
}

TEST(SubTransactionSet, AddsAndReordersEntries)
{
  // Arrange
  SubTransactionSet    set;
  const SubTransaction first  = SubTransaction::fromCbor(SUB_WITH_AUX_DATA_CBOR);
  const SubTransaction second = SubTransaction::fromCbor(SUB_WITHOUT_AUX_DATA_CBOR);
  CborReader           reader(SET_REVERSED_CBOR);

  // Act
  set.add(second);
  set.add(first);

  // Assert
  EXPECT_EQ(set.at(0), second);
  EXPECT_EQ(set.at(1), first);
  EXPECT_EQ(set.toCbor(), reader.readEncodedValue());
}

TEST(SubTransactionSet, RejectsDuplicateBodyHash)
{
  // Arrange
  SubTransactionSet    set;
  const SubTransaction first     = SubTransaction::fromCbor(SUB_WITHOUT_AUX_DATA_CBOR);
  const SubTransaction duplicate = SubTransaction::fromCbor(SUB_DUPLICATE_ID_CBOR);
  set.add(first);

  // Act and assert
  EXPECT_THROW(set.add(duplicate), Error);
  EXPECT_EQ(set.size(), 1U);
}

TEST(SubTransactionSet, ControlsTagAndCopiesIndependently)
{
  // Arrange
  SubTransactionSet set = SubTransactionSet::fromCbor(SET_TAGGED_CBOR);
  SubTransactionSet copy(set);
  CborReader        reader(SET_BARE_CBOR);

  // Act
  copy.setUseTag(false);

  // Assert
  EXPECT_FALSE(copy.useTag());
  EXPECT_TRUE(set.useTag());
  EXPECT_EQ(copy.toCbor(), reader.readEncodedValue());
  EXPECT_NE(copy, set);
  EXPECT_THROW(set.at(2), std::out_of_range);
}
