/**
 * @file WitnessSet.cpp
 *
 * @brief Transaction witness sets.
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

#include <Cometa/WitnessSet/WitnessSet.h>

#include "../CardanoInterop.h"

#include <cardano/witness_set/witness_set.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
WitnessSet::WitnessSet()
: m_value(nullptr)
{
  cardano_witness_set_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_witness_set_new(&value);
  CardanoObject<cardano_witness_set_t, cardano_witness_set_unref> owner(value);

  checkCardanoError(status, "WitnessSet");
  m_value = owner.release();
}

WitnessSet::WitnessSet(const WitnessSet& other)
: m_value(nullptr)
{
  cardano_witness_set_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_witness_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_witness_set_t, cardano_witness_set_unref> owner(value);

  checkCardanoError(status, "WitnessSet copy");
  m_value = owner.release();
}

WitnessSet::WitnessSet(WitnessSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

WitnessSet::~WitnessSet()
{
  cardano_witness_set_unref(&m_value);
}

WitnessSet
WitnessSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return WitnessSet(decodeCbor<cardano_witness_set_t, cardano_witness_set_unref>(cbor, cardano_witness_set_from_cbor));
}

WitnessSet
WitnessSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::optional<VkeyWitnessSet>
WitnessSet::vkeys() const
{
  cardano_vkey_witness_set_t* value = cardano_witness_set_get_vkeys(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return VkeyWitnessSet::adopt(value);
}

void
WitnessSet::setVkeys(const std::optional<VkeyWitnessSet>& value)
{
  if (value)
  {
    const VkeyWitnessSet copy(*value);
    checkCardanoError(cardano_witness_set_set_vkeys(checked(), checkedNativeHandle(copy)), "WitnessSet setVkeys");
  }
  else
  {
    checkCardanoError(cardano_witness_set_set_vkeys(checked(), nullptr), "WitnessSet setVkeys");
  }

  cardano_witness_set_clear_cbor_cache(checked());
}

std::optional<BootstrapWitnessSet>
WitnessSet::bootstrap() const
{
  cardano_bootstrap_witness_set_t* value = cardano_witness_set_get_bootstrap(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return BootstrapWitnessSet::adopt(value);
}

void
WitnessSet::setBootstrap(const std::optional<BootstrapWitnessSet>& value)
{
  if (value)
  {
    const BootstrapWitnessSet copy(*value);
    checkCardanoError(cardano_witness_set_set_bootstrap(checked(), checkedNativeHandle(copy)), "WitnessSet setBootstrap");
  }
  else
  {
    checkCardanoError(cardano_witness_set_set_bootstrap(checked(), nullptr), "WitnessSet setBootstrap");
  }

  cardano_witness_set_clear_cbor_cache(checked());
}

std::optional<NativeScriptSet>
WitnessSet::nativeScripts() const
{
  cardano_native_script_set_t* value = cardano_witness_set_get_native_scripts(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return NativeScriptSet::adopt(value);
}

void
WitnessSet::setNativeScripts(const std::optional<NativeScriptSet>& value)
{
  if (value)
  {
    const NativeScriptSet copy(*value);
    checkCardanoError(cardano_witness_set_set_native_scripts(checked(), checkedNativeHandle(copy)), "WitnessSet setNativeScripts");
  }
  else
  {
    checkCardanoError(cardano_witness_set_set_native_scripts(checked(), nullptr), "WitnessSet setNativeScripts");
  }

  cardano_witness_set_clear_cbor_cache(checked());
}

std::optional<PlutusV1ScriptSet>
WitnessSet::plutusV1Scripts() const
{
  cardano_plutus_v1_script_set_t* value = cardano_witness_set_get_plutus_v1_scripts(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return PlutusV1ScriptSet::adopt(value);
}

void
WitnessSet::setPlutusV1Scripts(const std::optional<PlutusV1ScriptSet>& value)
{
  if (value)
  {
    const PlutusV1ScriptSet copy(*value);
    checkCardanoError(cardano_witness_set_set_plutus_v1_scripts(checked(), checkedNativeHandle(copy)), "WitnessSet setPlutusV1Scripts");
  }
  else
  {
    checkCardanoError(cardano_witness_set_set_plutus_v1_scripts(checked(), nullptr), "WitnessSet setPlutusV1Scripts");
  }

  cardano_witness_set_clear_cbor_cache(checked());
}

std::optional<PlutusV2ScriptSet>
WitnessSet::plutusV2Scripts() const
{
  cardano_plutus_v2_script_set_t* value = cardano_witness_set_get_plutus_v2_scripts(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return PlutusV2ScriptSet::adopt(value);
}

void
WitnessSet::setPlutusV2Scripts(const std::optional<PlutusV2ScriptSet>& value)
{
  if (value)
  {
    const PlutusV2ScriptSet copy(*value);
    checkCardanoError(cardano_witness_set_set_plutus_v2_scripts(checked(), checkedNativeHandle(copy)), "WitnessSet setPlutusV2Scripts");
  }
  else
  {
    checkCardanoError(cardano_witness_set_set_plutus_v2_scripts(checked(), nullptr), "WitnessSet setPlutusV2Scripts");
  }

  cardano_witness_set_clear_cbor_cache(checked());
}

std::optional<PlutusV3ScriptSet>
WitnessSet::plutusV3Scripts() const
{
  cardano_plutus_v3_script_set_t* value = cardano_witness_set_get_plutus_v3_scripts(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return PlutusV3ScriptSet::adopt(value);
}

void
WitnessSet::setPlutusV3Scripts(const std::optional<PlutusV3ScriptSet>& value)
{
  if (value)
  {
    const PlutusV3ScriptSet copy(*value);
    checkCardanoError(cardano_witness_set_set_plutus_v3_scripts(checked(), checkedNativeHandle(copy)), "WitnessSet setPlutusV3Scripts");
  }
  else
  {
    checkCardanoError(cardano_witness_set_set_plutus_v3_scripts(checked(), nullptr), "WitnessSet setPlutusV3Scripts");
  }

  cardano_witness_set_clear_cbor_cache(checked());
}

std::optional<PlutusV4ScriptSet>
WitnessSet::plutusV4Scripts() const
{
  cardano_plutus_v4_script_set_t* value = cardano_witness_set_get_plutus_v4_scripts(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return PlutusV4ScriptSet::adopt(value);
}

void
WitnessSet::setPlutusV4Scripts(const std::optional<PlutusV4ScriptSet>& value)
{
  if (value)
  {
    const PlutusV4ScriptSet copy(*value);
    checkCardanoError(cardano_witness_set_set_plutus_v4_scripts(checked(), checkedNativeHandle(copy)), "WitnessSet setPlutusV4Scripts");
  }
  else
  {
    checkCardanoError(cardano_witness_set_set_plutus_v4_scripts(checked(), nullptr), "WitnessSet setPlutusV4Scripts");
  }

  cardano_witness_set_clear_cbor_cache(checked());
}

std::optional<PlutusDataSet>
WitnessSet::plutusData() const
{
  cardano_plutus_data_set_t* value = cardano_witness_set_get_plutus_data(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return PlutusDataSet::adopt(value);
}

void
WitnessSet::setPlutusData(const std::optional<PlutusDataSet>& value)
{
  if (value)
  {
    const PlutusDataSet copy(*value);
    checkCardanoError(cardano_witness_set_set_plutus_data(checked(), checkedNativeHandle(copy)), "WitnessSet setPlutusData");
  }
  else
  {
    checkCardanoError(cardano_witness_set_set_plutus_data(checked(), nullptr), "WitnessSet setPlutusData");
  }

  cardano_witness_set_clear_cbor_cache(checked());
}

std::optional<RedeemerList>
WitnessSet::redeemers() const
{
  cardano_redeemer_list_t* value = cardano_witness_set_get_redeemers(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return RedeemerList::adopt(value);
}

void
WitnessSet::setRedeemers(const std::optional<RedeemerList>& value)
{
  if (value)
  {
    const RedeemerList copy(*value);
    checkCardanoError(cardano_witness_set_set_redeemers(checked(), checkedNativeHandle(copy)), "WitnessSet setRedeemers");
  }
  else
  {
    checkCardanoError(cardano_witness_set_set_redeemers(checked(), nullptr), "WitnessSet setRedeemers");
  }

  cardano_witness_set_clear_cbor_cache(checked());
}

void
WitnessSet::addScript(const Script& script)
{
  checkCardanoError(cardano_witness_set_add_script(checked(), checkedNativeHandle(script)), "WitnessSet addScript");
  cardano_witness_set_clear_cbor_cache(checked());
}

std::vector<std::uint8_t>
WitnessSet::toCbor() const
{
  return encodeCbor(checked(), cardano_witness_set_to_cbor);
}

std::string
WitnessSet::toJson() const
{
  return encodeJson(checked(), cardano_witness_set_to_cip116_json);
}

void
WitnessSet::swap(WitnessSet& other) noexcept
{
  std::swap(m_value, other.m_value);
}

WitnessSet&
WitnessSet::operator=(const WitnessSet& other)
{
  if (this != &other)
  {
    WitnessSet copy(other);
    swap(copy);
  }
  return *this;
}

WitnessSet&
WitnessSet::operator=(WitnessSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_witness_set_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
WitnessSet::operator==(const WitnessSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
WitnessSet::operator!=(const WitnessSet& other) const
{
  return !(*this == other);
}

WitnessSet
WitnessSet::adopt(cardano_witness_set_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null WitnessSet");
  }

  return WitnessSet(value);
}

cardano_witness_set_t*
WitnessSet::nativeHandle() noexcept
{
  return m_value;
}

const cardano_witness_set_t*
WitnessSet::nativeHandle() const noexcept
{
  return m_value;
}

cardano_witness_set_t*
WitnessSet::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

WitnessSet::WitnessSet(cardano_witness_set_t* value) noexcept
: m_value(value)
{
}

cardano_witness_set_t*
WitnessSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from WitnessSet");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(WitnessSet& lhs, WitnessSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
