/**
 * @file ConstrPlutusDataTest.cpp
 *
 * @brief Unit tests for ConstrPlutusData.
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

#include <fstream>
#include <iterator>
#include <limits>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(ConstrPlutusData, DecodesBytesAndHexWithoutTrailingData)
{
  // Arrange
  const ConstrPlutusData          expected = ConstrPlutusData(0, PlutusList());
  CborReader                      reader("d87980");
  const std::vector<std::uint8_t> bytes = reader.readEncodedValue();

  // Act
  const ConstrPlutusData fromHex   = ConstrPlutusData::fromCbor("d87980");
  const ConstrPlutusData fromBytes = ConstrPlutusData::fromCbor(bytes);

  // Assert
  EXPECT_EQ(fromHex, expected);
  EXPECT_EQ(fromBytes, expected);
  EXPECT_EQ(fromHex.toCbor(), bytes);
  EXPECT_EQ(fromBytes.toJson(), expected.toJson());
}

TEST(ConstrPlutusData, RejectsInvalidInput)
{
  // Act & Assert
  EXPECT_THROW(ConstrPlutusData::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(ConstrPlutusData::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(ConstrPlutusData::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(ConstrPlutusData::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(ConstrPlutusData::fromCbor("d8798000"), std::invalid_argument);
  EXPECT_THROW(ConstrPlutusData::fromCbor("f5"), Error);
}

TEST(ConstrPlutusData, SupportsCopyMoveAssignmentAndSwap)
{
  // Arrange
  const ConstrPlutusData original = ConstrPlutusData(0, PlutusList());
  ConstrPlutusData       copy(original);

  // Act
  ConstrPlutusData moved(std::move(copy));
  copy = original;
  swap(copy, moved);
  ConstrPlutusData& alias = copy;
  copy                    = alias;
  copy                    = std::move(alias);

  // Assert
  EXPECT_EQ(copy, original);
  EXPECT_EQ(moved, original);
  EXPECT_FALSE(copy != original);
}

TEST(ConstrPlutusData, MutatesAlternativeAndFieldsWithoutChangingCopies)
{
  // Arrange
  ConstrPlutusData       constructor = ConstrPlutusData::fromCbor("d879811801");
  const ConstrPlutusData original(constructor);
  const PlutusData       data(constructor);

  // Act
  PlutusList fields = constructor.fields();
  fields.pushBack(PlutusData(2));
  constructor.setAlternative(std::numeric_limits<std::uint64_t>::max());
  constructor.setFields(fields);
  fields.pushBack(PlutusData(3));
  ConstrPlutusData extracted = data.asConstructor();
  extracted.setAlternative(7);

  // Assert
  EXPECT_EQ(constructor.alternative(), std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ(constructor.fields().size(), 2U);
  EXPECT_EQ(ConstrPlutusData::fromCbor(constructor.toCbor()), constructor);
  EXPECT_EQ(original.alternative(), 0U);
  EXPECT_EQ(original.fields().size(), 1U);
  EXPECT_EQ(data.kind(), PlutusDataKind::Constructor);
  EXPECT_EQ(data.asConstructor().alternative(), 0U);
}
