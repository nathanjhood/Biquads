/***************************************************************************//**
 * @file Parameters.hpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief Simple two-pole equalizer with variable oversampling.
 * @version 1.2.2.151
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

#ifndef STONEYDSP_BIQUADS_PARAMETERS_HPP_INCLUDED
#define STONEYDSP_BIQUADS_PARAMETERS_HPP_INCLUDED

// #include "StoneyDSP/Biquads.hpp"

namespace StoneyDSP {
/** @addtogroup StoneyDSP @{ */

namespace Biquads {
/** @addtogroup Biquads @{ */

class AudioPluginAudioProcessor;

class AudioPluginAudioProcessorParameters
{
public:
    //==========================================================================
    /** Constructor. */
    AudioPluginAudioProcessorParameters(AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);

    //==========================================================================
    /** Create Parameter Layout. */
    static void setParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& params);


private:
    //==========================================================================
    // This reference is provided as a quick way for the wrapper to
    // access the processor object that created it.
    AudioPluginAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorParameters)
};

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP

#endif // STONEYDSP_BIQUADS_PARAMETERS_HPP_INCLUDED
