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
//#include "Modules/Biquads.h"

class BiquadsAudioProcessor;

template <typename SampleType>
class ProcessWrapper
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    using ProcessSpec = juce::dsp::ProcessSpec;
    //==========================================================================
    /** Constructor. */
    ProcessWrapper(BiquadsAudioProcessor& p);

    //static void call(Caller& clock);

    //==============================================================================
    /** Sets the length of the ramp used for smoothing parameter changes. */
    void setRampDurationSeconds(double newDurationSeconds) noexcept;

    /** Returns the ramp duration in seconds. */
    double getRampDurationSeconds() const noexcept;

    /** Returns true if the current value is currently being interpolated. */
    bool isSmoothing() const noexcept;

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

    /*void timerCallback() override
    {
        while (true)
        {
            audioProcessor.setBypassParameter(bypassPtr);

            setOversampling();

            mixer.setWetMixProportion(mixPtr->get() * 0.01f);

            biquad.setFrequency(frequencyPtr->get());
            biquad.setResonance(resonancePtr->get());
            biquad.setGain(gainPtr->get());
            biquad.setFilterType(static_cast<stoneydsp::filters::FilterType>(typePtr->getIndex()));
            biquad.setTransformType(static_cast<stoneydsp::filters::TransformationType>(transformPtr->getIndex()));

            output.setGainLinear(juce::Decibels::decibelsToGain(outputPtr->get()));
        }
    }*/

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
    ProcessSpec& setup;

    //==========================================================================
    std::unique_ptr<juce::dsp::Oversampling<SampleType>> oversampler[5];

    //==========================================================================
    /** Instantiate objects. */
    juce::dsp::DryWetMixer<SampleType> mixer;
    //Biquads<SampleType> biquad;
    stoneydsp::filters::Biquads<SampleType> biquad;
    juce::dsp::Gain<SampleType> output;
    
    //==============================================================================
    /** Parameter Smoothers. */
    juce::SmoothedValue<SampleType, juce::ValueSmoothingTypes::Multiplicative> frq;
    juce::SmoothedValue<SampleType, juce::ValueSmoothingTypes::Linear> res;
    juce::SmoothedValue<SampleType, juce::ValueSmoothingTypes::Linear> lev;

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
    double rampDurationSeconds = 0.00005;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessWrapper)
};

#endif //  PLUGINWRAPPER_H_INCLUDED
