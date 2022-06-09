/*
  ==============================================================================

    PluginParameters.cpp
    Created: 29 May 2022 7:58:00pm
    Author:  Nathan J. Hood

  ==============================================================================
*/

#include "PluginParameters.h"
#include "PluginProcessor.h"

Parameters::Parameters(BiquadsAudioProcessor& p, APVTS& apvts) : audioProcessor (p), state(apvts)
{
}

void Parameters::setParameterLayout(Params& params)
{
    const auto dBMax = juce::Decibels::gainToDecibels(16.0f);
    const auto dBMin = juce::Decibels::gainToDecibels(0.0625f);
    const auto dBOut = juce::Decibels::gainToDecibels(0.5f, -120.0f) * 20.0f;

    const auto freqRange = juce::NormalisableRange<float>(20.00f, 20000.00f, 0.001f, 00.198894f);
    const auto resRange = juce::NormalisableRange<float>(00.00f, 1.00f, 0.01f, 1.00f);
    const auto gainRange = juce::NormalisableRange<float>(dBMin, dBMax, 0.01f, 1.00f);
    const auto mixRange = juce::NormalisableRange<float>(00.00f, 100.00f, 0.01f, 1.00f);
    const auto outputRange = juce::NormalisableRange<float>(dBOut, dBMax, 0.01f, 1.00f);

    const auto fString = juce::StringArray({ "LP2", "LP1", "HP2", "HP1" , "BP2", "BP2c", "LS2", "LS1c", "LS1", "HS2", "HS1c", "HS1", "PK2", "NX2", "AP2" });
    const auto tString = juce::StringArray({ "DFI", "DFII", "DFI t", "DFII t" });
    const auto osString = juce::StringArray({ "--", "2x", "4x", "8x", "16x" });

    const auto decibels = juce::String{ ("dB") };
    const auto frequency = juce::String{ ("Hz") };
    const auto reso = juce::String{ ("q") };
    const auto percentage = juce::String{ ("%") };

    auto genParam = juce::AudioProcessorParameter::genericParameter;
    auto inMeter = juce::AudioProcessorParameter::inputMeter;
    auto outParam = juce::AudioProcessorParameter::outputGain;
    auto outMeter = juce::AudioProcessorParameter::outputMeter;

    juce::ignoreUnused(inMeter);
    juce::ignoreUnused(outMeter);

    params.add
        //======================================================================
        (std::make_unique<juce::AudioProcessorParameterGroup>("masterID", "1", "seperatorA",
            //==================================================================
            std::make_unique<juce::AudioParameterChoice>("transformID", "Transform", tString, 3),
            std::make_unique<juce::AudioParameterChoice>("osID", "Oversampling", osString, 0),
            std::make_unique<juce::AudioParameterFloat>("outputID", "Output", outputRange, 00.00f, decibels, outParam),
            std::make_unique<juce::AudioParameterFloat>("mixID", "Mix", mixRange, 100.00f, percentage, genParam),
            std::make_unique<juce::AudioParameterBool>("bypassID", "Bypass", false),
            std::make_unique<juce::AudioParameterBool>("undoID", "Undo", false),
            std::make_unique<juce::AudioParameterBool>("redoID", "Redo", false)
            //==================================================================
            ));

    params.add
        //======================================================================
        (std::make_unique<juce::AudioProcessorParameterGroup>("BandOneID", "0", "seperatorB",
            //==================================================================
            std::make_unique<juce::AudioParameterFloat>("frequencyID", "Frequency", freqRange, 632.455f, frequency, genParam),
            std::make_unique<juce::AudioParameterFloat>("resonanceID", "Resonance", resRange, 00.10f, reso, genParam),
            std::make_unique<juce::AudioParameterFloat>("gainID", "Gain", gainRange, 00.00f, decibels, genParam),
            std::make_unique<juce::AudioParameterChoice>("typeID", "Type", fString, 0)
            //==================================================================
            ));
}