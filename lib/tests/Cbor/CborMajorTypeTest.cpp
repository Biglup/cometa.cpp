/**
 * @file CborMajorTypeTest.cpp
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

#include <Cometa/Cbor/CborMajorType.h>

#include <cardano/cbor/cbor_major_type.h>
#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(CborMajorTypeTest, MatchesDefinedValues)
{
  // Act & Assert
  EXPECT_EQ(static_cast<std::uint32_t>(CborMajorType::UnsignedInteger), static_cast<std::uint32_t>(CARDANO_CBOR_MAJOR_TYPE_UNSIGNED_INTEGER));
  EXPECT_EQ(toString(CborMajorType::UnsignedInteger), cardano_cbor_major_type_to_string(CARDANO_CBOR_MAJOR_TYPE_UNSIGNED_INTEGER));
  EXPECT_EQ(static_cast<std::uint32_t>(CborMajorType::NegativeInteger), static_cast<std::uint32_t>(CARDANO_CBOR_MAJOR_TYPE_NEGATIVE_INTEGER));
  EXPECT_EQ(toString(CborMajorType::NegativeInteger), cardano_cbor_major_type_to_string(CARDANO_CBOR_MAJOR_TYPE_NEGATIVE_INTEGER));
  EXPECT_EQ(static_cast<std::uint32_t>(CborMajorType::ByteString), static_cast<std::uint32_t>(CARDANO_CBOR_MAJOR_TYPE_BYTE_STRING));
  EXPECT_EQ(toString(CborMajorType::ByteString), cardano_cbor_major_type_to_string(CARDANO_CBOR_MAJOR_TYPE_BYTE_STRING));
  EXPECT_EQ(static_cast<std::uint32_t>(CborMajorType::Utf8String), static_cast<std::uint32_t>(CARDANO_CBOR_MAJOR_TYPE_UTF8_STRING));
  EXPECT_EQ(toString(CborMajorType::Utf8String), cardano_cbor_major_type_to_string(CARDANO_CBOR_MAJOR_TYPE_UTF8_STRING));
  EXPECT_EQ(static_cast<std::uint32_t>(CborMajorType::Array), static_cast<std::uint32_t>(CARDANO_CBOR_MAJOR_TYPE_ARRAY));
  EXPECT_EQ(toString(CborMajorType::Array), cardano_cbor_major_type_to_string(CARDANO_CBOR_MAJOR_TYPE_ARRAY));
  EXPECT_EQ(static_cast<std::uint32_t>(CborMajorType::Map), static_cast<std::uint32_t>(CARDANO_CBOR_MAJOR_TYPE_MAP));
  EXPECT_EQ(toString(CborMajorType::Map), cardano_cbor_major_type_to_string(CARDANO_CBOR_MAJOR_TYPE_MAP));
  EXPECT_EQ(static_cast<std::uint32_t>(CborMajorType::Tag), static_cast<std::uint32_t>(CARDANO_CBOR_MAJOR_TYPE_TAG));
  EXPECT_EQ(toString(CborMajorType::Tag), cardano_cbor_major_type_to_string(CARDANO_CBOR_MAJOR_TYPE_TAG));
  EXPECT_EQ(static_cast<std::uint32_t>(CborMajorType::Simple), static_cast<std::uint32_t>(CARDANO_CBOR_MAJOR_TYPE_SIMPLE));
  EXPECT_EQ(toString(CborMajorType::Simple), cardano_cbor_major_type_to_string(CARDANO_CBOR_MAJOR_TYPE_SIMPLE));
  EXPECT_EQ(static_cast<std::uint32_t>(CborMajorType::Undefined), static_cast<std::uint32_t>(CARDANO_CBOR_MAJOR_TYPE_UNDEFINED));
  EXPECT_EQ(toString(CborMajorType::Undefined), cardano_cbor_major_type_to_string(CARDANO_CBOR_MAJOR_TYPE_UNDEFINED));
}
