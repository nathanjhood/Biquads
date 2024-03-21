/***************************************************************************//**
 * @file Parameters.hpp
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
#define STONEYDSP_BIQUADS_PARAMETERS_HPP_INCLUDED

#include <juce_audio_basics/juce_audio_basics.h>                                // depends: juce_core
#include <juce_audio_processors/juce_audio_processors.h>                        // depends: juce_gui_extra, juce_audio_basics
#include <juce_core/juce_core.h>                                                //
#include <juce_data_structures/juce_data_structures.h>                          // depends: juce_events
#include <juce_events/juce_events.h>                                            // depends: juce_core
#include <juce_graphics/juce_graphics.h>                                        // depends: juce_events
#include <juce_gui_basics/juce_gui_basics.h>                                    // depends: juce_graphics, juce_data_structures
#include <juce_gui_extra/juce_gui_extra.h>                                      // depends: juce_gui_basics

// #include "Biquads.hpp"

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
    AudioPluginAudioProcessorParameters(AudioPluginAudioProcessor& p);

    //==============================================================================
    juce::UndoManager& getUndoManager() { return undoManager; }
    //==============================================================================
    juce::AudioProcessorValueTreeState& getApvts() { return apvts; }

private:
    //==========================================================================
    /**
     * @brief Set the ```ParameterLayout``` object.
     *
     * @param params
     */
    static void setParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& params);

    /**
     * @brief Create a ```ParameterLayout``` object.
     *
     * @return juce::AudioProcessorValueTreeState::ParameterLayout
     */
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    /**
     * @brief Create a ```AudioProcessorParameterGroup``` object.
     *
     * @return juce::AudioProcessorParameterGroup
     */
    static juce::AudioProcessorParameterGroup createParameterGroup();

    //==========================================================================
    // This reference is provided as a quick way for the wrapper to
    // access the processor object that created it.
    AudioPluginAudioProcessor& audioProcessor;

    std::unique_ptr<juce::UndoManager> undoManagerPtr                           { nullptr };
    juce::UndoManager& undoManager;

    std::unique_ptr<juce::AudioProcessorValueTreeState> apvtsPtr                { nullptr };
    juce::AudioProcessorValueTreeState& apvts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorParameters)
};

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP
