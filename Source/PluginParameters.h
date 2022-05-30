/*
  ==============================================================================

    PluginParameters.h
    Created: 29 May 2022 7:58:00pm
    Author:  Nathan J. Hood

  ==============================================================================
*/

#pragma once

#ifndef PLUGINPARAMETERS_H_INCLUDED
#define PLUGINPARAMETERS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class Parameters
{
public:
    using Params = std::vector<std::unique_ptr<juce::RangedAudioParameter>>;
    //==============================================================================
    /** Constructor. */
    Parameters(juce::AudioProcessorValueTreeState& apvts);

    //==============================================================================
    /** Create Parameter Layout. */
    static void createParameterLayout(Params& params);

    //==============================================================================
    /** Sets the length of the ramp used for smoothing parameter changes. */
    void setRampDurationSeconds(double newDurationSeconds) noexcept;

    /** Returns the ramp duration in seconds. */
    double getRampDurationSeconds() const noexcept;

    /** Returns true if the current value is currently being interpolated. */
    bool isSmoothing() const noexcept;

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

private:
    //==============================================================================
    juce::AudioParameterFloat* frequencyPtr{ nullptr };
    juce::AudioParameterFloat* resonancePtr{ nullptr };
    juce::AudioParameterFloat* gainPtr{ nullptr };
    juce::AudioParameterChoice* typePtr{ nullptr };
    juce::AudioParameterChoice* transformPtr{ nullptr };
    juce::AudioParameterFloat* outputPtr { nullptr };
    juce::AudioParameterChoice* osPtr{ nullptr };

    double sampleRate = 44100.0, rampDurationSeconds = 0.00005;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)
};

#endif //PLUGINPARAMETERS_H_INCLUDED