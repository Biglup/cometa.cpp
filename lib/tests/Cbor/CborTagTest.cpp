/**
 * @file CborTagTest.cpp
 *
 * @brief CBOR enum mappings.
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

#include <Cometa/Cbor/CborTag.h>

#include <cardano/cbor/cbor_tag.h>
#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(CborTagTest, MatchesDefinedValues)
{
  // Act & Assert
  EXPECT_EQ(static_cast<std::uint32_t>(CborTag::DateTimeString), static_cast<std::uint32_t>(CARDANO_CBOR_TAG_DATE_TIME_STRING));
  EXPECT_EQ(toString(CborTag::DateTimeString), cardano_cbor_tag_to_string(CARDANO_CBOR_TAG_DATE_TIME_STRING));
  EXPECT_EQ(static_cast<std::uint32_t>(CborTag::UnixTimeSeconds), static_cast<std::uint32_t>(CARDANO_CBOR_TAG_UNIX_TIME_SECONDS));
  EXPECT_EQ(toString(CborTag::UnixTimeSeconds), cardano_cbor_tag_to_string(CARDANO_CBOR_TAG_UNIX_TIME_SECONDS));
  EXPECT_EQ(static_cast<std::uint32_t>(CborTag::UnsignedBigNum), static_cast<std::uint32_t>(CARDANO_CBOR_TAG_UNSIGNED_BIG_NUM));
  EXPECT_EQ(toString(CborTag::UnsignedBigNum), cardano_cbor_tag_to_string(CARDANO_CBOR_TAG_UNSIGNED_BIG_NUM));
  EXPECT_EQ(static_cast<std::uint32_t>(CborTag::NegativeBigNum), static_cast<std::uint32_t>(CARDANO_CBOR_TAG_NEGATIVE_BIG_NUM));
  EXPECT_EQ(toString(CborTag::NegativeBigNum), cardano_cbor_tag_to_string(CARDANO_CBOR_TAG_NEGATIVE_BIG_NUM));
  EXPECT_EQ(static_cast<std::uint32_t>(CborTag::DecimalFraction), static_cast<std::uint32_t>(CARDANO_CBOR_TAG_DECIMAL_FRACTION));
  EXPECT_EQ(toString(CborTag::DecimalFraction), cardano_cbor_tag_to_string(CARDANO_CBOR_TAG_DECIMAL_FRACTION));
  EXPECT_EQ(static_cast<std::uint32_t>(CborTag::BigFloat), static_cast<std::uint32_t>(CARDANO_CBOR_TAG_BIG_FLOAT));
  EXPECT_EQ(toString(CborTag::BigFloat), cardano_cbor_tag_to_string(CARDANO_CBOR_TAG_BIG_FLOAT));
  EXPECT_EQ(static_cast<std::uint32_t>(CborTag::Set), static_cast<std::uint32_t>(CARDANO_CBOR_TAG_SET));
  EXPECT_EQ(toString(CborTag::Set), cardano_cbor_tag_to_string(CARDANO_CBOR_TAG_SET));
  EXPECT_EQ(static_cast<std::uint32_t>(CborTag::SelfDescribeCbor), static_cast<std::uint32_t>(CARDANO_CBOR_TAG_SELF_DESCRIBE_CBOR));
  EXPECT_EQ(toString(CborTag::SelfDescribeCbor), cardano_cbor_tag_to_string(CARDANO_CBOR_TAG_SELF_DESCRIBE_CBOR));
}
