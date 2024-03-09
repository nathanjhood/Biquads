/***************************************************************************//**
 * @file stoneydsp_StandardHeader.hpp
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

#pragma once

//==============================================================================
/** Current StoneyDSP version number.

    See also StoneyDSP::SystemStats::getStoneyDSPVersion() for a string version.
*/
#define STONEYDSP_MAJOR_VERSION      0
#define STONEYDSP_MINOR_VERSION      0
#define STONEYDSP_BUILDNUMBER        1

/** Current StoneyDSP version number.

    Bits 16 to 32 = major version.
    Bits 8 to 16 = minor version.
    Bits 0 to 8 = point release.

    See also StoneyDSP::SystemStats::getStoneyDSPVersion() for a string version.
*/
#define STONEYDSP_VERSION (                                                    \
    (STONEYDSP_MAJOR_VERSION << 16) +                                          \
    (STONEYDSP_MINOR_VERSION << 8 ) +                                          \
     STONEYDSP_BUILDNUMBER)

// Standard includes

#include <atomic>

#include <stdexcept>
#include <vector>

#include <iostream>
#include <iomanip>

#include <filesystem>
#include <fstream>

#include <string>
#include <sstream>

#include <climits> // INT_MAX

// Platform defs...

// #ifdef JUCE_CALLTYPE
//  #define STONEYDSP_CALLTYPE JUCE_CALLTYPE
// #endif
// #ifdef JUCE_CDECL
//  #define STONEYDSP_CDECL JUCE_CDECL
// #endif

/**
 * This macro defines the C calling convention used as the standard for
 * STONEYDSP calls.
 */
#if STONEYDSP_WINDOWS
 #define STONEYDSP_CALLTYPE   __stdcall
 #define STONEYDSP_CDECL      __cdecl
#else
 #define STONEYDSP_CALLTYPE
 #define STONEYDSP_CDECL
#endif

/**
 * @brief This is a shorthand macro for deleting a class's copy constructor and
 * copy assignment operator.
 *
*/
#define STONEYDSP_DECLARE_NON_COPYABLE(className) \
    className (const className&) = delete;\
    className& operator= (const className&) = delete;

/**
 * @brief This is a shorthand macro for deleting a class's move constructor and
 * move assignment operator.
 *
 */
#define STONEYDSP_DECLARE_NON_MOVEABLE(className) \
    className (className&&) = delete;\
    className& operator= (className&&) = delete;

/**
 * @brief This macro can be added to class definitions to disable the use of
 * new/delete to allocate the object on the heap, forcing it to only be used as
 * a stack or member variable.
 *
*/
#define STONEYDSP_PREVENT_HEAP_ALLOCATION \
   private: \
    static void* operator new (size_t) = delete; \
    static void operator delete (void*) = delete;
