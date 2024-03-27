/***************************************************************************//**
 * @file stoneydsp_core.h
 * @author Nathan J. Hood <nathanjhood@googlemail.com>
 * @brief
 * @version 1.0.0
 * @date 2024-02-21
 *
 * @copyright Copyright (c) 2024
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ******************************************************************************/


/*******************************************************************************

 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 stoneydsp_core
  vendor:             stoneydsp
  version:            1.0.0
  name:               StoneyDSP core classes
  description:        The essential set of basic StoneyDSP classes, as required by all the other StoneyDSP modules.
  website:            http://stoneydsp.com
  license:            MIT
  minimumCppStandard: 17

  dependencies:       juce_core
  OSXFrameworks:      Cocoa Foundation IOKit Security
  iOSFrameworks:      Foundation
  linuxLibs:          rt dl pthread
  mingwLibs:          uuid wsock32 wininet version ole32 ws2_32 oleaut32 imm32 comdlg32 shlwapi rpcrt4 winmm

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once

#define STONEYDSP_CORE_H_INCLUDED

#include "system/stoneydsp_TargetPlatform.hpp" // system...
#include "system/stoneydsp_StandardHeader.hpp" // version...

/**
 * @brief The ```StoneyDSP``` namespace.
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @copyright Copyright (c) 2024
 * @namespace StoneyDSP
 *
 */
namespace StoneyDSP
{
/** @addtogroup StoneyDSP
 *  @{
 */

// 'using' is much better than a typedef here, because it
// doesn't require us to define operator overloads for everything...
// using int32_t  = signed int;
// using uint8_t  = unsigned char;
// using uint32_t = unsigned int;
// using int64_t  = long long int;

/** A platform-independent 8-bit signed integer type. */
using int8      = signed char;
/** A platform-independent 8-bit unsigned integer type. */
using uint8     = unsigned char;
/** A platform-independent 16-bit signed integer type. */
using int16     = signed short;
/** A platform-independent 16-bit unsigned integer type. */
using uint16    = unsigned short;
/** A platform-independent 32-bit signed integer type. */
using int32     = signed int;
/** A platform-independent 32-bit unsigned integer type. */
using uint32    = unsigned int;

#if STONEYDSP_MSVC
  /** A platform-independent 64-bit integer type. */
  using int64  = __int64;
  /** A platform-independent 64-bit unsigned integer type. */
  using uint64 = unsigned __int64;
#else
  /** A platform-independent 64-bit integer type. */
  using int64  = long long;
  /** A platform-independent 64-bit unsigned integer type. */
  using uint64 = unsigned long long;
#endif

/**
 * @brief Handy function for avoiding unused variables warning.
 *
*/
template <typename... Types>
void ignoreUnused (Types&&...) noexcept {}

/**
 * @brief The SystemStats class.
 *
 */
class SystemStats  final
{
public:
    /**
     * @brief Returns the current version of ```STONEYDSP```.
     * See also the ```STONEYDSP_VERSION```, ```STONEYDSP_MAJOR_VERSION``` and
     * ```STONEYDSP_MINOR_VERSION``` macros.
     *
    */
    static std::string_view getStoneyDSPVersion();
private:
    SystemStats() = delete; // Only static methods!
    STONEYDSP_DECLARE_NON_COPYABLE (SystemStats)
};


  /// @} group StoneyDSP
} // namespace StoneyDSP

#ifndef STONEYDSP_STRINGIFY
# define STONEYDSP_STRINGIFY_HELPER(n) #n
# define STONEYDSP_STRINGIFY(n) STONEYDSP_STRINGIFY_HELPER(n)
#endif

#include "maths/stoneydsp_MathsIConstants.hpp"
#include "maths/stoneydsp_MathsIFunctions.hpp"
#include "maths/stoneydsp_MathsConstants.hpp"
#include "maths/stoneydsp_MathsFunctions.hpp"
#include "maths/stoneydsp_Coefficient.hpp"

#include "application/stoneydsp_Application.hpp"
#include "application/stoneydsp_ConsoleApplication.hpp"

namespace StoneyDSP
{
/** @addtogroup StoneyDSP
 *  @{
 */

/**
 * @brief The ```StoneyDSP::Core``` namespace.
 *
 */
namespace Core
{
/** @addtogroup Core
 *  @{
 */

  /// @} group Core
} // namespace Core

  /// @} group StoneyDSP
} // namespace StoneyDSP
