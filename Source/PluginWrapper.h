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
#include "Biquads.h"

class BiquadsAudioProcessor;

template <typename SampleType>
class ProcessWrapper
{
public:
    //==============================================================================
    /** Constructor. */
    ProcessWrapper(BiquadsAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);

    //==============================================================================
    /** Initialises the processor. */
    void prepare(double sampleRate, int samplesPerBlock);

    /** Resets the internal state variables of the processor. */
    void reset();

    //==============================================================================
    /** Updates the internal state variables of the processor. */
    void update();

    //==============================================================================
    void process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages);

private:
    //==============================================================================
    // This reference is provided as a quick way for the wrapper to
    // access the processor object that created it.
    BiquadsAudioProcessor& audioProcessor;

    //==============================================================================
    /** Sets the oversampling factor. */
    void setOversampling();

    //==============================================================================
    std::unique_ptr<juce::dsp::Oversampling<SampleType>> overSample[5];

    //==============================================================================
    /** Instantiate objects. */
    juce::dsp::ProcessSpec spec;
    juce::dsp::DryWetMixer<SampleType> mixer;
    Biquads<SampleType> biquad;

    //==============================================================================
    /** Init variables. */
    int curOS = 0, prevOS = 0, overSamplingFactor = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessWrapper)
};

#endif //  PLUGINWRAPPER_H_INCLUDED