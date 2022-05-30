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

class BiquadsAudioProcessor;

class Parameters
{
public:
    using Params = std::vector<std::unique_ptr<juce::RangedAudioParameter>>;
    //==========================================================================
    /** Constructor. */
    Parameters(BiquadsAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);

    //==========================================================================
    /** Create Parameter Layout. */
    static void setParameterLayout(Params& params);

private:
    //==========================================================================
    // This reference is provided as a quick way for the wrapper to
    // access the processor object that created it.
    BiquadsAudioProcessor& audioProcessor;

    //==========================================================================
    /** Parameter pointers. */
    juce::AudioParameterBool*               ioPtr                   { nullptr };
    juce::AudioParameterFloat*              frequencyPtr            { nullptr };
    juce::AudioParameterFloat*              resonancePtr            { nullptr };
    juce::AudioParameterFloat*              gainPtr                 { nullptr };
    juce::AudioParameterChoice*             typePtr                 { nullptr };
    juce::AudioParameterChoice*             transformPtr            { nullptr };
    juce::AudioParameterChoice*             osPtr                   { nullptr };
    juce::AudioParameterFloat*              outputPtr               { nullptr };
    juce::AudioParameterFloat*              mixPtr                  { nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)
};

#endif //PLUGINPARAMETERS_H_INCLUDED