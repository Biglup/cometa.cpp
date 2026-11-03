/**
 * @file DefaultHttpTransport.h
 *
 * @brief Platform HTTPS transport.
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

#pragma once

/* INCLUDES ******************************************************************/

#if defined(_WIN32)
#include <Cometa/Providers/WinHttpTransport.h>
#else
#include <Cometa/Providers/CurlHttpTransport.h>
#endif

/* TYPE DECLARATION **********************************************************/

namespace Cometa
{
#if defined(_WIN32)
/** @brief Native Windows HTTPS transport. */
using DefaultHttpTransport = WinHttpTransport;
#else
/** @brief libcurl HTTPS transport. */
using DefaultHttpTransport = CurlHttpTransport;
#endif
} // namespace Cometa
