/**
 * @file Processor.hpp
 * @author Nathan J. Hood (nathanjhood@googlemail.com)
 * @brief
 * @version 1.2.0
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */


#pragma once

#define STONEYDSP_BIQUADS_PROCESSOR_HPP_INCLUDED

// #define DONT_SET_USING_JUCE_NAMESPACE 1
// #include <JuceHeader.h>

#include "StoneyDSP/Biquads.hpp"

#include "Parameters.hpp"
#include "Wrapper.hpp"

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
/** @addtogroup Biquads
 *  @{
 */

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
    // //==============================================================================
    juce::UndoManager undoManager;
    juce::UndoManager& getUndoManager() { return undoManager; }
    // //==============================================================================
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    //==============================================================================
    juce::dsp::ProcessSpec spec;
    juce::dsp::ProcessSpec& getSpec() { return spec; }

    StoneyDSP::Biquads::AudioPluginAudioProcessorParameters& getParams() { return parameters; }

private:
    //==============================================================================
    /** Audio processor members. */
    juce::AudioProcessor::ProcessingPrecision processingPrecision;

    AudioPluginAudioProcessorParameters parameters;
    AudioPluginAudioProcessorWrapper<float> processorFlt;
    AudioPluginAudioProcessorWrapper<double> processorDbl;

    juce::AudioParameterBool* bypassState { nullptr };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};

  /// @} group Biquads
} // namespace Biquads

  /// @} group StoneyDSP
} // namespace StoneyDSP
