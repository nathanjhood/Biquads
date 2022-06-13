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

#include "../JuceLibraryCode/JuceHeader.h"
#include "Modules/Biquads.h"

class BiquadsAudioProcessor;

template <typename SampleType>
class ProcessWrapper
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    //==========================================================================
    /** Constructor. */
    ProcessWrapper(BiquadsAudioProcessor& p, APVTS& apvts);

    //==========================================================================
    /** Initialises the processor. */
    void prepare();

    /** Resets the internal state variables of the processor. */
    void reset();

    //==========================================================================
    void process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages);

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

    //==========================================================================
    std::unique_ptr<juce::dsp::Oversampling<SampleType>> overSample[5];

    //==========================================================================
    /** Instantiate objects. */
    juce::dsp::ProcessSpec spec;
    juce::dsp::DryWetMixer<SampleType> mixer;
    Biquads<SampleType> biquad;
    juce::dsp::Gain<SampleType> output;

    //==========================================================================
    /** Parameter pointers. */
    juce::AudioParameterFloat* frequencyPtr { nullptr };
    juce::AudioParameterFloat* resonancePtr { nullptr };
    juce::AudioParameterFloat* gainPtr { nullptr };
    juce::AudioParameterChoice* typePtr { nullptr };
    juce::AudioParameterChoice* transformPtr {nullptr};
    juce::AudioParameterChoice* osPtr { nullptr };
    juce::AudioParameterFloat* outputPtr { nullptr };
    juce::AudioParameterFloat* mixPtr { nullptr };
    juce::AudioParameterBool* bypassPtr { nullptr };

    //==========================================================================
    /** Init variables. */
    int curOS = 0, prevOS = 0, overSamplingFactor = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessWrapper)
};

#endif //  PLUGINWRAPPER_H_INCLUDED
