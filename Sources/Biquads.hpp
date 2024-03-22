/***************************************************************************//**
 * @file Biquads.hpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief Simple two-pole equalizer with variable oversampling.
 * @version 1.2.3.155
 * @date 2024-03-16
 *
 * @copyright Copyright (c) 2024 - Nathan J. Hood

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

 ******************************************************************************/

#pragma once
#define STONEYDSP_BIQUADS_HPP_INCLUDED

#if __has_include("AppConfig.h")
 #include "AppConfig.h"
#endif

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include <stoneydsp_audio/stoneydsp_audio.h>
#include <stoneydsp_core/stoneydsp_core.h>


#if JUCE_TARGET_HAS_BINARY_DATA
 #include "BinaryData.h"
#endif

namespace StoneyDSP
{
/** @addtogroup StoneyDSP
 *  @{
 */

/**
 * @brief The ```StoneyDSP::Biquads``` namespace.
 *
 */
namespace Biquads
{

class AudioPluginAudioProcessor;

/** @addtogroup Biquads
 *  @{
 */

namespace ProjectInfo
{
    extern const char* const  projectName;
    extern const char* const  companyName;
    extern const char* const  versionString;
    extern const int          versionNumber;
}

/** Will need these later... */
template <typename FloatType>
struct Cartesian
{
  inline Cartesian(FloatType x, FloatType y) : _x(x), _y(y) {}
private:
  FloatType _x, _y;
};
template struct Cartesian<float>;
template struct Cartesian<double>;

template <typename FloatType>
struct Polar
{
  inline Polar(FloatType angle, FloatType magnitude) : _angle(angle), _magnitude(magnitude) {}
private:
  FloatType _angle, _magnitude;
};
template struct Polar<float>;
template struct Polar<double>;

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP

#include "Parameters/Parameters.hpp"
#include "Wrapper/Wrapper.hpp"
#include "Processor/Processor.hpp"
#include "Editor/Editor.hpp"
