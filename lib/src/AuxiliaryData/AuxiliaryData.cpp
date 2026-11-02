/**
 * @file AuxiliaryData.cpp
 *
 * @brief Metadata and scripts attached to a transaction.
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

#include <Cometa/AuxiliaryData/AuxiliaryData.h>

#include "../CardanoInterop.h"

#include <cardano/auxiliary_data/auxiliary_data.h>
#include <cardano/auxiliary_data/plutus_v1_script_list.h>
#include <cardano/auxiliary_data/plutus_v2_script_list.h>
#include <cardano/auxiliary_data/plutus_v3_script_list.h>
#include <cardano/auxiliary_data/plutus_v4_script_list.h>
#include <cardano/auxiliary_data/transaction_metadata.h>
#include <cardano/scripts/native_scripts/native_script_list.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
AuxiliaryData::AuxiliaryData()
: m_value(nullptr)
{
  cardano_auxiliary_data_t* value  = nullptr;
  const cardano_error_t     result = cardano_auxiliary_data_new(&value);

  CardanoObject<cardano_auxiliary_data_t, cardano_auxiliary_data_unref> owner(value);

  checkCardanoError(result, "AuxiliaryData");

  m_value = owner.release();
}

AuxiliaryData::AuxiliaryData(const AuxiliaryData& other)
: m_value(nullptr)
{
  cardano_auxiliary_data_t* value  = nullptr;
  const cardano_error_t     result = cardano_auxiliary_data_deep_clone(other.checked(), &value);

  CardanoObject<cardano_auxiliary_data_t, cardano_auxiliary_data_unref> owner(value);

  checkCardanoError(result, "AuxiliaryData");

  m_value = owner.release();
}

AuxiliaryData::AuxiliaryData(AuxiliaryData&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

AuxiliaryData::~AuxiliaryData()
{
  cardano_auxiliary_data_unref(&m_value);
}

AuxiliaryData
AuxiliaryData::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  AuxiliaryData value(*decodeCbor<cardano_auxiliary_data_t, cardano_auxiliary_data_unref>(cbor, cardano_auxiliary_data_from_cbor));

  value.validate();

  return value;
}

AuxiliaryData
AuxiliaryData::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::optional<TransactionMetadata>
AuxiliaryData::metadata() const
{
  cardano_transaction_metadata_t*                                                   value = cardano_auxiliary_data_get_transaction_metadata(checked());
  CardanoObject<cardano_transaction_metadata_t, cardano_transaction_metadata_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const TransactionMetadata data(*owner.release());

  return TransactionMetadata(data);
}

void
AuxiliaryData::setMetadata(const std::optional<TransactionMetadata>& value)
{
  if (!value)
  {
    checkCardanoError(cardano_auxiliary_data_set_transaction_metadata(checked(), nullptr), "AuxiliaryData setMetadata");
    normalizeEncoding();
    return;
  }

  TransactionMetadata copy(*value);

  checkCardanoError(cardano_auxiliary_data_set_transaction_metadata(checked(), copy.checked()), "AuxiliaryData setMetadata");
  normalizeEncoding();
}

std::optional<NativeScriptList>
AuxiliaryData::nativeScripts() const
{
  cardano_native_script_list_t*                                                 value = cardano_auxiliary_data_get_native_scripts(checked());
  CardanoObject<cardano_native_script_list_t, cardano_native_script_list_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const NativeScriptList data(*owner.release());

  return NativeScriptList(data);
}

void
AuxiliaryData::setNativeScripts(const std::optional<NativeScriptList>& value)
{
  if (!value)
  {
    checkCardanoError(cardano_auxiliary_data_set_native_scripts(checked(), nullptr), "AuxiliaryData setNativeScripts");
    normalizeEncoding();
    return;
  }

  NativeScriptList copy(*value);

  checkCardanoError(cardano_auxiliary_data_set_native_scripts(checked(), copy.checked()), "AuxiliaryData setNativeScripts");
  normalizeEncoding();
}

std::optional<PlutusV1ScriptList>
AuxiliaryData::plutusV1Scripts() const
{
  cardano_plutus_v1_script_list_t*                                                    value = cardano_auxiliary_data_get_plutus_v1_scripts(checked());
  CardanoObject<cardano_plutus_v1_script_list_t, cardano_plutus_v1_script_list_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const PlutusV1ScriptList data(*owner.release());

  return PlutusV1ScriptList(data);
}

void
AuxiliaryData::setPlutusV1Scripts(const std::optional<PlutusV1ScriptList>& value)
{
  if (!value)
  {
    checkCardanoError(cardano_auxiliary_data_set_plutus_v1_scripts(checked(), nullptr), "AuxiliaryData setPlutusV1Scripts");
    normalizeEncoding();
    return;
  }

  PlutusV1ScriptList copy(*value);

  checkCardanoError(cardano_auxiliary_data_set_plutus_v1_scripts(checked(), copy.checked()), "AuxiliaryData setPlutusV1Scripts");
  normalizeEncoding();
}

std::optional<PlutusV2ScriptList>
AuxiliaryData::plutusV2Scripts() const
{
  cardano_plutus_v2_script_list_t*                                                    value = cardano_auxiliary_data_get_plutus_v2_scripts(checked());
  CardanoObject<cardano_plutus_v2_script_list_t, cardano_plutus_v2_script_list_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const PlutusV2ScriptList data(*owner.release());

  return PlutusV2ScriptList(data);
}

void
AuxiliaryData::setPlutusV2Scripts(const std::optional<PlutusV2ScriptList>& value)
{
  if (!value)
  {
    checkCardanoError(cardano_auxiliary_data_set_plutus_v2_scripts(checked(), nullptr), "AuxiliaryData setPlutusV2Scripts");
    normalizeEncoding();
    return;
  }

  PlutusV2ScriptList copy(*value);

  checkCardanoError(cardano_auxiliary_data_set_plutus_v2_scripts(checked(), copy.checked()), "AuxiliaryData setPlutusV2Scripts");
  normalizeEncoding();
}

std::optional<PlutusV3ScriptList>
AuxiliaryData::plutusV3Scripts() const
{
  cardano_plutus_v3_script_list_t*                                                    value = cardano_auxiliary_data_get_plutus_v3_scripts(checked());
  CardanoObject<cardano_plutus_v3_script_list_t, cardano_plutus_v3_script_list_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const PlutusV3ScriptList data(*owner.release());

  return PlutusV3ScriptList(data);
}

void
AuxiliaryData::setPlutusV3Scripts(const std::optional<PlutusV3ScriptList>& value)
{
  if (!value)
  {
    checkCardanoError(cardano_auxiliary_data_set_plutus_v3_scripts(checked(), nullptr), "AuxiliaryData setPlutusV3Scripts");
    normalizeEncoding();
    return;
  }

  PlutusV3ScriptList copy(*value);

  checkCardanoError(cardano_auxiliary_data_set_plutus_v3_scripts(checked(), copy.checked()), "AuxiliaryData setPlutusV3Scripts");
  normalizeEncoding();
}

std::optional<PlutusV4ScriptList>
AuxiliaryData::plutusV4Scripts() const
{
  cardano_plutus_v4_script_list_t*                                                    value = cardano_auxiliary_data_get_plutus_v4_scripts(checked());
  CardanoObject<cardano_plutus_v4_script_list_t, cardano_plutus_v4_script_list_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const PlutusV4ScriptList data(*owner.release());

  return PlutusV4ScriptList(data);
}

void
AuxiliaryData::setPlutusV4Scripts(const std::optional<PlutusV4ScriptList>& value)
{
  if (!value)
  {
    checkCardanoError(cardano_auxiliary_data_set_plutus_v4_scripts(checked(), nullptr), "AuxiliaryData setPlutusV4Scripts");
    normalizeEncoding();
    return;
  }

  PlutusV4ScriptList copy(*value);

  checkCardanoError(cardano_auxiliary_data_set_plutus_v4_scripts(checked(), copy.checked()), "AuxiliaryData setPlutusV4Scripts");
  normalizeEncoding();
}

Blake2bHash
AuxiliaryData::hash() const
{
  return Blake2bHash::compute(toCbor());
}

void
AuxiliaryData::normalizeEncoding()
{
  cardano_auxiliary_data_clear_cbor_cache(checked());
}

std::vector<std::uint8_t>
AuxiliaryData::toCbor() const
{
  return encodeCbor(checked(), cardano_auxiliary_data_to_cbor);
}

std::string
AuxiliaryData::toJson() const
{
  return encodeJson(checked(), cardano_auxiliary_data_to_cip116_json);
}

void
AuxiliaryData::swap(AuxiliaryData& other) noexcept
{
  std::swap(m_value, other.m_value);
}

AuxiliaryData&
AuxiliaryData::operator=(const AuxiliaryData& other)
{
  if (this != &other)
  {
    AuxiliaryData copy(other);

    swap(copy);
  }

  return *this;
}

AuxiliaryData&
AuxiliaryData::operator=(AuxiliaryData&& other) noexcept
{
  if (this != &other)
  {
    cardano_auxiliary_data_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
AuxiliaryData::operator==(const AuxiliaryData& other) const
{
  return metadata() == other.metadata() &&
    nativeScripts() == other.nativeScripts() &&
    plutusV1Scripts() == other.plutusV1Scripts() &&
    plutusV2Scripts() == other.plutusV2Scripts() &&
    plutusV3Scripts() == other.plutusV3Scripts() &&
    plutusV4Scripts() == other.plutusV4Scripts();
}

bool
AuxiliaryData::operator!=(const AuxiliaryData& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

AuxiliaryData::AuxiliaryData(cardano_auxiliary_data_t& value) noexcept
: m_value(&value)
{
}

cardano_auxiliary_data_t*
AuxiliaryData::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from AuxiliaryData");
  }

  return m_value;
}

void
AuxiliaryData::validate() const
{
  const std::optional<TransactionMetadata> metadataValue = metadata();
  const std::optional<NativeScriptList>    scripts       = nativeScripts();

  if (metadataValue)
  {
    metadataValue->validate();
  }

  if (scripts)
  {
    scripts->validate();
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(AuxiliaryData& lhs, AuxiliaryData& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
