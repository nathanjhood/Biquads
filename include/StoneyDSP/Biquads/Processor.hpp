/***************************************************************************//**
 * @file Processor.hpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief Simple two-pole equalizer with variable oversampling.
 * @version 1.2.2.167
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
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;
    void processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
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
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    //==============================================================================
    juce::dsp::ProcessSpec& getSpec() { return spec; }

private:
    //==============================================================================
    /** Audio processor members. */
    juce::UndoManager undoManager;
    juce::AudioProcessorValueTreeState apvts;

    juce::dsp::ProcessSpec spec;
    juce::AudioProcessor::ProcessingPrecision processingPrecision;
    //==============================================================================
    std::unique_ptr<AudioPluginAudioProcessorParameters> parametersPtr;
    std::unique_ptr<AudioPluginAudioProcessorWrapper<float>> processorFltPtr;
    std::unique_ptr<AudioPluginAudioProcessorWrapper<double>> processorDblPtr;

    // AudioPluginAudioProcessorParameters& parameters;
    // AudioPluginAudioProcessorWrapper<float>& processorFlt;
    // AudioPluginAudioProcessorWrapper<double>& processorDbl;

    //==============================================================================
    /** Parameter pointers. */
    juce::AudioParameterBool*       bypassState             { nullptr };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP
