/**
 * @file PluginWrapper.hpp
 * @author StoneyDSP (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#define STONEYDSP_BIQUADS_PLUGINWRAPPER_HPP

#include <JuceHeader.h>

namespace StoneyDSP {
namespace Biquads {

class AudioPluginAudioProcessor;

template <typename SampleType>
class AudioPluginAudioProcessorWrapper
{
public:
    //==============================================================================
    /** Constructor. */
    AudioPluginAudioProcessorWrapper(AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts, juce::dsp::ProcessSpec& spec);

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

    //==============================================================================
    void process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages);
    void processBlock(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages);
    void processBypass(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages);

    //==============================================================================
    /** Updates the internal state variables of the processor. */
    void update();

private:
    //==============================================================================
    // This reference is provided as a quick way for the wrapper to
    // access the processor object that created it.
    AudioPluginAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& state;
    juce::dsp::ProcessSpec& setup;

    juce::dsp::DryWetMixer<SampleType> mixer;
    juce::AudioParameterFloat* outputPtr  { nullptr };

    StoneyDSP::Maths::Coefficient<SampleType> coeff;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorWrapper)
};

} // namespace StoneyDSP
} // namespace Biquads
