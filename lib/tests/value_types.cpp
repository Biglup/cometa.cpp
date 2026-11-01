/**
 * \file value_types.cpp
 *
 * \author angel.castillo
 * \date   Nov 01, 2026
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

#include <cometa/cometa.hpp>
#include <cstdlib>
#include <gtest/gtest.h>
#include <limits>
#include <src/allocators.h>
#include <type_traits>
#include <utility>

using namespace Biglup::Cometa;

namespace
{
const std::string hashHex(56, '0');

Blake2bHash
keyHash()
{
  return Blake2bHash::fromHex(hashHex);
}
} // namespace

TEST(Blake2bHash, MatchesKnownDigest)
{
  EXPECT_EQ(Blake2bHash::compute({ 'a', 'b', 'c' }).toHex(), "bddd813c634239723171ef3fee98579b94964e3bb1cb3e427262c8c068d52319");
}

TEST(Blake2bHash, CopiesMovesAndSerializes)
{
  auto hash  = keyHash();
  auto copy  = hash;
  auto moved = std::move(hash);
  EXPECT_EQ(copy, moved);
  EXPECT_EQ(moved.toHex(), hashHex);
  EXPECT_THROW(hash.toHex(), std::logic_error);
  hash = copy;
  EXPECT_EQ(hash, copy);
  EXPECT_EQ(Blake2bHash::fromCbor(copy.toCbor()), copy);
  EXPECT_EQ(copy.toCbor().at(0), 0x58);
  EXPECT_EQ(copy.toCbor().at(1), 28);
  EXPECT_THROW(Blake2bHash::fromHex("0x"), std::invalid_argument);
  EXPECT_THROW(Blake2bHash::fromHex("abc"), std::invalid_argument);
  EXPECT_THROW(Blake2bHash::compute({}), std::invalid_argument);
  EXPECT_THROW(Blake2bHash::compute({ 1 }, 0), std::invalid_argument);
  EXPECT_THROW(Blake2bHash::compute({ 1 }, 65), std::invalid_argument);
}

TEST(AssetName, PreservesBinaryNamesAndEmptyNames)
{
  auto binary = AssetName::fromHex("610062ff");
  EXPECT_EQ(binary.toBytes(), (std::vector<std::uint8_t> { 0x61, 0, 0x62, 0xff }));
  EXPECT_EQ(binary.toString().size(), 4);
  EXPECT_EQ(AssetName::fromCbor(binary.toCbor()), binary);
  EXPECT_EQ(AssetName::fromString("Cometa").toHex(), "436f6d657461");
  EXPECT_EQ(AssetName::fromHex("").toCbor(), (std::vector<std::uint8_t> { 0x40 }));
  EXPECT_EQ(AssetName::fromString("").toBytes().size(), 0);
  EXPECT_NO_THROW(AssetName(std::vector<std::uint8_t>(32, 0)));
  EXPECT_THROW(AssetName(std::vector<std::uint8_t>(33, 0)), Error);
}

TEST(Credential, CopyMutationDoesNotChangeOriginal)
{
  Credential original(CredentialType::KeyHash, keyHash());
  auto       copy = original;
  copy.setType(CredentialType::ScriptHash);
  copy.setHash(Blake2bHash::fromHex(std::string(56, '1')));
  EXPECT_EQ(original.type(), CredentialType::KeyHash);
  EXPECT_EQ(original.hash().toHex(), hashHex);
  EXPECT_NE(copy, original);
  EXPECT_EQ(Credential::fromCbor(copy.toCbor()), copy);
  const auto cbor = original.toCbor();
  EXPECT_EQ(cbor.at(0), 0x82);
  EXPECT_EQ(cbor.at(1), 0);
  EXPECT_EQ(cbor.at(2), 0x58);
  EXPECT_EQ(cbor.at(3), 28);
}

TEST(Credential, RejectsInvalidValuesWithoutChangingDestination)
{
  Credential value(CredentialType::KeyHash, keyHash());
  const auto before = value;
  EXPECT_THROW(value.setHash(Blake2bHash::compute({ 1 })), std::invalid_argument);
  EXPECT_THROW(value.setType(static_cast<CredentialType>(42)), std::invalid_argument);
  EXPECT_EQ(value, before);
}

TEST(ValueTypes, RejectsTrailingAndMalformedCbor)
{
  auto bytes = keyHash().toCbor();
  bytes.push_back(0);
  EXPECT_THROW(Blake2bHash::fromCbor(bytes), std::invalid_argument);
  EXPECT_THROW(AssetName::fromCbor({ 0x41 }), Error);
  EXPECT_THROW(Credential::fromCbor({ 0x82, 0 }), Error);
}

TEST(ValueTypes, CopiesSurviveSourceDestruction)
{
  auto copy = []
  { Credential source(CredentialType::KeyHash, keyHash()); return Credential(source); }();
  EXPECT_EQ(copy.hash().toHex(), hashHex);
  Credential assigned(CredentialType::ScriptHash, keyHash());
  assigned = copy;
  copy.setType(CredentialType::ScriptHash);
  EXPECT_EQ(assigned.type(), CredentialType::KeyHash);
  auto* alias = &assigned;
  assigned    = *alias;
  assigned    = std::move(*alias);
  EXPECT_EQ(assigned.type(), CredentialType::KeyHash);
  auto moved = std::move(assigned);
  EXPECT_THROW(assigned.type(), std::logic_error);
  assigned = moved;
  EXPECT_EQ(assigned, moved);
  static_assert(std::is_nothrow_move_constructible_v<Credential>);
  static_assert(std::is_nothrow_move_assignable_v<Credential>);
}

TEST(ValueTypes, ReturnedBytesAreIndependent)
{
  auto value = AssetName::fromString("test");
  auto bytes = value.toBytes();
  bytes[0]   = 0;
  EXPECT_EQ(bytes[0], 0);
  EXPECT_EQ(value.toString(), "test");
}

namespace
{
std::size_t failureIndex    = 0;
std::size_t allocationIndex = 0;

void*
failingMalloc(std::size_t size)
{
  return allocationIndex++ == failureIndex ? nullptr : std::malloc(size);
}

class AllocationFailure
{
  public:

    explicit AllocationFailure(std::size_t index)
    {
      allocationIndex = 0;
      failureIndex    = index;
      cardano_set_allocators(failingMalloc, std::realloc, std::free);
    }

    ~AllocationFailure()
    {
      cardano_set_allocators(std::malloc, std::realloc, std::free);
    }

    AllocationFailure(const AllocationFailure&)            = delete;
    AllocationFailure& operator=(const AllocationFailure&) = delete;
};
} // namespace

TEST(Credential, FailedCopyAssignmentPreservesDestination)
{
  const Credential source(CredentialType::KeyHash, keyHash());
  Credential       destination(CredentialType::ScriptHash, Blake2bHash::fromHex(std::string(56, '1')));
  const auto       before          = destination;
  std::size_t      allocationCount = 0;
  {
    AllocationFailure count(std::numeric_limits<std::size_t>::max());
    const auto        copy = source;
    allocationCount        = allocationIndex;
    EXPECT_EQ(copy, source);
  }
  ASSERT_GT(allocationCount, 0);
  for (std::size_t i = 0; i < allocationCount; ++i)
  {
    {
      AllocationFailure fail(i);
      EXPECT_THROW(destination = source, std::bad_alloc);
    }
    EXPECT_EQ(destination, before);
    EXPECT_EQ(source.type(), CredentialType::KeyHash);
  }
}

namespace
{
template<class T>
T sampleValue();

template<>
Blake2bHash
sampleValue<Blake2bHash>()
{
  return keyHash();
}

template<>
AssetName
sampleValue<AssetName>()
{
  return AssetName::fromString("Cometa");
}

template<>
Credential
sampleValue<Credential>()
{
  return Credential(CredentialType::KeyHash, keyHash());
}

template<class T>
class ValueContract : public testing::Test
{
};

using ValueTypes = testing::Types<Blake2bHash, AssetName, Credential>;
TYPED_TEST_SUITE(ValueContract, ValueTypes);
} // namespace

TYPED_TEST(ValueContract, SupportsCopyMoveSelfAssignmentAndSwap)
{
  auto source = sampleValue<TypeParam>();
  auto copied = source;
  auto moved  = std::move(source);
  EXPECT_EQ(copied, moved);
  EXPECT_THROW(source.toCbor(), std::logic_error);
  source      = copied;
  auto* alias = &source;
  source      = *alias;
  EXPECT_EQ(source, copied);
  source = std::move(*alias);
  EXPECT_EQ(source, copied);
  source = std::move(moved);
  EXPECT_THROW(moved.toCbor(), std::logic_error);
  swap(source, moved);
  EXPECT_THROW(source.toCbor(), std::logic_error);
  EXPECT_EQ(moved, copied);
  static_assert(std::is_nothrow_move_constructible_v<TypeParam>);
  static_assert(std::is_nothrow_move_assignable_v<TypeParam>);
}

TYPED_TEST(ValueContract, ReleasesPartialCopiesOnEveryAllocationFailure)
{
  const auto  source = sampleValue<TypeParam>();
  std::size_t count  = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const TypeParam   copy(source);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);
  for (std::size_t i = 0; i < count; ++i)
  {
    AllocationFailure fail(i);
    EXPECT_THROW({ const TypeParam copy(source); }, std::bad_alloc);
  }
  EXPECT_EQ(source, sampleValue<TypeParam>());
}

TYPED_TEST(ValueContract, ReleasesParserStateOnAllocationFailure)
{
  const auto  cbor  = sampleValue<TypeParam>().toCbor();
  std::size_t count = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const auto        parsed = TypeParam::fromCbor(cbor);
    count                    = allocationIndex;
  }
  for (std::size_t i = 0; i < count; ++i)
  {
    AllocationFailure fail(i);
    try
    {
      const auto parsed = TypeParam::fromCbor(cbor);
      // Optional native caches can fail without invalidating the parsed value.
      EXPECT_EQ(parsed.toCbor(), cbor);
    }
    catch (const std::bad_alloc&)
    {
    }
    catch (const Error& error)
    {
      // Some native parser paths translate allocation errors into CBOR errors.
      EXPECT_NE(error.code().value(), 0);
    }
  }
}

TEST(ValueTypes, MatchesNativeJsonAndOrdering)
{
  const auto hash = keyHash();
  EXPECT_EQ(hash.toJson(), "\"" + hashHex + "\"");
  const Credential credential(CredentialType::KeyHash, hash);
  EXPECT_EQ(credential.toJson(), "{\"tag\":\"pubkey_hash\",\"value\":\"" + hashHex + "\"}");
  EXPECT_EQ(AssetName::fromString("Cometa").toJson(), "\"436f6d657461\"");
  EXPECT_LT(AssetName::fromString("a"), AssetName::fromString("b"));
  EXPECT_LT(hash, Blake2bHash::fromHex(std::string(56, '1')));
}
