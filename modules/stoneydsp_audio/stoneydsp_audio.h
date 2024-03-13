/***************************************************************************//**
 * @file stoneydsp_audio.h
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

  ID:                 stoneydsp_audio
  vendor:             stoneydsp
  version:            1.0.0
  name:               StoneyDSP audio classes
  description:        The StoneyDSP Audio modules.
  website:            http://stoneydsp.com
  license:            MIT
  minimumCppStandard: 17

  dependencies:       juce_dsp, stoneydsp_core

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once

#define STONEYDSP_AUDIO_H_INCLUDED

#include <juce_dsp/juce_dsp.h>
#include <stoneydsp_core/stoneydsp_core.h>

namespace StoneyDSP
{
/**
 * @brief The ```StoneyDSP::Audio``` namespace.
 *
 */
namespace Audio
{

} // namespace Audio
} // namespace StoneyDSP

#include "filter/stoneydsp_FilterTypes.hpp"
#include "filter/stoneydsp_TransformationTypes.hpp"
#include "filter/stoneydsp_Biquads.hpp"
