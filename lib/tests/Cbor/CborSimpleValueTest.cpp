/**
 * @file CborSimpleValueTest.cpp
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

#include <Cometa/Cbor/CborSimpleValue.h>

#include <cardano/cbor/cbor_simple_value.h>
#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(CborSimpleValueTest, MatchesDefinedValues)
{
  // Act & Assert
  EXPECT_EQ(static_cast<std::uint32_t>(CborSimpleValue::False), static_cast<std::uint32_t>(CARDANO_CBOR_SIMPLE_VALUE_FALSE));
  EXPECT_EQ(toString(CborSimpleValue::False), cardano_cbor_simple_value_to_string(CARDANO_CBOR_SIMPLE_VALUE_FALSE));
  EXPECT_EQ(static_cast<std::uint32_t>(CborSimpleValue::True), static_cast<std::uint32_t>(CARDANO_CBOR_SIMPLE_VALUE_TRUE));
  EXPECT_EQ(toString(CborSimpleValue::True), cardano_cbor_simple_value_to_string(CARDANO_CBOR_SIMPLE_VALUE_TRUE));
  EXPECT_EQ(static_cast<std::uint32_t>(CborSimpleValue::Null), static_cast<std::uint32_t>(CARDANO_CBOR_SIMPLE_VALUE_NULL));
  EXPECT_EQ(toString(CborSimpleValue::Null), cardano_cbor_simple_value_to_string(CARDANO_CBOR_SIMPLE_VALUE_NULL));
  EXPECT_EQ(static_cast<std::uint32_t>(CborSimpleValue::Undefined), static_cast<std::uint32_t>(CARDANO_CBOR_SIMPLE_VALUE_UNDEFINED));
  EXPECT_EQ(toString(CborSimpleValue::Undefined), cardano_cbor_simple_value_to_string(CARDANO_CBOR_SIMPLE_VALUE_UNDEFINED));
}
