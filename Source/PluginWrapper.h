/*
  ==============================================================================

    PluginWrapper.h
    Created: 8 May 2022 9:38:17pm
    Author:  StoneyDSP

  ==============================================================================
*/

#pragma once

#ifndef PLUGINWRAPPER_H_INCLUDED
#define PLUGINWRAPPER_H_INCLUDED

#include <JuceHeader.h>
#include "Modules/SecondOrderBiquads.h"

class BiquadsAudioProcessor;

template <typename SampleType>
class ProcessWrapper
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    using Spec = juce::dsp::ProcessSpec;
    using Audio = juce::AudioBuffer<SampleType>;
    using Midi = juce::MidiBuffer;
    //==========================================================================
    /** Constructor. */
    ProcessWrapper(BiquadsAudioProcessor& p);

    //==========================================================================
    /** Initialises the processor. */
    void prepare(Spec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

    //==========================================================================
    void process(Audio& buffer, Midi& midiMessages);

    void bypass(Audio& buffer, Midi& midiMessages);

    void juce::dsp::ProcessContextReplacing::

    //==========================================================================
    /** Updates the internal state variables of the processor. */
    void update();

    //==========================================================================
    /** Sets the oversampling factor. */
    void setOversampling();

    SampleType getLatencySamples() const noexcept;

private:
    //==========================================================================
    // This reference is provided as a quick way for the wrapper to
    // access the processor object that created it.
    BiquadsAudioProcessor& audioProcessor;
    APVTS& state;
    Spec& setup;

    //==========================================================================
    std::unique_ptr<juce::dsp::Oversampling<SampleType>> oversampler[5];

    //==========================================================================
    /** Instantiate objects. */
    juce::dsp::DryWetMixer<SampleType> mixer;
    Biquads<SampleType> biquad;
    juce::dsp::Gain<SampleType> output;

    //stoneydsp::filters::Biquads<SampleType> biquad;

    //==========================================================================
    /** Parameter pointers. */
    juce::AudioParameterFloat* frequencyPtr;
    juce::AudioParameterFloat* resonancePtr;
    juce::AudioParameterFloat* gainPtr;
    juce::AudioParameterChoice* typePtr;
    juce::AudioParameterChoice* transformPtr;
    juce::AudioParameterChoice* osPtr;
    juce::AudioParameterFloat* outputPtr;
    juce::AudioParameterFloat* mixPtr;
    juce::AudioParameterBool* bypassPtr;

    //==========================================================================
    /** Init variables. */
    int curOS = 0, prevOS = 0, oversamplingFactor = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessWrapper)
};

#endif //  PLUGINWRAPPER_H_INCLUDED
