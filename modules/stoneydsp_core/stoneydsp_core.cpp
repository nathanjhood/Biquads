/***************************************************************************//**
 * @file stoneydsp_core.cpp
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

#ifdef STONEYDSP_CORE_H_INCLUDED
 #error "Incorrect usage of 'stoneydsp_core.cpp'!"
#endif

#include "stoneydsp_core.h"

namespace StoneyDSP
{

std::string_view SystemStats::getStoneyDSPVersion()
{
    return "StoneyDSP v" STONEYDSP_STRINGIFY (STONEYDSP_MAJOR_VERSION)
                "." STONEYDSP_STRINGIFY (STONEYDSP_MINOR_VERSION)
                "." STONEYDSP_STRINGIFY (STONEYDSP_BUILDNUMBER);
}

} // namespace StoneyDSP

#include "maths/stoneydsp_Coefficient.cpp"