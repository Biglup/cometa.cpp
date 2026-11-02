/**
 * @file WitnessSet.h
 *
 * @brief Transaction witnesses for every supported script language.
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

#pragma once

/* INCLUDES ******************************************************************/

#include <Cometa/Scripts/Script.h>
#include <Cometa/WitnessSet/BootstrapWitnessSet.h>
#include <Cometa/WitnessSet/NativeScriptSet.h>
#include <Cometa/WitnessSet/PlutusDataSet.h>
#include <Cometa/WitnessSet/PlutusV1ScriptSet.h>
#include <Cometa/WitnessSet/PlutusV2ScriptSet.h>
#include <Cometa/WitnessSet/PlutusV3ScriptSet.h>
#include <Cometa/WitnessSet/PlutusV4ScriptSet.h>
#include <Cometa/WitnessSet/RedeemerList.h>
#include <Cometa/WitnessSet/VkeyWitnessSet.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_witness_set_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief A transaction's signatures, scripts, data, and redeemers. */
class WitnessSet
{
  public:

    /** @brief Creates an empty witness set. */
    WitnessSet();

    /**
     * @brief Copies all witnesses.
     * @param other The witnesses to copy.
     */
    WitnessSet(const WitnessSet& other);

    /**
     * @brief Moves witnesses.
     * @param other The witnesses to move.
     */
    WitnessSet(WitnessSet&& other) noexcept;

    /** @brief Destroys the witness set. */
    ~WitnessSet();

  public:

    /**
     * @brief Decodes one CBOR witness set.
     * @param cbor The encoded witness set.
     * @return The decoded witness set.
     */
    static WitnessSet fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded witness set.
     */
    static WitnessSet fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the vkeys field.
     * @return An independent value when present.
     */
    std::optional<VkeyWitnessSet> vkeys() const;

    /**
     * @brief Sets or clears the vkeys field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setVkeys(const std::optional<VkeyWitnessSet>& value);

    /**
     * @brief Gets the bootstrap field.
     * @return An independent value when present.
     */
    std::optional<BootstrapWitnessSet> bootstrap() const;

    /**
     * @brief Sets or clears the bootstrap field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setBootstrap(const std::optional<BootstrapWitnessSet>& value);

    /**
     * @brief Gets the nativeScripts field.
     * @return An independent value when present.
     */
    std::optional<NativeScriptSet> nativeScripts() const;

    /**
     * @brief Sets or clears the nativeScripts field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setNativeScripts(const std::optional<NativeScriptSet>& value);

    /**
     * @brief Gets the plutusV1Scripts field.
     * @return An independent value when present.
     */
    std::optional<PlutusV1ScriptSet> plutusV1Scripts() const;

    /**
     * @brief Sets or clears the plutusV1Scripts field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setPlutusV1Scripts(const std::optional<PlutusV1ScriptSet>& value);

    /**
     * @brief Gets the plutusV2Scripts field.
     * @return An independent value when present.
     */
    std::optional<PlutusV2ScriptSet> plutusV2Scripts() const;

    /**
     * @brief Sets or clears the plutusV2Scripts field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setPlutusV2Scripts(const std::optional<PlutusV2ScriptSet>& value);

    /**
     * @brief Gets the plutusV3Scripts field.
     * @return An independent value when present.
     */
    std::optional<PlutusV3ScriptSet> plutusV3Scripts() const;

    /**
     * @brief Sets or clears the plutusV3Scripts field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setPlutusV3Scripts(const std::optional<PlutusV3ScriptSet>& value);

    /**
     * @brief Gets the plutusV4Scripts field.
     * @return An independent value when present.
     */
    std::optional<PlutusV4ScriptSet> plutusV4Scripts() const;

    /**
     * @brief Sets or clears the plutusV4Scripts field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setPlutusV4Scripts(const std::optional<PlutusV4ScriptSet>& value);

    /**
     * @brief Gets the plutusData field.
     * @return An independent value when present.
     */
    std::optional<PlutusDataSet> plutusData() const;

    /**
     * @brief Sets or clears the plutusData field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setPlutusData(const std::optional<PlutusDataSet>& value);

    /**
     * @brief Gets the redeemers field.
     * @return An independent value when present.
     */
    std::optional<RedeemerList> redeemers() const;

    /**
     * @brief Sets or clears the redeemers field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setRedeemers(const std::optional<RedeemerList>& value);

    /**
     * @brief Adds a script to the appropriate language field.
     * @param script The script to copy.
     */
    void addScript(const Script& script);

    /**
     * @brief Encodes the witness set as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the witness set as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges witness sets.
     * @param other The witness set to exchange.
     */
    void swap(WitnessSet& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The witness set to copy.
     * @return This witness set.
     */
    WitnessSet& operator=(const WitnessSet& other);

    /**
     * @brief Assigns a moved witness set.
     * @param other The witness set to move.
     * @return This witness set.
     */
    WitnessSet& operator=(WitnessSet&& other) noexcept;

    /**
     * @brief Compares encoded witnesses.
     * @param other The witness set to compare.
     * @return True when equal.
     */
    bool operator==(const WitnessSet& other) const;

    /**
     * @brief Compares encoded witnesses.
     * @param other The witness set to compare.
     * @return True when different.
     */
    bool operator!=(const WitnessSet& other) const;

  private:

    /** @brief Allows sub-transactions to copy this value. */
    friend class SubTransaction;

    /**
     * @brief Adopts an owned witness set.
     * @param value The owned witness set.
     */
    explicit WitnessSet(cardano_witness_set_t* value) noexcept;

    /**
     * @brief Gets the stored witness set.
     * @return The borrowed witness set.
     */
    cardano_witness_set_t* checked() const;

  private:

    /** @brief The owned witness set. */
    cardano_witness_set_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges witness sets through argument-dependent lookup.
 * @param lhs The first witness set.
 * @param rhs The second witness set.
 */
void swap(WitnessSet& lhs, WitnessSet& rhs) noexcept;
} // namespace Cometa
