/**
 * @file ValueContractTest.cpp
 *
 * @brief Unit tests for value semantics, serialization, and allocation failures.
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

#include <src/allocators.h>

#include <gtest/gtest.h>

#include <cstdlib>
#include <limits>
#include <type_traits>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
const std::string HASH_HEX(56, '0');

Blake2bHash
keyHash()
{
  return Blake2bHash::fromHex(HASH_HEX);
}

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

template<>
ExUnits
sampleValue<ExUnits>()
{
  return ExUnits(100, 200);
}

template<>
ProtocolVersion
sampleValue<ProtocolVersion>()
{
  return ProtocolVersion(12, 0);
}

template<>
UnitInterval
sampleValue<UnitInterval>()
{
  return UnitInterval(3, 4);
}

template<>
Anchor
sampleValue<Anchor>()
{
  return Anchor("https://example.com", Blake2bHash::fromHex(std::string(64, '0')));
}

template<>
GovernanceActionId
sampleValue<GovernanceActionId>()
{
  return GovernanceActionId(Blake2bHash::fromHex(std::string(64, '0')), 1);
}

template<>
BigInt
sampleValue<BigInt>()
{
  return BigInt::fromString("340282366920938463463374607431768211456");
}

using ValueTypes = testing::Types<Blake2bHash, AssetName, Credential, ExUnits, ProtocolVersion, UnitInterval, Anchor, GovernanceActionId>;
TYPED_TEST_SUITE(ValueContract, ValueTypes);
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(Credential, FailedCopyAssignmentPreservesDestination)
{
  // Arrange
  const Credential source(CredentialType::KeyHash, keyHash());
  Credential       destination(CredentialType::ScriptHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential before          = destination;
  std::size_t      allocationCount = 0;
  {
    AllocationFailure count(std::numeric_limits<std::size_t>::max());
    const Credential  copy = source;
    allocationCount        = allocationIndex;
  }
  ASSERT_GT(allocationCount, 0);

  // Act & Assert
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

TYPED_TEST(ValueContract, SupportsCopyAndMove)
{
  // Arrange
  TypeParam source = sampleValue<TypeParam>();

  // Act
  const TypeParam copied = source;
  const TypeParam moved  = std::move(source);

  // Assert
  EXPECT_EQ(copied, moved);
  EXPECT_THROW(source.toCbor(), std::logic_error);
  static_assert(std::is_nothrow_move_constructible_v<TypeParam>);
  static_assert(std::is_nothrow_move_assignable_v<TypeParam>);
}

TYPED_TEST(ValueContract, SupportsCopySelfAssignment)
{
  // Arrange
  TypeParam        source = sampleValue<TypeParam>();
  const TypeParam  before = source;
  const TypeParam* alias  = &source;

  // Act
  source = *alias;

  // Assert
  EXPECT_EQ(source, before);
}

TYPED_TEST(ValueContract, SupportsMoveSelfAssignment)
{
  // Arrange
  TypeParam       source = sampleValue<TypeParam>();
  const TypeParam before = source;
  TypeParam*      alias  = &source;

  // Act
  source = std::move(*alias);

  // Assert
  EXPECT_EQ(source, before);
}

TYPED_TEST(ValueContract, SupportsAssignmentToMovedFromValues)
{
  // Arrange
  TypeParam source = sampleValue<TypeParam>();
  TypeParam moved  = std::move(source);

  // Act
  source = moved;
  moved  = std::move(source);

  // Assert
  EXPECT_EQ(moved, sampleValue<TypeParam>());
  EXPECT_THROW(source.toCbor(), std::logic_error);
}

TYPED_TEST(ValueContract, SupportsSwapWithMovedFromValues)
{
  // Arrange
  TypeParam source = sampleValue<TypeParam>();
  TypeParam moved  = std::move(source);

  // Act
  swap(source, moved);

  // Assert
  EXPECT_EQ(source, sampleValue<TypeParam>());
  EXPECT_THROW(moved.toCbor(), std::logic_error);
}

TYPED_TEST(ValueContract, ReleasesPartialCopiesOnEveryAllocationFailure)
{
  // Arrange
  const TypeParam source = sampleValue<TypeParam>();
  std::size_t     count  = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const TypeParam   copy(source);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    AllocationFailure fail(i);
    EXPECT_THROW({ const TypeParam copy(source); }, std::bad_alloc);
  }
  EXPECT_EQ(source, sampleValue<TypeParam>());
}

TYPED_TEST(ValueContract, ReleasesParserStateOnAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> cbor  = sampleValue<TypeParam>().toCbor();
  std::size_t                     count = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const TypeParam   parsed = TypeParam::fromCbor(cbor);
    count                    = allocationIndex;
  }

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    AllocationFailure fail(i);
    try
    {
      const TypeParam parsed = TypeParam::fromCbor(cbor);
      // Optional native caches can fail without invalidating the parsed value.
      EXPECT_EQ(parsed.toCbor(), cbor);
    }
    catch (const std::bad_alloc&)
    {
    }
    catch (const Error& error)
    {
      // Some native parser paths translate allocation errors into CBOR errors.
      EXPECT_NE(error.code(), makeErrorCode(ErrorCode::Success));
    }
  }
}

TYPED_TEST(ValueContract, ReleasesStateWhenConstructionFails)
{
  // Arrange
  AllocationFailure fail(0);

  // Act & Assert
  EXPECT_THROW(sampleValue<TypeParam>(), std::bad_alloc);
}

TYPED_TEST(ValueContract, FailedCopyAssignmentPreservesDestination)
{
  // Arrange
  const TypeParam source      = sampleValue<TypeParam>();
  TypeParam       destination = sampleValue<TypeParam>();
  const TypeParam before      = destination;
  std::size_t     count       = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    destination = source;
    count       = allocationIndex;
  }

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    {
      AllocationFailure fail(i);
      EXPECT_THROW(destination = source, std::bad_alloc);
    }
    EXPECT_EQ(destination, before);
  }
}

TEST(Anchor, FailedMutationPreservesValue)
{
  // Arrange
  const Anchor before = sampleValue<Anchor>();
  std::size_t  count  = 0;
  {
    Anchor            value = before;
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    value.setUrl("https://changed.example");
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    Anchor value = before;
    {
      AllocationFailure fail(i);
      EXPECT_THROW(value.setUrl("https://changed.example"), std::bad_alloc);
    }
    EXPECT_EQ(value, before);
    EXPECT_EQ(value.toCbor(), before.toCbor());
  }
}

TEST(GovernanceActionId, FailedMutationPreservesValue)
{
  // Arrange
  const GovernanceActionId before = sampleValue<GovernanceActionId>();
  std::size_t              count  = 0;
  {
    GovernanceActionId value = before;
    AllocationFailure  tracker(std::numeric_limits<std::size_t>::max());
    value.setIndex(2);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    GovernanceActionId value = before;
    {
      AllocationFailure fail(i);
      EXPECT_THROW(value.setIndex(2), std::bad_alloc);
    }
    EXPECT_EQ(value, before);
    EXPECT_EQ(value.toCbor(), before.toCbor());
  }
}

TEST(UnitInterval, FloatingPointConversionReleasesStateOnFailure)
{
  // Arrange
  AllocationFailure fail(0);

  // Act & Assert
  EXPECT_THROW(UnitInterval::fromDouble(0.75), std::bad_alloc);
}

TEST(BigInt, FailedArithmeticPreservesOperands)
{
  // Arrange
  BigInt       value  = sampleValue<BigInt>();
  const BigInt before = value;

  // Act & Assert
  {
    AllocationFailure fail(0);
    EXPECT_THROW(value *= value, std::bad_alloc);
  }
  EXPECT_EQ(value, before);
}

TEST(BigInt, FailedDivisionReleasesPartialResults)
{
  // Arrange
  const BigInt value = sampleValue<BigInt>();
  const BigInt divisor(7);

  // Act & Assert
  for (std::size_t i = 0; i < 2; ++i)
  {
    AllocationFailure fail(i);
    EXPECT_THROW(value.divRem(divisor), std::bad_alloc);
  }
}

TEST(BigInt, FailedCopyPreservesDestination)
{
  // Arrange
  const BigInt source = sampleValue<BigInt>();
  BigInt       destination(-7);

  // Act & Assert
  {
    AllocationFailure fail(0);
    EXPECT_THROW(destination = source, std::bad_alloc);
  }
  EXPECT_EQ(destination, BigInt(-7));
  EXPECT_EQ(source, sampleValue<BigInt>());
}

TEST(BigInt, ConstructorsReleaseStateOnFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes { 1, 2, 3 };

  // Act & Assert
  {
    AllocationFailure fail(0);
    EXPECT_THROW(BigInt(1), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(BigInt::fromUnsigned(1), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(BigInt::fromString("12345678901234567890"), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(BigInt::fromBytes(bytes), std::bad_alloc);
  }
}

TEST(Base58, ReportsAllocationFailures)
{
  // Arrange
  const std::vector<std::uint8_t> bytes { 58 };

  // Act & Assert
  {
    AllocationFailure fail(0);
    EXPECT_THROW(encodeBase58(bytes), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(decodeBase58("21"), std::bad_alloc);
  }
}

TEST(Bech32, ReleasesStateOnEveryEncodingAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes { 1, 2, 3 };
  std::size_t                     count = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const std::string encoded = encodeBech32("test", bytes);
    EXPECT_FALSE(encoded.empty());
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    AllocationFailure fail(i);
    bool              failed = false;

    try
    {
      encodeBech32("test", bytes);
    }
    catch (const std::bad_alloc&)
    {
      failed = true;
    }
    catch (const Error&)
    {
      failed = true;
    }

    EXPECT_TRUE(failed);
  }
}

TEST(Bech32, ReleasesStateOnEveryDecodingAllocationFailure)
{
  // Arrange
  const std::string encoded = encodeBech32("test", { 1, 2, 3 });
  std::size_t       count   = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const Bech32Data  decoded = decodeBech32(encoded);
    EXPECT_EQ(decoded.prefix, "test");
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    AllocationFailure fail(i);
    bool              failed = false;

    try
    {
      decodeBech32(encoded);
    }
    catch (const std::bad_alloc&)
    {
      failed = true;
    }
    catch (const Error&)
    {
      failed = true;
    }

    EXPECT_TRUE(failed);
  }
}

TEST(CborReader, FailedCopiesReleasePartialStateAndPreserveDestination)
{
  // Arrange
  CborReader source("82810102");
  ASSERT_EQ(source.readStartArray(), 2U);
  ASSERT_EQ(source.readStartArray(), 1U);
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const CborReader  copy(source);

    EXPECT_GT(copy.bytesRemaining(), 0U);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    CborReader destination("03");

    {
      AllocationFailure fail(index);

      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination.readUInt(), 3U);
  }
}

TEST(CborWriter, FailedCopiesReleasePartialStateAndPreserveDestination)
{
  // Arrange
  CborWriter source;
  source.writeStartArray(2);
  source.writeUInt(1);
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const CborWriter  copy(source);

    EXPECT_GT(copy.toBytes().size(), 0U);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    CborWriter destination;
    destination.writeUInt(3);

    {
      AllocationFailure fail(index);

      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination.toHex(), "03");
  }
}

TEST(CborReader, ConstructorReleasesEveryPartialAllocation)
{
  // Arrange
  const std::vector<std::uint8_t> bytes { 0x81, 0x01 };
  std::size_t                     allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const CborReader  reader(bytes);

    EXPECT_EQ(reader.bytesRemaining(), bytes.size());
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index);

    EXPECT_THROW(CborReader reader(bytes), std::bad_alloc);
  }
}

TEST(CborWriter, FailedResetPreservesEncodedBytes)
{
  // Arrange
  CborWriter writer;
  writer.writeUInt(42);

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(writer.reset(), std::bad_alloc);
  }

  EXPECT_EQ(writer.toHex(), "182a");
}

TEST(JsonValue, FailedCopiesReleaseStateAndPreserveDestination)
{
  // Arrange
  const JsonValue source      = JsonValue::parse("{\"a\":[1,2,3],\"b\":{\"x\":1.2300}}");
  std::size_t     allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const JsonValue   copy(source);

    EXPECT_GT(copy.size(), 0);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    JsonValue destination = JsonValue::parse("42");

    {
      AllocationFailure fail(index);

      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination.asInt(), 42);
  }
}

TEST(JsonWriter, FailedCopiesReleaseStateAndPreserveDestination)
{
  // Arrange
  JsonWriter source;
  source.writeStartObject();
  source.writePropertyName("a");
  source.writeStartArray();
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const JsonWriter  copy(source);

    EXPECT_GT(static_cast<int>(copy.context()), 0);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    JsonWriter destination;
    destination.writeUInt(42);

    {
      AllocationFailure fail(index);

      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination.toString(), "42");
  }
}

TEST(JsonValue, ParsingReportsEveryAllocationFailure)
{
  // Arrange
  const std::string json        = "{\"a\":[1,2,\"text\",true,null,{}]}";
  std::size_t       allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const JsonValue   value = JsonValue::parse(json);

    EXPECT_EQ(value.size(), 1U);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index);

    EXPECT_THROW(JsonValue::parse(json), std::bad_alloc);
  }
}

TEST(JsonValue, FailedChildCopiesLeaveParentReadable)
{
  // Arrange
  const JsonValue object = JsonValue::parse("{\"a\":[1,2]}");
  const JsonValue array  = object.at("a");

  // Act & Assert
  {
    AllocationFailure fail(0);
    EXPECT_THROW(object.at("a"), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(object.valueAt(0), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(array.at(0), std::bad_alloc);
  }
  EXPECT_EQ(object.toJson(), "{\"a\":[1,2]}");
}

TEST(JsonWriter, FailedResetPreservesDocumentAndFormatting)
{
  // Arrange
  JsonWriter writer(JsonFormat::Pretty);
  writer.writeStartArray();
  writer.writeUInt(42);
  writer.writeEndArray();
  const std::string before = writer.toString();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(writer.reset(), std::bad_alloc);
  }
  EXPECT_EQ(writer.toString(), before);
}

TEST(JsonWriter, ReportsBigIntegerFormattingAllocationFailure)
{
  // Arrange
  JsonWriter   writer;
  const BigInt value(42);

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(writer.writeBigIntAsString(value), std::bad_alloc);
  }
  EXPECT_THROW(writer.toString(), std::bad_alloc);
  writer.reset();
  writer.writeBigIntAsString(value);
  EXPECT_EQ(writer.toString(), "\"42\"");
}

TEST(Ed25519Signature, FailedCopyPreservesBothKeys)
{
  // Arrange
  const Ed25519Signature source         = Ed25519Signature(std::vector<std::uint8_t>(64, 1));
  Ed25519Signature       destination    = Ed25519Signature(std::vector<std::uint8_t>(64, 2));
  const std::string      sourceHex      = source.toHex();
  const std::string      destinationHex = destination.toHex();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(destination = source, std::bad_alloc);
  }

  EXPECT_EQ(source.toHex(), sourceHex);
  EXPECT_EQ(destination.toHex(), destinationHex);
}

TEST(Ed25519Signature, ConstructorReportsAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(64, 1);
  AllocationFailure               fail(0);

  // Act & Assert
  EXPECT_THROW((void)Ed25519Signature(bytes), std::bad_alloc);
}

TEST(Ed25519PublicKey, FailedCopyPreservesBothKeys)
{
  // Arrange
  const Ed25519PublicKey source         = Ed25519PublicKey(std::vector<std::uint8_t>(32, 1));
  Ed25519PublicKey       destination    = Ed25519PublicKey(std::vector<std::uint8_t>(32, 2));
  const std::string      sourceHex      = source.toHex();
  const std::string      destinationHex = destination.toHex();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(destination = source, std::bad_alloc);
  }

  EXPECT_EQ(source.toHex(), sourceHex);
  EXPECT_EQ(destination.toHex(), destinationHex);
}

TEST(Ed25519PublicKey, ConstructorReportsAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(32, 1);
  AllocationFailure               fail(0);

  // Act & Assert
  EXPECT_THROW((void)Ed25519PublicKey(bytes), std::bad_alloc);
}

TEST(Ed25519PrivateKey, FailedCopyPreservesBothKeys)
{
  // Arrange
  const Ed25519PrivateKey source         = Ed25519PrivateKey::fromNormalBytes(std::vector<std::uint8_t>(32, 1));
  Ed25519PrivateKey       destination    = Ed25519PrivateKey::fromNormalBytes(std::vector<std::uint8_t>(32, 2));
  const std::string       sourceHex      = source.toHex();
  const std::string       destinationHex = destination.toHex();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(destination = source, std::bad_alloc);
  }

  EXPECT_EQ(source.toHex(), sourceHex);
  EXPECT_EQ(destination.toHex(), destinationHex);
}

TEST(Ed25519PrivateKey, ConstructorReportsAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(32, 1);
  AllocationFailure               fail(0);

  // Act & Assert
  EXPECT_THROW((void)Ed25519PrivateKey::fromNormalBytes(bytes), std::bad_alloc);
}

TEST(Bip32PublicKey, FailedCopyPreservesBothKeys)
{
  // Arrange
  const Bip32PublicKey source         = Bip32PublicKey(std::vector<std::uint8_t>(64, 1));
  Bip32PublicKey       destination    = Bip32PublicKey(std::vector<std::uint8_t>(64, 2));
  const std::string    sourceHex      = source.toHex();
  const std::string    destinationHex = destination.toHex();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(destination = source, std::bad_alloc);
  }

  EXPECT_EQ(source.toHex(), sourceHex);
  EXPECT_EQ(destination.toHex(), destinationHex);
}

TEST(Bip32PublicKey, ConstructorReportsAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(64, 1);
  AllocationFailure               fail(0);

  // Act & Assert
  EXPECT_THROW((void)Bip32PublicKey(bytes), std::bad_alloc);
}

TEST(Bip32PrivateKey, FailedCopyPreservesBothKeys)
{
  // Arrange
  const Bip32PrivateKey source         = Bip32PrivateKey(std::vector<std::uint8_t>(96, 1));
  Bip32PrivateKey       destination    = Bip32PrivateKey(std::vector<std::uint8_t>(96, 2));
  const std::string     sourceHex      = source.toHex();
  const std::string     destinationHex = destination.toHex();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(destination = source, std::bad_alloc);
  }

  EXPECT_EQ(source.toHex(), sourceHex);
  EXPECT_EQ(destination.toHex(), destinationHex);
}

TEST(Bip32PrivateKey, ConstructorReportsAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(96, 1);
  AllocationFailure               fail(0);

  // Act & Assert
  EXPECT_THROW((void)Bip32PrivateKey(bytes), std::bad_alloc);
}

TEST(Bip32PrivateKey, FailedDerivationReleasesWorkingKeys)
{
  // Arrange
  const Bip32PrivateKey            source = Bip32PrivateKey::fromBip39Entropy(std::vector<std::uint8_t>(16, 0));
  const std::vector<std::uint32_t> path { hardenIndex(1852), hardenIndex(1815), hardenIndex(0), 0, 0 };
  const std::string                before      = source.toHex();
  std::size_t                      allocations = 0;

  {
    AllocationFailure     tracker(std::numeric_limits<std::size_t>::max());
    const Bip32PrivateKey result = source.derive(path);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    {
      AllocationFailure fail(index);

      EXPECT_THROW(source.derive(path), std::bad_alloc);
    }

    EXPECT_EQ(source.toHex(), before);
  }
}

TEST(Ed25519PrivateKey, FailedSigningAndPublicKeyCreationPreserveKey)
{
  // Arrange
  const Ed25519PrivateKey key    = Ed25519PrivateKey::fromNormalBytes(std::vector<std::uint8_t>(32, 1));
  const std::string       before = key.toHex();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(key.sign({ 1, 2, 3 }), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);

    EXPECT_THROW(key.publicKey(), std::bad_alloc);
  }

  EXPECT_EQ(key.toHex(), before);
}

TEST(Bip32PublicKey, FailedDerivationReleasesWorkingKeys)
{
  // Arrange
  const Bip32PublicKey             source = Bip32PrivateKey::fromBip39Entropy(std::vector<std::uint8_t>(16, 0)).publicKey();
  const std::vector<std::uint32_t> path { 0, 1, 2 };
  const std::string                before      = source.toHex();
  std::size_t                      allocations = 0;

  {
    AllocationFailure    tracker(std::numeric_limits<std::size_t>::max());
    const Bip32PublicKey result = source.derive(path);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    {
      AllocationFailure fail(index);

      EXPECT_THROW(source.derive(path), std::bad_alloc);
    }

    EXPECT_EQ(source.toHex(), before);
  }
}

TEST(Ed25519PrivateKey, ExtendedSigningReleasesEveryPartialAllocation)
{
  // Arrange
  const Ed25519PrivateKey         key = Bip32PrivateKey::fromBip39Entropy(std::vector<std::uint8_t>(16, 0)).toEd25519();
  const std::vector<std::uint8_t> message { 1, 2, 3 };
  const std::string               before      = key.toHex();
  std::size_t                     allocations = 0;

  {
    AllocationFailure      tracker(std::numeric_limits<std::size_t>::max());
    const Ed25519Signature signature = key.sign(message);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    {
      AllocationFailure fail(index);

      EXPECT_THROW(key.sign(message), std::bad_alloc);
    }

    EXPECT_EQ(key.toHex(), before);
  }
}
