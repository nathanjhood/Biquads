/**
 * @file PluginWrapper.h
 * @author StoneyDSP (nathanjhood@googlemail.com)
 * @brief
 * @version 0.1
 * @date 2023-09-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#define __STONEYDSP_BIQUADS_PLUGINWRAPPER_H__

#include <JuceHeader.h>

#include "Modules/SecondOrderBiquads.h"

class BiquadsAudioProcessor;

template <typename SampleType>
class ProcessWrapper
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    using Spec = juce::dsp::ProcessSpec;
    //==========================================================================
    /** Constructor. */
    ProcessWrapper(BiquadsAudioProcessor& p);

    ////==============================================================================
    ///** Sets the length of the ramp used for smoothing parameter changes. */
    //void setRampDurationSeconds(double newDurationSeconds) noexcept;

    ///** Returns the ramp duration in seconds. */
    //double getRampDurationSeconds() const noexcept;

    ///** Returns true if the current value is currently being interpolated. */
    //bool isSmoothing() const noexcept;

    //==========================================================================
    /** Initialises the processor. */
    void prepare(Spec& spec);

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
    Spec& setup;

    //==========================================================================
    std::unique_ptr<juce::dsp::Oversampling<SampleType>> oversampler[5];

    //==========================================================================
    /** Instantiate objects. */
    juce::dsp::DryWetMixer<SampleType> mixer;
    Biquads<SampleType> biquad;
    juce::dsp::Gain<SampleType> output;

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

    //==========================================================================
    /** Init variables. */
    int curOS = 0, prevOS = 0, oversamplingFactor = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessWrapper)
};
