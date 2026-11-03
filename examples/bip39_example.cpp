/**
 * @file bip39_example.cpp
 *
 * @brief Restores a wallet from BIP39 words and derives its first address.
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

#include <exception>
#include <iostream>
#include <string>
#include <vector>

/* CONSTANTS *****************************************************************/

namespace
{
const std::vector<std::string> MNEMONIC_WORDS {
  "antenna",
  "whale",
  "clutch",
  "cushion",
  "narrow",
  "chronic",
  "matrix",
  "alarm",
  "raise",
  "much",
  "stove",
  "beach",
  "mimic",
  "daughter",
  "review",
  "build",
  "dinner",
  "twelve",
  "orbit",
  "soap",
  "decorate",
  "bachelor",
  "athlete",
  "close"
};
const std::string PASSWORD = "password";
} // namespace

/* MAIN **********************************************************************/

int
main()
{
  try
  {
    std::cout << "Cardano BIP-039 Example\n";
    std::cout << "Converting mnemonic words to entropy...\n";

    const std::vector<std::uint8_t> entropy = Cometa::mnemonicToEntropy(MNEMONIC_WORDS);
    const std::vector<std::uint8_t> password(PASSWORD.begin(), PASSWORD.end());
    const Cometa::SecureKeyHandler  handler = Cometa::SecureKeyHandler::softwareBip32(
      entropy, password, [password]()
      { return password; });

    const Cometa::Bip32PublicKey   account    = handler.accountPublicKey(Cometa::AccountDerivationPath {});
    const Cometa::Ed25519PublicKey paymentKey = account.derive({ 0, 0 }).toEd25519();
    const Cometa::Ed25519PublicKey stakeKey   = account.derive({ 2, 0 }).toEd25519();
    const Cometa::Credential       payment(Cometa::CredentialType::KeyHash, paymentKey.hash());
    const Cometa::Credential       stake(Cometa::CredentialType::KeyHash, stakeKey.hash());
    const Cometa::Address          address = Cometa::BaseAddress(Cometa::NetworkId::Testnet, payment, stake).toAddress();

    std::cout << "First address: " << address.toString() << '\n';

    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "BIP39 example failed: " << error.what() << '\n';
    return 1;
  }
}
