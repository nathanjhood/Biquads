/***************************************************************************//**
 * @file Editor.hpp
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
#define STONEYDSP_BIQUADS_EDITOR_HPP_INCLUDED

// #include "../Biquads.hpp"

// #ifndef STONEYDSP_BIQUADS_HPP_INCLUDED
// #include <juce_audio_basics/juce_audio_basics.h>
// #include <juce_audio_devices/juce_audio_devices.h>
// #include <juce_audio_formats/juce_audio_formats.h>
// #include <juce_audio_plugin_client/juce_audio_plugin_client.h>
// #include <juce_audio_processors/juce_audio_processors.h>
// #include <juce_audio_utils/juce_audio_utils.h>
// #include <juce_core/juce_core.h>
// #include <juce_data_structures/juce_data_structures.h>
// #include <juce_dsp/juce_dsp.h>
// #include <juce_events/juce_events.h>
// #include <juce_graphics/juce_graphics.h>
// #include <juce_gui_basics/juce_gui_basics.h>
// #include <juce_gui_extra/juce_gui_extra.h>
// #endif

namespace StoneyDSP {
/** @addtogroup StoneyDSP @{ */

namespace Biquads {
/** @addtogroup Biquads @{ */

// class AudioPluginAudioProcessor;

//==============================================================================
class JUCE_API AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& audioProcessor;

    juce::UndoManager& undoManager;

    juce::ArrowButton undoButton { "Undo", 0.5f , juce::Colours::white };
    juce::ArrowButton redoButton { "Redo", 0.0f , juce::Colours::white };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP
