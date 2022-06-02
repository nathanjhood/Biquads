/*
  ==============================================================================

    PluginParameters.cpp
    Created: 29 May 2022 7:58:00pm
    Author:  Nathan J. Hood

  ==============================================================================
*/

#include "PluginParameters.h"
#include "PluginProcessor.h"

Parameters::Parameters(BiquadsAudioProcessor& p/*, APVTS& apvts*/) : audioProcessor (p), apvts(p.getAPVTS())
{
    /*ioPtr = static_cast                <juce::AudioParameterBool*>         (apvts.getParameter("ioID"));
    jassert(ioPtr != nullptr);*/

    frequencyPtr = static_cast         <juce::AudioParameterFloat*>        (apvts.getParameter("frequencyID"));
    jassert(frequencyPtr != nullptr);

    resonancePtr = static_cast         <juce::AudioParameterFloat*>        (apvts.getParameter("resonanceID"));
    jassert(resonancePtr != nullptr);

    gainPtr = static_cast              <juce::AudioParameterFloat*>        (apvts.getParameter("gainID"));
    jassert(gainPtr != nullptr);

    typePtr = static_cast              <juce::AudioParameterChoice*>       (apvts.getParameter("typeID"));
    jassert(typePtr != nullptr);

    transformPtr = static_cast         <juce::AudioParameterChoice*>       (apvts.getParameter("transformID"));
    jassert(transformPtr != nullptr);

    osPtr = static_cast                <juce::AudioParameterChoice*>       (apvts.getParameter("osID"));
    jassert(osPtr != nullptr);

    outputPtr = static_cast            <juce::AudioParameterFloat*>        (apvts.getParameter("outputID"));
    jassert(outputPtr != nullptr);

    mixPtr = static_cast              <juce::AudioParameterFloat*>        (apvts.getParameter("mixID"));
    jassert(mixPtr != nullptr);
}

void Parameters::setParameterLayout(Params& params)
{
    const auto dBMax = juce::Decibels::gainToDecibels(16.0f);
    const auto dBMin = juce::Decibels::gainToDecibels(0.0625f);

    const auto freqRange = juce::NormalisableRange<float>(20.00f, 20000.00f, 0.001f, 00.198894f);
    auto gainRange = juce::NormalisableRange<float>(dBMin, dBMax, 0.01f, 1.00f);
    const auto mixRange = juce::NormalisableRange<float>(00.00f, 100.00f, 0.01f, 1.00f);

    auto fString = juce::StringArray({ "LP2", "LP1", "HP2", "HP1" , "BP2", "BP2c", "LS2", "LS1c", "LS1", "HS2", "HS1c", "HS1", "PK2", "NX2", "AP2" });
    auto tString = juce::StringArray({ "DFI", "DFII", "DFI t", "DFII t" });
    auto osString = juce::StringArray({ "--", "2x", "4x", "8x", "16x" });

    params.add(std::make_unique<juce::AudioParameterBool>("ioID", "IO", false));
    params.add(std::make_unique<juce::AudioParameterFloat>("frequencyID", "Frequency", freqRange, 632.45f));
    params.add(std::make_unique<juce::AudioParameterFloat>("resonanceID", "Resonance", 00.00f, 01.00f, 00.10f));
    params.add(std::make_unique<juce::AudioParameterFloat>("gainID", "Gain", gainRange, 00.00f));
    params.add(std::make_unique<juce::AudioParameterChoice>("typeID", "Type", fString, 0));
    params.add(std::make_unique<juce::AudioParameterChoice>("transformID", "Transform", tString, 3));
    params.add(std::make_unique<juce::AudioParameterChoice>("osID", "Oversampling", osString, 0));
    params.add(std::make_unique<juce::AudioParameterFloat>("outputID", "Output", gainRange, 00.00f));
    params.add(std::make_unique<juce::AudioParameterFloat>("mixID", "Mix", mixRange, 100.00f));
}