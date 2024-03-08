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

#define STONEYDSP_BIQUADS_PLUGINPROCESSWRAPPER_HPP

#include <JuceHeader.h>

// #include "DSP/Biquads.hpp"
// #include "PluginProcessor.hpp"
class BiquadsAudioProcessor;

template <typename SampleType>
class ProcessWrapper
{
public:
    //==========================================================================
    /** Constructor. */
    ProcessWrapper(BiquadsAudioProcessor& p);
    //==========================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

    //==========================================================================
    void process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages);

    //==========================================================================
    /** Updates the internal state variables of the processor. */
    void update();

private:
    // This reference is provided as a quick way for the wrapper to
    // access the processor object that created it.
    BiquadsAudioProcessor& audioProcessor;

    juce::AudioProcessorValueTreeState& state;
    juce::dsp::ProcessSpec& setup;

    //==========================================================================
    /** Instantiate parameter objects. */
    juce::dsp::DryWetMixer<SampleType> mixer;
    juce::dsp::Gain<SampleType> output;
    //==========================================================================
    /** Instantiate parameter object pointers. */
    juce::AudioParameterFloat* outputPtr;
    juce::AudioParameterFloat* mixPtr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessWrapper)
};
