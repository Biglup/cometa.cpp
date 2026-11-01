/**
 * @file ErrorTest.cpp
 *
 * @brief Unit tests for error reporting.
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

#include <Cometa/AssetName.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Error, ExposesDocumentedCodeAndOperation)
{
  // Arrange
  const auto code = ErrorCode::InvalidArgument;

  // Act
  const Error error(code, "setHash");

  // Assert
  EXPECT_EQ(error.code(), makeErrorCode(code));
  EXPECT_STREQ(error.code().category().name(), "Cometa");
  EXPECT_NE(std::string(error.what()).find("setHash"), std::string::npos);
  EXPECT_FALSE(error.code().message().empty());
  EXPECT_NE(error.code(), std::make_error_code(std::errc::invalid_argument));
}

TEST(Error, DecoderUsesDocumentedCodes)
{
  // Arrange
  const std::vector<std::uint8_t> cbor { 0x01 };

  // Act & Assert
  try
  {
    const auto value = AssetName::fromCbor(cbor);
    FAIL() << "Expected a byte-string decoding error";
  }
  catch (const Error& error)
  {
    EXPECT_EQ(error.code(), makeErrorCode(ErrorCode::Decoding));
  }
}
