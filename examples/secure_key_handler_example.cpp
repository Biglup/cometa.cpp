/**
 * @file secure_key_handler_example.cpp
 *
 * @brief Restores a software signer and signs a sample transaction.
 *
 * @author angel.castillo
 * @date   Nov 03, 2026
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

#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

/* CONSTANTS *****************************************************************/

namespace
{
const std::string_view SERIALIZED_BIP32_KEY_HANDLER = "0a0a0a0a01010000005c97db5e09b3a4919ec75ed1126056241a1e5278731c2e0b01bea0a5f42c22db4131e0a4bbe75633677eb0e60e2ecd3520178f85c7e0d4be77a449087fe9674ee52f946b07c1b56d228c496ec0d36dd44212ba8af0f6eed1a82194dd69f479c603";
const std::string_view TX_CBOR                      = "84a40081825820f6dd880fb30480aa43117c73bfd09442ba30de5644c3ec1a91d9232fbe715aab000182a20058390071213dc119131f48f54d62e339053388d9d84faedecba9d8722ad2cad9debf34071615fc6452dfc743a4963f6bec68e488001c7384942c13011b0000000253c8e4f6a300581d702ed2631dbb277c84334453c5c437b86325d371f0835a28b910a91a6e011a001e848002820058209d7fee57d1dbb9b000b2a133256af0f2c83ffe638df523b2d1c13d405356d8ae021a0002fb050b582088e4779d217d10398a705530f9fb2af53ffac20aef6e75e85c26e93a00877556a10481d8799fd8799f40ffd8799fa1d8799fd8799fd87980d8799fd8799f581c71213dc119131f48f54d62e339053388d9d84faedecba9d8722ad2caffd8799fd8799fd8799f581cd9debf34071615fc6452dfc743a4963f6bec68e488001c7384942c13ffffffffffd8799f4040ffff1a001e8480a0a000ffd87c9f9fd8799fd8799fd8799fd87980d8799fd8799f581caa47de0ab3b7f0b1d8d196406b6af1b0d88cd46168c49ca0557b4f70ffd8799fd8799fd8799f581cd4b8fc88aec1d1c2f43ca5587898d88da20ef73964b8cf6f8f08ddfbffffffffffd8799fd87980d8799fd8799f581caa47de0ab3b7f0b1d8d196406b6af1b0d88cd46168c49ca0557b4f70ffd8799fd8799fd8799f581cd4b8fc88aec1d1c2f43ca5587898d88da20ef73964b8cf6f8f08ddfbffffffffffd8799f4040ffd87a9f1a00989680ffffd87c9f9fd8799fd87a9fd8799f4752656c65617365d8799fd87980d8799fd8799f581caa47de0ab3b7f0b1d8d196406b6af1b0d88cd46168c49ca0557b4f70ffd8799fd8799fd8799f581cd4b8fc88aec1d1c2f43ca5587898d88da20ef73964b8cf6f8f08ddfbffffffffffff9fd8799f0101ffffffd87c9f9fd8799fd87b9fd9050280ffd87980ffff1b000001884e1fb1c0d87980ffffff1b000001884e1fb1c0d87980ffffff1b000001884e1fb1c0d87980fffff5f6";
const std::string      PASSWORD                     = "password";

/** @brief Decodes a hexadecimal fixture. @param hex The encoded bytes. @return The decoded bytes. */
std::vector<std::uint8_t>
hexBytes(std::string_view hex)
{
  if (hex.size() % 2 != 0)
  {
    throw std::invalid_argument("Hex fixture has an odd number of digits");
  }

  std::vector<std::uint8_t> bytes;
  bytes.reserve(hex.size() / 2);

  for (std::size_t index = 0; index < hex.size(); index += 2)
  {
    const unsigned long value = std::stoul(std::string(hex.substr(index, 2)), nullptr, 16);
    bytes.push_back(static_cast<std::uint8_t>(value));
  }

  return bytes;
}
} // namespace

/* MAIN **********************************************************************/

int
main()
{
  try
  {
    std::cout << "Cardano secure key handler Example\n";
    const std::vector<std::uint8_t> password(PASSWORD.begin(), PASSWORD.end());
    const Cometa::SecureKeyHandler  handler = Cometa::SecureKeyHandler::deserializeSoftware(
      hexBytes(SERIALIZED_BIP32_KEY_HANDLER), [password]()
      { return password; });

    const Cometa::Bip32PublicKey account = handler.accountPublicKey(Cometa::AccountDerivationPath {});
    std::cout << "Extended account public key: " << account.toHex() << '\n';

    const Cometa::Transaction    transaction = Cometa::Transaction::fromCbor(TX_CBOR);
    const Cometa::VkeyWitnessSet witnesses   = handler.signTransaction(transaction, { Cometa::DerivationPath {} });
    const Cometa::VkeyWitness    witness     = witnesses.at(0);

    std::cout << "Signature: " << witness.signature().toHex() << '\n';
    std::cout << "Public key: " << witness.publicKey().toHex() << '\n';
    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Secure key handler example failed: " << error.what() << '\n';
    return 1;
  }
}
