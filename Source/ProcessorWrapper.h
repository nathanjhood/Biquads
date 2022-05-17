/*
  ==============================================================================

    ProcessorWrapper.h
    Created: 8 May 2022 9:38:17pm
    Author:  StoneyDSP

  ==============================================================================
*/

#pragma once

#ifndef PROCESSORWRAPPER_H_INCLUDED
#define PROCESSORWRAPPER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Coefficients.h"
#include "Transforms.h"


template <typename SampleType>
class ProcessWrapper
{
public:
    //==============================================================================
    /** Constructor. */
    ProcessWrapper(AudioProcessorValueTreeState& apvts);

    //==============================================================================
    /** Create Parameter Layout. */
    static void createParameterLayout(std::vector<std::unique_ptr<RangedAudioParameter>>& params);

    //==============================================================================
    /** Initialises the processor. */
    void prepare(double sampleRate, int samplesPerBlock, int numChannels);

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
    //BiquadsAudioProcessor& audioProcessor;

    //==============================================================================
    /** Instantiate objects. */
    juce::dsp::ProcessSpec spec;
    Coefficients<SampleType> coeffs;
    Transformations<SampleType> transform;
    //Biquads<SampleType> biquad;

    //==============================================================================
    /** Parameter pointers. */
    juce::AudioParameterBool*       ioPtr           { nullptr };
    juce::AudioParameterFloat*      frequencyPtr    { nullptr };
    juce::AudioParameterFloat*      bandwidthPtr    { nullptr };
    juce::AudioParameterFloat*      gainPtr         { nullptr };
    juce::AudioParameterChoice*     transformPtr    { nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessWrapper)
};

#endif //  PROCESSORWRAPPER_H_INCLUDED