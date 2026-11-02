/**
 * @file JsonContextTest.cpp
 *
 * @brief JSON enum mappings.
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

#include <Cometa/Json/JsonContext.h>

#include <cardano/json/json_context.h>
#include <gtest/gtest.h>

/* IMPLEMENTATION ************************************************************/

TEST(JsonContextTest, MatchesDefinedValues)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(Cometa::JsonContext::Root), static_cast<int>(CARDANO_JSON_CONTEXT_ROOT));
  EXPECT_EQ(static_cast<int>(Cometa::JsonContext::Object), static_cast<int>(CARDANO_JSON_CONTEXT_OBJECT));
  EXPECT_EQ(static_cast<int>(Cometa::JsonContext::Array), static_cast<int>(CARDANO_JSON_CONTEXT_ARRAY));
}
