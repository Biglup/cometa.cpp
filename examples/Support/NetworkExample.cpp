/**
 * @file NetworkExample.cpp
 *
 * @brief Shared setup for the preprod transaction examples.
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

#include "NetworkExample.h"

#include <Cometa/Address/BaseAddress.h>
#include <Cometa/Common/Credential.h>
#include <Cometa/Crypto/Bip32PublicKey.h>
#include <Cometa/KeyHandlers/AccountDerivationPath.h>
#include <Cometa/KeyHandlers/DerivationPath.h>
#include <Cometa/Providers/CurlHttpTransport.h>
#include <Cometa/SlotConfig.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Encrypted signer fixture used by the native examples. */
const std::string SERIALIZED_SIGNER = "0a0a0a0a01010000005c97db5e09b3a4919ec75ed1126056241a1e5278731c2e0b01bea0a5f42c22db4131e0a4bbe75633677eb0e60e2ecd3520178f85c7e0d4be77a449087fe9674ee52f946b07c1b56d228c496ec0d36dd44212ba8af0f6eed1a82194dd69f479c603";

/** @brief Decodes a hex fixture. @param hex The text. @return The bytes. */
std::vector<std::uint8_t>
fromHex(const std::string& hex)
{
  std::vector<std::uint8_t> bytes;
  bytes.reserve(hex.size() / 2);

  for (std::size_t index = 0; index < hex.size(); index += 2)
  {
    bytes.push_back(static_cast<std::uint8_t>(std::stoul(hex.substr(index, 2), nullptr, 16)));
  }

  return bytes;
}

/** @brief Supplies the example fixture passphrase. @return The bytes. */
std::vector<std::uint8_t>
samplePassphrase()
{
  const std::string password = "password";
  return std::vector<std::uint8_t>(password.begin(), password.end());
}
} // namespace

/* IMPLEMENTATION ************************************************************/

SecureKeyHandler
CometaExamples::sampleSigner()
{
  return SecureKeyHandler::deserializeSoftware(fromHex(SERIALIZED_SIGNER), &samplePassphrase);
}

Address
CometaExamples::paymentAddress(const SecureKeyHandler& signer)
{
  const Bip32PublicKey account = signer.accountPublicKey(AccountDerivationPath {});
  const Credential     payment(CredentialType::KeyHash, account.derive({ 0, 0 }).toEd25519().hash());
  const Credential     stake(CredentialType::KeyHash, account.derive({ 2, 0 }).toEd25519().hash());
  return BaseAddress(NetworkId::Testnet, payment, stake).toAddress();
}

BlockfrostProvider
CometaExamples::preprodProvider()
{
  const char* projectId = std::getenv("BLOCKFROST_API_KEY");

  if (projectId == nullptr || *projectId == '\0')
  {
    throw std::runtime_error("Set BLOCKFROST_API_KEY to a preprod project ID");
  }

  return BlockfrostProvider(NetworkMagic::Preprod, projectId, std::make_shared<CurlHttpTransport>());
}

std::uint64_t
CometaExamples::expirySlot()
{
  const std::chrono::system_clock::time_point expiry =
    std::chrono::system_clock::now() + std::chrono::hours(2);
  const std::uint64_t unixTime = static_cast<std::uint64_t>(
    std::chrono::duration_cast<std::chrono::seconds>(expiry.time_since_epoch()).count());
  return SlotConfig::forNetwork(NetworkMagic::Preprod).slotFromUnixTime(unixTime);
}

void
CometaExamples::signAndSubmit(
  const SecureKeyHandler&   signer,
  const BlockfrostProvider& provider,
  Transaction&              transaction)
{
  transaction.applyVkeyWitnesses(signer.signTransaction(transaction, { DerivationPath {} }));

  const Blake2bHash id = provider.submitTransaction(transaction);
  std::cout << "Submitted transaction: " << id.toHex() << '\n';

  if (!provider.confirmTransaction(id, std::chrono::minutes(4)))
  {
    throw std::runtime_error("Transaction was not confirmed within four minutes");
  }

  std::cout << "Confirmed transaction: " << id.toHex() << '\n';
}
