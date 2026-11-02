/**
 * @file CborReaderStateTest.cpp
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

#include <Cometa/Cbor/CborReaderState.h>

#include <cardano/cbor/cbor_reader_state.h>
#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(CborReaderStateTest, MatchesDefinedValues)
{
  // Act & Assert
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::Undefined), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_UNDEFINED));
  EXPECT_EQ(toString(CborReaderState::Undefined), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_UNDEFINED));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::UnsignedInteger), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_UNSIGNED_INTEGER));
  EXPECT_EQ(toString(CborReaderState::UnsignedInteger), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_UNSIGNED_INTEGER));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::NegativeInteger), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_NEGATIVE_INTEGER));
  EXPECT_EQ(toString(CborReaderState::NegativeInteger), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_NEGATIVE_INTEGER));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::ByteString), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_BYTESTRING));
  EXPECT_EQ(toString(CborReaderState::ByteString), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_BYTESTRING));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::StartIndefiniteLengthByteString), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_START_INDEFINITE_LENGTH_BYTESTRING));
  EXPECT_EQ(toString(CborReaderState::StartIndefiniteLengthByteString), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_START_INDEFINITE_LENGTH_BYTESTRING));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::EndIndefiniteLengthByteString), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_END_INDEFINITE_LENGTH_BYTESTRING));
  EXPECT_EQ(toString(CborReaderState::EndIndefiniteLengthByteString), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_END_INDEFINITE_LENGTH_BYTESTRING));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::TextString), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_TEXTSTRING));
  EXPECT_EQ(toString(CborReaderState::TextString), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_TEXTSTRING));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::StartIndefiniteLengthTextString), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_START_INDEFINITE_LENGTH_TEXTSTRING));
  EXPECT_EQ(toString(CborReaderState::StartIndefiniteLengthTextString), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_START_INDEFINITE_LENGTH_TEXTSTRING));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::EndIndefiniteLengthTextString), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_END_INDEFINITE_LENGTH_TEXTSTRING));
  EXPECT_EQ(toString(CborReaderState::EndIndefiniteLengthTextString), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_END_INDEFINITE_LENGTH_TEXTSTRING));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::StartArray), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_START_ARRAY));
  EXPECT_EQ(toString(CborReaderState::StartArray), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_START_ARRAY));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::EndArray), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_END_ARRAY));
  EXPECT_EQ(toString(CborReaderState::EndArray), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_END_ARRAY));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::StartMap), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_START_MAP));
  EXPECT_EQ(toString(CborReaderState::StartMap), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_START_MAP));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::EndMap), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_END_MAP));
  EXPECT_EQ(toString(CborReaderState::EndMap), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_END_MAP));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::Tag), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_TAG));
  EXPECT_EQ(toString(CborReaderState::Tag), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_TAG));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::SimpleValue), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_SIMPLE_VALUE));
  EXPECT_EQ(toString(CborReaderState::SimpleValue), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_SIMPLE_VALUE));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::HalfPrecisionFloat), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_HALF_PRECISION_FLOAT));
  EXPECT_EQ(toString(CborReaderState::HalfPrecisionFloat), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_HALF_PRECISION_FLOAT));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::SinglePrecisionFloat), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_SINGLE_PRECISION_FLOAT));
  EXPECT_EQ(toString(CborReaderState::SinglePrecisionFloat), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_SINGLE_PRECISION_FLOAT));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::DoublePrecisionFloat), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_DOUBLE_PRECISION_FLOAT));
  EXPECT_EQ(toString(CborReaderState::DoublePrecisionFloat), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_DOUBLE_PRECISION_FLOAT));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::Null), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_NULL));
  EXPECT_EQ(toString(CborReaderState::Null), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_NULL));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::Boolean), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_BOOLEAN));
  EXPECT_EQ(toString(CborReaderState::Boolean), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_BOOLEAN));
  EXPECT_EQ(static_cast<std::uint32_t>(CborReaderState::Finished), static_cast<std::uint32_t>(CARDANO_CBOR_READER_STATE_FINISHED));
  EXPECT_EQ(toString(CborReaderState::Finished), cardano_cbor_reader_state_to_string(CARDANO_CBOR_READER_STATE_FINISHED));
}
