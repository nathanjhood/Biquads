/***************************************************************************//**
 * @file Processor.hpp
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
#define STONEYDSP_BIQUADS_PROCESSOR_HPP_INCLUDED

#include <juce_audio_basics/juce_audio_basics.h>                                // depends: juce_core
#include <juce_audio_formats/juce_audio_formats.h>                              // depends: juce_audio_basics
#include <juce_audio_processors/juce_audio_processors.h>                        // depends: juce_gui_extra, juce_audio_basics
#include <juce_core/juce_core.h>                                                //
#include <juce_data_structures/juce_data_structures.h>                          // depends: juce_events
#include <juce_dsp/juce_dsp.h>                                                  // depends: juce_audio_formats
#include <juce_events/juce_events.h>                                            // depends: juce_core
#include <juce_graphics/juce_graphics.h>                                        // depends: juce_events
#include <juce_gui_basics/juce_gui_basics.h>                                    // depends: juce_graphics, juce_data_structures
#include <juce_gui_extra/juce_gui_extra.h>                                      // depends: juce_gui_basics

// #include "Biquads.hpp"
// #include "Parameters.hpp"
// #include "Wrapper.hpp"

namespace StoneyDSP {
/** @addtogroup StoneyDSP @{ */

namespace Biquads {
/** @addtogroup Biquads @{ */

class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;
    //==============================================================================
    juce::AudioProcessorParameter* getBypassParameter() const override;
    bool isBypassed() const noexcept;
    void setBypassParameter(juce::AudioParameterBool* newBypass) noexcept;
    //==============================================================================
    bool supportsDoublePrecisionProcessing() const override;
    juce::AudioProcessor::ProcessingPrecision getProcessingPrecision() const noexcept;
    bool isUsingDoublePrecision() const noexcept;
    void setProcessingPrecision(juce::AudioProcessor::ProcessingPrecision newPrecision) noexcept;
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    //==============================================================================
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&,  juce::MidiBuffer&) override;
    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;
    void processBlockBypassed(juce::AudioBuffer<float>& buffer,  juce::MidiBuffer& midiMessages) override;
    void processBlockBypassed(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) override;
    using juce::AudioProcessor::processBlock;
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    //==============================================================================
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void getCurrentProgramStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void setCurrentProgramStateInformation(const void* data, int sizeInBytes) override;
    //==============================================================================
    juce::UndoManager& getUndoManager() { return undoManager; }
    //==============================================================================
    juce::AudioProcessorValueTreeState& getApvts() { return apvts; }
    //==============================================================================
    juce::dsp::ProcessSpec& getSpec() { return spec; }
    //==============================================================================
    const AudioPluginAudioProcessorParameters& getParameters() { return parameters; }

private:
    //==============================================================================
    AudioPluginAudioProcessorWrapper<float>& getProcessorFlt() { return processorFlt; }
    AudioPluginAudioProcessorWrapper<double>& getProcessorDbl() { return processorDbl; }
    //==============================================================================
    juce::AudioProcessor::ProcessingPrecision processingPrecision;
    juce::dsp::ProcessSpec spec;
    //==============================================================================
    std::unique_ptr<AudioPluginAudioProcessorParameters> parametersPtr { nullptr };
    AudioPluginAudioProcessorParameters& parameters;
    //==============================================================================
    juce::UndoManager& undoManager;
    juce::AudioProcessorValueTreeState& apvts;
    //==============================================================================
    std::unique_ptr<AudioPluginAudioProcessorWrapper<float>> processorFltPtr { nullptr };
    std::unique_ptr<AudioPluginAudioProcessorWrapper<double>> processorDblPtr { nullptr };
    //==============================================================================
    AudioPluginAudioProcessorWrapper<float>& processorFlt;
    AudioPluginAudioProcessorWrapper<double>& processorDbl;
    //==============================================================================
    juce::AudioParameterBool* bypassState { nullptr };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP
