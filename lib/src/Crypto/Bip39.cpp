/**
 * @file Bip39.cpp
 *
 * @brief English BIP39 mnemonic conversion.
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

#include <Cometa/Crypto/Bip39.h>

#include "../CardanoInterop.h"

#include <cardano/bip39.h>

#include <array>

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
std::vector<std::string>
entropyToMnemonic(const std::vector<std::uint8_t>& entropy)
{
  std::array<const char*, 24> words {};
  std::size_t                 count = 0;

  checkCardanoError(cardano_bip39_entropy_to_mnemonic_words(byteData(entropy), entropy.size(), words.data(), &count), "entropyToMnemonic");

  std::vector<std::string> result;
  result.reserve(count);

  for (std::size_t index = 0; index < count; ++index)
  {
    result.emplace_back(words[index]);
  }

  return result;
}

std::vector<std::uint8_t>
mnemonicToEntropy(const std::vector<std::string>& words)
{
  std::vector<const char*> pointers;
  pointers.reserve(words.size());

  for (const std::string& word: words)
  {
    if (word.find('\0') != std::string::npos)
    {
      throw std::invalid_argument("Mnemonic words cannot contain null characters");
    }

    pointers.push_back(word.c_str());
  }

  const char*               empty = "";
  std::vector<std::uint8_t> entropy(32);
  std::size_t               size = 0;

  checkCardanoError(cardano_bip39_mnemonic_words_to_entropy(pointers.empty() ? &empty : pointers.data(), pointers.size(), entropy.data(), entropy.size(), &size), "mnemonicToEntropy");

  entropy.resize(size);

  return entropy;
}
} // namespace Cometa
