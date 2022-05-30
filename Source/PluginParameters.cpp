/*
  ==============================================================================

    PluginParameters.cpp
    Created: 29 May 2022 7:58:00pm
    Author:  Nathan J. Hood

  ==============================================================================
*/

#include "PluginParameters.h"
#include "PluginProcessor.h"

Parameters::Parameters(BiquadsAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts) : audioProcessor (p)
{
    ioPtr = static_cast                <juce::AudioParameterBool*>         (apvts.getParameter("ioID"));
    jassert(ioPtr != nullptr);

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
    auto dBMax = juce::Decibels::gainToDecibels(16.0f);
    auto dBMin = juce::Decibels::gainToDecibels(0.0625f);

    auto freqRange = juce::NormalisableRange<float>(20.00f, 20000.00f, 0.01f, 00.198894f);
    auto gainRange = juce::NormalisableRange<float>(dBMin, dBMax, 0.01f, 1.00f);
    auto mixRange = juce::NormalisableRange<float>(00.00f, 100.00f, 0.01f, 1.00f);

    auto fString = juce::StringArray({ "Low Pass (2)", "Low Pass (1)", "High Pass (2)", "High Pass (1)" , "Band Pass", "Band Pass (Q)", "Low Shelf (2)", "Low Shelf (1)", "Low Shelf (1c)", "High Shelf (2)", "High Shelf (1)", "High Shelf (1c)", "Peak", "Notch", "All Pass" });
    auto tString = juce::StringArray({ "Direct Form I", "Direct Form II", "Direct Form I (t)", "Direct Form II (t)" });
    auto osString = juce::StringArray({ "1x", "2x", "4x", "8x", "16x" });

    params.push_back(std::make_unique<juce::AudioParameterBool>("ioID", "IO", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("frequencyID", "Frequency", freqRange, 632.45f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("resonanceID", "Resonance", 00.00f, 01.00f, 00.10f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("gainID", "Gain", gainRange, 00.00f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("typeID", "Type", fString, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("transformID", "Transform", tString, 3));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osID", "Oversampling", osString, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("outputID", "Output", gainRange, 00.00f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mixID", "Mix", mixRange, 100.00f));
}